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
#include "../Armory/AnalysisLibrary.h"

using namespace std;

int main(int argc, char *argv[]){

  if(argc < 6 ) {
    //TODO put the mass number out of paraFile
    printf("Usage: ./WSSearch  expFile  A N Z  paraFile  outRoot\n");
    printf("       expFile = experimental energies\n");
    printf("             A = mass number\n");
    printf("             N = neutron number\n");
    printf("             Z = proton number [ ZERO for neutron levels, rc = r0 ]\n");
    printf("      paraFile = search setting\n");
    printf("       outRoot = output Root File name [empty for no saving ]\n");
    exit(0);   }

  string readFile = argv[1];
  int A  = atoi(argv[2]);
  int N  = atoi(argv[3]);
  int Z  = atoi(argv[4]);
  string paraFile = argv[5];

  string rootFile = "";
  bool saveRootFile = false;
  if( argc == 7 ) {
    rootFile = argv[6];
    saveRootFile = true;
  }

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

  TFile * fileOut = NULL;
  TTree * tree  = NULL;
  if( saveRootFile ){
    fileOut= new TFile(rootFile.c_str(), "recreate");
    tree = new TTree("tree", "tree");
  }
  
  int v0N = 0, r0N = 0, a0N = 0;
  int vsoN = 0, rsoN = 0, asoN = 0;
  
    
  for( double v0 = v0min; v0 <= v0max; v0 = v0 + v0step ) v0N++;
  for( double r0v = r0min ; r0v <= r0max; r0v = r0v + r0step) r0N ++;
  for( double a0v = a0min; a0v <= a0max; a0v = a0v + a0step) a0N++;
  for( double vso = vsomin; vso <= vsomax; vso = vso + vsostep ) vsoN++; 
  for( double rsov = rsomin ; rsov <= rsomax; rsov = rsov + rsostep) rsoN++ ;
  for( double asov = asomin; asov <= asomax; asov = asov + asostep) asoN++;

  TMacro searchRangeMsg;
  TString str;
  str.Form("============ Search Range : %s.", paraFile.c_str());
  searchRangeMsg.AddLine(str.Data());
  str.Form(" mass A : %d,  N : %d", A, N);
  searchRangeMsg.AddLine(str.Data());
  str.Form(" proton number : %d,  rc = r0", Z);
  searchRangeMsg.AddLine(str.Data());
  str.Form("  V0: (%9.4f, %9.4f) | step: %9.4f | %4d", v0min, v0max, v0step, v0N );
  searchRangeMsg.AddLine(str.Data());
  str.Form("  R0: (%9.4f, %9.4f) | step: %9.4f | %4d", r0min, r0max, r0step, r0N );
  searchRangeMsg.AddLine(str.Data());
  str.Form("  a0: (%9.4f, %9.4f) | step: %9.4f | %4d", a0min, a0max, a0step, a0N );
  searchRangeMsg.AddLine(str.Data());
  str.Form(" VSO: (%9.4f, %9.4f) | step: %9.4f | %4d", vsomin, vsomax, vsostep, vsoN );
  searchRangeMsg.AddLine(str.Data());
  str.Form(" RSO: (%9.4f, %9.4f) | step: %9.4f | %4d", rsomin, rsomax, rsostep, rsoN );
  searchRangeMsg.AddLine(str.Data());
  str.Form(" aSO: (%9.4f, %9.4f) | step: %9.4f | %4d", asomin, asomax, asostep, asoN );
  searchRangeMsg.AddLine(str.Data());
  str.Form("============ Total cal. : %d", v0N * r0N * a0N * vsoN * rsoN * asoN );
  searchRangeMsg.AddLine(str.Data());
  
  searchRangeMsg.Print();

  if( saveRootFile ) searchRangeMsg.Write("searchRange");
  
  
  WoodsSaxon ws;
  
  ws.SetNucleus(A, Z);
  ws.IsNeutron();
  double reducedMass = mn * A / (1+A);
  ws.SetMass( reducedMass );
  ws.SetRange2(0.0001, 0.1, 200);

  double V0, R0, r0, a0;
  double VSO, RSO, rSO, aSO;
  double rms = TMath::QuietNaN();

  if( saveRootFile ){
    tree->Branch("V0", &V0, "V0/D");
    tree->Branch("R0", &R0, "R0/D");
    tree->Branch("r0", &r0, "r0/D");
    tree->Branch("a0", &a0, "a0/D");
    tree->Branch("VSO", &VSO, "VSO/D");
    tree->Branch("RSO", &RSO, "RSO/D");
    tree->Branch("rSO", &rSO, "rSO/D");
    tree->Branch("aSO", &aSO, "aSO/D");
    tree->Branch("rms", &rms, "rms/D");
  }
  
  printf("############################## Fit V0 and Vso\n");

  double rmsMin = 1000;
  double V0best, R0best, r0best, a0best;
  double Vsobest, Rsobest, rsobest, asobest;
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

  for( double v0 = v0min; v0 <= v0max; v0 = v0 + v0step   ){    
    for( double r0v = r0min ; r0v <= r0max; r0v = r0v + r0step){
      for( double a0v = a0min; a0v <= a0max; a0v = a0v + a0step){
        for( double vso = vsomin; vso <= vsomax; vso = vso + vsostep ){
          for( double rsov = rsomin ; rsov <= rsomax; rsov = rsov + rsostep){
            for( double asov = asomin; asov <= asomax; asov = asov + asostep){

              ws.SetWSPars(v0, r0v, a0v, vso, rsov, asov, r0v);

              V0 = v0;
              R0 = ws.GetR0();
              r0 = r0v;
              a0 = a0v;

              VSO = vso;
              RSO = ws.GetRSO();
              rSO = rsov;
              aSO = asov;

              
              //############## Display
              if( count % 10 == 0) {
                system("clear");              
                double expTime = stpWatch.RealTime()* numCal/count / 60.;
                printf("============ Input from %s.\n", readFile.c_str());
                for( int i = 0; i < NLJ.size() ; i++) printf("NLJ: %6s | %f \n", NLJ[i].c_str(), BE[i]);    
                printf("==============================\n");
                searchRangeMsg.Print();
                printf("==============================\n");
                printf("%7d/%7d [%4.1f%%]| %6.0f[%6.0f] min\n",  count, numCal, count*100./numCal, stpWatch.RealTime()/60., expTime);
                printf("==============================\n");
                stpWatch.Start(kFALSE);            
                printf("%10s|%7s, %7s, %7s, %7s, %7s, %7s, %7s, %7s| %7s\n", "", "V0", "R0", "r0", "a0", "VSO", "RSO", "rSO", "aSO", "rms");
                printf("%10s|%7.2f, %7.5f, %7.5f, %7.5f, %7.2f, %7.5f, %7.5f, %7.5f| %f\e[0m\n", "current", ws.GetV0(), ws.GetR0(), ws.Getr0(), ws.Geta0(), ws.GetVSO(), ws.GetRSO(), ws.GetrSO(), ws.GetaSO(), rms);
                printf("%10s|\e[31m%7.2f, %7.5f, %7.5f, %7.5f, %7.2f, %7.5f, %7.5f, %7.5f| %f\e[0m\n", "Best", V0best, R0best, r0best, a0best, Vsobest, Rsobest, rsobest, asobest, rmsMin);
                printf("==============================\n");
                printf("      Experiment      |       Woods-Saxon    |\n");
                for( int i = 0; i < NLJ.size(); i++){
                  for( int j = 0; j < orbStringBest.size(); j++){
                    if( NLJ[i] == orbStringBest[j] ){
                      double diff = BE[i] -  energyBest[j];
                      printf(" %d %6s (%9.5f) | %d %6s (%9.5f) | diff : %f \n",i,  NLJ[i].c_str(), BE[i], j, orbStringBest[j].c_str(), energyBest[j], diff);
                      continue;
                    }
                  }
                }
              }
              //################ end of display

              ws.CalWSEnergies();
              
              count++;
              rms = 0;
              int nDiff = 0;
              for( int i = 0; i < NLJ.size(); i++){
                for( int j = 0; j < ws.orbString.size(); j++){
                  if( NLJ[i] == ws.orbString[j] ){
                    double diff = BE[i] -  ws.energy[j];
                    rms += pow(diff, 2);
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
                V0best = V0;
                R0best = R0;
                r0best = r0;
                a0best = a0;
                Vsobest = VSO;
                Rsobest = RSO;
                rsobest = rSO;
                asobest = aSO;
                energyBest = ws.energy;
                orbStringBest = ws.orbString;
               
                //ws.PrintWSParas();
                //ws.PrintEnergyLevels();
                //printf("%10s|\e[31m%7.2f, %7.5f, %7.5f, %7.5f, %7.2f, %7.5f, %7.5f, %7.5f| %f\e[0m\n", "Best", V0best, R0best, r0best, a0best, Vsobest, Rsobest, rsobest, asobest, rmsMin);
                //sleep(2);
              }
              if( saveRootFile ) tree->Fill();
              
            }// end of VSO loop
          }// end of V0 loop
          if( saveRootFile ) tree->Write("tree", TObject::kOverwrite);
        }// end of aso loop
      }// end of Rso loop
    }// end of a0 loop
  }// end of R0 loop

  if( saveRootFile ) {
    fileOut->Write();
    fileOut->Close();
  }

  ws.SetV0  ( V0best  );
  ws.SetR0  ( R0best  );
  ws.Seta0  ( a0best  );
  ws.SetVSO ( Vsobest );
  ws.SetRSO ( Rsobest );
  ws.SetaSO ( asobest );
  ws.SetRc  ( R0best  );
  
  ws.CalWSEnergies();
  printf("================ Best Fit Woods-Saxon parameters \n");
  ws.PrintWSParas();
    
  printf("      Experiment    |    Woods-Saxon     |\n");
  rms = 0;
  for( int i = 0; i < NLJ.size(); i++){
    for( int j = 0; j < ws.orbString.size(); j++){
      if( NLJ[i] == ws.orbString[j] ){
        double diff = (BE[i] -  ws.energy[j]) * 1000.;
        rms += pow(diff,2);
        printf(" %d %6s (%9.6f) | %d %6s (%9.6f) | diff : %7.2f keV \n",i,  NLJ[i].c_str(), BE[i], j, ws.orbString[j].c_str(), ws.energy[j], diff);
        continue;
      }
    }
  }
  rms = rms/(int)NLJ.size();
  rms = sqrt(rms);
  printf("                                                 rms : \033[0;31m%7.2f\033[0m keV\n",rms);
  ws.PrintEnergyLevels();
  
  
  return 0;
}

