/****************************************************************
  Fitting experimental energy with WS potential
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-1-31
*****************************************************************/

#include <stdio.h>
#include <string>
#include <stdlib.h> //for atoi, atof
#include <cmath> //exp
#include <vector> 
#include <fstream> 

#include "TMath.h"
#include "RK4.h"
#include "WS.h"
#include "matrix.h"
#include "matrix.cpp"

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

  if( argc != 9 && argc != 10 && argc != 11 && argc != 12) { 
    printf("Usage: ./WSFit expFile A V0 r0 a0 VSO rSO aSO nStep dr (showParErr)\n");
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
    printf("    showParErr = show Parameters Errors [1/0]\n");
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
  int nStep = 200;
  if( argc >= 10 ) nStep = atoi(argv[9]);
  double dr = 0.1;
  if( argc >= 11 ) dr = atof(argv[10]);
  bool showParErr = 0;
  if( argc >= 12 ) showParErr = atoi(argv[11]);
  
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
  
  WoodsSaxon ws;
  
  ws.V0 = V0ini ;    ws.a0  = A0;
  ws.VSO = VSOini ;  ws.aSO = aso;
  ws.SetWSRadius(A, r0, rso);
  
  ws.dr = dr; ws.nStep = nStep;
  
  ws.PrintWSParas();
  ws.CalWSEnergies();

  //==== Find parameters error
  if( showParErr ) {
    int fitSize = NLJ.size();
    int paraSize = 6; 
    Matrix Y(fitSize, 1);
    Matrix f(fitSize, 1);
    Matrix F(fitSize, paraSize); // F = grad(f)

    printf("---- calculating parameters errors ...\n");
    for( int i = 1; i <= fitSize; i++){
      Y(i,1) = BE[i-1];

      for( int j = 0; j < ws.orbString.size(); j++){
        if( NLJ[i-1] == ws.orbString[j] ){
          f(i,1) = ws.energy[j];
          break;
        }
      }
    }

    //Y.PrintM("Y");
    //f.PrintM("f");

    for( int i = 1; i <= fitSize; i++){

      //--- gradiant of V0
      //printf("====================== V0: %d \n", i);
      double dp = 0.001; // deviation of parameters
      ws.V0 = V0ini + dp;
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 1) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.V0 = V0ini;

      //--- gradiant of r0
      //printf("====================== r0: %d \n", i);
      dp = 0.01;
      ws.SetWSRadius(A, r0 + dp, rso);
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 2) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.SetWSRadius(A, r0, rso);
        
      //--- gradiant of a0
      //printf("====================== a0: %d \n", i);
      ws.a0 = A0 + dp;
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 3) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.a0 = A0;
    
      //--- gradiant of VSO
      //printf("====================== VSO: %d \n", i);
      dp = 0.01;
      ws.VSO = VSOini + dp;
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 4) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.VSO = VSOini;

      //--- gradiant of rSO
      //printf("====================== rSO: %d \n", i);
      dp = 0.01;
      ws.SetWSRadius(A, r0, rso + dp);
      //ws.PrintWSParas();
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 5) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.SetWSRadius(A, r0, rso);
        
      //--- gradiant of aSO
      //printf("====================== aSO: %d \n", i);
      ws.aSO = aso + dp;
      //ws.PrintWSParas();
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 6) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.aSO = aso;
    }

    Matrix Ft = F.Transpose();
    //F.PrintM("F");  
    Matrix FtF = Ft*F;
    //FtF.PrintM("FtF");
    Matrix CoVar = FtF.Inverse();
    //CoVar.PrintM("CoVar");
    Matrix dY = f - Y;
    //dY.PrintM("dY");
    double SSR = (dY.Transpose() * dY) (1,1);
    //printf(" === SSR : %f, RMS : %f \n", SSR, sqrt(SSR/fitSize));
    int DF = fitSize - paraSize;
    if( DF <= 0 ) {
      DF = 1;
      printf("degree of freedom is assumed to be 1\n");
    }
    printf("degree of freedom is %d \n", DF);
    Matrix dpar =  CoVar * SSR / DF;
    dpar.PrintM("VarMatrix");
    for( int i = 1; i <= paraSize-1; i++){
      printf(" %f,", sqrt(abs(dpar(i,1))));
    }
    printf(" %f\n", sqrt(abs(dpar(paraSize, 1))));
    
    //==== restore WS calcualtion
    ws.V0 = V0ini ;    ws.a0  = A0;
    ws.VSO = VSOini ;  ws.aSO = aso;
    ws.SetWSRadius(A, r0, rso);  
    ws.CalWSEnergies();
  }

  printf("############################## Calculate WS levels and compare\n");
  
  printf("      Experiment      |      Woods-Saxon      |     diff\n");
  double rms = 0;
  int numMatch = 0;
  for( int i = 0; i < NLJ.size(); i++){
    for( int j = 0; j < ws.orbString.size(); j++){
      if( NLJ[i] == ws.orbString[j] ){
        double diff = BE[i] -  ws.energy[j];
        rms += pow(diff,2);
        numMatch ++;
        printf(" %d %6s (%9.6f) | %d %6s (%9.6f) | %8.2f keV \n",i,  NLJ[i].c_str(), BE[i], j, ws.orbString[j].c_str(), ws.energy[j], diff * 1000.);
        continue;
      }
    }
  }
  
  if( numMatch != NLJ.size() ){
    rms = TMath::QuietNaN();
    printf("========================================= RMS = \e[31mfail\e[0m keV \n");
  }else{
    rms = sqrt(rms/NLJ.size());
    printf("========================================= RMS = \e[31m%8.2f\e[0m keV \n", rms*1000.);
  }

  ws.PrintEnergyLevels();
  
  
  return 0;
}

