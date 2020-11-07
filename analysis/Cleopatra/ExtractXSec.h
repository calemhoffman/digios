/***********************************************************************
 * 
 *  This is ExtractXSec for *.out for Ptolemy
 * 
 *  This program will extract cross section distribution from Ptolmey output.
 *  save as *.Xsec.txt and *.root for distribution
 * 
 *  save ExPtolemy.txt for excitation energies and total X-section 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ ExtractXSec.C -o ExtractXSec `root-config --cflags --glibs`
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
#include <TF1.h>
#include <TObjArray.h>
#include "../Armory/AnalysisLibrary.h"

using namespace std;

TObjArray * gList = NULL;
double distfunct(double *x, double *par){
  
  TGraph * gTemp = (TGraph *) gList->At(par[0]);
  
  return (gTemp->Eval(x[0]))* sin(x[0] * TMath::DegToRad());
}

bool isFloat( string str ) {
  int length = str.length();
  for( int i = 0; i < length; i++){
    string it = str.substr(i,1);
    if( it == " " || it == "."|| it == "1"|| it == "2"|| 
         it == "3"|| it == "4"|| it == "5"|| it == "6"|| it == "7"|| it == "8"|| it == "9"|| it == "0"){
      continue;
    }else{
      return false;
    } 
  }
  return true;
}

int ExtractXSec (string readFile, int indexForElastic=1) {
  
  //indexForElastic = 1 ; for Ratio
  //indexForElastic = 2 ; for Total
   
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
  
  //================================== read file.out
  string line;
  int lineNum = 0;
  vector<double> dataXsec;
  bool startExtract = false;
  bool angleFilled = false;
  int numCal = 0;
  int reactionFlag = 0;
  bool preFindForElastic = false;
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
    findStr = "0INPUT... CHANNEL";
    findLen = findStr.length();
    pos = line.find(findStr);
    if( pos != string::npos ) {
      reaction.push_back( line.substr(pos + findLen + 1) );
      reactionFlag = 1; // 1 for (d,d) or (p,p)
      //printf("%d |----------- (d,d), %d\n", lineNum, reactionFlag);
      continue; // nextline
    }
    
    findStr = "REACTION:";
    findLen = findStr.length();
    pos = line.find(findStr);
    if( pos != string::npos ) {
      reaction.push_back( line.substr(pos + findLen + 1) );
      reactionFlag = 2; // 2 for (d,p) or (p,d)
      //printf("%d |----------- (d,p), %d\n", lineNum, reactionFlag);
      continue; // nextline
    }
    
    //----- pre find for Elastic scattering
    findStr = "0        OPTICAL MODEL SCATTERING FOR THE OUTGOING CHANNEL";
    pos = line.find(findStr);
    if( pos != string::npos ) {
      preFindForElastic = true;
      //printf("%d |----------- pre Elastics Flag : %d\n", lineNum, preFindForElastic);
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
        
        //printf("%d |---- angle range found.\n", lineNum);
        
      }
      continue; //nextline
    }
    
    //-----  check if start extracting Xsec or not 
    findStr = "dumpdumpdump";
    if( reactionFlag == 1 && preFindForElastic ){
      findStr = "C.M.    LAB     RUTHERFORD";
    }
    if( reactionFlag == 2 ){
      findStr = "0  C.M.  REACTION     REACTION   LOW L  HIGH L   % FROM";
    }
    pos = line.find(findStr);
    if( pos != string::npos ) {
      startExtract = true;
      //printf("%d |----- start extraction \n", lineNum);
      continue;
    }
    
    //----- start extracting Xsec
    if( startExtract ){
      
      if( line.length() <  20 ) continue;
      
      //printf("%d   |%s \n", line.length(), line.c_str());
      double num1, num2;
      if( reactionFlag == 1 ){ // Elastics (d,d) or (p,p)
        num1 = atof( line.substr(0, 7).c_str());
        if( indexForElastic == 1 ){
          num2 = atof( line.substr(15, 15).c_str());
        }else{
          num2 = atof( line.substr(28, 14).c_str());
        }
      }
      
      if( reactionFlag == 2 ){ // InElastics (d,p) or (p,d)
        if( isFloat( line.substr(0, 7) ) ){
          num1 = atof( line.substr(0, 7).c_str());
          num2 = atof( line.substr(7, 19).c_str());
        }else{
          num1 = 0.0;
          num2 = 0.0;
        }
      }
      
      if( num1 != 0. && num2 != 0. ){
        if( !angleFilled ) angle.push_back(num1);
        dataXsec.push_back(num2);
      } 
    }
    
    //------ find total Xsec, if found stop extraction
    findStr = "dumpdumpdump";
    if( reactionFlag == 1 && preFindForElastic){
      findStr = "0TOTAL REACTION CROSS SECTION =";
    }
    if( reactionFlag == 2){
      findStr = "0TOTAL:";
    }
    findLen = findStr.length();
    pos = line.find(findStr);
    if( pos != string::npos ) {
      
      totalXsec.push_back( atof(line.substr(pos + findLen).c_str()) );
      
      printf("%2d | %s | total Xsec(4pi): %f mb \n", numCal,  title.back().c_str(), totalXsec.back());
      
      //push back dataXsec to dataMatrix
      dataMatrix.push_back(dataXsec);
      
      //printf("%d |----- end extraction \n", lineNum);
      
      angleFilled = true;
      startExtract = false; 
      reactionFlag = 0;
      preFindForElastic = false;
      continue;
    }
    
  }
  file_in.close();
  
  //================================== summary
  printf("====================== Total number of line read : %d \n", lineNum);  
  printf("Angle : %5.2f, %5.2f | step : %5.2f \n", angleMin, angleMax, angleStep);
  printf("Number of Angle read : %lu \n", angle.size());
  printf("Number of  data read : %lu \n", dataXsec.size());
  printf("Number of Reaction   : %lu \n", reaction.size());
  
  
  printf("----------------------------- list of Calculation \n");
  //... find suitable lenght for displaying reaction string
  int reactionStrLen = 0;
  for( int i = 0; i < numCal ; i++){
    int len = reaction[i].length();
    if( len > reactionStrLen ) reactionStrLen = len;
  }
  
  vector<double> partialXsec;
  partialXsec.clear();
  for( int i = 0; i < numCal ; i++){
    
    double partialSumXsec = 0.0;
    for( int j = 0; j < (dataMatrix[i]).size() ; j++ ){
      //double theta = (angle[j] + angleStep/2.) * TMath::DegToRad();
      double theta = (angle[j]) * TMath::DegToRad();
      double dTheta = angleStep * TMath::DegToRad();
      double phi = TMath::TwoPi();
      partialSumXsec += dataMatrix[i][j] * sin( theta ) * dTheta * phi ;
    }
    partialXsec.push_back(partialSumXsec);
    
    size_t pos = title[i].find(")");
    printf("%*s| %s | Xsec(%3.0f-%3.0f deg) : %f mb\n", reactionStrLen + 3, reaction[i].c_str(), title[i].substr(pos+1).c_str(), angleMin, angleMax, partialSumXsec);
  }
  printf("---------------------------------------------------\n");
  
  //================================== save *.Ex.txt
  string saveExName = readFile;
  int len = saveExName.length();
  saveExName = saveExName.substr(0, len - 4); 
  saveExName += ".Ex.txt";
  printf("Output : %s \n", saveExName.c_str());
  FILE * file_Ex;
  file_Ex = fopen(saveExName.c_str(), "w+");
  
  fprintf(file_Ex, "//generated_by_ExtractXSec.h\n");

  for( int i = 0; i < numCal ; i++){
      fprintf(file_Ex, "%9.5f     %9.5f\n", Ex[i], partialXsec[i]);
  }
  fprintf(file_Ex, "#=====End_of_File\n");
  fclose(file_Ex);
  
  //================================== save file.Xsec.txt
  string saveFileName = readFile;
  len = saveFileName.length();
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
  
  //================================== Save in ROOT
  len = saveFileName.length();
  saveFileName = saveFileName.substr(0, len - 9);
  TString fileName = saveFileName;
  fileName += ".root"; 
  printf("Output : %s \n", fileName.Data());
  TFile * fileOut = new TFile(fileName, "RECREATE" );
  
  gList = new TObjArray();
  gList->SetName("TGraph of distributions");
  TObjArray * fList = new TObjArray();
  gList->SetName("TF1 of distributions");
  
  TGraph ** gGraph = new TGraph *[numCal];
  TF1 ** dist = new TF1*[numCal];
  
  for( int i = 0; i < numCal ; i++){
    gGraph[i] = new TGraph();
    TString name = reaction[i];
    name += "|";
    name += title[i];
    gGraph[i]->SetName(name);
    for( int j = 0; j < angle.size() ; j++){
        gGraph[i]->SetPoint(j, angle[j], dataMatrix[i][j]);
    }
    gList->Add(gGraph[i]);
    
    name.Form("dist%d", i);
    dist[i] = new TF1(name, distfunct, angleMin, angleMax, 1 );
    dist[i]->SetParameter(0, i);
    
    fList->Add(dist[i]);
    
    //delete tempFunc;
    
  }
  gList->Write("qList", 1);
  fList->Write("pList", 1);
  
  
  fileOut->Write();
  fileOut->Close();
  printf("---------------------------------------------------\n");
  
  return 0;
}
 
int ExtractXSecFromText(string readFile){
  
  //read file
  ifstream file_in;
  file_in.open(readFile.c_str(), ios::in);
  if( !file_in){
    printf("Unable to open %s \n", readFile.c_str());
    exit(1);
  }
  
    //---- variable for Xsec
  vector<vector<double>> dataMatrix;
  vector<double> angle;
  vector<double> Ex;
  
  //================================== read file.out
  string line;
  int lineNum = 0;
  vector<double> dataXsec;
    
  int numCal = 0;
  printf("======================================================\n");
  
  bool headerDone = false;
  
  while(getline(file_in, line)){
    lineNum ++;
    
    if( line.substr(0,1) == "#" ) continue;
    
    //printf("%d | %s\n", lineNum, line.c_str());
    
    //after the comment line, the next line must be column name
    vector<string> header= SplitStr(line, " ");
    //printf("---%lu #", header.size());
    //for( int i = 0; i < header.size(); i++){
    //  printf("%s|", header[i].c_str());
    //}
    //printf("\n");
  
    if ( !headerDone ) {
      for( int i = 1 ; i < header.size(); i++){
        string energy = header[i].substr(3, header[i].length());
        Ex.push_back(atof(energy.c_str()));
        //printf("%f---\n", Ex.back());
      }
      headerDone = true;
    }else{
    
      vector<double> temp;
      for( int i = 0; i < header.size(); i++){
        if( i == 0 ) angle.push_back(atof(header[i].c_str()));
        if( i > 0  ) temp.push_back(atof(header[i].c_str()));        
      }
      dataMatrix.push_back(temp);
      
    }
    
  }
  file_in.close();
  
  double angleMin = angle.front();
  double angleMax = angle.back();
  double angleStep = (angleMax - angleMin)/(angle.size()-1);
  
  //------ print  out read data
  printf("====================== data read as:\n");
  printf("%5s,  ", "Ex");
  for(int i = 0; i < Ex.size(); i++){
    printf("%6.3f|", Ex[i]);
  }
  printf("\n");
  for(int i = 0; i < dataMatrix.size(); i++){
    printf("%5.2f,  ", angle[i]);
    for( int j = 0; j < dataMatrix[i].size(); j++) printf("%6.3f|", dataMatrix[i][j]);
    printf("\n");
  }
  printf("---------------------------------\n");
  printf("angle min :%f\n", angleMin);
  printf("angle max :%f\n", angleMax);
  printf("angle step:%f\n", angleStep);
  printf("---------------------------------\n");
  
  //------- calculate summed cross section
  vector<double> partialXsec(Ex.size());
  for( int i = 0; i < dataMatrix.size() ; i++){
    for( int j = 0; j < (dataMatrix[i]).size() ; j++ ){
      //double theta = (angle[j] + angleStep/2.) * TMath::DegToRad();
      double theta = (angle[i]) * TMath::DegToRad();
      double dTheta = angleStep * TMath::DegToRad();
      double phi = TMath::TwoPi();
      partialXsec[j] += dataMatrix[i][j] * sin( theta ) * dTheta * phi ;
    }
  }
  for(int i = 0; i < Ex.size(); i++){
    printf("Ex=%6.3f| Xsec(%3.0f-%3.0f deg) : %f mb\n", Ex[i] , angleMin, angleMax, partialXsec[i]);
  }
  printf("---------------------------------------------------\n");
  
  //================================== save *.Ex.txt
  string saveExName = readFile;
  int len = saveExName.length();
  saveExName = saveExName.substr(0, len - 4); 
  saveExName += ".Ex.txt";
  printf("Output : %s \n", saveExName.c_str());
  FILE * file_Ex;
  file_Ex = fopen(saveExName.c_str(), "w+");
  fprintf(file_Ex, "#Generated_by_ExtractXsec.h\n");
  for( int i = 0; i < Ex.size() ; i++){
      fprintf(file_Ex, "%9.5f     %9.5f\n", Ex[i], partialXsec[i]);
  }
  fprintf(file_Ex, "#=====End_of_File\n");
  fclose(file_Ex);
  
  
  //================================== Save in ROOT
  string saveFileName = readFile;
  len = saveFileName.length();
  saveFileName = saveFileName.substr(0, len - 4);
  TString fileName = saveFileName;
  fileName += ".root"; 
  printf("Output : %s \n", fileName.Data());
  TFile * fileOut = new TFile(fileName, "RECREATE" );
  
  gList = new TObjArray();
  gList->SetName("TGraph of distributions");
  TObjArray * fList = new TObjArray();
  gList->SetName("TF1 of distributions");
  
  TGraph ** gGraph = new TGraph *[numCal];
  TF1 ** dist = new TF1*[numCal];
  
  for( int i = 0; i < Ex.size() ; i++){
    gGraph[i] = new TGraph();
    TString name ;
    name.Form("Ex=%6.3f MeV", Ex[i]);
    gGraph[i]->SetName(name);
    for( int j = 0; j < angle.size() ; j++){
        gGraph[i]->SetPoint(j, angle[j], dataMatrix[j][i]);
    }
    gList->Add(gGraph[i]);
    
    name.Form("dist%d", i);
    dist[i] = new TF1(name, distfunct, angleMin, angleMax, 1 );
    dist[i]->SetParameter(0, i);
    
    fList->Add(dist[i]);
    
    //delete tempFunc;
    
  }
  gList->Write("qList", 1);
  fList->Write("pList", 1);
  
  
  fileOut->Write();
  fileOut->Close();
  printf("---------------------------------------------------\n");
  
  
  return 0;
  
}

