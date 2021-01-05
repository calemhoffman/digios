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
#include "FindThetaCM.h"

int main(int argc, char *argv[]){
   
  printf("=================================================================\n");
  printf("===     Find ThetaCM convrage for each detector at Ex        ====\n");
  printf("=================================================================\n");

  if(argc < 2 || argc > 6) { 
    printf("Usage: ./FindThetaCM Ex\n");
    printf("Usage: ./FindThetaCM Ex nDiv\n");
    printf("Usage: ./FindThetaCM Ex nDiv X-Ratio\n");
    printf("Usage: ./FindThetaCM Ex nDiv X-Ratio reactionTxt detGeoTxt\n");
    exit(0); 
  }  
  
  double Ex = 0;
  double xRatio = 0.95;
  int nDiv = 1;
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
    reactionTxt = argv[4];
  }
  if ( argc >= 6 ){
    detGeoTxt = argv[5];
  }
  
  FindThetaCM(Ex, nDiv, xRatio, reactionTxt, detGeoTxt);
  
  return 0;
}
