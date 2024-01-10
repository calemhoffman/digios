/****************************************************************
  Eigen-energies for Wood-Saxon potential with Spin-Orbital.
  Using Rungu-Kuttan-4 method to solve the wavefunction numerically

  When the energy is not eigen-energyies, the wavefunction is diverge. 
  So, by checking the wavefunction at far-away, the eigen-energy can be found.
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-8-16
*****************************************************************/

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h> //for atoi, atof
#include <math.h> //exp
#include <vector>
#include <fstream>
#include "RK4.h"
#include "WS.h"

//#include "../Cleopatra/Isotope.h"

using namespace std;

int main(int argc, char *argv[]){

  if(argc != 9 && argc != 11 && argc != 12 ) {
    printf("====== Runge-Kutta 4-th order method to solve SE =====\n");
    printf("Usage: ./WSRange V0 k r0 a0 VSO rSO aSO  rc  <nStep dx> <fileName>\n");
    printf("        V0 : Wood-Saxons depth [MeV]\n");            ///1
    printf("         k : isospin-dependent (+ for proton, - for neutron) \n");                 ///2
    printf("             V  = V0 * ( 1 + k * (N-Z)/A ) \n");
    printf("        r0 : reduced half-maximum radius (fm) \n");  ///3
    printf("        a0 : diffusiveness (fm) \n");                ///4
    printf("       VSO : Spin-orbital depth [MeV]\n");           ///5
    printf("       rSO : reduced half-maximum radius (fm) \n");  ///6
    printf("       aSO : diffusiveness (fm) \n");                ///7
    printf("        rc : reduced Coulomb radius (fm) \n");       ///8
    printf("     nStep : number of Step (300)\n");               ///9
    printf("        dr : stepSize [fm] (0.1)\n");                ///10
    printf("  fileName : default is range.txt\n");               ///11
    exit(0);
  }

  printf("================== is neutron or proton ? [N or Z] ");
  char ans1[80];
  int temp = scanf("%79s", ans1);
  if( strcmp(ans1 , "N") != 0  && strcmp(ans1,"Z") != 0 ) {
    printf(" Invalid nucleon type. either 'N' or 'Z'. bye. \n");
    return -404;
  }

  printf("================== method : \n");
  string optMsg[4];
  optMsg[0] = "increase A (no asymmetry term in V0 )";
  optMsg[1] = "increase N , fixed Z";
  optMsg[2] = "increase Z , fixed N";
  optMsg[3] = "increase A, approx. stable nuclei";
  for( int i = 0 ; i < 4 ; i++){
    printf(" %d) %s \n", i+1, optMsg[i].c_str());
  }
  printf("---------- Your choice is [1, 2, 3] :");
  int opt;
  temp = scanf("%d", &opt);

  int range[2];
  int NZfixed = 0;

  if( opt == 1 || opt == 4 ){
    printf(" minimum A ? ");
    temp = scanf("%d", &range[0]);
    printf(" maximum A ? ");
    temp = scanf("%d", &range[1]);
  }

  if( opt == 2 ){
    printf(" fixed Z number : ");
    temp = scanf("%d", &NZfixed );
    printf(" minimum N ? ");
    temp = scanf("%d", &range[0]);
    printf(" maximum N ? ");
    temp = scanf("%d", &range[1]);
  }

  if( opt == 3 ){
    printf(" fixed N number : ");
    temp = scanf("%d", &NZfixed );
    printf(" minimum Z ? ");
    temp = scanf("%d", &range[0]);
    printf(" maximum Z ? ");
    temp = scanf("%d", &range[1]);
  }

  int maxA = NZfixed + range[1];
  
  printf("=======================\n");
  printf(" Calculate %s energies with %s \n", strcmp(ans1, "N") == 0 ? "Neutron" : "Proton", optMsg[opt-1].c_str() );

  if( opt == 1 ) printf(" A : %d, %d \n", range[0], range[1] );
  if( opt == 2 ) printf(" N : %d, %d , fixed Z = %d\n", range[0], range[1], NZfixed );
  if( opt == 3 ) printf(" Z : %d, %d , fixed N = %d\n", range[0], range[1], NZfixed );
  
  WoodsSaxon ws;
  
  double V0 = atof(argv[1]);
  double kappa = atof(argv[2]);
  ws.Setr0( atof(argv[3]) );
  ws.Seta0( atof(argv[4] ) );
  
  ws.SetVSO( atof(argv[5]) );
  ws.SetrSO( atof(argv[6]) );
  ws.SetaSO( atof(argv[7]) );

  ws.Setrc( atof(argv[8]) );
  
  ws.SetRange2(0.001, 0.1, 200);
  if( argc >= 10 ){
    ws.SetRange2(0.001, atof(argv[10]), atof(argv[9]) );
  }

  string outFileName = "range.txt";
  if( argc >= 12 ) outFileName = argv[11];
  
  double torr = 500; // u < torr
  double eTorr = 0.001; // energy torr
  int maxLoop = 300;
  double dKE = 0.2;
  

  if ( strcmp(ans1 , "N") == 0  ) {
    ws.IsNeutron();
  }else{
    ws.IsProton();
  }

  vector<string> orbitalStr;
  orbitalStr.push_back("0s1/2");   ///2
  orbitalStr.push_back("0p3/2");
  orbitalStr.push_back("0p1/2");   ///8
  if( maxA > 8 ) {
    orbitalStr.push_back("0d5/2");
    orbitalStr.push_back("1s1/2");
    orbitalStr.push_back("0d3/2");   ///20
  }
  if( maxA > 20 ){
    orbitalStr.push_back("0f7/2");   ///28
    orbitalStr.push_back("1p3/2");
    orbitalStr.push_back("0f5/2");
  }
  if( maxA > 40 ){
    orbitalStr.push_back("1p1/2");   ///40
    orbitalStr.push_back("0g9/2");   ///50
    orbitalStr.push_back("0g7/2");   
    orbitalStr.push_back("1d5/2");   
    orbitalStr.push_back("0h11/2"); 
    orbitalStr.push_back("1d3/2");   
  }
  if( maxA > 82 ){
    orbitalStr.push_back("2s1/2");   ///82
    orbitalStr.push_back("0h9/2"); 
    orbitalStr.push_back("1f7/2"); 
    orbitalStr.push_back("0i13/2"); 
    orbitalStr.push_back("2p3/2"); 
    orbitalStr.push_back("1f5/2"); 
  }
  if( maxA > 126 ){
    orbitalStr.push_back("2p1/2");    ///126 
    orbitalStr.push_back("1g9/2"); 
    orbitalStr.push_back("0i11/2"); 
    orbitalStr.push_back("0j15/2"); 
    orbitalStr.push_back("0j15/2"); 
    orbitalStr.push_back("2d5/2"); 
    orbitalStr.push_back("3s1/2"); 
    orbitalStr.push_back("2d3/2"); 
    orbitalStr.push_back("1g7/2");    ///184 
  }

  FILE * file_out;
  file_out = fopen(outFileName.c_str(), "w+");

  fprintf(file_out, "# V0 = %f\n", V0 );
  fprintf(file_out, "# r0 = %f\n", ws.Getr0() );
  fprintf(file_out, "# a0 = %f\n", ws.Geta0() );
  fprintf(file_out, "#VSO = %f\n", ws.GetVSO() );
  fprintf(file_out, "#rSO = %f\n", ws.GetrSO() );
  fprintf(file_out, "#aSO = %f\n", ws.GetaSO() );
  fprintf(file_out, "#  k = %f\n", kappa );
  fprintf(file_out, "# nucleon : %s\n", ans1 );
  fprintf(file_out, "#  method : %s\n", optMsg[opt-1].c_str() );
  fprintf(file_out, "#=========================\n");

  for( float ANZ = range[0] ; ANZ <= range[1] ; ANZ  += 1.0){

    if( opt == 1 ){ /// change in A, ANZ = A
      ws.SetNucleus ( ANZ, 0);
      ws.CalRadius();
      ws.SetV0( V0 );
      //ws.SetV0 ( V0 * ( 1 +  kappa * ( 2.0* ws.GetN() - ws.GetA() ) / ws.GetA() ) ); 
    }

    if( opt == 2 ){ /// change in N, ANZ = N
      ws.SetNucleus( ANZ + NZfixed, NZfixed );
      ws.CalRadius();

      ws.SetV0 ( V0 * ( 1 +  kappa * ( 2.0* ws.GetN() - ws.GetA() ) / ws.GetA() ) ); 
    }
    
    if( opt == 3 ){ /// change in Z, ANZ = Z
      ws.SetNucleus( ANZ + NZfixed, ANZ );
      ws.CalRadius();

      ws.SetV0 ( V0 * ( 1 +  kappa * ( 2.0* ws.GetN() - ws.GetA() ) / ws.GetA() ) ); 
    }

    if( opt == 4 ){ /// approx stable nuclei
      ws.SetNucleus ( ANZ, 0.008 + 0.495 * ANZ - 0.00076 * ANZ *ANZ + 1.4e-6 * ANZ * ANZ * ANZ); // aproximate stable vallage
      ws.CalRadius();

      ws.SetV0 ( V0 * ( 1 +  kappa * ( 2.0* ws.GetN() - ws.GetA() ) / ws.GetA() ) ); 
    }
  
    double reducedMass = 931.5 * (ws.GetA() )/(1.008664 + ws.GetA());
    ws.SetMass(reducedMass);
    
    ws.ClearVector();
    ws.PrintWSParas();

    ws.CalWSEnergies(false, 7, 500, 1e-7, 400, 0.2, false);

    //print all energy
    ws.PrintEnergyLevels();

    if( ANZ == range[0] ) {
      fprintf(file_out, "%6s, ", "A");
      fprintf(file_out, "%6s, ", "Z");
      fprintf(file_out, "%6s, ", "N");
      fprintf(file_out, "%6s, ", "V0");
      for( int i = 0; i < orbitalStr.size(); i++){
        string temp = orbitalStr[i];
        temp.insert(2,"_{");
        temp = temp + "}";
        fprintf(file_out,"%9s  ", temp.c_str());
      }
      fprintf(file_out,"\n");
    }
    //matching orbital
    fprintf(file_out, "%6.2f, ", ws.GetA());
    fprintf(file_out, "%6.2f, ", ws.GetZ());
    fprintf(file_out, "%6.2f, ", ws.GetN());
    fprintf(file_out, "%6.2f, ", ws.GetV0());
    for( int i = 0; i < orbitalStr.size(); i++){
      bool isMatched = false;
      for( int j = 0; j < (ws.energy).size(); j++){
        if( orbitalStr[i] == ws.orbString[j] ) {
          fprintf(file_out,"%9.4f, ", ws.energy[j]);
          isMatched = true;
        }
      }
      if( isMatched == false ) fprintf(file_out,"%9s, " , "");
    }
    fprintf(file_out,"\n");
    
    
    
  }

  fclose(file_out);
  printf("========== end of program ======= \n");

  return 0;
}

