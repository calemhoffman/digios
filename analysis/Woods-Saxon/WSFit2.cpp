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

  if(argc < 5 || argc > 11) { 
    printf("Usage: ./WSFit expFile A V0 VSO <dV0 dVSO r0 a0 rso, aso>\n");
    printf("       expFile = experimental energies\n");
    printf("             A = mass number\n");
    printf("            V0 = inital guess of central depth [MeV]\n");
    printf("           VSO = inital guess of Spin-Orbital depth [MeV]\n");
    printf("--------------------------------- optional\n");
    printf("           dV0 = seraching range from V0 - dV0, V0 + dV0 [MeV], defualt is +-0.5\n");
    printf("          dVSO = seraching range from VSO - dVSO, VSO + dVSO [MeV], default is +-3\n");
    printf("            r0 = reduced radius for central potential, default 1.25 fm\n");
    printf("            a0 = reduced radius for central potential, default 0.65 fm\n");
    printf("           rso = reduced radius for Spin-Orbital potential, default 1.0 fm\n");
    printf("           aso = reduced radius for Spin-Orbital potential, default 0.65 fm\n");
    exit(0); 
  }
  
  string readFile = argv[1];
  int A=atoi(argv[2]);
  double V0ini = atof(argv[3]);
  double VSOini = atof(argv[4]);
  
  double dV0 = 0.5;   if( argc >= 6 )  dV0 = atof(argv[5]);
  double dVSO = 3.0;  if( argc >= 7 ) dVSO = atof(argv[6]);
  double r0 = 1.25;   if( argc >= 8 )   r0 = atof(argv[7]);
  double A0 = 0.65;   if( argc >= 9 )   A0 = atof(argv[8]);
  double rso = 1.0;   if( argc >= 10 ) rso = atof(argv[9]);
  double aso = 0.65;  if( argc >= 11 ) aso = atof(argv[10]);
  
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
  
  printf("============ Input from %s.\n", readFile.c_str());
  for( int i = 0; i < NLJ.size() ; i++){
    
    printf("NLJ: %6s | %f \n", NLJ[i].c_str(), BE[i]);
    
  }
  printf("==============================\n");
  
  //Find centroid or s-state, then fit 
  vector<double> centroidBE;
  vector<string> centroidNLJ; 
  
  centroidBE.clear();
  centroidNLJ.clear();
  
    
  V0 = 0.0;R0 = r0 * pow(A, 1./3.); a0 = A0;
  VSO = 0.0; RSO = rso * pow(A, 1./3.); aSO = aso;
  
  printf("================ Woods-Saxon parameters \n");
  printf(" A: %d, dr:%5.3f fm, nStep: %3d, range: %5.3fm \n", A, 0.1, 200, 20.); // default
  printf("  R0: %8.4f fm,  a0: %8.4f fm \n", R0, a0);
  printf(" RS0: %8.4f fm, aS0: %8.4f fm \n", RSO, aSO);
  printf("=======================\n");
  
  
  printf("############################## Fit V0 and Vso\n");

  double rmsMin = 1000;
  double V0best = V0ini;
  double Vsobest = VSOini;
  for( double v0 = -dV0; v0 <= dV0; v0 = v0 + 0.2   ){
    V0 = V0ini + v0;
    printf("---------------------\n");
    for( double vso = -dVSO; vso <= dVSO; vso = vso + 0.5 ){
      VSO = VSOini + vso;
      WS();
      //printf("    Experiment  |  Woods-Saxon   |\n");
      double rms = 0;
      for( int i = 0; i < NLJ.size(); i++){
        for( int j = 0; j < orbString.size(); j++){
          if( NLJ[i] == orbString[j] ){
            double diff = BE[i] -  energy[j];
            rms += pow(diff,2);
            //printf(" %d %6s (%9.6f) | %d %6s (%9.6f) | diff : %f \n",i,  NLJ[i].c_str(), BE[i], j, orbString[j].c_str(), energy[j], diff);
            continue;
          }
        }
      }
      rms = sqrt(rms);
      if( rms < rmsMin ) {
        rmsMin = rms;
        V0best = V0;
        Vsobest = VSO;
      }
      printf("V0 : %f, VSO : %f,  rms : %f \n", V0,  VSO, rms);
    }
  }
  printf("======= The best fit V0 : %f, VSO : %f , rms : %f \n", V0best, Vsobest, rmsMin);
  
  V0 = V0best;
  VSO = Vsobest;
  WS();
  printf("    Experiment  |  Woods-Saxon   |\n");
  for( int i = 0; i < NLJ.size(); i++){
    for( int j = 0; j < orbString.size(); j++){
      if( NLJ[i] == orbString[j] ){
        double diff = BE[i] -  energy[j];
        printf(" %d %6s (%9.6f) | %d %6s (%9.6f) | diff : %f \n",i,  NLJ[i].c_str(), BE[i], j, orbString[j].c_str(), energy[j], diff);
        continue;
      }
    }
  }
  
  
  printf("================ Woods-Saxon parameters \n");
  printf(" A: %d, dr:%5.3f fm, nStep: %3d, range: %5.3fm \n", A, 0.1, 300, 30.); // default
  printf("  V0: %8.4f MeV,  R0: %8.4f fm,  a0: %8.4f fm \n", V0, R0, a0);
  printf(" VSO: %8.4f MeV, RS0: %8.4f fm, aS0: %8.4f fm \n", VSO, RSO, aSO);
  printf("=======================\n");
  for( int i = 0; i < energy.size() ; i++){
    printf("%2d| %8s,  %12.6f | %12.5f | %12.7E | %12f\n",i, orbString[i].c_str(), energy[i], errorU[i], errorE[i], errorUratio[i]);  
  }
  
  
  return 0;
}

