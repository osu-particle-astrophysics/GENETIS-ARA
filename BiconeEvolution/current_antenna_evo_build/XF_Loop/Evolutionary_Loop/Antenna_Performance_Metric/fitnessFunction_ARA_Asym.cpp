/*	fitnessFunction - ARA.cpp
	Written by David Liu
	Revised by Max Clowdus and Julie Rolla on 5 Dec 2018
	Revised by Suren Gourapura to accept NPOP as an argument on 6 Jan 2019

	This program reads data from XF and ouputs fitness scores to a file named fitnessScores.csv.
  	It reads the data from the AraSim output files from the Antenna_Performance_Metric folder, and outputs to file named fitnessScore.csv.

compile with: g++ -std=c++11 fitnessFunction_ARA_Asym.cpp -o fitnessFunction.exe
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
int NSEEDS; /* this is the number of parallel jobs we run, each with a different random number seed. */
int NSECTIONS = 2;
const int HEADER = 729; // Which line is Veff(ice) on from the AraSim output files?
const int NLINES = 737; // How many lines are there in the AraSim output files?
const double EPSILON = 0.000001; // Catches errors. If the first individual's fitness score is less than this value, we rerun the program.
double GeoScaleFactor; // factor by which we are scaling down our antennas

/* Here we declare our function headers. */

void Read(char* filename, ifstream& inputFile, string* araLineArray, vector<double> &fitnessScores, int individualCounter, double scaleFactor, double* antennaOuterRadii1, double* antennaOuterRadii2, int NSEEDS, vector<double> &vEffList, vector<double> &lowErrorBars, vector<double> &highErrorBars, double GeoScaleFactor);

void WriteFitnessScores(vector<double> fitnessScores, vector<double> vEffList, vector<double> lowErrorBars, vector<double> highErrorBars, int NPOP);

/** MAIN FUNCTION **/

int main(int argc, char** argv)
{
	/* Define NPOP first. Due to argv[1] being length NPOP+2, we just hope the user correctly inputs NPOP first, then the list of file names */
	int NPOP = atoi(argv[1]); // The atoi function converts from string to int
	int NSEEDS = atoi(argv[2]); 
	double scaleFactor = stod(argv[3]); // scaling variable for the exponent in the constraint. 
	char* antennaFile = argv[4];
	double GeoScaleFactor = stod(argv[5]); // Factor by which we scale down the antenna dimensions
	//Don't worry about the fact that the loop passes 6 arguments and this declares 5!
	//We call it later in the read function using argv[indivial+5] to read the input file
	// Quick variable declarations:
	vector<double> fitnessScores (NPOP, 0.0); // Stores our fitness scores for each individual.
	vector<double> vEffList (NPOP, 0.0); // Stores our vEff for each individual
	vector<double> lowErrorBars (NPOP, 0.0); // Stores our low error bar for each individual
	vector<double> highErrorBars (NPOP, 0.0); // Stores out high error bar for each individual
	string *araLineArray = NULL; // Stores the actual lines read in from the .txt files.
	ifstream inputFile; // Opens the .txt files.
	double* antennaRadii = new double[NPOP];
	double* antennaLengths1 = new double[NPOP];
	double* antennaThetas1 = new double[NPOP];
	double* antennaOuterRadii1 = new double[NPOP];
	double* antennaLengths2 = new double[NPOP];
	double* antennaThetas2 = new double[NPOP];
	double* antennaOuterRadii2 = new double[NPOP];
	vector<string> row;
	string item;

	cout << "Fitness function initialized." << endl;

	//cout << NPOP << endl << scaleFactor << endl << antennaFile << endl;
	
	if(argc != NPOP + 6) // +1 for fitnessFunction.exe, +1 for NPOP, +1 for NSEEDS, +1 for scaleFactor, +1 for fileName, +1 for GeoFactor
		cout << "Error, number of elements in argument of fitnessFunction.exe is incorrect! Specify NPOP first, then NSEEDS, then the scale factor, then the antenna file, then the Geoscale factor, then specify all AraSim output data files, preserving individual order." << endl << endl;
	
	else
	{
		double score = 0.0;
		
		ifstream antennaInput;
		antennaInput.open(antennaFile);
		if(!antennaInput.is_open()){
		  cout << endl << "Error!" << antennaFile << "could not be opened" << endl;
		}
		string currentLine = "Blank";
		while(currentLine.substr(0,8).compare("Matrices")!=0){
		getline(antennaInput, currentLine);
		}
		for(int i = 0; i < NPOP * NSECTIONS; i+=NSECTIONS){
			for (int j = 0; j < NSECTIONS; j++) {
		  		row.clear();
		  		getline(antennaInput, currentLine);
		  		//antennaRadii[i] = stod(currentLine.substr(0,currentLine.find(",")));
		  		//cout << antennaRadii[i] << endl;

		  		stringstream lineStream(currentLine); 

		  		while (getline(lineStream, item, ',')) { 
		    			row.push_back(item);
		    
		  		}
		  		cout << "R: " << row[0] << " L: " << row[1] << " T: " << row[2] << endl;
		  		
		  		antennaRadii[i / NSECTIONS] = stod(row[0]);
				if (j == 0) {
		  			antennaLengths1[i / NSECTIONS] = stod(row[1]);
		  			antennaThetas1[i / NSECTIONS] = stod(row[2]);
		  			antennaOuterRadii1[i / NSECTIONS] = antennaRadii[i / NSECTIONS] + (antennaLengths1[i / NSECTIONS]*tan(antennaThetas1[i / NSECTIONS]));
		  			cout << "Calculated Outer Radii: " << antennaOuterRadii1[i / NSECTIONS] << endl;
				}
				else if (j == 1) {
					antennaLengths2[i / NSECTIONS] = stod(row[1]);
					antennaThetas2[i / NSECTIONS] = stod(row[2]);
					antennaOuterRadii2[i / NSECTIONS] = antennaRadii[i / NSECTIONS] + (antennaLengths2[i / NSECTIONS]*tan(antennaThetas2[i / NSECTIONS]));
					cout << "Calculated Outer Radii: " << antennaOuterRadii2[i / NSECTIONS];
				}
			}
		}
		
			for(int individualCounter = 1; individualCounter <= NPOP; individualCounter++)
			{
				araLineArray = new string[NLINES];
				/* Note the +1 on argv below. This along with the counter starting at 1 starts argv at 2, the third element of argv. Again, the first element is fitnessFunction.exe and the second elemenet is NPOP.*/
				cout << "Entering Read" << endl;
				Read(argv[individualCounter+5], inputFile, araLineArray, fitnessScores, individualCounter, scaleFactor, antennaOuterRadii1, antennaOuterRadii2, NSEEDS, vEffList, lowErrorBars, highErrorBars, GeoScaleFactor);
				cout << "Data successfully read. Data successfully written." << endl;
				delete [] araLineArray;
				araLineArray = NULL;
			}
			
			WriteFitnessScores(fitnessScores, vEffList, lowErrorBars, highErrorBars, NPOP);
	cout << "Fitness scores successfully written." << endl << "Fitness function concluded." << endl;
	}

	delete[] antennaRadii;
	delete[] antennaLengths1;
	delete[] antennaThetas1;
	delete[] antennaOuterRadii1;
	delete[] antennaLengths2;
	delete[] antennaThetas2;
	delete[] antennaOuterRadii2;

	return (0);
}

// Subroutines:

void Read(char* filename, ifstream& inputFile, string* araLineArray, vector<double> &fitnessScores, int individualCounter, double scaleFactor, double* antennaOuterRadii1, double* antennaOuterRadii2, int NSEEDS, vector<double> &vEffList, vector<double> &lowErrorBars, vector<double> &highErrorBars, double GeoScaleFactor)
{
	string txt = filename;
	//double GeoScaleFactor = 2; // Scales the geometric constraint 
	// ^moved to be defined in the main function
	double RadiusConstraint = 7.5; //7.5; // Size fo the borehole, so fitness scores of antennas above this are reduced

	double sumvEff=0.;
	double sumSquareLowError=0.;
	double sumSquareHighError=0.;
	
	for (int iseed=1;iseed<=NSEEDS;iseed++) {
	  
	  string thistxt = txt.substr(0,txt.length()-4);

	  thistxt = thistxt + "_" + to_string(iseed) + ".txt"; // should be of the form AraOut_$gen_$individual_$seed.txt

	  inputFile.open(thistxt.c_str());
	  
	  // Error message if we can't open the .uan files from XF.
	  if(!inputFile.is_open())
	    {
	      cout << endl << "Error! " << filename << " could not be opened!" << endl;
	    }
	  
	  // Read the data.
	  else
	    {
	      string currentLine="Empty"; // Stores the current line we're reading
	      int commaToken=0; // Stores the comma separating m^3 and km^3
	      int spaceToken=0; // Stores the space separating km^3 from units
	      int colonToken=0;
	      //double vEff = 0 Stores the string form of the effective volume
	      int lineNumber = 0;
	      getline(inputFile,currentLine);
	      while (currentLine.length() < 15 ||currentLine.substr(0, 13).compare("test Veff(ice")){
		getline(inputFile,currentLine);
	      }
	      
	      commaToken=currentLine.find(",");
	      
	      spaceToken=currentLine.find(" ",commaToken+2);
	      string thisvEff = currentLine.substr(commaToken + 2, (currentLine.substr(commaToken+2).find(" ")));
	      cout << thisvEff << endl;
	      sumvEff+=stod(thisvEff);
	      
	      
	      // while (currentLine.length() < 15 ||currentLine.substr(0, 17).compare("And Veff(water eq.")){
	      //cout << currentLine.substr(0,19) << endl;
		getline(inputFile,currentLine);
		getline(inputFile,currentLine);
		//	cout << "Stuck in new while loop" << endl;
		//  }

	      colonToken = currentLine.find(":");
	      spaceToken = currentLine.find(" ",colonToken+2);
	      string lowErrorBar = currentLine.substr(colonToken+2, spaceToken);
	      sumSquareLowError += (stod(lowErrorBar)*stod(lowErrorBar)); 
	      
	      colonToken = currentLine.find(":", colonToken+2);
	      spaceToken = currentLine.find(" ",colonToken+2);
	      string highErrorBar = currentLine.substr(colonToken+2, spaceToken);
	      sumSquareHighError += (stod(highErrorBar)*stod(highErrorBar)); 
	      
      
	      inputFile.close();
	      inputFile.clear();
	      
	    } // if the file is there
	} // end loop over seeds
        double vEff=sumvEff/(double)NSEEDS;
	cout <<"Veff: " <<  vEff << endl;
	vEffList[individualCounter-1] = vEff;
	cout << "Constraint value is: " << (RadiusConstraint/GeoScaleFactor) << endl;
	cout << "Outer Radius 1 is: " << antennaOuterRadii1[individualCounter-1] << endl;
	cout << "Outer Radius 2 is: " << antennaOuterRadii2[individualCounter-1] << endl;
	if(antennaOuterRadii1[individualCounter-1] >= (RadiusConstraint/GeoScaleFactor) || antennaOuterRadii2[individualCounter-1] >= (RadiusConstraint/GeoScaleFactor)){
	  cout<< "Constraint Applied" << endl;
		if (antennaOuterRadii1[individualCounter-1] >= antennaOuterRadii2[individualCounter-1]) {
	  		fitnessScores[individualCounter-1] = (vEff)*exp(-pow(scaleFactor*(antennaOuterRadii1[individualCounter-1]-(RadiusConstraint/GeoScaleFactor))/(1),2));
	  		cout << "Factor Applied: " << exp(-pow(scaleFactor*(antennaOuterRadii1[individualCounter-1]-(RadiusConstraint/GeoScaleFactor))/(1),2)) << endl;
	  		cout << "FS reduced to: " << fitnessScores[individualCounter-1] << endl;
		}
		else if (antennaOuterRadii2[individualCounter-1] > antennaOuterRadii1[individualCounter-1]) {
		fitnessScores[individualCounter-1] = (vEff)*exp(-pow(scaleFactor*(antennaOuterRadii2[individualCounter-1]-(RadiusConstraint/GeoScaleFactor))/(1),2));
	  		cout << "Factor Applied: " << exp(-pow(scaleFactor*(antennaOuterRadii2[individualCounter-1]-(RadiusConstraint/GeoScaleFactor))/(1),2)) << endl;
	  		cout << "FS reduced to: " << fitnessScores[individualCounter-1] << endl;
		}
	}
	else{
	  cout << "Constraint Not Applied" << endl;
	  fitnessScores[individualCounter-1] = (vEff);
	  cout << "FS should equal Veff: " << fitnessScores[individualCounter-1] << endl;
	}

	double lowErrorBar = sqrt(sumSquareLowError)/(double)NSEEDS;
	double highErrorBar = sqrt(sumSquareHighError)/(double)NSEEDS;

	lowErrorBars[individualCounter-1]=lowErrorBar;
	highErrorBars[individualCounter-1]=highErrorBar;

}

void WriteFitnessScores(vector<double> fitnessScores, vector<double> vEffList, vector<double> lowErrorBars, vector<double> highErrorBars, int NPOP)
{
	ofstream fitnessFile;
	fitnessFile.open("fitnessScores.csv");
	fitnessFile << "The Ohio State University GENETIS Data." << endl;
	fitnessFile << "Current generation's fitness scores:" << endl;
	cout << fitnessScores[0] << endl;
	
	for(int i=0; i<NPOP; i++)
	{
		fitnessFile << fitnessScores[i] << endl;
	}
	fitnessFile.close();

	ofstream vEffFile;
	vEffFile.open("vEffectives.csv");
	vEffFile << "The Ohio State University GENETIS Data." << endl;
	vEffFile << "Current generation's vEff scores:" << endl;
	
	
	for(int i=0; i<NPOP; i++)
	{
		vEffFile << vEffList[i] << endl;
	}
	vEffFile.close();


	ofstream errorFile;
	errorFile.open("errorBars.csv");
	errorFile << "The Ohio State University GENETIS Data." << endl;
	errorFile << "Current generation's errorBar scores:" << endl;
	
	
	for(int i=0; i<NPOP; i++)
	{
	  errorFile << lowErrorBars[i] << "," << highErrorBars[i] << endl;
	}
	errorFile.close();
}
