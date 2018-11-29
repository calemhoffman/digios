/***********************************************************************
 * 
 *  This is Cleopatra, a sucessor of Ptolemy
 *          only for (d,p), (d,p), (d,d), or (p,p)
 * 
 *  1) it read a simple infile.in from reaction_setting file
 *  2) use Ptolemy to calculate the the creation
 *  3) extract the cross sec distribution into txt and root file
 * 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ cleopatra.C -o cleopatra `root-config --cflags --glibs`
 *
 *------------------------------------------------------
 *  The reaction_setting file is simple like:
 *
 *  206Hg(d,p)207Hg(1s1/2 0.000) 10MeV/u AK
 *
 *  the first is similar to usual reaction setting, the word AK is a
 *     short name for Optical Potential, user can put as many line as
 *     they like, Cleopatra can create the suitable infile.in for Ptolomy
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>     /* atof */
#include <cmath>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include "InFileCreator.h"
#include "ExtractXSec.h"
#include <TApplication.h>
#include "PlotTGraphTObjArray.h"

using namespace std;

int main (int argc, char *argv[]) { //TODO add angle range
   
  printf("=================================================================\n");
  printf("=====  Cleopatra, Ptolemy for (d,p),(p,d), (p,p) and (d,d)  =====\n");
  printf("=================================================================\n");

  if(argc < 2 || argc > 5) { 
    printf("Usage: ./Cleopatra input_file (angMin = 0 deg, angMax = 180 deg, angStep = 1 deg)\n");
    printf("Usage: ./Cleopatra input_file angMin angMax (angStep = 1 deg)\n");
    printf("Usage: ./Cleopatra input_file angMin angMax angStep\n");
    exit(0); 
  }else{
    printf("From file : %s \n", argv[1]);
  }

  //================= read infile. extract the reactions, write pptolemy infile for each reaction
  string readFile = argv[1];
  double angMin = 0.;
  double angMax = 180.;
  double angStep = 1.;
  if( argc >= 4 ){
    angMin = atof(argv[2]);
    angMax = atof(argv[3]);    
  }
  if( argc == 5 ){
    angStep = atof(argv[4]);
  }
  
  string ptolemyInFileName = argv[1];
  ptolemyInFileName += ".in";
  printf("=================== Create InFile\n");
  InFileCreator( readFile, ptolemyInFileName, angMin, angMax, angStep);
  
  //================= run ptolemy
  
  char command[200];
  string ptolemyOutFileName = argv[1];
  ptolemyOutFileName += ".out";
  sprintf(command, "./ptolemy <%s> %s", ptolemyInFileName.c_str(),  ptolemyOutFileName.c_str());
  printf("=================== Run Ptolemy\n");
  printf("%s \n", command);
  system(command);

  //================= extract the Xsec and save as txt and root
  printf("=================== Extract Cross-section\n");
  ExtractXSec(ptolemyOutFileName);

  //================= Call root to plot the d.s.c.
  printf("=================== Plot Result using ROOT.\n");
  string rootFileName = argv[1];
  rootFileName += ".root";
  TApplication app ("app", &argc, argv);
  PlotTGraphTObjArray(rootFileName);
  app.Run(); //nothing run after
  return 0;
} 
