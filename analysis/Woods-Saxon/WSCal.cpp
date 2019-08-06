/****************************************************************
  Eigen-energies for Wood-Saxon potential with Spin-Orbital.
  Using Rungu-Kuttan-4 method to solve the wavefunction numerically

  When the energy is not eigen-energyies, the wavefunction is diverge. 
  So, by checking the wavefunction at far-away, the eigen-energy can be found.
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-1-31
*****************************************************************/

#include <stdio.h>
#include <string>
#include <stdlib.h> //for atoi, atof
#include <math.h> //exp
#include <vector> 
#include "RK4.h"
#include "WS.h"

using namespace std;

int main(int argc, char *argv[]){

  if(argc != 9 && argc != 4 && argc !=5 && argc != 7 && argc != 11) {
    printf("====== Runge-Kutta 4-th order method to solve SE =====\n");
    printf("Usage: ./WSCal V0 R0 a0 <VSO RSO aSO>  <Z Rc>  <nStep dx>\n");
    printf("        V0 : Wood-Saxons depth [MeV]\n");
    printf("        R0 : half-maximum radius (fm) \n");
    printf("        a0 : diffusiveness (fm) \n");
    printf("       VSO : Spin-orbital depth [MeV]\n");
    printf("       RSO : half-maximum radius (fm) \n");
    printf("       aSO : diffusiveness (fm) \n");
    printf("         Z : charge number \n");
    printf("        Rc : Coulomb radius (fm) \n");
    printf("      nStep: number of Step (300)\n");
    printf("        dr : stepSize [fm] (0.1)\n");
    exit(0);
  }
  WoodsSaxon ws;

  ws.V0 = atof(argv[1]);
  ws.R0 = atof(argv[2]);
  ws.a0 = atof(argv[3]);
  
  ws.VSO = 0.;
  ws.RSO = 1.;
  ws.aSO = 1.;
  
  if( argc == 5 ){
    ws.VSO = atof(argv[4]);
    ws.RSO = ws.R0;
    ws.aSO = ws.a0;
  }
  
  if( argc >= 7 ){
    ws.VSO = atof(argv[4]);
    ws.RSO = atof(argv[5]);
    ws.aSO = atof(argv[6]);
  }

  ws.Z = 0; 
  ws.Rc = 1.25; 
  if( argc >= 9 ){
    ws.Z = atoi(argv[7]);
    ws.Rc = atof(argv[8]);
  }
  
  ws.nStep = 200; 
  ws.dr = 0.1; 
  if( argc >= 11 ){
    ws.nStep = atoi(argv[9]);
    ws.dr = atof(argv[10]);
  }
  
  double torr = 500; // u < torr
  double eTorr = 0.001; // energy torr
  int maxLoop = 300;
  double dKE = 0.2;
  
  //calculate WS eigen energy
  //ws.dr = 0.1; ws.nStep = 200;
  
  ws.IsNeutron();
  if( ws.Z != 0 ) ws.IsProton();
  ws.PrintWSParas();
  ws.CalWSEnergies();
  
  //print all energy
  printf("================ result\n");
  printf(" %8s,  %12s |      u(%4.1f) | %12s |%12s \n", "Orbital", "Energy [MeV]", ws.dr * ws.nStep, "de", "u-Ratio"); 
  for( int i = 0; i < ws.energy.size() ; i++){
    if( abs(ws.errorU[i]) > torr || ws.errorE[i] > eTorr) continue;
    printf("%2d| %8s,  %12.6f | %12.5f | %12.7E | %12f\n",i, ws.orbString[i].c_str(), ws.energy[i], ws.errorU[i], ws.errorE[i], ws.errorUratio[i]);  
  }
  printf("========== end of program ======= \n");
  
  return 0;
}

