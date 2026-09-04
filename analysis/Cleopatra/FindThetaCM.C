/***********************************************************************
 * 
 *  This is FindThetaCM.C, To calculate the thetaCM convrage for each detector
 *  
 *  This required two inout files: basicReactionConfig.txt
 *                                 detectorGeo.txt
 * 
 *-------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>     /* atof */
#include <cmath>
#include <vector>
#include <string>
#include "FindThetaCM.h"

using namespace std;

int main(int argc, char *argv[]){
   
  printf("=================================================================\n");
  printf("===     Find ThetaCM convrage for each detector at Ex        ====\n");
  printf("=================================================================\n");

  if(argc < 2 || argc > 7) { 
    printf("Usage: ./FindThetaCM Ex\n");
    printf("Usage: ./FindThetaCM Ex nDiv\n");
    printf("Usage: ./FindThetaCM Ex nDiv X-Ratio\n");
    printf("Usage: ./FindThetaCM Ex nDiv X-Ratio loop\n");
    printf("Usage: ./FindThetaCM Ex nDiv X-Ratio loop reactionTxt detGeoTxt\n");
    exit(0); 
  }  
  
  double Ex = 0;
  double xRatio = 0.95;
  int nDiv = 1;
  short loop = 1;
  string reactionTxt = "reactionConfig.txt";
  string detGeoTxt = "detectorGeo.txt";
  
  if ( argc >= 2 ){
    Ex = atof(argv[1]);
  }
  if ( argc >= 3 ){
    nDiv = atoi(argv[2]);
  }
  if ( argc >= 4 ){
    xRatio = atof(argv[3]);
  }
  if ( argc >= 5 ){
    loop = atoi(argv[4]);
  }
  if ( argc >= 6 ){
    reactionTxt = argv[5];
  }
  if ( argc >= 7 ){
    detGeoTxt = argv[6];
  }
  
  FindThetaCM(Ex, nDiv, xRatio, loop, reactionTxt, detGeoTxt);
  
  return 0;
}
