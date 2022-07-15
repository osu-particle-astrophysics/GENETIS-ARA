/*	This GA is adapted from CalPoly's hybrid roulette / tournament method to work with Ohio State's loop. 
	Written by David Liu
	Revised by Suren Gourapura to accept NPoP on 29 Dec 2018
	
	Everything is carefully commented, so things should hopefully be clear.
	
*/

// Compile using: g++ -std=c++11 roulette_algorithm_cut_test.cpp -o roulette_algorithm.exe
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
void roulette(vector<vector<vector<float>>> &varInput, vector<vector<vector<float>>> &varOutput, vector<float> &fitness, default_random_engine& random_sequence);

/* 	Inputs for function tournament:
	varInput = variables we read from previous generation
	varOutput = variables we generate for next generation
	parents = tensor to store the variables used to generate an individual
	fitness = fitness scores
*/


void tournament(vector<vector<vector<float> > > &varInput, vector<vector<vector<float> > > &varOutput, vector<float> &fitness, default_random_engine& random_sequence);

// Debolt, Patton, Sipe: New modular functions 10/9/2020
int new_tournement(vector<float> fitness, int pool_size);
// Runs a tournament selecton and returns location of individual selected

int new_roulette(vector<float> fitness);
// Runs a roulette selection and returns location of the selected individual

void reproduction(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, int reproduction_no, int pool_size);
// Calls the selection functions to choose a set amount of individuals to be passed into the next generation

void crossover(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, int crossover_no, int pool_size, int reproduction_no);
// Calls the selection functions to create parents and then generates two children per pair of parents that get passed into the next generation

void mutation(vector<vector<vector<float> > > & varOutput, int reproduction_no, int crossover_no, float max_length, float max_radius, float max_sepration, float max_outer_radius);
// Introduces new variables into the next generation

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

const int NVARS = 3; // This global constant controls the number of genes we are working with. This is equal to the number of variables per section (e.g. length, radius, angle, coordinates, etc)

const int PARENT_NO = 2; // This global constant controls the number of potential parents per child. If 1, each child will be a clone of the parent.

const int DNA_GARBAGE_END = 9; // This global constant defines where in generationDNA.csv data begins. Data should begin on the NEXT line after this; so if 8, the first line of data is on line 9.

// ALGORITHM RELATED CONSTANTS

int NPOP; /* This global constant represents how many individuals there are per generation. It's value is determined by the user, in argv[2]. It cannot be cast as a constant, because the user's value of NPOP has to be defined inside int main. BE VERY CAREFUL NOT TO REDEFINE NPOP SOMEWHERE IN THE CODE! */

const float MUTABILITY  = 0.22f; // This global constant defines overall mutability. Every generation will have this percentage of their offspring mutated.

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

const float TOURNEY_PROPORTION = 0.8f; // This value represents what percentage of the children generated use tournament-style selection. Must be 0 < x < 1.

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

float min_length = 10.0; // in cm
float max_length = 140;  // in cm

float max_theta = atan(max_outer_radius/min_length);

float max_seperation = 2.5;
float min_seperation = 2.5;  

const float reproduction_percent = .16;
const float crossover_percent = .72;

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
	default_random_engine generator;
	generator.seed(time(0));
	// generator.seed(1); // for debugging
	srand((unsigned)time(0)); // Let's just seed our random number generator off the bat (moved up from below)
  //srand(1); // for debugging
  // I'm going to try recording all of the generator values from these runs to look for patterns
  // First, I need to make a file to write to
  ofstream generator_file;
	generator_file.open("Generation_Data/generators.csv");
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
	int reproduction_no = reproduction_percent*varOutput.size();
	int crossover_no = crossover_percent*varOutput.size();
	while(crossover_no%2 !=0)
	  {
	    crossover_no = crossover_no+1;
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
	
    	if(argc != 4)
        {cout << "Error: Usage. Specify start or cont, as well as NPOP (EX: start 10)." << endl;}
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

			// I'm modifying this so that we start with 50 identical individuals
			// This should help us demonstrate that we are in fact evolving
			std::normal_distribution <float> distribution_radius(INITIAL_MEAN_C1_G1, INITIAL_STD_DVN_C1_G1);
			std::normal_distribution <float> distribution_length(INITIAL_MEAN_C1_G2, INITIAL_STD_DVN_C1_G2);
			std::normal_distribution <float> distribution_angle(INITIAL_MEAN_C1_G3, INITIAL_STD_DVN_C1_G3);

			float limit = 1000.0; // making this crazy high to take out cutoff
			for(int i=0;i<NPOP;i++)
			{
				for(int j=0;j<NSECTIONS;j++)
				{
					for(int k=0;k<NVARS;k++)
					{
						if (k == 0)
						{
							//float r = distribution_radius(generator);
							float r;
							if(j == 0)
							{
								r = 2.08711;
							}
							else if (j == 1)
							{
								r = 0.30175;
							}
								// write generator to a file
								generator_file << generator << endl;	
							while(r<=0) // We don't accept negative or zero values
							{
								//r = distribution_radius(generator);
								//r = 1.70596;
								if(j == 0)
              	{
                	r = 2.08711;
              	}
             		else if (j == 1)
              	{
                	r = 0.30175;
              	}
								generator_file << generator << endl;
							}
							varOutput[i][j][k]= r;
						}
					
						// EDIT LOCATION FOR CUT
						else if (k == 1)
						{
							//float l = distribution_length(generator);
							float l;
							if(j == 0)
              {
                l = 89.924;
              }
              else if (j == 1)
              {
                l = 45.3616;
              }
							//float l = 121.371;
							generator_file << generator << endl;
							while(l<10.0) // now we don't accept below 37.5 cm
							{
								//l = distribution_length(generator);
								//l = 121.371;
								if(j == 0)
              	{
                	l = 89.924;
              	}
              	else if (j == 1)
              	{
                	l = 45.3616;
              	}
								generator_file << generator << endl;
							}
						
							varOutput[i][j][k]= l;
						}	

						else if (k == 2)
						{
							//float a = distribution_angle(generator);
							float a;
							if(j == 0)
              {
              	a = 0.0161734;
              }
              else if (j == 1)
              {
              	a = 0.0910478;
              }
							//float a = 0.034273;
							generator_file << generator << endl;	
							while(a<0.0) // We don't accept negative values
							{
								//a = distribution_angle(generator);
								//a = 0.034273;
								if(j == 0)
              	{
                	a = 0.0161734;
              	}
              	else if (j == 1)
              	{
                	a = 0.0910478;
              	}
								generator_file << generator << endl;
							}
						varOutput[i][j][k]= a;
						}	
					}
					if(varOutput[i][j][0]+varOutput[i][j][1]*tan(varOutput[i][j][2]) >= max_outer_radius)
					  {
					    j = j-1;
					  }
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
			/*ofstream datasize;
			datasize.open("datasize.txt");
			datasize << meanForGridSize/50.0 << ";";
			datasize.close();*/	
		}
		else if(string(argv[1]) == "cont")
		{
			  dataRead(varInput,fitness); // Read in the stuff from previous generation
			  if (checkConvergence(varInput,fitness) == 2) // We check for convergence. If we've converged then end loop
			  {
				  remove("Generation_Data/highfive.txt"); // we delete the old highfive.txt that has a 0 in it
				  ofstream highfive;
				  highfive.open("Generation_Data/highfive.txt"); // we create a new highfive.txt that will have a 1 in it
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
			    const float ROULETTE_PROPORTION = 1.0f-TOURNEY_PROPORTION; 

			    reproduction(varInput, varOutput, fitness, ROULETTE_PROPORTION, TOURNEY_PROPORTION, reproduction_no, pool_size);
			    crossover(varInput, varOutput, fitness, ROULETTE_PROPORTION, TOURNEY_PROPORTION, crossover_no, pool_size, reproduction_no);
			    mutation(varOutput, reproduction_no, crossover_no, max_length, max_radius, max_seperation, max_outer_radius);

			    cout << NSECTIONS << endl;
			    dataWrite(NPOP, varOutput, freq_coeffs, freqVector);
			    double meanTotal = 0.0;
			    for(int i=0; i<NPOP; i++)
			      {
				meanTotal = meanTotal + varOutput[i][0][1];
			      }
			    float meanForGridSize = meanTotal / NPOP;
			    /*ofstream datasize;
			    datasize.open("datasize.txt");
			    datasize << meanForGridSize/50.0 << ";";
			    datasize.close();
			    */
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
  generationDNA.open("Generation_Data/generationDNA.csv");
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
		  generationDNA << varVector[i][j][k] << "," << 0.025 << "\n";
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

void roulette(vector<vector<vector<float>>> &varInput, vector<vector<vector<float>>> &varOutput, vector<float> &fitness, default_random_engine& random_sequence)
{
	/* Roulette selection works as follows:
		Every parent in the population has a chance of having their genes selected. Each gene is selected by the probability of:
		
			     fitness
		p = -------------------
		    sum of all fitness
		
		Which is to say higher fitness score parents are more likely to have their genes selected, but all genes have a chance of
		being expressed. We use a partial sum to select to guarantee that at least one gene will be selected by the end.
		So currently as written we do it this way:
		
		p1 = f1 / F
		p2 = (f1 + f2) / F
		p3 = (f1 + f2 + f3) / F
		...
		p10 = F / F
		
		We then create roul_no * MUTABILITY mutant offspring; which is to say we run through and randomly change genes by a small amount.
		I currently am selecting two parents through roulette wheel selection for every 
	*/
	
	float fitness_total=0;
	int roulette_no=NPOP*(1-TOURNEY_PROPORTION);
	int tourney_no=NPOP*TOURNEY_PROPORTION;
	
	// Sanity check to make sure we have NPOP offspring from using proportions. Just in case someone does something weird with like 47 individuals
	// Per generation and a tourney proportion of like 31%.
	
	while (roulette_no+tourney_no!=NPOP)
	{
		if (roulette_no+tourney_no>NPOP && roulette_no>tourney_no)
		{
			roulette_no+=-1;
		}
		else if (roulette_no+tourney_no>NPOP && roulette_no<tourney_no)
		{
			tourney_no+=-1;
		}
		else if (roulette_no+tourney_no<NPOP && roulette_no>tourney_no)
		{
			roulette_no+=1;
		}
		else if (roulette_no+tourney_no<NPOP && roulette_no<tourney_no)
		{
			tourney_no+=1;
		}
	}
			
	vector<vector<int>> selected (roulette_no,vector<int>(PARENT_NO,0));
	
	// Calculate total fitness for our probabilities.
	for(int i=0;i<NPOP;i++)

	{
		fitness_total+=fitness[i];
	}
	
	/** Roulette Wheel Selection **/
	/*
		How these loops work:
		i loop runs through and makes the entire loop run for each individual offspring
		in other words, the i loop generates parents for the ith offpsring
		
		j loop runs through and runs the parent selection loop until we have enough parents for each offspring
		in other words, the j loop generates j parents for the ith offspring
		
		k loop runs through and actually selects parents. It does this by taking the partial sum of each successive parent in
		the fitness vector. Then it looks how what percentage of the total sum the partial sum is, and comparing that percentage to
		the random number 1-100 generated earlier. If the partial sum is greater than the random number previously generated,
		it records the location of which parent's fitness score was added to exceed the partial sum barrier
		(i.e. if it had to add 7 partial sums together, it records 6, since vectors record locations 0-9, so the 7th parent is at location [6])
		It then breaks the loop and returns to the j loop
		
		The j loop then records the parent's location in a matrix. Each row in our matrix represents a particular offspring,
		and each column represents parents. So the data structure for the selected matrix is as follows:
		
		              PARENT LOCATION    PARENT LOCATION    PARENT LOCATION    ...    PARENT LOCATION    
		OFFSPRING 1        count              count              count                     count
		OFFSPRING 2        count              count              count                     count
		OFFSPRING 3        count              count              count                     count
		...                ...                ...                ...                       ...
		OFFSPRING X        count              count              count                     count
		
		Incidentally, this matrix is of size roulette_no x PARENT_NO, which is currently 5x2.
		But if we increase the number of offspring, we increase rows
		And if we increase the number of parents, we increase columns.
	*/

	// EDIT 8/6/20: Amy wants me to make it possible to record the parents of each individual
	// this might be tricky if we turn on tournament mode, but for now it should be ok
	// (search the date above to get back to this comment)
	// I'll start by opening a file to print to

	ofstream parent_file;
	parent_file.open("Generation_Data/parents.csv");

	for(int i=0;i<roulette_no;i++)  // run for however many kids we are generating
	{
		for(int j=0;j<PARENT_NO;j++)  // run for however many parents per kid
		{
			float partial_sum=0.0f; // reset the partial sum to 0 every time we look for a new parent
			float r=rand()%100; // we randomly select a new chance that must be met every time we find a parent
			
			for(int k=0;k<NPOP;k++) // running through the possible parents (10)
			{
				partial_sum = partial_sum + fitness[k];
				if(r<(100*partial_sum/fitness_total)) // if our randomly selected chance is less than the partial sum / total sum * 100, we have a parent
				{
					selected[i][j]=k; // assign parent number j for kid number i
					parent_file << "Child " << i << " has parent " << k << " from the previous generation."
					<< endl;
					break;
				}
			}
		}
	}

	parent_file.close();

	// Now we actually use the parents and begin breeding offpsring
	// These are just crossbreeds without any mutations yet.
	/* We use uniform crossover rather than single or double point selection. What this means is:
	
	When we do crossover, we randomly select genes from either parent. So each gene has a 50% chance
	from each parent to be selected. Because every gene can be exchanged randomly, instead of like in
	single / double point selection where long strands of chromosomes are exchanged, we search the entire
	solution space much more quickly and thoroughly than we would with traditional genetic crossover.
	
	Incidentally, we also don't do what most GAs do with generating two children per two parents (example here):
	Child 1: P1G1 P2G2 P2G3 P2G4 P1G5 P1G6
	Child 2: P2G1 P1G1 P1G3 P1G4 P2G5 P2G6
	
	Where each child has the gene from the opposite parent as her sibling, to increase genetic diversity
	and give each parent from the previous generation more of a possibility of being a parent. This further
	improves the thoroughness of the solution space search.
	*/
	
	// I also want to get the genes from the parents for each individual
	// I'll put those in another file called genes.csv
	ofstream gene_file;
	gene_file.open("Generation_Data/genes.csv");

	for(int i=0;i<roulette_no;i++)
	{
		for(int j=0;j<NSECTIONS;j++)
		{
			for(int k=0;k<NVARS;k++)
			{
				int pick1=rand()%PARENT_NO; // generates a random integer number between 0 and number of parents
				//cout << "Parent " << pick1 << " selected with value " << varInput[selected[i][pick1]][j][k] << endl;
				varOutput[i][j][k]=varInput[selected[i][pick1]][j][k]; // This looks hella messy, but really all it's saying is go to randomly chosen parent and give the offspring the allele there.
				// I need to write to thegenes file what genes come from what parent
				gene_file << "Individual " << i << " has gene " << k << " from parent " << selected[i][pick1] << endl;
			}
		}
	}
	
	gene_file.close();

	// Now we have roulette_no offspring, but we're not done yet! We need to mutate them. To do that we first need to know
	// the average and the deviation of the parent generation. So let's go ahead and calculate that here:
	
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
	
	// Now we can do the mutations!
	// Guess what. I'm gonna wanna record these too!
	ofstream mutations_file;
	mutations_file.open("Generation_Data/mutations.csv");
	ofstream generator_file;
	generator_file.open("Generation_Data/generators.csv");
	vector<bool> mutate_flag (roulette_no,false); // Stores if a kid has already been exposed to mutagens. No need to mutate them further
	
	// Calculate how many mutants we need to generate
	
	int roul_mut = roulette_no * MUTABILITY;
	
	// commented out the below so we only declare the random sequence once (top of main())
	//default_random_engine generator;
	//generator.seed(time(0));
	//generator.seed(1);

	for(int i=0;i<roul_mut;i++)
	{ 
		int r = rand()%roulette_no; // Let's pick a random kid to expose to mutagens

		while(mutate_flag[r] == true) // If we already mutated that kid, pick again
		{
			r = rand()%roulette_no;
		}

		mutations_file << "Child " << r << " had "; 

		for(int j=0;j<NSECTIONS;j++)
		{
			int numberOfMutations = rand()%NVARS + 1; // We apply anywhere from 1 - NVARS mutations)
			
			mutations_file << numberOfMutations << " mutatations " << endl;

			for(int k=0; k < numberOfMutations; k++)
			{
				/* This section determines the magnitude of the mutation we apply and which gene to mutate */
				int chromosomeMutation = rand()%NSECTIONS; // We randomly select which chromosome to mutate
				int geneMutation = rand()%NVARS; // We randomly select which gene to mutate
				//default_random_engine generator;
				//generator.seed(time(0));
				//generator.seed(1);
				//generator_file << random_sequence << endl;
				std::normal_distribution <float> distribution(meanTensor[chromosomeMutation][geneMutation],dvnTensor[chromosomeMutation][geneMutation]);
				
				mutations_file << "	gene " << geneMutation << " of chromosome " << chromosomeMutation
				<< " mutated by ";

				
				/* This section determines whether or not the mutation adds or subtracts, and actually applies it */
				int coefficient=rand()%2;


				// COMMENTED OUT HERE
				/*
				if(coefficient==0)
				{
					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+(distribution(generator)/MUT_MODULATOR);  // divides by MUT_MODULATOR to modulate the effect so we don't get big mutations.
				}
				else
				{
					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]-(distribution(generator)/MUT_MODULATOR);  // divides by MUT_MODULATOR to modulate the effect so we don't get big mutations.
				}				
				while(varOutput[r][chromosomeMutation][geneMutation]<=0) // we really don't want negative values or zero values
				{
					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+distribution(generator);
				}
				*/

				// EDIT LOCATION FOR CUT HERE
				// COMMENTED OUT THE ABOVE BLOCK
				// If the length is less than 37.5 cm, we want to redo the mutation
				// I'm going to change this by a decent amount
				// Let's start by getting the amount of the mutation

				double mutation_amount = (distribution(random_sequence)/MUT_MODULATOR);
				generator_file << random_sequence << endl;
				// now we can do the same thing as in the above block

				// we want to make sure we don't mutate until we know the mutation is acceptable
				// in the case of the length, don't accept the mutation if it makes the length < 37.5 cm
				// for the angle and radius, don't accept it if they become < 0 cm
				// so we have an if statement to decide what the limiting value is

				double min_value = 0.0;

				if (geneMutation == 1)
				{
					min_value = 10.0; //37.5; // minimum length
				} 
				else
				{
					min_value = 0.0; // minimum radius/theta
				}

				// next, we will see what the mutation would be
				// if the mutation gives a value greater than min_value, we accept it
				// if not, we pick again before applying it
				// logically, this should only be necessary for when we subtract the mutation_amount
				// but since it comes from a gaussian, it may be negative, so do it for adding too


				if(coefficient==0)
				{

					// we're going to start by just not mutating an individual if it would go below the min
					if (varOutput[r][chromosomeMutation][geneMutation]+mutation_amount >= min_value)
					{
						varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+mutation_amount;

						mutations_file << mutation_amount << endl;
					}

					else
					{
						mutations_file << "(null due to minimum value)" << endl;
					}
				}

				else
				{

					// we're going to start by not mutating an individual if it would go below the min
					if (varOutput[r][chromosomeMutation][geneMutation]-mutation_amount >= min_value)
					{
						varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]-mutation_amount;

					mutations_file << (-1.0)*mutation_amount << endl;
					}

					else
					{
						mutations_file << "(null due to minimum value)" << endl;
					}
				}		
				mutate_flag[r]=true;
			}
		}
	}
	for(int v = 0; v < roulette_no; v++)
	{
		mutations_file << mutate_flag[v] << " ";
	}
	mutations_file.close();
	generator_file.close();
}


void tournament(vector<vector<vector<float>>> &varInput, vector<vector<vector<float>>> &varOutput, vector<float> &fitness, default_random_engine& random_sequence)
{
	/* Tournament selection works as follows:
		We draw a random lottery of individuals from the population and select the best.
		The non-select are then put back in the population, and we draw another random lottery.
		This goes on until we have enough parents.
		*/
	
	// Initialize arrays and integers we'll need to do this.
	int roulette_no=NPOP*(1-TOURNEY_PROPORTION);
	int tourney_no=NPOP*TOURNEY_PROPORTION;
	
	while (roulette_no+tourney_no!=NPOP)
	{
		if (roulette_no+tourney_no>NPOP && roulette_no>tourney_no)
		{
			roulette_no+=-1;
		}
		else if (roulette_no+tourney_no>NPOP && roulette_no<tourney_no)
		{
			tourney_no+=-1;
		}
		else if (roulette_no+tourney_no<NPOP && roulette_no>tourney_no)
		{
			roulette_no+=1;
		}
		else if (roulette_no+tourney_no<NPOP && roulette_no<tourney_no)
		{
			tourney_no+=1;
		}
	}
	
	vector<bool> selection_flag (NPOP, false);
	vector<vector<int>> selection_matrix (tourney_no,vector<int>(PARENT_NO,0));
	
	// Incidentally we'll need mean and std dvn for the mutations later anyway, so:
	vector<vector<float>> meanTensor (NSECTIONS,vector<float>(NVARS,0));
	vector<vector<float>> dvnTensor (NSECTIONS,vector<float>(NVARS,0));
	
	cout << "Tournament selection initialized." << endl;
	
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
	
	cout << "Statistics initialized." << endl;
	// Let's start selecting parents:
	
	vector<int> contenders (TOURNEY_LOTTERY_SIZE,0); // Array to store who we select
	vector<bool> contender_flag (NPOP, false);

	
	for(int i=0; i<tourney_no; i++)
	{
		for(int j=0; j<PARENT_NO;j++)
		{		
			int s = rand()%NPOP;
			contender_flag[s] = true;
			contenders[0] = s;
			for(int k=1; k<TOURNEY_LOTTERY_SIZE; k++)
			{
				s = rand()%NPOP;
				while(contender_flag[s] == true || selection_flag[s] == true)
					s = rand()%NPOP;
				contender_flag[s] = true;
				contenders[j] = s;
			}
			cout << "Individual " << i << " Parent " << j << " Pool selected" << endl;
			
			// Ok now we have a bunch of potential parents in the population. Now we sort through them and figure out who is the best.

			int most_fit = 0;
			for(int k=0; k<TOURNEY_LOTTERY_SIZE; k++)
			{
				if(fitness[contenders[i]]>fitness[most_fit])
					most_fit=i;
			}
			selection_matrix[i][j]=most_fit;
			selection_flag[most_fit] = true;
			
			cout << "Individual " << i << " Parent " << j << " selected." << endl;
			
			// We reset the flags for the entire parent generation EXCEPT the parents who have been selected to be parents
			
			for(int k=0; k<TOURNEY_LOTTERY_SIZE; k++)
			{
				contenders[k] = 0;
				contender_flag[k] = false;
				cout << "Contender flags reset." << endl;
			}
		}
		
		// Now we reset all flags for the entire parent generation since we're moving onto the next child
		
		for(int j=0; j<NPOP; j++)
		{
			selection_flag[j] = false;
		}
		cout << "Flags reset." << endl;
	}
	
	cout << "Tournament parents selected." << endl;	

	// Now that we have all the parents we need to generate our child generation, we'll go ahead and generate them here:
	
	for(int i=0; i<tourney_no; i++) // this loop generates for each child
	{
		for(int j=0; j<NSECTIONS; j++) // this loop changes which chromosome we're building
		{
			for(int k=0; k<NVARS; k++) // This loop changes which gene we're slotting in
			{
				int pick1=rand()%PARENT_NO; // generates a random integer number between 0 and number of parents
				varOutput[i+roulette_no][j][k]=varInput[selection_matrix[i][pick1]][j][k]; // This looks hella messy, but really all it's saying is go to randomly chosen parent and give the offspring the allele there.
			}
		}
	}
	
	// Finally, we will introduce mutations to the offspring at random. 
	
	vector<bool> mutate_flag (tourney_no,false); // Stores if a kid has already been exposed to mutagens. No need to mutate them further
	
	// Calculate how many offspring to mutate
	
	int tour_mut = tourney_no * MUTABILITY;
	

	// commented out the below so we only instantiate the random sequence once (top of main())
	//default_random_engine generator;
	//generator.seed(time(0));
	//generator.seed(1);

	for(int i=0;i<tour_mut;i++)
	{ 
		int r = rand()%tourney_no; // Let's pick a random kid to expose to mutagens
		
		while(mutate_flag[r] == true) // If we already mutated that kid, pick again
			r = rand()%tourney_no;
		for(int j=0;j<NSECTIONS;j++)
		{
			int numberOfMutations = rand()%NVARS; // We apply anywhere from 1 - NVARS mutations)
			if (numberOfMutations == 0)
			{
				numberOfMutations++;
			}
			for(int k=0; k < numberOfMutations; k++)
			{
				/* This section determines the magnitude of the mutation we apply and which gene to mutate */
				int chromosomeMutation = rand()%NSECTIONS; // We randomly select which chromosome to mutate
				int geneMutation = rand()%NVARS; // We randomly select which gene to mutate
				//std::default_random_engine generator;
				//generator.seed(time(0));
				//generator.seed(1);
				std::normal_distribution <float> distribution(meanTensor[chromosomeMutation][geneMutation],dvnTensor[chromosomeMutation][geneMutation]);
				
				/* This section determines whether or not the mutation adds or subtracts, and actually applies it */
				int coefficient=rand()%2;

				/*
				if(coefficient==0)
					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+distribution(generator)/MUT_MODULATOR;  // divides by MUT_MODULATOR to modulate the effect so we don't get big mutations.
				else if(coefficient==1)
					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]-distribution(generator)/MUT_MODULATOR;  // divides by MUT_MODULATOR to modulate the effect so we don't get big mutations.
				while(varOutput[r][chromosomeMutation][geneMutation]<=0) // we really don't want negative values or zero values
					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+distribution(generator);
				*/

				// EDIT LOCATION FOR CUT HERE
				// COMMENTED OUT THE ABOVE BLOCK
				// If the length is less than 37.5 cm, we want to redo the mutation
				// I'm going to change this by a decent amount
				// Let's start by getting the amount of the mutation

				double mutation_amount = (distribution(random_sequence)/MUT_MODULATOR);
				// now we can do the same thing as in the above block

				// we want to make sure we don't mutate until we know the mutation is acceptable
				// in the case of the length, don't accept the mutation if it makes the length < 37.5 cm
				// for the angle and radius, don't accept it if they become < 0 cm
				// so we have an if statement to decide what the limiting value is

				double min_value = 0.0;

				if (geneMutation == 1)
				{
					min_value = 10.0; // minimum length
				} 
				else
				{
					min_value = 0.0; // minimum radius/theta
				}

				// next, we will see what the mutation would be
				// if the mutation gives a value greater than min_value, we accept it
				// if not, we pick again before applying it
				// logically, this should only be necessary for when we subtract the mutation_amount
				// but since it comes from a gaussian, it may be negative, so do it for adding too

				if(coefficient==0)
				{

					// we're going to start by not mutating an individual if it would go below the min
					if (varOutput[r][chromosomeMutation][geneMutation]-mutation_amount >= min_value)
					{
						varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]-mutation_amount;
					}
					/*

					// we need to give a way for the while loop to check the mutation
					int check = 0; // this will be either 0 or 1

					// as long as the mutation gives something less than the minimum value, keep picking
					while (check != 1)
					{
						mutation_amount = (distribution(generator)/MUT_MODULATOR);
						if (varOutput[r][chromosomeMutation][geneMutation]+mutation_amount >= min_value)
						{
							check = 1;
						}
						cout << mutation_amount << endl;
						cout << varOutput[r][chromosomeMutation][geneMutation] << endl;
					}
					*/
					// once it exits this while loop, we know it has chosen a mutation that
					// yields a value for the gene which is above the minimum
					// so now we add it				

					//varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+mutation_amount;  
				}
				else
				{

					// we're going to start by not mutating an individual if it would go below the min
					if (varOutput[r][chromosomeMutation][geneMutation]-mutation_amount >= min_value)
					{
						varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+mutation_amount;
					}
					/*

					// we need to give a way for the while loop to check the mutation
					int check = 0; // this will be either 0 or 1

					// as long as the mutation gives something less than the minimum value, keep picking
					while (check != 1)
					{
						mutation_amount = (distribution(generator)/MUT_MODULATOR);
						if (varOutput[r][chromosomeMutation][geneMutation]-mutation_amount >= min_value)
						{
							check = 1;
						}
						cout << mutation_amount << endl;
						cout << varOutput[r][chromosomeMutation][geneMutation] << endl;
					}
					// once it exits this while loop, we know it has chosen a mutation that
					// yields a value for the gene which is above the minimum
					// so now we add it				

					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+mutation_amount;  
					*/
				}		


				// this next part has been made obsolete
				/*
				while(varOutput[r][chromosomeMutation][geneMutation]<=0) // we really don't want negative values or zero values
				{
					varOutput[r][chromosomeMutation][geneMutation]=varOutput[r][chromosomeMutation][geneMutation]+distribution(generator);
				}
				*/

				mutate_flag[r]=true;
			}
		}
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
      // place integer number in contenders vector
      contenders.push_back(random_num);
    }

  int max = 0;
  for(int j=0; j<pool_size; j++)
    {
     // compare fitness scores coresponding to the integers in contenders
      if(fitness[contenders[j]] > fitness[contenders[max]])
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
      

void reproduction(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, int reproduction_no, int pool_size)
{
  cout << "reproduction flag" << endl;
  //NEED: Current gen; next generation; roul_perecentage, tour_percentage, reproduction_no, fitness, pool_size
  int roul_no = roul_percentage*reproduction_no;
  int tour_no = tour_percentage*reproduction_no;

  if(reproduction_no == 1)
    {
      roul_no = 1; // this is because roulette will not be left to purely random chance for small population sizes
    }

  while(roul_no + tour_no < reproduction_no)
    {
      tour_no += 1;
    }

  if(roul_no > 0)
    {
      cout << "roullete flag"<< endl;
      int r_select = new_roulette(fitness);
      for(int i=0; i<roul_no; i++)
	{
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
      cout << "tournament flag"<< endl;
      int t_select = new_tournament(fitness, pool_size);
      for(int x=roul_no; x<tour_no+roul_no; x++)
	{
	  for(int y=0; y<NSECTIONS; y++)
	    {
	      for(int z=0; z<NVARS; z++)
		{
		  varOutput[x][y][z] = varInput[t_select][y][z];
		}
	    }
	}
      cout << "reproduction finished" << endl;
    }
}


void crossover(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, int crossover_no, int pool_size, int reproduction_no)
{
  cout << "crossover flag" << endl;
  // crossover_no contains the total number of individuals that are to be generated using this opperator
  int roul_no = roul_percentage * crossover_no;
  int tour_no = tour_percentage * crossover_no;
  vector<int> parents_loc;
  double swap;
  uniform_real_distribution<double> choice(0.0, 1.0);

  // generate the parents
for(int r=0; r<roul_no; r++)
{
  parents_loc.push_back(new_roulette(fitness));
}
for(int t=0; t<tour_no; t++)
{
  parents_loc.push_back(new_tournament(fitness, pool_size));
}

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
	    if(varOutput[i+reproduction_no][j][0]+(varOutput[i+reproduction_no][j][1]* (tan(varOutput[i+reproduction_no][j][2]))) > max_outer_radius || varOutput[i+1+reproduction_no][j][0]+(varOutput[i+1+reproduction_no][j][1]*(tan(varOutput[i+1+reproduction_no][j][2]))) > max_outer_radius)
	      {
		j= j-1;
	      }
      }
  }
}

					      
void mutation(vector<vector<vector<float> > > & varOutput, int reproduction_no, int crossover_no, float max_length, float max_radius, float max_seperation, float max_outer_radius)
{
  cout << "mutation flag" << endl;
  uniform_real_distribution<float> l_mut(min_length, max_length);
  uniform_real_distribution<float> r_mut(0, max_radius);
  uniform_real_distribution<float> a_mut(0, max_theta);
  uniform_real_distribution<float> s_mut(min_seperation, max_seperation);

  for(int i=reproduction_no+crossover_no; i< varOutput.size(); i++)
    {
      for(int j=0; j<NSECTIONS; j++)
	{
	  //mutation generators
	  varOutput[i][j][0] = r_mut(generator);
	  varOutput[i][j][1] = l_mut(generator);
	  varOutput[i][j][2] = a_mut(generator);
	  varOutput[i][j][3] = s_mut(generator);
	  
	  if(varOutput[i][j][0] + (varOutput[i][j][1] * ( tan(varOutput[i][j][2]))) > max_outer_radius)
	    {
	      j = j-1;
	    }
	  
	}
    }
  cout << "mutation finished flag" << endl;
}
