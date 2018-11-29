/***********************************************************************
 * 
 *  This is PlotResultInRoot.C for ExtractXSec *.root output
 * 
 *  The Xsec are stored in (TObjArray *) gList
 * 
 *  This program is simple get plot all the member in the gList 
 * 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ PlotResultInROOT.C -o PlotResultInROOT `root-config --cflags --glibs`
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>
#include <TApplication.h>
#include "PlotTGraphTObjArray.h"
using namespace std;


int main (int argc, char *argv[]) {
  
  printf("=================================================================\n");
  printf("==================== Plot Results in ROOT =======================\n");
  printf("=================================================================\n");

  if(argc != 2) { 
    printf("Usage: ./PlotTGraphTObjArray root_file\n");
    exit(0); 
  }else{
    printf("From file : %s \n", argv[1]);
    printf("=========== Press Ctrl+C to end.\n");
  }
  
  string readFile = argv[1];
  
  TApplication app ("app", &argc, argv);

  PlotTGraphTObjArray(readFile);
  
  app.Run(); //anything after this line is not running
  
  return 0;

} 
