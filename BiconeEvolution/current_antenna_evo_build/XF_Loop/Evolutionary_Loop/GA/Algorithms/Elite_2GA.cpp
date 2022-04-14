/*	this GA is adapted from CalPoly's hybrid roulette / tournament method to work with Ohio State's loop. 
	Written by David Liu
	Revised by Suren Gourapura to accept NPoP on 29 Dec 2018
	
	Everything is carefully commented, so things should hopefully be clear.
	
*/

// Compile using: g++ -std=c++11 Curved_GA.cpp -o GA.exe

// call format: ./a.out start/cont NPOP 1 repro_no cross_no roulette_int tournament_int rank_int



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

// Debolt, Patton, Sipe: New modular functions 10/9/2020
int new_tournement(vector<float> fitness, int pool_size);
// Runs a tournament selecton and returns location of individual selected

int new_roulette(vector<float> fitness);
// Runs a roulette selection and returns location of the selected individual

int Rank(vector<float> fitness);

int Elite(vector<float> fitness); 

void reproduction(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int reproduction_no, int pool_size, int elite);
// Calls the selection functions to choose a set amount of individuals to be passed into the next generation

void crossover(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int crossover_no, int pool_size, int reproduction_no);
// Calls the selection functions to create parents and then generates two children per pair of parents that get passed into the next generation

void mutation(vector<vector<vector<float> > > & varOutput, int reproduction_no, int crossover_no, float max_length, float max_radius, float max_sepration, float max_outer_radius, float max_A, float max_B);
// Introduces new variables into the next generation

void insertionSort(vector<float> & fitness, vector<vector<vector<float> > > & varInput);

// Declare some global constants

//Debolt 10/7/2020:
// adding global random number generator
default_random_engine generator(time(NULL));

double MINIMUM_FREQUENCY = 0.08333; // This global constant defines the minimum frequency of the range we're working in, in GHz; Initial value is what AraSim uses
//MINIMUM_FREQUENCY = MINIMUM_FREQUENCY*MULTIPLIER_FACTOR

double MAXIMUM_FREQUENCY = 1.0667; // This global constant defines the maximum frequency of the range we're working in, in GHz; Initial value is what AraSim uses
//MAXIMUM_FREQUENCY = MAXIMUM_FREQUENCY*MULTIPLIER_FACTOR

double FREQ_STEP = 0.01667; // This global constant defines the step size between frequencies as we scan.
//FREQ_STEP = FREQ_STEP*MULTIPLIER_FACTOR



//	DNA RELATED CONSTANTS

const int NSECTIONS = 2;  // This global constant controls the number of chromosomes we are working with. This is equal to the number of sections in the antenna.

const int NVARS = 4; // This global constant controls the number of genes we are working with. This is equal to the number of variables per section (e.g. length, radius, angle, coordinates, etc)

const int DNA_GARBAGE_END = 9; // This global constant defines where in generationDNA.csv data begins. Data should begin on the NEXT line after this; so if 8, the first line of data is on line 9.

// ALGORITHM RELATED CONSTANTS

int NPOP; /* This global constant represents how many individuals there are per generation. It's value is determined by the user, in argv[2]. It cannot be cast as a constant, because the user's value of NPOP has to be defined inside int main. BE VERY CAREFUL NOT TO REDEFINE NPOP SOMEWHERE IN THE CODE! */

// new variables 10/9/2020 subject to change
float max_outer_radius = 7.5; // in cm

float max_radius = max_outer_radius;

float min_length = 37.5; // in cm
float max_length = 140;  // in cm

float max_theta = atan(max_outer_radius/min_length);

// just using the values from Leo/Eliot's script
float min_A = -1; //-1.0;//-0.008;
float max_A = 1; //1.0;//-0.002;

// how did they decide on these values?
float min_B = -1; //-1;//-0.1; 
float max_B = 1; //tan(max_theta); //1;//0.8; // They represent slopes (what are the axes?)

float max_seperation = 2.5;
float min_seperation = 2.5;  


//main function

int main(int argc, char const *argv[])
{
  // EDIT 8/8/20: we need to only instantiate the generator (and seed) once
  // needs to not be seeded Ryan and Kai 10/27/2020
  default_random_engine generator;
  generator.seed(time(NULL));
	// generator.seed(1); // for debugging
  srand(time(NULL)); // Let's just seed our random number generator off the bat (moved up from below)
  //srand(1); // for debugging
  // I'm going to try recording all of the generator values from these runs to look for patterns
  // First, I need to make a file to write to
  ofstream generator_file;
	generator_file.open("Generation_Data/generators.csv");
	generator_file << "First generator: " << endl << generator << endl;	


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
	
	//Define opperator and selection numbers

	int reproduction_no = atoi(argv[3]);
	//cout << reproduction_no << endl;
	int crossover_no = atoi(argv[4]);
	const float ROULETTE_PROPORTION =(atoi(argv[5]))/10.0;
	const float TOURNEY_PROPORTION = (atoi(argv[6]))/10.0;
	const float RANK_PROPORTION = (atoi(argv[7]))/10.0;
	const int elite = (atoi(argv[8]));

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
	  {cout << "Error: Usage. Specify start or cont, as well as NPOP (EX: start 10)." << endl << "Form: start/cont NPOP repro_no cross_no roul_no tour_no rank_no elite:1/0" << endl;} 
    	else
    	{
      		if(string(argv[1]) == "start")
		{
		  // seession with kai (Debolt 10/27/2020) normal dist instead of uniform???
		        std::uniform_real_distribution <float> distribution_radius(0, max_radius);   
			std::uniform_real_distribution <float> distribution_length(min_length, max_length); 
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
 
					// check if outer radius is too large
					if(a == 0.0 && max_outer_radius > end_point && end_point >=0.0)
					  {
					    intersect_condition =1; // No intersection
					  }
                                        else if( a != 0.0 && max_outer_radius > end_point && end_point >= 0.0 && max_outer_radius > vertex && vertex >= 0.0)
                                            {
                                                intersect_condition = 1; // No intersection
                                            }
                                        else{
                                                intersect_condition = 0; // Intersection
                                            }
                                        }	
				}
			}
			
	
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
			  reproduction(varInput, varOutput, fitness, ROULETTE_PROPORTION, TOURNEY_PROPORTION, RANK_PROPORTION, reproduction_no, pool_size, elite);
			  crossover(varInput, varOutput, fitness, ROULETTE_PROPORTION, TOURNEY_PROPORTION, RANK_PROPORTION, crossover_no, pool_size, reproduction_no);
			  mutation(varOutput, reproduction_no, crossover_no, max_length, max_radius, max_seperation, max_outer_radius, max_A, max_B);
			  //cout << NSECTIONS << endl;
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
	generator_file.close();
	return (0);
}
	
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

int Elite(vector<float> fitness)
{
  return(0);
}

void reproduction(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int reproduction_no, int pool_size, int elite)
{
  // cout << "reproduction flag" << endl;
  //NEED: Current gen; next generation; roul_perecentage, tour_percentage, reproduction_no, fitness, pool_size

  if(elite == 1)
    {
      reproduction_no = reproduction_no - elite;
      int E_select = Elite(fitness);
      for(int i= 0; i<NSECTIONS; i++)
	{
	  for(int j=0; j<NVARS; j++)
	    {
	      varOutput[0][i][j] = varInput[E_select][i][j];
	    }
	}
    }

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
      for(int i=elite; i<roul_no; i++)
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
       for(int r=tour_no+roul_no; r<reproduction_no + elite; r++)
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
  if(elite == 1)
    {
      reproduction_no = reproduction_no+elite; 
    }
  cout << "Roulette Finished" << endl;
}
void crossover(vector<vector<vector<float> > > & varInput, vector<vector<vector<float> > > & varOutput, vector<float> fitness, float roul_percentage, float tour_percentage, float rank_percentage, int crossover_no, int pool_size, int reproduction_no)
{
  //cout << "crossover flag" << endl;
  // crossover_no contains the total number of individuals that are to be generated using this opperator
  int roul_no = roul_percentage * crossover_no;
  int tour_no = tour_percentage * crossover_no;
  int rank_no = rank_percentage * crossover_no;
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
            // Check for both individuals!
	    if (a_1 == 0.0 &&  max_outer_radius > end_point_1 && end_point_1 >= 0.0 && a_2 == 0.0 && max_outer_radius > end_point_2 && end_point_2 >= 0.0)
	      {
		j=j;
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
 
            else
	      {
              j= j-1; 
	      }
      }
  }
 cout << "Crossover Finished" << endl;
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
	  if(a == 0.0 && max_outer_radius > end_point && end_point >= 0.0)
	    {
	      j=j;
	    }
          else if(a != 0.0 && max_outer_radius > end_point && end_point >= 0.0 && max_outer_radius > vertex && vertex >= 0.0)
	  {
            j= j;
          }
          else{
		//cout << "Cannot create individual!" << endl;
            j= j-1;
          }
	  
	}
    }
  cout << "mutation finished" << endl;
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
}






