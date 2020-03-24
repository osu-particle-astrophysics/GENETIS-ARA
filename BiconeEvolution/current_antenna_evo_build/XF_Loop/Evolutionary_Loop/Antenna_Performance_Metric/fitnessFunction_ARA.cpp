/*	fitnessFunction - ARA.cpp
	Written by David Liu
	Revised by Max Clowdus and Julie Rolla on 5 Dec 2018
	Revised by Suren Gourapura to accept NPOP as an argument on 6 Jan 2019

	This program reads data from XF and ouputs fitness scores to a file named fitnessScores.csv.
  	It reads the data from the AraSim output files from the Antenna_Performance_Metric folder, and outputs to file named fitnessScore.csv.
 */
 
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/* Here we declare our global variables. */

int NPOP; /* This global constant represents how many individuals there are per generation. It's value is determined by the user, in argv[1]. It cannot be cast as a constant, because the user's value of NPOP has to be defined inside int main. BE VERY CAREFUL NOT TO REDEFINE NPOP SOMEWHERE IN THE CODE! */
const int HEADER = 729; // Which line is Veff(ice) on from the AraSim output files?
const int NLINES = 737; // How many lines are there in the AraSim output files?
const double EPSILON = 0.000001; // Catches errors. If the first individual's fitness score is less than this value, we rerun the program.

/* Here we declare our function headers. */

void Read(char* filename, ifstream& inputFile, string* araLineArray, vector<double> &fitnessScores, int individualCounter, double scaleFactor, double* antennaOuterRadii);

void WriteFitnessScores(vector<double> fitnessScores);

/** MAIN FUNCTION **/

int main(int argc, char** argv)
{
	/* Define NPOP first. Due to argv[1] being length NPOP+2, we just hope the user correctly inputs NPOP first, then the list of file names */
	NPOP = atoi(argv[1]); // The atoi function converts from string to int
	double scaleFactor = stod(argv[2]);
	char* antennaFile = argv[3];
	//Don't worry about the fact that the loop passes 4 arguments and this declares 3!
	//We call it later in the read function using argv[indivial+3] to read the input file
	// Quick variable declarations:
	vector<double> fitnessScores (NPOP, 0.0); // Stores our fitness scores for each individual.
	string *araLineArray = NULL; // Stores the actual lines read in from the .txt files.
	ifstream inputFile; // Opens the .txt files.
	double* antennaRadii = new double[NPOP];
	double* antennaLengths = new double[NPOP];
	double* antennaThetas = new double[NPOP];
	double* antennaOuterRadii = new double[NPOP];
	vector<string> row;
	string item;

	cout << "Fitness function initialized." << endl;

	//cout << NPOP << endl << scaleFactor << endl << antennaFile << endl;
	
	if(argc != NPOP + 4) // +1 for fitnessFunction.exe, +1 for NPOP. +1 for scaleFactor +1 for fileName
		cout << "Error, number of elements in argument of fitnessFunction.exe is incorrect! Specify NPOP first, then the scale factor, then specify all AraSim output data files, preserving individual order." << endl << endl;
	
	else
	{
		double score = 0.0;
		
		ifstream antennaInput;
		antennaInput.open(antennaFile);
		if(!antennaInput.is_open()){
		  cout<< endl << "Error!" << antennaFile << "could not be opened" << endl;
		}
		string currentLine = "Blank";
		while(currentLine.substr(0,8).compare("Matrices")!=0){
		getline(antennaInput, currentLine);
		}
		for(int i = 0; i <NPOP; i++){
		  row.clear();
		  getline(antennaInput, currentLine);
		  //antennaRadii[i] = stod(currentLine.substr(0,currentLine.find(",")));
		  //cout << antennaRadii[i] << endl;

		  stringstream lineStream(currentLine); 

		  while (getline(lineStream, item, ',')) { 
		    row.push_back(item);
		  }
		  cout << "R: " << row[0] << " L: " << row[1] << " Theta: " << row[2] << endl;
		  
		  antennaRadii[i] = stod(row[0]);
		  antennaLengths[i] = stod(row[1]);
		  antennaThetas[i] = stod(row[2]);
		  antennaOuterRadii[i] = antennaRadii[i] + (antennaLengths[i]*tan(antennaThetas[i]));
		  cout << "Calculated Outer Radii: " << antennaOuterRadii[i] << endl;

		}

			for(int individualCounter = 1; individualCounter <= NPOP; individualCounter++)
			{
				araLineArray = new string[NLINES];
				/* Note the +1 on argv below. This along with the counter starting at 1 starts argv at 2, the third element of argv. Again, the first element is fitnessFunction.exe and the second elemenet is NPOP.*/
				Read(argv[individualCounter+3], inputFile, araLineArray, fitnessScores, individualCounter, scaleFactor, antennaOuterRadii);
				cout << "Data successfully read. Data successfully written." << endl;
				delete [] araLineArray;
				araLineArray = NULL;
			}
			
	WriteFitnessScores(fitnessScores);
	cout << "Fitness scores successfully written." << endl << "Fitness function concluded." << endl;
	}
	delete[] antennaRadii;
	delete[] antennaLengths;
	delete[] antennaThetas;
	delete[] antennaOuterRadii;
	return (0);
}

// Subroutines:

void Read(char* filename, ifstream& inputFile, string* araLineArray, vector<double> &fitnessScores, int individualCounter, double scaleFactor, double* antennaOuterRadii)
{
	string txt = filename;
	inputFile.open(txt.c_str());
	
	// Error message if we can't open the .uan files from XF.
	if(!inputFile.is_open())
	{
		cout << endl << "Error! " << filename << ".txt could not be opened!" << endl;
	}
	
	// Read the data.
	else
	{
		string currentLine="Empty"; // Stores the current line we're reading
		int commaToken=0; // Stores the comma separating m^3 and km^3
		int spaceToken=0; // Stores the space separating km^3 from units
		string vEff="0"; // Stores the string form of the effective volume
		int lineNumber = 0;
		getline(inputFile,currentLine);
		while (currentLine.length() < 15 ||currentLine.substr(0, 13).compare("test Veff(ice")){
				getline(inputFile,currentLine);
			}
		
		commaToken=currentLine.find(",");
		spaceToken=currentLine.find(" ",commaToken+2);
		vEff = currentLine.substr(commaToken + 2, (spaceToken-commaToken-1));
		cout << vEff << endl;

		if(antennaOuterRadii[individualCounter-1] >= 12.7){
		  fitnessScores[individualCounter-1] = stod(vEff)*exp(-pow(scaleFactor*(antennaOuterRadii[individualCounter-1]-12.7)/12.7,2));
		  }else{
		    fitnessScores[individualCounter-1] = stod(vEff);
		  }
		inputFile.close();
		inputFile.clear();
	}
}

void WriteFitnessScores(vector<double> fitnessScores)
{
	ofstream fitnessFile;
	fitnessFile.open("fitnessScores.csv");
	fitnessFile << "The Ohio State University GENETIS Data." << endl;
	fitnessFile << "Current generation's fitness scores:" << endl;
	
	for(int i=0; i<NPOP; i++)
	{
		fitnessFile << fitnessScores[i] << endl;
	}
	fitnessFile.close();
}
