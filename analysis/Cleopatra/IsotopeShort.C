/***********************************************************************
 *
 *  This is IsotopeShort.C for isotope mass-related quatilies
*   for python web
 *
 * -----------------------------------------------------
 *  To compile
 *
 *  g++ IsotopeShort.C -o IsotopesShort
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Feb-20, 2021
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>
#include "Isotope.h"

using namespace std;

void Usage(){
   cout << "./NuclearData Sym" << endl;
   cout << "./NuclearData A Z" << endl;
   exit(0);
}

int main (int argc, char *argv[]) {

  if ( argc != 2 && argc != 3 && argc != 6) Usage();

  Isotope iso1, iso2;
  int    Z, A, Za, Aa;
  if (argc == 2){
    iso1.SetIsoByName(argv[1]);
  }
  if (argc == 3){
    A= atoi(argv[1]);
    Z= atoi(argv[2]);
    iso1.SetIso(A, Z);
  }

  //iso1.Print();
  printf("A:%3d\n", iso1.A);
  printf("Z:%3d\n", iso1.Z);
  printf("N:%3d\n", iso1.A-iso1.Z);
  printf("Name:%s|\n",iso1.Name.c_str());
  printf("Mass:%13.4f\n", iso1.Mass);
  printf("Sn:%13.4f\n", iso1.CalSp(0,1));
  printf("Sp:%13.4f\n", iso1.CalSp(1,0));
  printf("Sa:%13.4f\n", iso1.CalSp2(4,2));
  printf("S2n:%13.4f\n", iso1.CalSp(0,2));

}
