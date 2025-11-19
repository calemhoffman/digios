#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TSelector.h"
#include "TMath.h"
#include "TBenchmark.h"
#include "TF1.h"
#include "TObjArray.h"
#include "TCutG.h"
#include "TClonesArray.h"
#include "TMacro.h"

#include <string>
#include <fstream>

void runsCheck2(TString prefix = "gen", int runID = -1){
   
  TString folderPath;
  if( runID == -1 ){
    folderPath.Form("../root_data/%s_run*.root", prefix.Data());
  }else{
    folderPath.Form("../root_data/%s_run%03d.root", prefix.Data(), runID);
  }
  const char* treeName="gen_tree";
   
  //==============================================
  
  printf("================================================\n");
  printf("  using the timing macro in the root files  \n");
  printf("================================================\n");
   
  TFile * f = NULL;
  TMacro * timing = NULL;

  printf("building fileList\n");
  TString cmd;
  cmd.Form(".!ls -1 %s | sort > foundRunList.txt", folderPath.Data());
  gROOT->ProcessLine(cmd);
   
  std::ifstream file;
  file.open("foundRunList.txt");
  std::vector<TString> rootFileName;
  if( file.is_open() ){
    std::string x;
    while( file >> x){
      rootFileName.push_back(x);
    }
  }else{
    printf("... fail\n");
    gROOT->ProcessLine(".!rm -f foundRunList.txt");
    return;
  }
   
  gROOT->ProcessLine(".!rm -f foundRunList.txt"); 
   
  FILE * paraOut;
  TString filename;
  
  if( runID == -1 ){
    filename.Form("run_Summary.txt");
    paraOut = fopen(filename.Data(), "w+");
  }
  int numFile = rootFileName.size();
   
  printf("%30s, %10s, %8s, %16s, %16s, %11s, %11s, %7s\n", "file",  "#event", "size[MB]", "start-TS", "end-TS", "duration[s]", "[min]", "[hour]");   
   
  for( int i = 0; i < numFile ; i++){
      
    if( runID == -1 ) f = new TFile (rootFileName[i], "read");
    if( runID > 0  ) f = new TFile (Form("../root_data/%s_run%03d.root", prefix.Data(), runID), "read");
      
    if( !f->IsOpen()) continue; 
      
    TMacro * timing = (TMacro *)f->Get("info");
    
    if( timing == NULL ) continue;
    
    TString t1 = timing->GetListOfLines()->At(0)->GetName();
    TString t2 = timing->GetListOfLines()->At(1)->GetName();
    TString nE = timing->GetListOfLines()->At(2)->GetName();

    // extract substring after '=' in t1
    Int_t pos = t1.Index("=");
    TString t1_after = t1(pos+1, t1.Length()-pos-1);
    ULong64_t firstTime = t1_after.Atoll();

    pos = t2.Index("=");
    TString t2_after = t2(pos+1, t2.Length()-pos-1);
    ULong64_t lastTime = t2_after.Atoll();
    
    pos = nE.Index("=");
    TString nE_after = nE(pos+1, nE.Length()-pos-1);
    int totalEvent = nE_after.Atoi();
      
    double time1 = firstTime/1e8;
    double time2 = lastTime/1e8;
    double dTime = (lastTime - firstTime)/1e8;
      
    double size = f->GetSize(); // in byte
    printf("%30s, %10d, %8.2f, %16llu, %16llu, %11.3f, %11.3f, %7.3f\n", rootFileName[i].Data(),  totalEvent, size/1024/1024, firstTime, lastTime, dTime, dTime/60., dTime/60./60.); 
      
    if( runID == -1 ){  
      if( i == 0 ) fprintf(paraOut, "%50s, %10s, %8s, %16s, %16s, %11s, %11s, %7s\n", 
                                    "file",  "#event", "size[MB]", "start-TS", "stop-TS", "duration[s]", "[min]", "[hour]");    
      fprintf(paraOut, "%50s, %10d, %8.2f, %16llu, %16llu, %11.3f, %11.3f, %11.3f\n", rootFileName[i].Data(),  totalEvent, size/1024/1024, firstTime, lastTime, dTime, dTime/60., dTime/60./60.);   
      fflush(paraOut);  
    } 
  }
    
  if ( runID == -1 ) {
    fclose(paraOut);
    printf("=========== saved summery to %s.\n", filename.Data());
  }
  
  gROOT->ProcessLine(".q");
   
}
