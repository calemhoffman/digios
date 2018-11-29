/***********************************************************************
 * 
 *  This is ExtractXSec for *.out for Ptolemy
 * 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ InFileCreator.C -o InFileCreator `root-config --cflags --glibs`
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TGraph.h>
#include <TObjArray.h>

using namespace std;

int ExtractXSec (string readFile) {
   
  //--- open file.out
  ifstream file_in;
  file_in.open(readFile.c_str(), ios::in);
  if( !file_in){
    printf("Unable to open %s \n", readFile.c_str());
    exit(1);
  }

  //---- variable for Xsec
  vector<string> title;
  vector<vector<double>> dataMatrix;
  vector<double> angle;
  vector<double> Ex;
  vector<double> totalXsec;
  vector<string> reaction;
  double angleMin = 0;
  double angleMax = 0;
  double angleStep = -1;
  
  //---- read file.out
  string line;
  int lineNum = 0;
  vector<double> dataXsec;
  bool startExtract = false;
  bool angleFilled = false;
  int numCal = 0;
  printf("======================================================\n");
  while(getline(file_in, line)){
    lineNum ++;
    //printf("%d , %s\n", lineNum, line.c_str());
    
    size_t pos;
    string findStr;
    int findLen;
    
    //---- find Title
    findStr = "$============================================";
    findLen = findStr.length();
    pos = line.find(findStr);
    if( pos != string::npos){
      title.push_back(line.substr(pos + findLen+1));
      
      pos = title.back().find("(");
      string ex  = title.back().substr(3, pos-3);
      Ex.push_back( atof(ex.c_str()) );
      
      //clear dataXsex for new data set
      dataXsec.clear();
      
      numCal ++;
      continue;
    }
    
    //---- find Reaction
    findStr = "REACTION:";
    findLen = findStr.length();
    pos = line.find(findStr);
    if( pos != string::npos ) {
      reaction.push_back( line.substr(pos + findLen + 1) );
      continue;
    }
    
    //----- find angle stetting when not known
    if( angleStep == -1 ){
      findStr = "anglemin=";
      findLen = findStr.length();
      pos = line.find(findStr);
      if( pos != string::npos){
        
        size_t pos1 = line.find("anglemax=");
        angleMin = atof( line.substr(pos+findLen, pos1 - pos - findLen).c_str() );
        
        pos = pos1;
        pos1 = line.find("anglestep=");
        angleMax = atof( line.substr(pos+findLen, pos1 - pos - findLen).c_str() );
        
        angleStep = atof( line.substr(pos1 + findLen+1).c_str() );
        
      }
      
      continue;
    }
    
    //----- check if start extracting Xsec or not 
    findStr = "0  C.M.  REACTION     REACTION   LOW L  HIGH L   % FROM";
    pos = line.find(findStr);
    if( pos != string::npos ) {
      startExtract = true;
      continue;
    }
    
    //----- start extracting Xsec
    if( startExtract ){
      
      if( line.length() < 20 ) continue;
      
      //printf("   |%s \n", line.c_str());
      double num1 = atof( line.substr(0, 7).c_str());
      double num2 = atof( line.substr(7, 19).c_str());
      
      if( num1 != 0. && num2 != 0. ){
        if( !angleFilled ) angle.push_back(num1);
        dataXsec.push_back(num2);
      }
      
    }
    
    //------ find total Xsec, if found stop extraction
    findStr = "0TOTAL:";
    findLen = findStr.length();
    pos = line.find(findStr);
    if( pos != string::npos ) {
      
      totalXsec.push_back( atof(line.substr(pos + findLen).c_str()) );
      
      printf("%2d | %s | total Xsec(4pi): %f mb \n", numCal,  title.back().c_str(), totalXsec.back());
      
      //push back dataXsec to dataMatrix
      dataMatrix.push_back(dataXsec);
      
      angleFilled = true;
      startExtract = false;      
      continue;
    }
    
  }
  file_in.close();
  
  //---- summary
  printf("====================== Total number of line read : %d \n", lineNum);  
  printf("Angle : %5.2f, %5.2f | step : %5.2f \n", angleMin, angleMax, angleStep);
  printf("Number of Angle read : %lu \n", angle.size());
  printf("Number of  data read : %lu \n", dataXsec.size());
  printf("Number of Reaction   : %lu \n", reaction.size());
  
  
  printf("----------------------------- list of Calculation \n");
  for( int i = 0; i < numCal ; i++){
    
    double partialSumXsec = 0.0;
    for( int j = 0; j < (dataMatrix[i]).size() ; j++ ){
      //double theta = (angle[j] + angleStep/2.) * TMath::DegToRad();
      double theta = (angle[j]) * TMath::DegToRad();
      double dTheta = angleStep * TMath::DegToRad();
      double phi = TMath::TwoPi();
      partialSumXsec += dataMatrix[i][j] * sin( theta ) * dTheta * phi ;
    }
    
    size_t pos = title[i].find(")");
    printf("%50s| %s | Xsec(%3.0f-%3.0f deg) : %f mb\n", reaction[i].c_str(), title[i].substr(pos+1).c_str(), angleMin, angleMax, partialSumXsec);
  }
  printf("---------------------------------------------------\n");
  
  
  //---- open file.Xsec.txt
  string saveFileName = readFile;
  int len = saveFileName.length();
  saveFileName = saveFileName.substr(0, len - 4); 
  saveFileName += ".Xsec.txt";
  printf("Output : %s \n", saveFileName.c_str());
  FILE * file_out;
  file_out = fopen(saveFileName.c_str(), "w+");
  
  for( int i = 0; i < numCal ; i++){
      fprintf(file_out, "#%14s\n", reaction[i].c_str());
  }
  
  int space = 19;
  fprintf(file_out, "%8s\t", "Angel");
  for( int i = 0; i < numCal ; i++){
      fprintf(file_out, "%*s", space, title[i].c_str());
  }
  fprintf(file_out, "\n");
  
  for( int i = 0; i < angle.size() ; i ++){
    fprintf(file_out, "%8.3f\t", angle[i]);
    for( int j = 0; j < numCal ; j++){
        fprintf(file_out, "%*f", space, dataMatrix[j][i]);
    }
    fprintf(file_out, "\n");
  }
  fclose(file_out);
  
  //Save in ROOT
  len = saveFileName.length();
  saveFileName = saveFileName.substr(0, len - 9);
  TString fileName = saveFileName;
  fileName += ".root"; 
  printf("Output : %s \n", fileName.Data());
  TFile * fileOut = new TFile(fileName, "RECREATE" );
  
  TObjArray * gList = new TObjArray();
  for( int i = 0; i < numCal ; i++){
    TGraph * gTemp = new TGraph();
    TString name = reaction[i];
    name += "|";
    name += title[i];
    gTemp->SetName(name);
    for( int j = 0; j < angle.size() ; j++){
        gTemp->SetPoint(j, angle[j], dataMatrix[i][j]);
    }
    gList->Add(gTemp);
  }
  
  gList->Write("gList", 1);
  fileOut->Write();
  fileOut->Close();
  printf("---------------------------------------------------\n");

  return 0;
}
 
