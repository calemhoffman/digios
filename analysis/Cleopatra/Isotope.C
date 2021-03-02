/***********************************************************************
 * 
 *  This is Isotope.C for isotope mass-related quatilies
 * 
 * -----------------------------------------------------
 *  To compile
 * 
 *  g++ Isotope.C -o Isotope
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
   cout << "./Isotope Sym" << endl;
   cout << "./Isotope A Z" << endl;
   ///cout << "./Isotope A Z A' Z'" << endl;
   ///cout << " Opt = rkm, e.g. 001 only calculate mass" << endl;
   ///cout << "       |||_ mass " << endl;
   ///cout << "       ||__ kinematics " << endl;
   ///cout << "       |___ reaction kinematics " << endl;
   exit(0);
}

int main (int argc, char *argv[]) {
  
  printf("=================================================================\n");
  printf("==========       Isotope mass-related quantities       ==========\n");
  printf("=================================================================\n");
  
  if ( argc != 2 && argc != 3 && argc != 6) Usage();

  Isotope iso1, iso2;
  int    Z, A, Za, Aa;
  //string Opt = argv[1];
  
  if (argc == 2){
    iso1.SetIsoByName(argv[1]);
  } 
  if (argc == 3){
    A= atoi(argv[1]);
    Z= atoi(argv[2]);
    iso1.SetIso(A, Z);
  //}else if ( argc == 6){
  //  A= atoi(argv[2]);
  //  Z= atoi(argv[3]);
  //  Aa= atoi(argv[4]);
  //  Za= atoi(argv[5]);
  //  iso1.SetIso(A, Z);
  //  iso2.SetIso(Aa,Za);   
  }

  iso1.Print();

  iso1.ListShell();
  
}
