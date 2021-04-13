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

#include "../Armory/AnalysisLibrary.h"
using namespace std;

int main(int argc, char *argv[]){

  if( argc != 9 && argc != 11 && argc != 13 && argc != 14) { 
    printf("Usage: ./WSFit expFile A V0 r0 a0 VSO rSO aSO Z rc nStep dr (showParErr)\n");
    printf("       expFile = experimental energies\n");
    printf("             A = mass number\n");
    printf("            V0 = inital guess of central depth [MeV]\n");
    printf("            r0 = reduced radius for central potential [fm]\n");
    printf("            a0 = reduced radius for central potential [fm]\n");
    printf("           VSO = inital guess of Spin-Orbital depth [MeV]\n");
    printf("           rso = reduced radius for Spin-Orbital potential [fm]\n");
    printf("           aso = reduced radius for Spin-Orbital potential [fm]\n");
    printf("             Z = charge number, if for neutron, Z = 0.\n");
    printf("            rc = reduced radius for Coulomb potential [fm]\n");
    printf("         nStep = number of r-grid\n");
    printf("            dr = grid size [fm]\n");
    printf("    showParErr = show Parameters Errors [1/0]\n");
    exit(0); 
  }

  //======================== Read arguments
  string readFile = argv[1];
  int    A      = atoi(argv[2]);
  double V0ini  = atof(argv[3]);
  double r0     = atof(argv[4]);
  double A0     = atof(argv[5]);
  double VSOini = atof(argv[6]);
  double rso    = atof(argv[7]);
  double aso    = atof(argv[8]);
  
  int    Z      = 0;
  if( argc > 10 ) Z = atoi(argv[9]);

  double rc = 1.25;
  if( argc > 11 ) rc = atof(argv[10]);

  int nStep = 200;
  if( argc >= 12 ) nStep = atoi(argv[11]);

  double dr = 0.1;
  if( argc >= 13 ) dr = atof(argv[12]);

  bool showParErr = 0;
  if( argc >= 14 ) showParErr = atoi(argv[13]);

  //========================= Read input file
  ifstream file_in;
  file_in.open(readFile.c_str(), ios::in);

  if( !file_in ){
    printf(" cannot read file: %s. \n", readFile.c_str());
    return 0 ; 
  }
  
  vector<string> NLJ; ///orbital label
  vector<double> BE;  ///binding enegry of orbital
  vector<double> Error; ///error/uncertaintly of the binding energy
  
  NLJ.clear();
  BE.clear();
  Error.clear();
  
  while( file_in.good() ) {
    string tempLine;
    getline(file_in, tempLine );
    
    if( tempLine.substr(0, 1) == "#" ) continue;
    if( tempLine.size() < 1 ) continue;
    
    vector<string> str0 = SplitStr(tempLine, " ");
    if( str0.size() == 0 ) continue;
    
    NLJ.push_back(str0[0]);
    BE.push_back(atof(str0[1].c_str()));
    if( str0.size() == 3) Error.push_back(atof(str0[2].c_str()));
    
  }
  
  printf("============ Input from %s.\n", readFile.c_str());
  for( int i = 0; i < NLJ.size() ; i++){  
    printf("NLJ: %6s | %f \n", NLJ[i].c_str(), BE[i]);  
  }
  printf("==============================\n");


  //========================== Woods-Saxon Calucaltion
  WoodsSaxon ws;

  ws.SetNucleus(A, Z);
  Z == 0 ? ws.IsNeutron(): ws.IsProton();
  ws.SetWSPars(V0ini, r0, A0, VSOini, rso, aso, rc);
  ws.SetRange(0.0001, 0.0001 + dr * nStep, nStep);
  ws.SetBoundaryCondition(0.0, 1.0);
  
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

      ///--- gradiant of V0
      ///printf("====================== V0: %d \n", i);
      double dp = 0.001; // deviation of parameters
      ws.SetV0(V0ini + dp);
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 1) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.SetV0(V0ini);

      ///--- gradiant of r0
      ///printf("====================== r0: %d \n", i);
      dp = 0.01;
      ws.Setr0(r0 + dp);
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 2) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.Setr0(r0);
        
      ///--- gradiant of a0
      ///printf("====================== a0: %d \n", i);
      ws.Seta0( A0 + dp);
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 3) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.Seta0(A0);
    
      //--- gradiant of VSO
      //printf("====================== VSO: %d \n", i);
      dp = 0.01;
      ws.SetVSO(VSOini + dp);
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 4) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.SetVSO(VSOini);

      //--- gradiant of rSO
      //printf("====================== rSO: %d \n", i);
      dp = 0.01;
      ws.SetrSO(rso + dp);
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 5) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.SetrSO(rso);
        
      //--- gradiant of aSO
      //printf("====================== aSO: %d \n", i);
      ws.SetaSO(aso + dp);
      ws.CalWSEnergies();
      for( int j = 0; j < ws.orbString.size(); j++){
        //printf("%d %s %f \n", j, ws.orbString[j].c_str(), ws.energy[j]);
        if( NLJ[i-1] == ws.orbString[j] ){
          F(i, 6) = (ws.energy[j] - f(i,1))/dp;
          //printf("%f , %f | %f\n", ws.energy[j] , f(i,1) , F(i,1));
          break;
        }
      }
      ws.SetaSO(aso);
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
    ws.SetWSPars(V0ini, r0, A0, VSOini, rso, aso, rc);
    ws.CalWSEnergies();
  }

  printf("############################## Calculate WS levels and compare\n");
  
  printf("       Experiment      |      Woods-Saxon      |     diff\n");
  double rms = 0;
  double chisq = 0;
  int numMatch = 0;
  for( int i = 0; i < NLJ.size(); i++){
    for( int j = 0; j < ws.orbString.size(); j++){
      if( NLJ[i] == ws.orbString[j] ){
        double diff = BE[i] -  ws.energy[j];
        rms += pow(diff,2);
        if(Error.size() == NLJ.size() )chisq += pow(diff,2)/pow(Error[i],2);
        numMatch ++;
        printf(" %2d %6s (%9.6f) | %2d %6s (%9.6f) | %8.2f keV \n",i,  NLJ[i].c_str(), BE[i], j, ws.orbString[j].c_str(), ws.energy[j], diff * 1000.);
        continue;
      }
    }
  }
  
  if( numMatch != NLJ.size() ){
    rms = TMath::QuietNaN();
    printf("=========================================  RMS = \e[31mfail\e[0m keV \n");
  }else{
    rms = sqrt(rms/NLJ.size());
    printf("=========================================  RMS = \e[31m%8.2f\e[0m keV | Chi-sq = %8.2f.\n", rms*1000., chisq);
  }

  ws.PrintEnergyLevels();

  //============= Save radial function

  string saveFileName = readFile;
  saveFileName.erase(saveFileName.end()-4, saveFileName.end());
  saveFileName = saveFileName + ".WF.dat";

  ws.SaveWaveFunction(saveFileName);
  
  printf("========== end of program ======= \n");
  return 0;
}

