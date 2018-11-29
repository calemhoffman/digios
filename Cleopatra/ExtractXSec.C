/***********************************************************************
 * 
 *  This is ExtractXSec for *.out for Ptolemy
 * 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ InFileCreator.C -o InFileCreator `root-config --cflags --glibs`
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>
#include "ExtractXSec.h"

using namespace std;

int main (int argc, char *argv[]) {
   
  printf("=================================================================\n");
  printf("========== Extract Cross-Section From Ptolemy out file ==========\n");
  printf("=================================================================\n");

  if(argc != 2) { 
    printf("Usage: ./ExtractXSec input_file\n");
    exit(0); 
  }else{
    printf("From file : %s \n", argv[1]);
  }

  string readFile = argv[1];

  ExtractXSec(readFile);

} 
