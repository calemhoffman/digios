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

#include <TROOT.h> 
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TStopwatch.h>
#include <TMacro.h>

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

  if(argc != 5 ) {
    //TODO put the mass number out of paraFile
    printf("Usage: ./WSSearch expFile A paraFile outRoot\n");
    printf("       expFile = experimental energies\n");
    printf("             A = mass number\n");
    printf("      paraFile = search setting\n");
    printf("       outRoot = output Root File name\n");
    exit(0);   }

  string readFile = argv[1];
  int A  = atoi(argv[2]);
  string paraFile = argv[3];
  string rootFile = argv[4];

  //========= reading expFile
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

  file_in.close();
  
  printf("============ Input from %s.\n", readFile.c_str());
  for( int i = 0; i < NLJ.size() ; i++){
    printf("NLJ: %6s | %f \n", NLJ[i].c_str(), BE[i]);    
  }
  printf("==============================\n");

  //========= reading paraFile
  ifstream file_para;
  file_para.open(paraFile.c_str(), ios::in);

  if( !file_para ){
    printf(" cannot read file: %s. \n", paraFile.c_str());
    return 0 ;
  }

  double v0step, v0min, v0max;
  double r0step, r0min, r0max;
  double a0step, a0min, a0max;
  double vsostep, vsomin, vsomax;
  double rsostep, rsomin, rsomax;
  double asostep, asomin, asomax;

  int i = 0;
  while( file_para.good() ) {
    string tempLine;
    getline(file_para, tempLine );

    if( tempLine.substr(0, 1) == "#" ) continue;
    if( tempLine.size() < 1 ) continue;
    
    vector<string> str0 = SplitStr(tempLine, " ");
    if( str0.size() == 0 ) continue;

    //if( i == 0 ) A = atoi(str0[0].c_str());
    if( i == 0  ) v0min  = atof(str0[0].c_str());
    if( i == 1  ) v0max  = atof(str0[0].c_str());
    if( i == 2  ) v0step = atof(str0[0].c_str());
    if( i == 3  ) r0min  = atof(str0[0].c_str());
    if( i == 4  ) r0max  = atof(str0[0].c_str());
    if( i == 5  ) r0step = atof(str0[0].c_str());
    if( i == 6  ) a0min  = atof(str0[0].c_str());
    if( i == 7  ) a0max  = atof(str0[0].c_str());
    if( i == 8  ) a0step = atof(str0[0].c_str());

    if( i == 9  ) vsomin  = atof(str0[0].c_str());
    if( i == 10  ) vsomax  = atof(str0[0].c_str());
    if( i == 11  ) vsostep = atof(str0[0].c_str());
    if( i == 12  ) rsomin  = atof(str0[0].c_str());
    if( i == 13  ) rsomax  = atof(str0[0].c_str());
    if( i == 14  ) rsostep = atof(str0[0].c_str());
    if( i == 15  ) asomin  = atof(str0[0].c_str());
    if( i == 16  ) asomax  = atof(str0[0].c_str());
    if( i == 17  ) asostep = atof(str0[0].c_str());
    
    i++;
  }

  file_para.close();
  
  printf("============ Search Range : %s.\n", paraFile.c_str());
  printf(" mass A : %d \n", A);
  printf("  V0: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", v0min, v0max, v0step, (v0max-v0min)/v0step +1 );
  printf("  R0: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", r0min, r0max, r0step, (r0max-r0min)/r0step +1 );
  printf("  a0: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", a0min, a0max, a0step, (a0max-a0min)/a0step +1 );
  printf(" VSO: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", vsomin, vsomax, vsostep, (vsomax-vsomin)/vsostep +1 );
  printf(" RSO: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", rsomin, rsomax, rsostep, (rsomax-rsomin)/rsostep +1 );
  printf(" aSO: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", asomin, asomax, asostep, (asomax-asomin)/asostep +1 );
  printf("==============================\n");


  TFile * fileOut = new TFile(rootFile.c_str(), "recreate");
  TTree * tree = new TTree("tree", "tree");

  TMacro searchFile(argv[3]);
  searchFile.Write("searchPara");
  
  WoodsSaxon ws;

  double rms = TMath::QuietNaN();
  tree->Branch("V0", &ws.V0, "V0/D");
  tree->Branch("R0", &ws.R0, "R0/D");
  tree->Branch("r0", &ws.r0, "r0/D");
  tree->Branch("a0", &ws.a0, "a0/D");
  tree->Branch("VSO", &ws.VSO, "VSO/D");
  tree->Branch("RSO", &ws.RSO, "RSO/D");
  tree->Branch("rSO", &ws.rSO, "rSO/D");
  tree->Branch("aSO", &ws.aSO, "aSO/D");
  tree->Branch("rms", &rms, "rms/D");

  //PrintWSPara(A, 0.1, 200);
  
  printf("############################## Fit V0 and Vso\n");

  double rmsMin = 1000;
  double V0best, R0best, a0best;
  double Vsobest, Rsobest, asobest;
  vector<double> energyBest;
  vector<string> orbStringBest;
  
  // count number of Calculation
  int numCal = 0;
  for( double r0v = r0min ; r0v <= r0max; r0v = r0v + r0step){
    for( double a0v = a0min; a0v <= a0max; a0v = a0v + a0step){
      for( double rsov = rsomin ; rsov <= rsomax; rsov = rsov + rsostep){
        for( double asov = asomin; asov <= asomax; asov = asov + asostep){
          for( double vso = vsomin; vso <= vsomax; vso = vso + vsostep ){
            for( double v0 = v0min; v0 <= v0max; v0 = v0 + v0step ){            
                numCal ++;
            }// end of VSO loop
          }// end of V0 loop
        }// end of aso loop
      }// end of Rso loop
    }// end of a0 loop
  }// end of R0 loop

  //========= real calculation
  TStopwatch stpWatch;
  stpWatch.Start();
  int count = 0;
  for( double r0v = r0min ; r0v <= r0max; r0v = r0v + r0step){
    ws.R0 = r0v * pow(A, 1./3.); 
    ws.r0 = r0v;
    //printf("---------------------R0  : %f fm (%f fm)\n", R0, r0);
    for( double a0v = a0min; a0v <= a0max; a0v = a0v + a0step){
      ws.a0 = a0v;
      //printf("---------------------a0  : %f fm\n", a0);
      for( double rsov = rsomin ; rsov <= rsomax; rsov = rsov + rsostep){
        ws.RSO = rsov * pow(A, 1./3.); 
        ws.rSO = rsov;
        //printf("---------------------RSO : %f fm (%f fm)\n", RSO, rSO);        
        for( double asov = asomin; asov <= asomax; asov = asov + asostep){
          ws.aSO = asov;
          //printf("---------------------aSO : %f fm\n", aSO);
          for( double v0 = v0min; v0 <= v0max; v0 = v0 + v0step   ){
            ws.V0 = v0;
            double expTime = stpWatch.RealTime()* numCal/count / 60.;
            system("clear");
            printf("============ Input from %s.\n", readFile.c_str());
            for( int i = 0; i < NLJ.size() ; i++){
              printf("NLJ: %6s | %f \n", NLJ[i].c_str(), BE[i]);    
            }
            printf("==============================\n");
            printf("============ Search Range : %s.\n", paraFile.c_str());
            printf(" mass A : %d \n", A);
            printf("  V0: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", v0min, v0max, v0step, (v0max-v0min)/v0step +1 );
            printf("  r0: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", r0min, r0max, r0step, (r0max-r0min)/r0step +1 );
            printf("  a0: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", a0min, a0max, a0step, (a0max-a0min)/a0step +1 );
            printf(" VSO: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", vsomin, vsomax, vsostep, (vsomax-vsomin)/vsostep +1 );
            printf(" rSO: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", rsomin, rsomax, rsostep, (rsomax-rsomin)/rsostep +1 );
            printf(" aSO: (%9.4f, %9.4f) | step: %9.4f | %4.0f\n", asomin, asomax, asostep, (asomax-asomin)/asostep +1 );
            printf("==============================\n");
            printf("%7d/%7d [%4.1f%%]| %6.0f[%6.0f] min | %f\n", 
                   count, numCal, count*100./numCal, stpWatch.RealTime()/60., expTime, rmsMin);
            printf("==============================\n");
            stpWatch.Start(kFALSE);            
            printf("r0 : %f fm, rSO : %f fm\n", ws.r0, ws.rSO);
            printf("======= Current calculation \n");
            printf("%7s, %7s, %7s, %7s, %7s, %7s| %7s\n", "V0", "R0", "a0", "VSO", "RSO", "aSO", "rms");
            printf("%7.2f, %7.5f, %7.5f, %7.2f, %7.5f, %7.5f| %f\e[0m\n", ws.V0, ws.R0, ws.a0, ws.VSO, ws.RSO, ws.aSO, rms);
            
            printf("======= Best parameters \n");
            printf("\e[31m%7.2f, %7.5f, %7.5f, %7.2f, %7.5f, %7.5f| %f\e[0m\n", 
                       V0best, R0best, a0best, Vsobest, Rsobest, asobest, rmsMin);
            printf("    Experiment  |  Woods-Saxon   |\n");
            for( int i = 0; i < NLJ.size(); i++){
              for( int j = 0; j < orbStringBest.size(); j++){
                if( NLJ[i] == orbStringBest[j] ){
                  double diff = BE[i] -  energyBest[j];
                  printf(" %d %6s (%9.6f) | %d %6s (%9.6f) | diff : %f \n",i,  NLJ[i].c_str(), BE[i], j, orbStringBest[j].c_str(), energyBest[j], diff);
                  continue;
                }
              }
            }
            for( double vso = vsomin; vso <= vsomax; vso = vso + vsostep ){
              ws.VSO = vso;
              ws.CalWSEnergies();
              count++;
              rms = 0;
              int nDiff = 0;
              for( int i = 0; i < NLJ.size(); i++){
                for( int j = 0; j < ws.orbString.size(); j++){
                  if( NLJ[i] == ws.orbString[j] ){
                    double diff = BE[i] -  ws.energy[j];
                    rms += pow(diff,2);
                    nDiff ++;
                    continue;
                  }
                }
              }
              if( rms == 0 || nDiff != NLJ.size() ){
                rms = TMath::QuietNaN();
              }else{
                rms = sqrt(rms/NLJ.size());
              }
              if( rms < rmsMin ) {
                rmsMin = rms;
                V0best = ws.V0;
                R0best = ws.R0;
                a0best = ws.a0;
                Vsobest = ws.VSO;
                Rsobest = ws.RSO;
                asobest = ws.aSO;
                energyBest = ws.energy;
                orbStringBest = ws.orbString;
              }
              tree->Fill();
              tree->Write("tree", TObject::kOverwrite);
            }// end of VSO loop
          }// end of V0 loop
        }// end of aso loop
      }// end of Rso loop
    }// end of a0 loop
  }// end of R0 loop
  fileOut->Write();
  fileOut->Close();

  ws.V0 = V0best;
  ws.R0 = R0best;
  ws.a0 = a0best;
  ws.VSO = Vsobest;
  ws.RSO = Rsobest;
  ws.aSO = asobest;
  ws.CalWSEnergies();
  printf("================ Best Fit Woods-Saxon parameters \n");
  ws.PrintWSParas();
    
  printf("    Experiment  |  Woods-Saxon   |\n");
  for( int i = 0; i < NLJ.size(); i++){
    for( int j = 0; j < ws.orbString.size(); j++){
      if( NLJ[i] == ws.orbString[j] ){
        double diff = BE[i] -  ws.energy[j];
        printf(" %d %6s (%9.6f) | %d %6s (%9.6f) | diff : %f \n",i,  NLJ[i].c_str(), BE[i], j, ws.orbString[j].c_str(), ws.energy[j], diff);
        continue;
      }
    }
  }
  
  ws.PrintEnergyLevels();
  
  
  return 0;
}

