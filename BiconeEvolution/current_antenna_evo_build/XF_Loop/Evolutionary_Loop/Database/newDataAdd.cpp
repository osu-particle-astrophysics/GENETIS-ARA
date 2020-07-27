//
//  reading.cpp
//  
//
//  Created by Alex Patton on 7/2/20.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


using namespace std;
void Read(char*,int,int,double**);
int Write(char*,double**,int,int, int[], int&);
void WritePath(char*, int, int[],int&);

int main(int argc, char** argv) {
    
  int indivs=atoi(argv[1]);
  char* DNAFile= argv[2];
  char* DataFile=argv[3];
  char* indivFile = argv[4];
  int genes=atoi(argv[5]);
    
  ifstream* input= new ifstream;
   
  double** data = new double*[genes];
    
  for(int i = 0; i < genes; i++){
    data[i] = new double[indivs];
  }

  int indivCount = 0;
  int* indivArray = new int [indivs];
  int rowCount = 0;

    
  for(int i = 0; i < indivs; i++){
    indivArray[i]=0;
  }
    
  Read(DNAFile, indivs, genes, data);
  indivCount = Write(DataFile, data, indivs, genes, indivArray, rowCount);
  WritePath(indivFile, indivCount, indivArray,rowCount);
   
  for(int i = 0; i < genes; i++){
    delete data[i];
  }
  delete[] data;
  delete[] indivArray;
  
  delete input;
  return 0;
}



void Read(char* filename, int indivs, int genes, double **data){
    
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
        
    string halfline="";
        
    for(int i = 0; i < indivs; i++){
      getline(inputFile, currentLine);
           
            
      for(int j = 0; j < genes-1; j++){
	int commaToken=currentLine.find(",");
	data[j][i] = stod(currentLine.substr(0,commaToken));
          
	halfline = currentLine.substr(commaToken+1);
	currentLine = halfline;
                
      }
      data[genes-1][i] = stod(halfline);
    }
  }
  inputFile.close();
}

int Write(char* filename, double **data, int indivs, int genes, int indivArray[], int &rowCount){
  string txt = filename;
  int indivCount = 0;
    
  string currentLine="Empty";
  int rowNum=0;
  bool newData = true;
  ifstream readRow;
  readRow.open(txt.c_str());
  double* dataList= new double[genes];
   
  while(getline(readRow, currentLine)){
        
    for(int j = 0; j < genes; j++){
      int commaToken=currentLine.find(",");
      dataList[j] = stod(currentLine.substr(0,commaToken));
            
      string halfline = currentLine.substr(commaToken+1);
      currentLine = halfline;
            
    }
      
    rowNum = stoi(currentLine.substr(3));
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
            
            
      for(int j = 0; j < genes; j++){
	int commaToken=currentLine.find(",");
	dataList[j] = stod(currentLine.substr(0,commaToken));
                
	string halfline = currentLine.substr(commaToken+1);
	currentLine = halfline;
                
      }
            
      rowNum = stoi(currentLine.substr(3));
      bool* newDataArray = new bool[genes];
            
      for(int j = 0; j < genes; j++){
	newDataArray[j]=true;
	if(data[j][i]-dataList[j]>0.0001||data[j][i]-dataList[j]<-0.00001){
                    
	}else{
	  newDataArray[j]=false;
	}
      }
      int falseCount = 0;
      for(int j = 0; j < genes; j++){
	if(!newDataArray[j]){
	  falseCount++;
	}
      }
      delete[] newDataArray;
      if(falseCount==genes){
	newData=false;
      }
            
    }
    inFile.close();
    if(newData){
      ofstream out;
      out.open(txt.c_str(),ios_base::app);
      for(int j = 0; j < genes; j++){
	out << data[j][i] << ",";
      }
      out<<"Row" << rowNum+1<<"\n";
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
