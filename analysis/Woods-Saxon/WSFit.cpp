/****************************************************************
  Fitting experimental energy with WS potential
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-1-31
*****************************************************************/

#include <stdio.h>
#include <string>
#include <stdlib.h> //for atoi, atof
#include <math.h> //exp
#include <vector> 
#include <fstream> 
#include "RK4.h"
#include "WS.h"

using namespace std;


vector<string> SplitStr(string tempLine, string splitter, int shift = 0){

  vector<string> output;

  size_t pos;
  do{
    pos = tempLine.find(splitter); // fine splitter
    if( pos == 0 ){ //check if it is splitter again
      tempLine = tempLine.substr(pos+1);
      continue;
    }

    string secStr;
    if( pos == string::npos ){
      secStr = tempLine;
    }else{
      secStr = tempLine.substr(0, pos+shift);
      tempLine = tempLine.substr(pos+shift);
    }

    //check if secStr is begin with space
    if( secStr.substr(0, 1) == " "){
      secStr = secStr.substr(1);
    }

    output.push_back(secStr);
    //printf(" |%s---\n", secStr.c_str());
    
  }while(pos != string::npos );

  return output;
}


int main(int argc, char *argv[]){

  if(argc != 11) { 
    printf("Usage: ./WSFit expFile A V0 r0 a0 VSO rSO aSO nStep dr\n");
    printf("       expFile = experimental energies\n");
    printf("             A = mass number\n");
    printf("            V0 = inital guess of central depth [MeV]\n");
    printf("            r0 = reduced radius for central potential [fm]\n");
    printf("            a0 = reduced radius for central potential [fm]\n");
    printf("           VSO = inital guess of Spin-Orbital depth [MeV]\n");
    printf("           rso = reduced radius for Spin-Orbital potential [fm]\n");
    printf("           aso = reduced radius for Spin-Orbital potential [fm]\n");
    printf("         nStep = number of r-grid\n");
    printf("            dr = grid size [fm]\n");
    exit(0); 
  }
  
  string readFile = argv[1];
  int A=atoi(argv[2]);
  double V0ini = atof(argv[3]);
  double r0 = atof(argv[4]);
  double A0 = atof(argv[5]);
  double VSOini = atof(argv[6]);
  double rso = atof(argv[7]);
  double aso = atof(argv[8]);
  int nStep = atoi(argv[9]);
  double dr = atof(argv[10]);
  
  ifstream file_in;
  file_in.open(readFile.c_str(), ios::in);

  if( !file_in ){
    printf(" cannot read file: %s. \n", readFile.c_str());
    return 0 ; 
  }
  
  vector<string> NLJ; //orbital label
  vector<double> BE;  //binding enegry of orbital
  
  NLJ.clear();
  BE.clear();
  
  while( file_in.good() ) {
    string tempLine;
    getline(file_in, tempLine );
    
    if( tempLine.substr(0, 1) == "#" ) continue;
    if( tempLine.size() < 1 ) continue;
    
    vector<string> str0 = SplitStr(tempLine, " ");
    if( str0.size() == 0 ) continue;
    
    NLJ.push_back(str0[0]);
    BE.push_back(atof(str0[1].c_str()));
    
  }
  
  //printf("============ Input from %s.\n", readFile.c_str());
  //for( int i = 0; i < NLJ.size() ; i++){  
  //printf("NLJ: %6s | %f \n", NLJ[i].c_str(), BE[i]);  
  //}
  //printf("==============================\n");
    
  wsp.V0 = V0ini ;wsp.R0 = r0 * pow(A, 1./3.); wsp.a0 = A0;
  wsp.VSO = VSOini ; wsp.RSO = rso * pow(A, 1./3.); wsp.aSO = aso;
  
  PrintWSParas(A, dr , nStep, wsp);
  
  printf("############################## Calculate WS levels and compare\n");
  WS(nStep, dr);
  printf("      Experiment      |      Woods-Saxon      |     diff\n");
  double rms = 0;
  int numMatch = 0;
  for( int i = 0; i < NLJ.size(); i++){
    for( int j = 0; j < orbString.size(); j++){
      if( NLJ[i] == orbString[j] ){
        double diff = BE[i] -  energy[j];
        rms += pow(diff,2);
        numMatch ++;
        printf(" %d %6s (%9.6f) | %d %6s (%9.6f) | %8.2f keV \n",i,  NLJ[i].c_str(), BE[i], j, orbString[j].c_str(), energy[j], diff * 1000.);
        continue;
      }
    }
  }
  
  if( numMatch != NLJ.size() ){
    rms = -404;
    printf("========================================= RMS = \e[31mfail\e[0m keV \n");
  }else{
    rms = sqrt(rms);
    printf("========================================= RMS = \e[31m%8.2f\e[0m keV \n", rms*1000.);
  }

  PrintEnergyLevels();
  
  
  return 0;
}

