//
//  failreading.cpp
//
//
//  Created by Alex Patton on 4/13/20.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


using namespace std;
void Read(char*,int,double[],double[],double[]);
int Write(char*,double[],double[],double[],int, int[],int[],int[]);
void WritePath(char*, int, int[]);
void WriteFail(char*, int, int[], int[]);

int main(int argc, char** argv) {
    
  int indivs=atoi(argv[1]);
  char* DNAFile= argv[2];
  char* DataFile=argv[3];
  char* indivFile = argv[4];
  char* failFile = argv[5];
    
  ifstream* input= new ifstream;
    
  double* length= new double [indivs];
  double* radius= new double [indivs];
  double* theta= new double [indivs];
  int indivCount = 0;
  int* indivArray = new int [indivs];
  int* rowArray = new int [indivs];
  int* failArray = new int [indivs];
    
  for(int i = 0; i < indivs; i++){
    indivArray[i]=0;
    rowArray[i]=0;
    failArray[i]=0;
  }
    
  Read(DNAFile, indivs, length, radius, theta);
  indivCount = Write(DataFile,length,radius,theta,indivs, indivArray,rowArray,failArray);
  WritePath(indivFile, indivCount, indivArray);
  int failCount = indivs-indivCount;
  WriteFail(failFile, failCount, failArray, rowArray);
    
    
    
    
  delete[] length;
  delete[] radius;
  delete[] theta;
  delete[] indivArray;
  delete[] rowArray;
  delete[] failArray;
  delete input;
  return 0;
}


void Read(char* filename, int indivs, double length[], double radius[], double theta[]){
    
  string txt = filename;
  ifstream inputFile;
  inputFile.open(txt.c_str());
    
    
  if(!inputFile.is_open()){
    cout << endl << "Error! " << filename << " could not be opened!" \
	 << endl;
  }else{
    string currentLine="Empty";
    int lineNumber=0;
    getline(inputFile, currentLine);
    while(currentLine.substr(0,12).compare("Matrices for")){
      getline(inputFile, currentLine);
    }
        
    for(int i = 0; i < indivs; i++){
      getline(inputFile, currentLine);
      int commaToken=currentLine.find(",");
            
            
      length[i] = stod(currentLine.substr(0,commaToken));
            
      string halfLine = currentLine.substr(commaToken+1);
      commaToken=halfLine.find(",");
            
            
      radius[i] = stod(halfLine.substr(0, commaToken));
            
      theta[i] = stod(halfLine.substr(commaToken+1));
            
    }
  }
  inputFile.close();
}

int Write(char* filename, double lengths[], double radii[], double thetas[], int indivs, int indivArray[], int rowArray[],int failArray[]){
  string txt = filename;
  int indivCount = 0;
  int failCount =0;
  string currentLine="Empty";
  int rowNum=0;
  bool newData = true;
  for(int i = 0; i < indivs; i++){
    ifstream inFile;
    inFile.open(txt.c_str());
    newData = true;
    while ( getline(inFile, currentLine)) {
            
            
      int commaToken=currentLine.find(",");
            
            
      double length = stod(currentLine.substr(0,commaToken));
            
            
      string halfLine = currentLine.substr(commaToken+1);
      commaToken=halfLine.find(",");
            
      double radius = stod(halfLine.substr(0, commaToken));
            
      string thirdLine = halfLine.substr(commaToken+1);
      commaToken=thirdLine.find(",");
            
            
      double theta = stod(thirdLine.substr(0,commaToken));
            
            
      rowNum = stoi(thirdLine.substr(commaToken+4));
            
      if((lengths[i]-length>0.00001||lengths[i]-length<-0.00001)||(radii[i]-radius>0.00001||radii[i]-radius<-0.00001)||(thetas[i]-theta>0.00001||thetas[i]-theta<-0.00001)){
                
	/*
                 for(int j = i-1; i>=0; i--){
                 if((lengths[i]-lengths[j]>0.00001||lengths[i]-lengths[j]<-0.00001)||(radii[i]-radii[j]>0.00001||radii[i]-radii[j]<-0.00001)||(thetas[i]-thetas[j]>0.00001||thetas[i]-thetas[j]<-0.00001)){
                 
                 }else{
                 newData=false;
                 }
                 }
	*/
                
                
      }else{
                
	newData=false;
	rowArray[failCount]=rowNum;
	failArray[failCount]=i;
	failCount++;
      }
            
    }
    inFile.close();
    if(newData){
            
        
      indivArray[indivCount]=i;
      indivCount++;
    }
  }
  return indivCount;
    
}

void WritePath(char* filename, int indivCount, int indivArray[]){
    
  string txt = filename;
  ofstream out;
  out.open(txt.c_str());
  if(indivCount>0){
    out<< indivArray[0]+1 <<endl;
  }
  for(int i = 1; i < indivCount; i++){
    out <<indivArray[i]+1 << endl;
    //This +1 is because the uan counts start from 1 not 0
  }
    
    
  out.close();
    
}


void WriteFail(char* filename, int failCount, int failArray[], int rowArray[]){
  string txt = filename;
  ofstream out;
  out.open(txt.c_str());
  if(failCount>0){
    out<< failArray[0]+1 <<
      " Row"<<rowArray[0] << endl;
  }
  for(int i = 1; i < failCount; i++){
    out<< failArray[i]+1 << " Row"<<rowArray[i] << endl;
    //This +1 is because the uan counts start from 1 not 0
  }
    
    
  out.close();
    
    
}
