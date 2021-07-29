/*	this GA is adapted from CalPoly's hybrid roulette / tournament method to work with Ohio State's loop. 
	Written by David Liu
	Revised by Suren Gourapura to accept NPoP on 29 Dec 2018
	
	Everything is carefully commented, so things should hopefully be clear.
	
*/

// Compile using: g++ -std=c++11 Rank_GA.cpp -o GA.exe

// Here is how to run:
// call format: ./a.out start/cont NPOP 1 reproduction_percent crossover_percent roulette_fraction*10 tournament_fraction*10 rank_fraction*10
// Meaning:
// start/cont:			indicates whether we are starting from 0th gen or not
// NPOP:			size of population
// reproduction_percent:	number in population to be formed using reproduction (beyond 0th gen)
// crossover_percent:		number in population to be formed through crossover (beyond 0th gen)
// roulette_fraction*10:	fraction of parents selected through roulette (10 = 100%)
// tournament_fraction*10:	fraction of pareents selected through tournament (10 = 100%)
// rank_fraction*10:		fraction of parents selected through rank (10 = 100%)
// 	NOTE: Make sure the last three ADD UP TO 10
// 
// Example:
// 	./a.out cont 50 1 3 36 8 2 0
// 	^ runs beyond 0th gen with 50 individuals
// 	^ 3 individuals formed through reproduction, 36 through crossover, 11 through mutation
// 	^ 80% of parents selected through roulette
// 	^ 20% selected through roulette
// 	^ 0% through rank


#include <time.h>
#include <math.h>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <math.h>
#include <ctime>

using namespace std;

// Headers for subroutines we'll call in main

/*	Inputs for function dataRead: 
	varInput = tensor for variables to be read by the program (parent population)
	fitness = vector storing the fitness scores for each parent
*/
void dataRead(vector<vector<vector<float>>> &varInput, vector<float> &fitness);

/*	Inputs for function dataWrite: 
	numChildren = total number of children (population size)
	varVector = variable data for the children to be written to generationDNA
	freq_coeffs = number of frequencies we're running with (named coeffs since later we'll be using pulses with coefficients)
	freqVector = all the frequency coefficients are stored in this vector
*/
void dataWrite(int numChildren, vector<vector<vector<float>>> &varVector, int freq_coeffs, vector<double> freqVector);

/* 	Inputs for function checkConvergence:
	varInput = variables we read from previous generation
	fitness = previous generation's fitness scores
*/
int checkConvergence(vector<vector<vector<float>>> &varInput, vector<float> &fitness);

/*	Inputs for function roulette:
	varInput = variables we read from previous generation
	varOutput = variables we generate for next generation
	parents = tensor to store the variables used to generate an individual
	fitness = fitness scores
*/

// Debolt, Patton, Sipe: New modular functions 10/9/2020
int new_tournement(vector<float> fitness, int pool_size);
// Runs a tournament selecton and returns location of individual selected

int new_roulette(vector<float> fitness);
// Runs a roulette selection and returns location of the selected individual

int Rank(vector<float> fitness); 

void reproduction(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int reproduction_no, int pool_size);
// Calls the selection functions to choose a set amount of individuals to be passed into the next generation

void crossover(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int crossover_no, int pool_size, int reproduction_no);
// Calls the selection functions to create parents and then generates two children per pair of parents that get passed into the next generation

void mutation(vector<vector<vector<float> > > & varOutput, int reproduction_no, int crossover_no, float max_length, float max_radius, float max_sepration, float max_outer_radius, float max_A, float max_B);
// Introduces new variables into the next generation

void insertionSort(vector<float> & fitness, vector<vector<vector<float> > > & varInput);

// Declare some global constants

/*
	Nomenclature:
	I refer to the data lines and data structure as "chromosomes" and "genes".
	
	DATA STRUCTURE:
	
	                        GENE 1           GENE 2           ...           GENE NVARS
	CHROMOSOME 1           VARIABLE         VARIABLE        VARIABLE         VARIABLE
	
	CHROMOSOME 2           VARIABLE         VARIABLE        VARIABLE         VARIABLE
	
	...
	
	CHROMOSOME NSECTIONS   VARIABLE         VARIABLE        VARIABLE         VARIABLE
	
	Each "chromosome" will hold the data for a particular section of the antenna (if we try to form paperclip antennae later,
	I envision this being useful for example). Each "gene" represents a particular type of variable; for example,
	Gene 1 might represent length. Then gene 1 on EVERY chromosome should represent the length of that section.
	Then in the resulting matrix, (0,0) (1,0) (2,0) ... all encode for the length of section 0, section 1, section 2, etc.
	This should theoretically provide some sort of order and stability across the board. This also generalizes this program
	to be able to work with an arbitrary number of sections or variables.
*/
//Debolt 10/7/2020:
// adding global random number generator
default_random_engine generator(time(NULL));

// Machtay 2/21/20: I am moving some of the global variables into the main function
// I'll leave the things I am moving into main commented out here for now in case we need to undo it

double MINIMUM_FREQUENCY = 0.08333; // This global constant defines the minimum frequency of the range we're working in, in GHz; Initial value is what AraSim uses
//MINIMUM_FREQUENCY = MINIMUM_FREQUENCY*MULTIPLIER_FACTOR

double MAXIMUM_FREQUENCY = 1.0667; // This global constant defines the maximum frequency of the range we're working in, in GHz; Initial value is what AraSim uses
//MAXIMUM_FREQUENCY = MAXIMUM_FREQUENCY*MULTIPLIER_FACTOR

double FREQ_STEP = 0.01667; // This global constant defines the step size between frequencies as we scan.
//FREQ_STEP = FREQ_STEP*MULTIPLIER_FACTOR



//	DNA RELATED CONSTANTS

const int NSECTIONS = 2;  // This global constant controls the number of chromosomes we are working with. This is equal to the number of sections in the antenna.

const int NVARS = 4; // This global constant controls the number of genes we are working with. This is equal to the number of variables per section (e.g. length, radius, angle, coordinates, etc)

const int PARENT_NO = 2; // This global constant controls the number of potential parents per child. If 1, each child will be a clone of the parent.

const int DNA_GARBAGE_END = 9; // This global constant defines where in generationDNA.csv data begins. Data should begin on the NEXT line after this; so if 8, the first line of data is on line 9.

// ALGORITHM RELATED CONSTANTS

int NPOP; /* This global constant represents how many individuals there are per generation. It's value is determined by the user, in argv[2]. It cannot be cast as a constant, because the user's value of NPOP has to be defined inside int main. BE VERY CAREFUL NOT TO REDEFINE NPOP SOMEWHERE IN THE CODE! */

const float MUTABILITY  = 0.6f; // This global constant defines overall mutability. Every generation will have this percentage of their offspring mutated.

/* TOURNAMENT RELATED CONSTANTS
	HOW TOURNAMENT WORKS:
	
	We randomly select a sample of TOURNEY_LOTTERY_SIZE individuals from the parent population. We then select the individual from that lottery who has the HIGHEST fitness score.
	That individual becomes a parent.
	
	The other individuals are returned to the pool. Then we randomly select a new sample of TOURNEY_LOTTERY_SIZE individuals from the new pool, and repeat the process until we have enough
	parents.
	
	Then, we return ALL individuals to the pool, regardless of if they were chosen as parents or not and we begin picking new parents for a NEW offspring. So the process is
	
	INDIVIDUAL X:
	
	
	------------------------------
	| POOL OF POTENTIAL PARENTS |   <-----------
	------------------------------             |
	|                                          |
	|                                          |
	| RANDOM DRAWING OF INDIVIDUALS            |
	V                                          |
	                                           |
	---------------------                      |
	| RANDOM SELECTION |                       | RETURN ALL NON-SELECTEES BACK TO THE POTENTIAL POOL
	---------------------                      | FOR ANOTHER CHANCE TO BE SELECTED AND BECOME PARENTS
	|                                          | 
	|                                          |
	| WHO HAS THE HIGHEST FITNESS SCORE? ------|
	V                                          
	                                           
	------------------                         
	| RANDOM PARENT |   -------> THIS PARENT BECOMES A PARENT FOR INDIVIDUAL 1.
	------------------
	
	
	
	MODIFYING TOURNAMENT:
	To change the number of potential parents, change TOURNEY_LOTTERY_SIZE.
	
	To change the number of offspring we generate, change TOURNEY_PROPORTION.
	
	MUTABILITY defines how many mutations we introduce per generation. If we introduce a lot, we'll have much quicker
	changes in our fitness scores, but we might take a while to actually converge.
*/

// changed for read in this variable in externaly
//const float TOURNEY_PROPORTION = 0.2f; // This value represents what percentage of the children generated use tournament-style selection. Must be 0 < x < 1.

const int TOURNEY_LOTTERY_SIZE = 4; // The number of potential parents selected every time we pick a parent.

// STATISTICS RELATED CONSTANTS

const float CONVERGENCE = 0.00; // Our convergence threshold. If the standard deviation of the last generation is within this value, we terminate the loop and return data.

// Gene One, Gene One controls radius currently

float INITIAL_MEAN_C1_G1 = 1.5f;

float INITIAL_STD_DVN_C1_G1 = 0.75f; 

// Gene Two, Gene two controls length currently 

float INITIAL_MEAN_C1_G2 = 50.0f; //5.0; //50.0f; 

float INITIAL_STD_DVN_C1_G2 = 30.0f; //1.5; //15.0f; 

// Gene Three, Gene three controls angle (in radians) currently 

const float INITIAL_MEAN_C1_G3 = M_PI / 24;

const float INITIAL_STD_DVN_C1_G3 = M_PI / 36;

// All other genes are currently unused
const float INITIAL_MEAN_CX_GY = 0.0f;

const float INITIAL_STD_DVN_CX_GY = 0.0f;

const float MUT_MODULATOR = 4.0f; // This is inversely proportional to the magnitude of mutations. At 2, all mutations are halved in magnitude. The higher this goes, the slower evolution becomes.

// new variables 10/9/2020 subject to change
float max_outer_radius = 7.5; // in cm

float max_radius = max_outer_radius;

float min_length = 37.5; // in cm
float max_length = 140;  // in cm

float max_theta = atan(max_outer_radius/min_length);

// just using the values from Leo/Eliot's script
float min_A = -1;//-0.008;
float max_A = 1;//-0.002;

// how did they decide on these values?
float min_B = -1;//-0.1; 
float max_B = 1;//0.8; // They represent slopes (what are the axes?)

float max_seperation = 2.5;
float min_seperation = 2.5;  

const float reproduction_percent = 0.1;
const float crossover_percent = 0.7;

//main function

int main(int argc, char const *argv[])
{



    /*
	argc = number of console commands
	argv[] = actual console commands 
	varInput = tensor for variables read by the program (parents)
	varOutput = tensor for variables written by the program (children)
	fitness = vector for fitness scores read by the program
    */
	
	/* Create vector for parsing in variables. We're only working with length and radius at the moment, so this is only 1x2.
	// But later on, we'll be working with many more variables, so this should theoretically save some time when we do by writing this way.
	
	// Vector varInput is an  n x NSECTIONS x NVARS tensor, where n is the number of individuals in the population. Basically, every matrix stores the data for each individual.
	// Honestly we probably don't need a matrix for all the variables, but we MIGHT need that much space down the line, so might as well. Data structure is:
	
	INDIVIDUAL 1
	                 GENE 1     GENE 2    GENE 3     GENE 4      ...     GENE Z
	CHROMOSOME 1    RADIUS 1   LENGTH 1    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 2	RADIUS 2   LENGTH 2    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 3	RADIUS 3   LENGTH 3    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 4	RADIUS 4   LENGTH 4    VAR 3      VAR 4      ...      VAR Z
	... 
	CHROMOSOME Y	RADIUS Y   LENGTH Y    VAR 3       VAR 4     ...      VAR Z
	INDIVIDUAL 2
	                 GENE 1     GENE 2    GENE 3     GENE 4      ...     GENE Z
	CHROMOSOME 1    RADIUS 1   LENGTH 1    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 2	RADIUS 2   LENGTH 2    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 3	RADIUS 3   LENGTH 3    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 4	RADIUS 4   LENGTH 4    VAR 3      VAR 4      ...      VAR Z
	... 
	CHROMOSOME Y	RADIUS Y   LENGTH Y    VAR 3       VAR 4     ...      VAR Z
	
	...
	
	INDIVIDUAL X
	                 GENE 1     GENE 2    GENE 3     GENE 4      ...     GENE Z
	CHROMOSOME 1    RADIUS 1   LENGTH 1    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 2	RADIUS 2   LENGTH 2    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 3	RADIUS 3   LENGTH 3    VAR 3      VAR 4      ...      VAR Z
	CHROMOSOME 4	RADIUS 4   LENGTH 4    VAR 3      VAR 4      ...      VAR Z
	... 
	CHROMOSOME Y	RADIUS Y   LENGTH Y    VAR 3       VAR 4     ...      VAR Z
	
	*/

	/*

	We need to adjust the antennas dimensions and frequencies according to our scalefactor, which will be an argument. 
	We're gonna add this on to the end, so the scalefactor will be the final argument.

	 */


	// EDIT 8/8/20: we need to only instantiate the generator (and seed) once
  // needs to not be seeded Ryan and Kai 10/27/2020
	default_random_engine generator;
	generator.seed(time(NULL));
	// generator.seed(1); // for debugging
	srand((unsigned)time(NULL)); // Let's just seed our random number generator off the bat (moved up from below)
  //srand(1); // for debugging
  // I'm going to try recording all of the generator values from these runs to look for patterns
  // First, I need to make a file to write to
  ofstream generator_file;
	generator_file.open("generators.csv");
	generator_file << "First generator: " << endl << generator << endl;	

	//We need to define the scale facor first	
	double GEOSCALE_FACTOR = stod(argv[3]);

	//gene 1 (Radius)
	INITIAL_MEAN_C1_G1 /= GEOSCALE_FACTOR;
	INITIAL_STD_DVN_C1_G1 /= GEOSCALE_FACTOR;

	//gene 2 (Length)
	INITIAL_MEAN_C1_G2 /= GEOSCALE_FACTOR;
	INITIAL_STD_DVN_C1_G2 /= GEOSCALE_FACTOR;

	//gene 3 (Theta)
	//(gene 3 does not need to change; the angle is the same when we scale down the radius and length by the same factor)

	//The frequencies get scaled inversely with the dimensions
	MINIMUM_FREQUENCY *= GEOSCALE_FACTOR; 

	MAXIMUM_FREQUENCY *= GEOSCALE_FACTOR; 
	
	FREQ_STEP *= GEOSCALE_FACTOR; 





	// First, define NPOP using the user's input. The atoi function converts from string to int
	NPOP = atoi(argv[2]);

    	vector<vector<vector<float>>> varInput (NPOP,vector<vector<float> >(NSECTIONS,vector <float>(NVARS, 0.0f)));
	
	// Vector fitness literally just exists to tell us the fitness scores later
	
    	vector<float> fitness (NPOP, 0.0f);
	
	// Create vector that will hold the children's variables. Same structure as varInput.
	
    	vector<vector<vector<float>>> varOutput (NPOP,vector<vector<float> >(NSECTIONS,vector <float>(NVARS, 0.0f)));
	
	int freq_coeffs = round((MAXIMUM_FREQUENCY - MINIMUM_FREQUENCY) / FREQ_STEP + 1);
	
	vector<double> freqVector (freq_coeffs, 0.0);
	
	// We'll just calculate and input our frequencies now - they aren't used anywhere else in the program.
	
	//Define reproduction and crossover numbers

	//int reproduction_no = reproduction_percent*varOutput.size();
	int reproduction_no = atoi(argv[4]);
	//int crossover_no = crossover_percent*varOutput.size();
	int crossover_no = atoi(argv[5]);
	const float TOURNEY_PROPORTION = (atoi(argv[7]))/10.0;
	const float ROULETTE_PROPORTION = (atoi(argv[6]))/10.0;
	const float RANK_PROPORTION = (atoi(argv[8]))/10.0;


	while(crossover_no%2 !=0)
	  {
	    crossover_no = crossover_no-1;
	  }
	int pool_size = .07*varOutput.size();
	if(pool_size == 0)
	  {
	    pool_size=1;
	  }
	freqVector[0] = MINIMUM_FREQUENCY;
	
	for(int i=1; i<freq_coeffs; i++)
	  {
		freqVector[i] = MINIMUM_FREQUENCY + (FREQ_STEP * i);
	}
	
    	// Read in input arguments and parse in data from files
	
	cout << "Bicone algorithm initialized." << endl;
	//cout << argc << " arguments" << endl;
    	if(argc != 9)
	  {cout << "Error: Usage. Specify start or cont, as well as NPOP (EX: start 10)." << endl << "Form: start/cont NPOP 1 repro_no cross_no ratioT/R" << endl;} 
    	else
    	{
      		if(string(argv[1]) == "start")
		{
			/*
				These loops should generate a random number from a normalized distribution with mean 0.5 and sigma 0.25
				to populate every entry in our matrix that we're using. Currently, that's only a 1x2 vector, but later
				it could be a lot bigger.
				Sample:
			
				Current:
			
				r r 0 0 0 ... 0
				0 0 0 0 0 ... 0
				.             .
				.             .
				.             .
				0 0 0 0 0 ... 0
			
				My thought process currently is each row represents sections of the antenna (to accommodate weird shapes)
				and then each column represents a specific variable in that section (currently just length and radius)
			*/
		  // seession with kai (Debolt 10/27/2020) normal dist instead of uniform???
		        std::uniform_real_distribution <float> distribution_radius(0, max_radius);   //INITIAL_MEAN_C1_G1, INITIAL_STD_DVN_C1_G1);
			std::uniform_real_distribution <float> distribution_length(min_length, max_length); //INITIAL_MEAN_C1_G2, INITIAL_STD_DVN_C1_G2);
			std::uniform_real_distribution <float> distribution_A(min_A, max_A);
			std::uniform_real_distribution <float> distribution_B(min_B, max_B);
			float limit = 7.5;
			for(int i=0;i<NPOP;i++)
			{
				for(int j=0;j<NSECTIONS;j++)
				{
					bool intersect_condition=0;
                                        while(!intersect_condition)
                                        {

					for(int k=0;k<NVARS;k++)
					{
						if (k == 0)
						{
							float r = distribution_radius(generator);
								// write generator to a file
								generator_file << generator << endl;	
							while(r<=0) // We don't accept negative or zero values
							{
								r = distribution_radius(generator);
								generator_file << generator << endl;
							}
							varOutput[i][j][k]= r;
						}
					
						// EDIT LOCATION FOR CUT
						else if (k == 1)
						{
							float l = distribution_length(generator);
							generator_file << generator << endl;
							while(l<37.5) // now we don't accept below 37.5 cm
							{
								l = distribution_length(generator);
								generator_file << generator << endl;
							}
						
							varOutput[i][j][k]= l;
						}	
						// A is the quadratic coefficient
						else if (k == 2)
						{
							float A = distribution_A(generator);
							generator_file << generator << endl;	
							varOutput[i][j][k]= A;
						}
						// B is the linear coefficient	
						else if (k == 3)
						{
							float B = distribution_B(generator);
							generator_file << generator << endl;
							varOutput[i][j][k] = B;
						}
					}

                                        float R= varOutput[i][j][0];
                                        float l= varOutput[i][j][1];
                                        float a= varOutput[i][j][2];
                                        float b= varOutput[i][j][3];
                                        float end_point = (a*l*l + b*l + R); // Final value of function f(l)
                                        float vertex = (R - (b*b)/(4*a)); // Vertex of the quadratic
 
                                        //cout << end_point << endl;
                                        //cout << vertex << endl;
					// check if outer radius is too large
					// We need to check 4 conditions
					// First, is the outer radius too large?
					// 	If the function is concave down, compare to the parabola's vertex
					// 	If the function is concave up, compare to the parabola's final value (f(l))
					// Second, make sure the bicone doesn't self interset when being revolved around the x axis
					// 	This just means the minimum value of f(l) must be greater than 0
					// 		If the parabola is concave up, check the vertex > 0
					// 		IF the parabola is concave down, check the final value f(l) > 0
                                        if(max_outer_radius > end_point && end_point >= 0.0 && max_outer_radius > vertex && vertex >= 0.0)
                                            {
                                                intersect_condition = 1; // No intersection
                                            }
                                        else{
                                                intersect_condition = 0; // Intersection
                                            }
                                        }
					// Old check for outer radius
					/*
					if(varOutput[i][j][0]+varOutput[i][j][1]*tan(varOutput[i][j][2]) > max_outer_radius)
					  {
					    j = j-1;
					  }*/
				}
			}
			
			/* For the time being we comment this out. Need to figure out how to make every gene
			 * counted with a different starting seed later.
		
			for(int i=0;i<NPOP;i++)
			{
				for(int j=0;j<NSECTIONS;j++)
				{
					for(int k=0;k<NVARS;k++)
					{
						float r = distribution(generator);
					
						while(r<=0) // We don't accept negative or zero values
							r = distribution(generator);
					
						varOutput[i][j][k]= r;
					}
				}
			}
			*/
		
			// Next up we write to file generationDNA
			dataWrite(NPOP, varOutput, freq_coeffs, freqVector);
			double meanTotal = 0.0;
			for(int i=0; i<NPOP; i++)
				{
					meanTotal = meanTotal + varOutput[i][0][1];
				}
			float meanForGridSize = meanTotal / NPOP;
			ofstream datasize;
			datasize.open("datasize.txt");
			datasize << meanForGridSize/50.0 << ";";
			datasize.close();	
		}
		else if(string(argv[1]) == "cont")
		{
			  dataRead(varInput,fitness); // Read in the stuff from previous generation
			  // cout << "Calling Sorting Method" << endl;
			  insertionSort(fitness, varInput); 
			  if (checkConvergence(varInput,fitness) == 1) // We check for convergence. If we've converged then end loop
			  {
				  remove("highfive.txt"); // we delete the old highfive.txt that has a 0 in it
				  ofstream highfive;
				  highfive.open("highfive.txt"); // we create a new highfive.txt that will have a 1 in it
				  highfive << 1;
				  highfive.close();
			  }
			  else // If no convergence, generate a new generation and write a new generationDNA.csv
			  {
			    // code block prior to modular functions. Revised 10/9/2020
			    /*
				  roulette(varInput,varOutput,fitness,generator);
				  cout << "Roulette complete." << endl;
				  tournament(varInput,varOutput,fitness,generator);
				  cout << "Tournament complete." << endl;
				  dataWrite(NPOP, varOutput, freq_coeffs, freqVector);
				  double meanTotal = 0.0;
				  for(int i=0; i<NPOP; i++)
				  {
					  meanTotal = meanTotal + varOutput[i][0][1];
				  }
				  float meanForGridSize = meanTotal / NPOP;
				  ofstream datasize;
				  datasize.open("datasize.txt");
				  datasize << meanForGridSize/50.0 << ";";
				  datasize.close();	
			    */

			    reproduction(varInput, varOutput, fitness, ROULETTE_PROPORTION, TOURNEY_PROPORTION, RANK_PROPORTION, reproduction_no, pool_size);
			    crossover(varInput, varOutput, fitness, ROULETTE_PROPORTION, TOURNEY_PROPORTION, RANK_PROPORTION, crossover_no, pool_size, reproduction_no);
			    mutation(varOutput, reproduction_no, crossover_no, max_length, max_radius, max_seperation, max_outer_radius, max_A, max_B);
			    cout << NSECTIONS << endl;
			    dataWrite(NPOP, varOutput, freq_coeffs, freqVector);
			    double meanTotal = 0.0;
			    for(int i=0; i<NPOP; i++)
			      {
				meanTotal = meanTotal + varOutput[i][0][1];
			      }
			    float meanForGridSize = meanTotal / NPOP;
			    ofstream datasize;
			    datasize.open("datasize.txt");
			    datasize << meanForGridSize/50.0 << ";";
			    datasize.close();
			    
			  }
		}
	}
	generator_file.close();
	return (0);
}

	/*		
		As currently written function dataWrite outputs an NSECTIONS x NVARS sized matrix. 
		NSECTIONS represents how many segments in our antenna we're generating for.
		Currently this is only 1 since we're just working with evolving a dipole.
		NVARS represents how many variables in that section we're working with.
		Currently this is only 2 since we're just working with length and radius.
		
		Currently, this function should output something like:
		
		Hybrid of Roulette and Tournament -- Thanks to Cal Poly, Jordan Potter
		Author was David Liu
		Notable Contributors: Julie, Hannah Hasan, and Adam Blenk
		Done at The Ohio State University
		Working on behalf of Dr. Amy Connolly
		And the ANITA project
		Revision date: 21 March 2018 1800 EST
		Matrices for this Generation: 
		R1,L1
		R2,L2
		...
		RX,LX
		
		In the future, it'll output something like:
		
		Hybrid of Roulette and Tournament -- Thanks to Cal Poly, Jordan Potter
		Author was David Liu
		Notable Contributors: Julie, Hannah Hasan, and Adam Blenk
		Done at The Ohio State University
		Working on behalf of Dr. Amy Connolly
		And the ANITA project
		Revision date: 21 March 2018 1800 EST
		Matrices for this Generation: 
		R1,L1,VARA,VARB...VARZ
		R2,L2,VARA,VARB...VARZ
		.                 .
		.                 .
		.                 .
		R99,L99,VARX,VARY...VARZ
		R1,L1,VARA,VARB...VARZ
		R2,L2,VARA,VARB...VARZ
		.                 .
		.                 .
		.                 .
		R99,L99,VARX,VARY...VARZ
		...
		R1,L1,VARA,VARB...VARZ
		R2,L2,VARA,VARB...VARZ
		.                 .
		.                 .
		.                 .
		R99,L99,VARX,VARY...VARZ
	*/
	
void dataWrite(int numChildren, vector<vector<vector<float> > >& varVector, int freq_coeffs, vector<double> freqVector)
{
  ofstream generationDNA;
  generationDNA.open("generationDNA.csv");
  generationDNA << "Hybrid of Roulette and Tournament -- Thanks to Cal Poly / Jordan Potter" << "\n";
  generationDNA << "Author was David Liu" << "\n";
  generationDNA << "Notable contributors: Julie Rolla, Hannah Hasan, and Adam Blenk" << "\n";
  generationDNA << "Done at The Ohio State University" << "\n";
  generationDNA << "Working on behalf of Dr. Amy Connolly" << "\n";
  generationDNA << "And the ANITA project" << "\n";
  generationDNA << "Revision date: 21 March 2018 1800 EST" << "\n";
  for(int i=0;i<freq_coeffs;i++)
    {
      if(i==freq_coeffs-1)
	{
	  generationDNA << freqVector[i] << "\n";
	}
      else
	{
	  generationDNA << freqVector[i] << ",";
	}
    }
  generationDNA << "Matrices for this Generation: " << "\n";
  for(int i=0;i<numChildren;i++)
    {
      for(int j=0;j<NSECTIONS;j++)
	{
	  for(int k=0;k<NVARS;k++)
	    {
	      if(k==(NVARS-1))
		{
		  generationDNA << varVector[i][j][k] << "\n";
		}
	      else
		{
		  generationDNA << varVector[i][j][k] << ",";
		}
	    }
	}
    }
  generationDNA.close();
}

void dataRead(vector<vector<vector<float> > >& varInput, vector<float>& fitness)
{
  ifstream generationDNA;
  generationDNA.open("Generation_Data/generationDNA.csv");
  int csv_file_size = DNA_GARBAGE_END + (NPOP * NSECTIONS);
  string csvContent[csv_file_size]; //contain each line of csv
  string strToDbl; //data change from string to float, then written to varInput or fitness.
  
  /*
    This loop reads through the .csv file line by line.
    If we're in data (past line 9), it reads in the line.
  */
  
  for(int i=0;i<csv_file_size;i++)
    {
      getline(generationDNA,csvContent[i]);
      if (i>=DNA_GARBAGE_END)
	{
	  double j=floor((i-DNA_GARBAGE_END)/NSECTIONS);  // Figure out which individual we're looking at
	  int p=i-DNA_GARBAGE_END-NSECTIONS*j; // pulls out which row of their matrix we're looking at 
	  istringstream stream(csvContent[i]);
	  for(int k=0;k<NVARS;k++)
	    {
	      getline(stream,strToDbl,',');
	      varInput[j][p][k] = atof(strToDbl.c_str());
	    }
	  
	}


    }

  generationDNA.close();
  

  // Now we need to read the fitness scores:
  
  ifstream fitnessScores;
  fitnessScores.open("Generation_Data/fitnessScores.csv");
  string fitnessRead[NPOP+2];
  
  for(int i=0;i<(NPOP+2);i++)
    {
      getline(fitnessScores,fitnessRead[i]);
      if(i>=2)
	{
	  fitness[i-2] = atof(fitnessRead[i].c_str());
	  if (fitness[i-2]<0)
	    {
	      fitness[i-2] = 0; // If the fitness score is less than 0, we set it to 0 to not throw things off
	    }
	}   
    }
  
  fitnessScores.close();
}

int checkConvergence(vector<vector<vector<float>>> &varInput, vector<float> &fitness)
{
	/* 	
		To test convergence we look at standard deviation. To calculate it for each variable,
		we need the mean first; then we calculate sigma. I'm writing them as tensors for future
		because currently we only test the length for convergence, but down the line we may need
		to test every variable.
	*/
	
	vector<vector<float>> meanTensor (NSECTIONS,vector<float>(NVARS,0));
	vector<vector<float>> dvnTensor (NSECTIONS,vector<float>(NVARS,0));
	
	for (int i=0;i<NVARS;i++)
	{
		for (int j=0;j<NSECTIONS;j++)
		{
			float totalSum=0.0f;
			for (int k=0;k<NPOP;k++)
			{
				totalSum+=varInput[k][j][i];
			}
			float mean = totalSum / NPOP;
			meanTensor[j][i]=mean;
		}
	}
	
	for (int i=0;i<NVARS;i++)
	{
		for (int j=0;j<NSECTIONS;j++)
		{
			float dvnSum=0.0f;
			for (int k=0;k<NPOP;k++)
			{
				dvnSum+=pow((varInput[k][j][i]-meanTensor[j][i]),2);
			}
			float dvn = pow((dvnSum / (NPOP - 1)),1.0/2.0);
			dvnTensor[j][i]=dvn;
		}
	}
	
	if (dvnTensor[0][1] <= CONVERGENCE) // This is hella specific and will need to be fixed later.
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int new_tournament(vector<float> fitness, int pool_size)
{
  //needs: fitness, previous gen, random generator;
 
  vector<int> contenders; // tounament pool individuals
  int random_num; // random number from 0-length of the fitness vector
  uniform_real_distribution<float> choice(0, fitness.size());

  for( int i =0; i<pool_size; i++)
    {
      // generate random integer
      random_num = rand() % fitness.size();
      // cout << random_num << endl;
      // place integer number in contenders vector
      contenders.push_back(random_num);
    }

  int max = 0; 
  for(int j=0; j<pool_size; j++) 
    {
     // compare fitness scores coresponding to the integers in contenders
      if(fitness[contenders[j]] > fitness[contenders[max]]) // changed for ryan's test make sure this is a greater than sign for main loop Always check to make sure this is a greater than sign
	{
	  max = j;
	}
    }
  // return the integer that corresponds to the highest fitness score

  return(contenders[max]);
  
}

int new_roulette(vector<float> fitness)
{
  vector<float> probabilities;
  float total_fitness = 0;

  for(int i=0; i< fitness.size();i++)
    {
      total_fitness = total_fitness + fitness[i];
    }
  for(int j =0; j< fitness.size();j++)
    {
      probabilities.push_back(fitness[j]/total_fitness);
    }

  uniform_real_distribution<float> choice(0.0, 1.0);
  float select = choice(generator);

  int x=0;
  float probability_sum = 0;
  for(int i=0; probability_sum <= select; i++)
    {
      probability_sum = probability_sum + probabilities[i];
      x=i;
    }
  return(x);
}

int Rank(vector<float> fitness)
{
  vector<float> probabilities;
  float sum_npop=0;
  for(int i=1; i<=fitness.size(); i++)
    {
      sum_npop = sum_npop + i;
    }
  // cout << sum_npop << endl;
  for(int j=0; j<fitness.size(); j++)
    {
      probabilities.push_back((fitness.size()-j)/(sum_npop));
      // cout << probabilities[j] << endl; 
    }

  uniform_real_distribution<float> choice(0.0, 1.0);
  float select = choice(generator);

  int x=0;
  float probability_sum =0;
  for (int k=0; probability_sum <= select; k++)
    {
      probability_sum = probability_sum + probabilities[k];
      x=k;
    }
  return(x);
}     

void reproduction(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int reproduction_no, int pool_size)
{
  // cout << "reproduction flag" << endl;
  //NEED: Current gen; next generation; roul_perecentage, tour_percentage, reproduction_no, fitness, pool_size
  int roul_no = roul_percentage*reproduction_no;
  int tour_no = tour_percentage*reproduction_no;
  int rank_no = rank_percentage*reproduction_no;

  if(reproduction_no == 1)
    {
      roul_no = 1; // this is because roulette will not be left to purely random chance for small population sizes
    }

  while(roul_no + tour_no + rank_no < reproduction_no)
    {
      tour_no += 1;
    }

  if(roul_no > 0)
    {
      //      cout << "roullete flag"<< endl;
      //int r_select = new_roulette(fitness);
      for(int i=0; i<roul_no; i++)
	{
	  int r_select = new_roulette(fitness); //new_roulette
	  for(int j=0; j<NSECTIONS; j++)
	    {
	      for(int k=0; k<NVARS; k++)
		{
		  varOutput[i][j][k] = varInput[r_select][j][k];
		}
	    }

	}
    }
  if(tour_no > 0)
    {
      // cout << "tournament flag"<< endl;
      // int t_select = new_tournament(fitness, pool_size);
      for(int x=roul_no; x<tour_no+roul_no; x++)
	{
	  int t_select = new_tournament(fitness, pool_size);
	  for(int y=0; y<NSECTIONS; y++)
	    {
	      for(int z=0; z<NVARS; z++)
		{
		  varOutput[x][y][z] = varInput[t_select][y][z];
		}
	    }
	}
      // cout << "reproduction finished" << endl;
    }
  if(rank_no > 0)
    {
      // cout << "rank flag" << endl;
       for(int r=tour_no+roul_no; r<tour_no+roul_no+rank_no; r++)
	 {
	   int k_select = Rank(fitness);
           for(int y=0; y<NSECTIONS; y++)
            {
              for(int z=0; z<NVARS; z++)
                {
                  varOutput[r][y][z] = varInput[k_select][y][z];
                }
            }
	 }
    }
}

void crossover(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int crossover_no, int pool_size, int reproduction_no)
{
  //cout << "crossover flag" << endl;
  // crossover_no contains the total number of individuals that are to be generated using this opperator
  int roul_no = roul_percentage * crossover_no;
  int tour_no = tour_percentage * crossover_no;
  int rank_no = rank_percentage * crossover_no;

  // MACHTAY 6/10/21
  // We can sometimes get problems when we have certain percentages and crossover sizes
  // I'm making an arbitrary decision to add to roulette if we're off by 1
  // If we're off by 2, we can systematically add to the ones that are missing 1
  // This occurs due to the truncation of float set equal to an int
 
  // Off by 1
  if(roul_no + tour_no + rank_no == crossover_no - 1)
  {
    roul_no += 1;
  }
  else if(roul_no + tour_no + rank_no == crossover_no - 2)
  {
    if(roul_percentage * crossover_no - roul_no >= 0.5)
    {
      roul_no += 1;
    }
    if(tour_percentage * crossover_no - tour_no >= 0.5)
    {
      tour_no += 1;
    }
    if(rank_percentage * crossover_no - rank_no >= 0.5)
    {
      rank_no += 1;
    }
  }

  vector<int> parents_loc;
  double swap;
  uniform_real_distribution<double> choice(0.0, 1.0);

  // generate the parents
for(int r=0; r<roul_no; r++)
{
  parents_loc.push_back(new_roulette(fitness)); //new_roulette
}
for(int t=roul_no; t<roul_no+tour_no; t++) //changed to start at roul and go to crossover to stop seg fault 11/23/2020 Debolt//
{
  parents_loc.push_back(new_tournament(fitness, pool_size));
}
for(int k=roul_no+tour_no; k<crossover_no; k++)
{
 parents_loc.push_back(Rank(fitness));
} 
// cout << "parent vector:"<< parents_loc.size() << "?= crossover no:" << crossover_no << endl;

 if(parents_loc.size() != crossover_no)
   {
     cout << "error: parent vector is not proper length" << endl;
   }

// generate children
for(int i=0; i<parents_loc.size(); i=i+2)
  {
    for(int j=0; j<NSECTIONS; j++)

      {
	for(int k=0; k<NVARS; k++)
	  {
	    swap = choice(generator);
	    if(swap < .5)
	      {
		varOutput[i+reproduction_no][j][k] = varInput[parents_loc[i]][j][k];
		varOutput[i+1+reproduction_no][j][k] = varInput[parents_loc[i+1]][j][k];
	      }
	    else
	      {
		varOutput[i+reproduction_no][j][k] = varInput[parents_loc[i+1]][j][k];
		varOutput[i+1+reproduction_no][j][k] = varInput[parents_loc[i]][j][k];
	      }
	  }
		// Check the first individual we made
            float R_1= varOutput[i+reproduction_no][j][0];                                                 
            float l_1= varOutput[i+reproduction_no][j][1]; 
            float a_1= varOutput[i+reproduction_no][j][2];  
            float b_1= varOutput[i+reproduction_no][j][3]; 
            float end_point_1 = (a_1*l_1*l_1 + b_1*l_1 + R_1); 
            float vertex_1 = (R_1 - (b_1*b_1)/(4*a_1)); 
		// Check the second individual we made
	    float R_2= varOutput[i+1+reproduction_no][j][0];
            float l_2= varOutput[i+1+reproduction_no][j][1];
            float a_2= varOutput[i+1+reproduction_no][j][2];
            float b_2= varOutput[i+1+reproduction_no][j][3];
            float end_point_2 = (a_2*l_2*l_2 + b_2*l_2 + R_2);
            float vertex_2 = (R_2 - (b_2*b_2)/(4*a_2));

            //cout << end_point << endl; 
            //cout << vertex << endl;
            // Check for both individuals!!
            if(max_outer_radius > end_point_1 && end_point_1 >= 0.0 && max_outer_radius > vertex_1 && vertex_1 >= 0.0 && max_outer_radius > end_point_2 && end_point_2 >= 0.0 && max_outer_radius > vertex_2 && vertex_2 >= 0.0){ 
              j=j;
		//cout << "Crossover complete" << endl;
		
            }
	    else if(a_1 !=0.0 && max_outer_radius > end_point_1 && end_point_1 >= 0.0 && max_outer_radius > vertex_1 && vertex_1 >= 0.0 && a_2 != 0 && max_outer_radius > end_point_2 && end_point_2 >= 0.0 && max_outer_radius > vertex_2 && vertex_2 >= 0.0)
              {
              j=j;

              }
            else if (a_1 == 0.0 &&  max_outer_radius > end_point_1 && end_point_1 >= 0.0 && a_2 != 0 && max_outer_radius > end_point_2 && end_point_2 >= 0.0 && max_outer_radius > vertex_2 && vertex_2 >= 0.0)
              {
              j=j;
              }

            else if(a_2 == 0.0 && max_outer_radius > end_point_2 && end_point_2 >= 0.0 && a_1 !=0.0 && max_outer_radius > end_point_1 && end_point_1 >= 0.0 && max_outer_radius > vertex_1 && vertex_1 >= 0.0)
              {
              j=j;
              }
            else{
		//cout << "Could not crossover!" << endl; 
              j= j-1; 
            }
	    // previous outer radius check
	    /*
	    if(varOutput[i+reproduction_no][j][0]+(varOutput[i+reproduction_no][j][1]* (tan(varOutput[i+reproduction_no][j][2]))) > max_outer_radius || varOutput[i+1+reproduction_no][j][0]+(varOutput[i+1+reproduction_no][j][1]*(tan(varOutput[i+1+reproduction_no][j][2]))) > max_outer_radius)
	    {
		j= j-1;
	    }
	    */
      }
  }
}
					      
void mutation(vector<vector<vector<float> > > & varOutput, int reproduction_no, int crossover_no, float max_length, float max_radius, float max_seperation, float max_outer_radius, float max_A, float max_B)
{
  //cout << "mutation flag" << endl;
  uniform_real_distribution<float> l_mut(min_length, max_length);
  uniform_real_distribution<float> r_mut(0, max_radius);
  uniform_real_distribution<float> a_mut(min_A, max_A);
  uniform_real_distribution<float> b_mut(min_B, max_B);
  uniform_real_distribution<float> s_mut(min_seperation, max_seperation);

  for(int i=reproduction_no+crossover_no; i< varOutput.size(); i++)
    {
      for(int j=0; j<NSECTIONS; j++)
	{
	  //mutation generators
	  varOutput[i][j][0] = r_mut(generator);
	  varOutput[i][j][1] = l_mut(generator);
	  varOutput[i][j][2] = a_mut(generator);
	  varOutput[i][j][3] = b_mut(generator);
	  varOutput[i][j][4] = s_mut(generator);
	  // gene values
          float R= varOutput[i][j][0]; 
          float l= varOutput[i][j][1]; 
          float a= varOutput[i][j][2]; 
          float b= varOutput[i][j][3]; 
          float end_point = (a*l*l + b*l + R); 
          float vertex = (R - (b*b)/(4*a)); 
          //cout << end_point << endl; 
          //cout << vertex << endl; 
          if(max_outer_radius > end_point && end_point >= 0.0 && max_outer_radius > vertex && vertex >= 0.0){
            j= j;
          } 
          else{
		//cout << "Cannot create individual!" << endl;
            j= j-1;
          }
	// old outer radius check
	/*
	   if(varOutput[i][j][0] + (varOutput[i][j][1] * ( tan(varOutput[i][j][2]))) > max_outer_radius)
	  {
	    j = j-1;
	  }*/
	  
	}
    }
  // cout << "mutation finished flag" << endl;
}

void insertionSort(vector<float> & fitness, vector<vector<vector<float> > > & varInput)
{
	int i,j,x,y;
	for(i = 0; i < fitness.size(); i++)
	{
	  //cout << "index: " << i << " of "<< fitness.size() << endl;
		double temp = fitness[i];
		vector<vector<vector<float>>> location (1,vector<vector<float> >(NSECTIONS,vector <float>(NVARS, 0.0f)));
		//cout << " location vector established " << endl;
		for(int a = 0; a < NSECTIONS; a++)
		{
			for(int b = 0; b < NVARS; b++)
			{
				location[0][a][b] = varInput[i][a][b];
				
			}
			//	cout<< "saved paremeters" << endl;
		}
		for(j = i; j > 0 && fitness[j-1] < temp; j--)
		{
			fitness[j] = fitness[j-1];
			for(x = 0; x < NSECTIONS; x++)
			{
				for(y = 0; y < NVARS; y++)
					{
						varInput[j][x][y] = varInput[j-1][x][y];
					}
			}
		}
		fitness[j]=temp;
		//cout << "fitness Scores re-allocated" << endl;
		for(int a = 0; a < NSECTIONS; a++)
                {
                        for(int b = 0; b < NVARS; b++)
                        {
                                varInput[j][a][b] = location[0][a][b];
                        }
                }
		//cout << "Parameters re-allocated" << endl;
	}
	/*	for (int r=0; r < fitness.size(); r++)
	  {
	    cout << fitness[r]<< endl;
	    } */
}






