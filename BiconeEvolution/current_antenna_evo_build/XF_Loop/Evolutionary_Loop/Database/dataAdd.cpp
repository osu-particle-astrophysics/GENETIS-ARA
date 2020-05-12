//
//  reading.cpp
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
int Write(char*,double[],double[],double[],int, int[], int&);
void WritePath(char*, int, int[],int&);

int main(int argc, char** argv) {
    
  int indivs=atoi(argv[1]);
  char* DNAFile= argv[2];
  char* DataFile=argv[3];
  char* indivFile = argv[4];
    
  ifstream* input= new ifstream;
   
  double* length= new double [indivs];
  double* radius= new double [indivs];
  double* theta= new double [indivs];
  int indivCount = 0;
  int* indivArray = new int [indivs];
  int rowCount = 0;

    
  for(int i = 0; i < indivs; i++){
    indivArray[i]=0;
  }
    
  Read(DNAFile, indivs, length, radius, theta);
  indivCount = Write(DataFile,length,radius,theta,indivs, indivArray, rowCount);
  WritePath(indivFile, indivCount, indivArray,rowCount);
   
  delete[] length;
  delete[] radius;
  delete[] theta;
  delete[] indivArray;
  
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

int Write(char* filename, double lengths[], double radii[], double thetas[], int indivs, int indivArray[], int &rowCount){
  string txt = filename;
  int indivCount = 0;
    
  string currentLine="Empty";
  int rowNum=0;
  bool newData = true;
  ifstream readRow;
  readRow.open(txt.c_str());
  while(getline(readRow, currentLine)){
    int commaToken=currentLine.find(",");
        
        
    double length = stod(currentLine.substr(0,commaToken));
        
        
    string halfLine = currentLine.substr(commaToken+1);
    commaToken=halfLine.find(",");
        
    double radius = stod(halfLine.substr(0, commaToken));
        
    string thirdLine = halfLine.substr(commaToken+1);
    commaToken=thirdLine.find(",");
        
        
    double theta = stod(thirdLine.substr(0,commaToken));
        
        
    rowNum = stoi(thirdLine.substr(commaToken+4));
    if(rowNum > rowCount){
      rowCount=rowNum;
    }
  }
  readRow.close();
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
                
               
                
                    
      }else{
	newData=false;
      }
            
    }
    inFile.close();
    if(newData){
      ofstream out;
      out.open(txt.c_str(),ios_base::app);
      out<<lengths[i]<<","<<radii[i]<<","<<thetas[i]<<","<< "Row"<<rowNum+1<<"\n";
      out.close();
            
           
      indivArray[indivCount]=i;
        
      indivCount++;
    }
  }
  return indivCount;
    
}

void WritePath(char* filename, int indivCount, int indivArray[], int &rowCount){

  string txt = filename;
  ofstream out;
  out.open(txt.c_str());
  if(indivCount>0){
    rowCount++;
    out<< indivArray[0]+1 << " Row" << rowCount << endl;
  }
  for(int i = 1; i < indivCount; i++){
    rowCount++;
    out<<indivArray[i]+1 << " Row" << rowCount << endl;
    //This +1 is because the uan counts start from 1 not 0
  }
  out.close();
    
}
