/****************************************************************
  Eigen-energies for Wood-Saxon potential with Spin-Orbital.
  Using Rungu-Kuttan-4 method to solve the wavefunction numerically

  When the energy is not eigen-energyies, the wavefunction is diverge. 
  So, by checking the wavefunction at far-away, the eigen-energy can be found.
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-8-16
*****************************************************************/

#include <stdio.h>
#include <string>
#include <stdlib.h> //for atoi, atof
#include <math.h> //exp
#include <vector>
#include <fstream>
#include "RK4.h"
#include "WS.h"

using namespace std;

int main(int argc, char *argv[]){

  if(argc != 9 && argc != 11 && argc != 13) {
    printf("====== Runge-Kutta 4-th order method to solve SE =====\n");
    printf("Usage: ./WSRange A1 A2 V0 r0 a0 <VSO rSO aSO>  <Z rc>  <nStep dx>\n");
    printf("        A1 : start mass number\n");
    printf("        A2 : stop mass number\n");
    printf("        V0 : Wood-Saxons depth [MeV]\n");
    printf("        r0 : reduced half-maximum radius (fm) \n");
    printf("        a0 : diffusiveness (fm) \n");
    printf("       VSO : Spin-orbital depth [MeV]\n");
    printf("       rSO : reduced half-maximum radius (fm) \n");
    printf("       aSO : diffusiveness (fm) \n");
    printf("         Z : charge number \n");
    printf("        rc : reduced Coulomb radius (fm) \n");
    printf("      nStep: number of Step (300)\n");
    printf("        dr : stepSize [fm] (0.1)\n");
    exit(0);
  }
  WoodsSaxon ws;

  int A1 = atoi(argv[1]);
  int A2 = atoi(argv[2]);
  
  ws.V0 = atof(argv[3]);
  ws.r0 = atof(argv[4]);
  ws.a0 = atof(argv[5]);
  
  ws.VSO = atof(argv[6]);
  ws.rSO = atof(argv[7]);
  ws.aSO = atof(argv[8]);
  
  ws.Z = 0; 
  ws.Rc = 1.25; 
  if( argc >= 11 ){
    ws.Z = atoi(argv[9]);
    ws.rc = atof(argv[10]);
  }
  
  ws.nStep = 200; 
  ws.dr = 0.1; 
  if( argc >= 13 ){
    ws.nStep = atoi(argv[11]);
    ws.dr = atof(argv[12]);
  }
  
  double torr = 500; // u < torr
  double eTorr = 0.001; // energy torr
  int maxLoop = 300;
  double dKE = 0.2;
  
  //calculate WS eigen energy
  //ws.dr = 0.1; ws.nStep = 200;
  
  ws.IsNeutron();
  if( ws.Z != 0 ) ws.IsProton();

  vector<string> orbitalStr;


  FILE * file_out;
  string outFileName = "range.txt";
  file_out = fopen(outFileName.c_str(), "w+");

  fprintf(file_out, "# V0 = %f\n", ws.V0 );
  fprintf(file_out, "# r0 = %f\n", ws.r0 );
  fprintf(file_out, "# a0 = %f\n", ws.a0 );
  fprintf(file_out, "#VSO = %f\n", ws.VSO );
  fprintf(file_out, "#rSO = %f\n", ws.rSO );
  fprintf(file_out, "#aSO = %f\n", ws.aSO );
  fprintf(file_out, "#=========================\n");
  
  for( int massA = A2 ; massA >= A1 ; massA--){
    
    ws.A = massA;
    ws.ClearVector();
    ws.CalRadius();
    ws.PrintWSParas();
    ws.CalWSEnergies();
  
    //print all energy
    ws.PrintEnergyLevels();

    if( massA == A2 ) {
      orbitalStr = ws.orbString;
      fprintf(file_out, "%6s, ", "massA");
      for( int i = 0; i < orbitalStr.size(); i++){
	string temp = orbitalStr[i];
	temp.insert(2,"_{");
	temp = temp + "}";
	fprintf(file_out,"%9s  ", temp.c_str());
      }
      fprintf(file_out,"\n");
      fprintf(file_out, "%6d, ", massA);
      for( int i = 0; i < orbitalStr.size(); i++){
	fprintf(file_out,"%9.2f, ", ws.energy[i]);
      }
      fprintf(file_out,"\n");
    }else{
      
      //matching orbital
      fprintf(file_out, "%6d, ", massA);
      for( int i = 0; i < orbitalStr.size(); i++){
	bool isMatched = false;
	for( int j = 0; j < (ws.energy).size(); j++){
	  if( orbitalStr[i] == ws.orbString[j] ) {
	    fprintf(file_out,"%9.2f, ", ws.energy[j]);
	    isMatched = true;
	  }
	}
	if( isMatched == false ) fprintf(file_out,"%9s, " , "");
      }
      fprintf(file_out,"\n");
      

    }
    
  }

  fclose(file_out);
  printf("========== end of program ======= \n");

  return 0;
}

