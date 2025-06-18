#include "BinaryReader.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TString.h"
#include "TMacro.h"

#include <sys/time.h> /** struct timeval, select() */
inline unsigned int getTime_us(){
  unsigned int time_us;
  struct timeval t1;
  struct timezone tz;
  gettimeofday(&t1, &tz);
  time_us = (t1.tv_sec) * 1000 * 1000 + t1.tv_usec;
  return time_us;
}

//^============================================
int main(int argc, char* argv[]) {

  printf("=======================================================\n");
  printf("===          Event Builder  raw data --> root       ===\n");
  printf("=======================================================\n");  

  if( argc <= 3){
    printf("%s [outfile] [timeWindow] [saveTrace] [sol-1] [sol-2] ... \n", argv[0]);
    printf("      outfile : output root file name\n");
    printf("   timeWindow : nano-sec; if < 0, no event build\n"); 
    printf("    saveTrace : 1 = save trace, 0 = no trace\n");
    printf("        sol-X : the sol file(s)\n");
    return -1;
  }

  unsigned int runStartTime = getTime_us();

  TString outFileName = argv[1];
  int timeWindow = atoi(argv[2]);
  const bool saveTrace = atoi(argv[3]);

  const int nFile = argc - 4;
  TString inFileName[nFile];
  for( int i = 0 ; i < nFile ; i++){
    inFileName[i] = argv[i+4];
  }

  //*=============== setup reader
  BinaryReader * reader = new BinaryReader[nFile];
  for( int i = 0 ; i < nFile ; i++){
    reader[i].Open(inFileName[i].Data());
    reader[i].Scan();
    const auto& hits = reader[i].GetHits();
    printf("File %s has %u hits\n", inFileName[i].Data(), reader[i].GetNumData());
  }



  return 0;
}