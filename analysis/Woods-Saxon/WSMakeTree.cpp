/****************************************************************
  Generate the tree for all WS parameters
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-3-22
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
#include <TString.h>
#include <TStopwatch.h>
#include <TBenchmark.h>
#include <TMacro.h>


TBenchmark gClock;
Bool_t shown = 0;

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
    printf("Usage: ./WSMakeTree  A  Z  paraFile  outRoot\n");
    printf("             A = mass number\n");
    printf("             Z = proton number [ ZERO for neutron level ]\n");
    printf("      paraFile = search setting\n");
    printf("       outRoot = output Root File name\n");
    exit(0);   }

  int A  = atoi(argv[1]);
  int Z  = atoi(argv[2]);
  string paraFile = argv[3];
  string rootFile = argv[4];

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
  
  
  int v0N = 0, r0N = 0, a0N = 0;
  int vsoN = 0, rsoN = 0, asoN = 0;
  
    
  for( double v0 = v0min; v0 <= v0max; v0 = v0 + v0step ) v0N++;
  for( double r0v = r0min ; r0v <= r0max; r0v = r0v + r0step) r0N ++;
  for( double a0v = a0min; a0v <= a0max; a0v = a0v + a0step) a0N++;
  for( double vso = vsomin; vso <= vsomax; vso = vso + vsostep ) vsoN++; 
  for( double rsov = rsomin ; rsov <= rsomax; rsov = rsov + rsostep) rsoN++ ;
  for( double asov = asomin; asov <= asomax; asov = asov + asostep) asoN++;

  
  //printf("============ Search Range : %s.\n", paraFile.c_str());
  //printf(" mass A : %d \n", A);
  //printf("  V0: (%9.4f, %9.4f) | step: %9.4f | %4d\n", v0min, v0max, v0step, v0N );
  //printf("  R0: (%9.4f, %9.4f) | step: %9.4f | %4d\n", r0min, r0max, r0step, r0N );
  //printf("  a0: (%9.4f, %9.4f) | step: %9.4f | %4d\n", a0min, a0max, a0step, a0N );
  //printf(" VSO: (%9.4f, %9.4f) | step: %9.4f | %4d\n", vsomin, vsomax, vsostep, vsoN );
  //printf(" RSO: (%9.4f, %9.4f) | step: %9.4f | %4d\n", rsomin, rsomax, rsostep, rsoN );
  //printf(" aSO: (%9.4f, %9.4f) | step: %9.4f | %4d\n", asomin, asomax, asostep, asoN );
  
  TFile * fileOut = new TFile(rootFile.c_str(), "recreate");
  TTree * tree = new TTree("tree", "tree");

  //TMacro searchFile(argv[2]);
  //searchFile.Write("searchPara");
  
  TMacro searchRangeMsg;
  TString str;
  str.Form("============ Search Range : %s.", paraFile.c_str());
  searchRangeMsg.AddLine(str.Data());
  str.Form(" mass A : %d", A);
  searchRangeMsg.AddLine(str.Data());
  str.Form(" proton number Z : %d,  rc = r0", Z);
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
  searchRangeMsg.Write("searchRange");
  
  WoodsSaxon ws;

  double r0, rSO;
  tree->Branch("V0", &ws.V0, "V0/D");
  tree->Branch("R0", &ws.R0, "R0/D");
  tree->Branch("r0", &ws.r0, "r0/D");
  tree->Branch("a0", &ws.a0, "a0/D");
  tree->Branch("VSO", &ws.VSO, "VSO/D");
  tree->Branch("RSO", &ws.RSO, "RSO/D");
  tree->Branch("rSO", &ws.rSO, "rSO/D");
  tree->Branch("aSO", &ws.aSO, "aSO/D");
  
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
  
  printf("============================== total calculation: %d\n", numCal);

  //========= real calculation
  printf("############################## Creating tree\n");
  gClock.Reset();
  gClock.Start("timer");
  
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
        //printf("---------------------RSO : %f fm (%f fm)\n", RSO, rso);        
        for( double asov = asomin; asov <= asomax; asov = asov + asostep){
          ws.aSO = asov;
          //printf("---------------------aSO : %f fm\n", aSO);
          for( double v0 = v0min; v0 <= v0max; v0 = v0 + v0step   ){
            ws.V0 = v0;
            for( double vso = vsomin; vso <= vsomax; vso = vso + vsostep ){
              ws.VSO = vso;
              count++;
              
              
              tree->Fill();
              gClock.Stop("timer");
              Double_t time = gClock.GetRealTime("timer");
              gClock.Start("timer");
              
              if ( !shown ) {
                if (fmod(time, 10) < 1 ){
                   printf( "%10d[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
                         count, 
                         TMath::Nint((count+1)*100./numCal),
                         TMath::Floor(time/60.), 
                         time - TMath::Floor(time/60.)*60.,
                         numCal*time/(count+1.)/60.);
                   shown = 1;
                   tree->Write("tree", TObject::kOverwrite);
                }
              }else{
                if (fmod(time, 10) > 9 ){
                   shown = 0;
                }
              }
            }// end of VSO loop
          }// end of V0 loop
        }// end of aso loop
      }// end of Rso loop
    }// end of a0 loop
  }// end of R0 loop
  fileOut->Write();
  
  
  printf("======== Done. Number of tree entries : %lld\n", tree->GetEntries());
  
  fileOut->Close();
  
  return 0;
}

