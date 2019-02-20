/***********************************************************************
 * 
 *  This is Transfer.C for simulation of transfer reaction.
 * 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ Transfer.C -o Transfer `root-config --cflags --glibs`
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Feb-4, 2019
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>
#include "Transfer.h"

using namespace std;

int main (int argc, char *argv[]) {
  
  printf("=================================================================\n");
  printf("==========     Simulate Transfer reaction in HELIOS    ==========\n");
  printf("=================================================================\n");
  
  if(argc == 2) { 
    printf("Usage: ./Transfer [1] [2] [3] [4] [5] \n");
    printf("       default file name \n");
    printf("   [1] reactionConfig.txt (intput) reaction Setting \n");
    printf("   [2] detectorGeo.txt    (intput) detector Setting \n");
    printf("   [3] Ex.txt             (intput) Excitation energies \n");
    printf("   [4] DWBA.root          (intput) thetaCM distribution from DWBA \n");
    printf("   [5] transfer.root      (output) rootFile name for output \n");
    printf("   [6] reaction.dat       (output) Key reaction parameters \n");
    printf("------------------------------------------------------\n");
    return 0 ; 
  }

  string       basicConfig = "reactionConfig.txt";
  string  heliosDetGeoFile = "detectorGeo.txt";
  string    excitationFile = "Ex.txt"; //when no file, only ground state
  TString      ptolemyRoot = "DWBA.root"; // when no file, use isotropic distribution of thetaCM
  TString     saveFileName = "transfer.root";
  TString         filename = "reaction.dat"; //when no file, no output
   
  if( argc == 2){
    basicConfig = argv[1];
    heliosDetGeoFile = "";
    excitationFile = "";
    filename = "";
    ptolemyRoot = "";
  }
  
  if( argc == 3){
    basicConfig = argv[1];
    heliosDetGeoFile = argv[2];
    excitationFile = "";
    filename = "";
    ptolemyRoot = "";
  }
  
  if( argc == 4) {
    basicConfig = argv[1];
    heliosDetGeoFile = argv[2];
    excitationFile = argv[3];
    filename = "";
    ptolemyRoot = "";
  }
  
  if( argc == 5) {
    basicConfig = argv[1];
    heliosDetGeoFile = argv[2];
    excitationFile = argv[3];
    ptolemyRoot = argv[4];
    filename = "";
    ptolemyRoot = "";
  }
  
  if( argc == 6) {
    basicConfig = argv[1];
    heliosDetGeoFile = argv[2];
    excitationFile = argv[3];
    ptolemyRoot = argv[4];
    saveFileName = argv[5];
    filename = "";
  }
  
  if( argc == 7) {
    basicConfig = argv[1];
    heliosDetGeoFile = argv[2];
    excitationFile = argv[3];
    ptolemyRoot = argv[4];
    saveFileName = argv[5];
    filename = argv[6];
  }
  
  //printf(" %s \n", basicConfig.c_str());
  //printf(" %s \n", heliosDetGeoFile.c_str());
  //printf(" %s \n", excitationFile.c_str());
  //printf(" %s \n", saveFileName.Data());
  //printf(" %s \n", ptolemyRoot.Data());
  //printf(" %s \n", filename.Data());
  
  Transfer( basicConfig, heliosDetGeoFile, excitationFile, ptolemyRoot, saveFileName,  filename);
  
}
