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

  if(argc != 10 && argc != 5 && argc !=6 && argc != 8 && argc != 12) {
    printf("====== Runge-Kutta 4-th order method to solve SE =====\n");
    printf("Usage: ./WSCal A V0 R0 a0 <VSO RSO aSO>  <Z Rc>  <nStep dx>\n");
    printf("         A : Mass number [MeV]\n");
    printf("        V0 : Wood-Saxons depth [MeV]\n");
    printf("        r0 : reduced half-maximum radius (fm) \n");
    printf("        a0 : diffusiveness (fm) \n");
    printf("       VSO : Spin-orbital depth [MeV]\n");
    printf("       rSO : reduced half-maximum radius (fm) \n");
    printf("       aSO : diffusiveness (fm) \n");
    printf("         Z : charge number, if for neutron, Z = 0.\n");
    printf("        Rc : Coulomb radius (fm) \n");
    printf("     nStep : number of Step (200)\n");
    printf("        dr : stepSize [fm] (0.1)\n");
    exit(0);
  }
  WoodsSaxon ws;

  int massA = atoi(argv[1]);

  ws.SetNucleus(massA,0);
  ws.IsNeutron();

  ws.SetV0( atof(argv[2]) );
  ws.SetR0( atof(argv[3]) * pow(massA, 1./3.) );
  ws.Seta0( atof(argv[4]) );
  
  ws.SetVSO( 0.);
  ws.SetRSO( 1.);
  ws.SetaSO( 1.);
  
  if( argc == 6 ){
    ws.SetVSO ( atof(argv[5]) );
  }
  
  if( argc >= 8 ){
    ws.SetVSO ( atof(argv[5]) );
    ws.SetRSO ( atof(argv[6]) * pow(massA, 1./3.) );
    ws.SetaSO ( atof(argv[7]) );
  }

  ws.Setrc(1.25);
  if( argc >= 10 ){
    int Z = atoi(argv[8]) ;
    ws.SetRc ( atof(argv[9]) );
    if( Z > 0 ) {
      ws.SetNucleus(massA, Z);
      ws.IsProton();
    }
  }
  
  ws.SetRange2(0.0001, 0.1, 200);
  if( argc >= 12 ){
    ws.SetRange2(0.0001, atof(argv[11]), atoi(argv[10]));
  }
  
  double torr = 500; /// u < torr
  double eTorr = 1e-5; /// energy torr
  int maxLoop = 400;
  double dKE = 0.3;

  ws.PrintWSParas();
  ws.CalWSEnergies(false, 7, torr, eTorr, maxLoop, dKE, false);
  
  ws.PrintEnergyLevels();

  //========== Save Radial function
  
  printf("========== end of program ======= \n");
  
  return 0;
}

