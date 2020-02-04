/***********************************************************************
 * 
 *  This is ExtractXSecFromText for *.txt file
 *
 *  This program will extract cross section distribution
 *  save as  *.root for distribution
 * 
 *  save *.Ex.txt for excitation energies and total X-section 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ ExtractXSecFromText.C -o ExtractXSecFromText `root-config --cflags --glibs`
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Dec-29, 2019
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>
#include "ExtractXSec.h"

using namespace std;

int main (int argc, char *argv[]) {
   
  printf("=================================================================\n");
  printf("==========   Extract Cross-Section From text file    ==========\n");
  printf("=================================================================\n");

  if(argc < 2 ) { 
    printf("Usage: ./ExtractXSecFromText input_file\n");
    exit(0); 
  }else{
    printf("From file : %s \n", argv[1]);
  }

  string readFile = argv[1];
  ExtractXSecFromText(readFile);

} 
