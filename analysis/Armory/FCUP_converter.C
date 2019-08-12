#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

void FCUP_converter(TString FCUPFile){

  ifstream file;

  file.open(FCUPFile.Data());

  int nLine = 0;
  
  TString saveFileName= FCUPFile;
  int found ;
  do{
    found = saveFileName.First("/");
    saveFileName.Remove(0, found + 1);
  }while( found >= 0 );
  saveFileName = saveFileName +  ".root";
  
  printf("save to : %s \n", saveFileName.Data());

  TFile * saveFile = new TFile(saveFileName, "recreate");
  TTree * tree = new TTree("tree", "tree");

  ULong64_t timeStamp;
  float fCup;
  tree->Branch("f_t", &timeStamp, "timeStamp/l");
  tree->Branch("f", &fCup, "fCup/F");

  int splitPos = 26;

  while( file.good() ){

    string tempLine;
    getline(file, tempLine);
    nLine ++;
    if(nLine % 100000 == 0) printf("%d, %s \n", nLine, tempLine.c_str());

    if( tempLine.length() == 0 ) continue;

    if( tempLine.length() < 26) {
      splitPos = 18;
      //printf("%d, %s | %d\n", nLine, tempLine.c_str(), tempLine.length() );
    }else{
      splitPos = 26;
    }
    string time = tempLine.substr(0, splitPos);
    fCup = atof(tempLine.substr(splitPos).c_str());

    //convert date to 10ns.
    string date = time.substr(0, 11);
    int hour = atoi(time.substr(11, 2).c_str());
    int minute = atoi(time.substr(14, 2).c_str());
    float second = atof(time.substr(17).c_str());

    //printf("\033[0;0H%d, %s \n", nLine, tempLine.c_str());

    int day = atoi(date.substr(8, 2).c_str());
    
    //for 207Hg 
    if( day >= 10 ) {
      //offset
      ULong64_t offset1 = ( 12 * 24. * 60. * 60. + 11 * 60. * 60. + 50 * 60. + 29 ) * 1e8;
      ULong64_t offset2 = 23806029991042; //timeStamp for run41
      
      timeStamp = ( day * 24. * 60. * 60. +  hour * 60. * 60. + minute * 60. + second ) * 1e8;

      timeStamp = timeStamp - offset1 + offset2;
      //printf("======= %d, %s, %15lld, %f\n", nLine,  tempLine.c_str(),  timeStamp, fCup);
      tree->Fill();
      //tree->Write("tree", TObject::kOverwrite);
    }
  }

  tree->Write("tree", TObject::kOverwrite);
  saveFile->Close();

}
