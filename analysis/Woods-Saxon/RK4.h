/****************************************************************
  Rungu-Kuttan 4 method on solving 1-D Differential equation
	d^2u/dr^2 = G(r,u)
  
  In the Pot(r), the potential is for Wood-Saxon.
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-1-31
*****************************************************************/

#ifndef RK4
#define RK4

#include <stdio.h>
#include <stdlib.h> //for atoi, atof
#include <cmath> //exp
#include <vector> 
#include "../Cleopatra/constant.h"

using namespace std;

/*
  double mp = 938.272;
  double mn = 939.5654133;
  double hbarc = 197.326979;
  double amu = 931.5;
  double ee  = 1.439964454;
*/

class RKFourth {
public:
  RKFourth(){

    mu = mn;
    
    Z = 0;
    Rc = 10000;

    V0 = 0;
    R0 = 10;
    a0 = 1;

    VSO = 0;
    RSO = 10;
    aSO = 1;

    LS = 0;

    dr = 0.1;
    nStep = 200;

    isSaveWaveFunction = false;

    wf.clear();
  }

  ~RKFourth(){}
	
  double mu; // mass of nucleon
	
  double V0; // in negative MeV
  double R0; // in fm
  double a0; // in fm

  double VSO; // in MeV
  double RSO; // in fm
  double aSO; // in fm

  double Z;   // total charge
  double Rc;  // in fm

  double LS;  // spin-orbital 
	
  double dr;  // in fm
  int nStep;  // number of step
  
  bool isSaveWaveFunction;

  vector<double> wf;

  void SaveWaveFunction(bool save = true){
    if( save == true ) {
      isSaveWaveFunction = true;
    }else{
      isSaveWaveFunction = false;
    }
  }
  
  double Pot(double r){
    // Wood-Saxon
    double WSCentral = V0/(1+exp((r-R0)/a0));
    // spin-orbital 
    double SO = - LS * VSO * exp((r-RSO)/aSO) / pow(1+exp((r-RSO)/aSO), 2) / aSO/ r ;
    // Coulomb
    double Vc = 0;
    if( r > Rc ){
      Vc = Z * ee / r;
    }else{
      Vc = Z * ee * (3*Rc*Rc - r*r)/(2*Rc*Rc*Rc);
    }
    return WSCentral + SO + Vc;
  }

  double dPot(double r){
    //derivative of Wood-Saxon
    double dWSCentral = - V0 * exp((r-R0)/a0) / pow(1+exp((r-R0)/a0), 2) / a0;
    //derivative of spin-orbital
    double dSO = LS * VSO * pow(1./cosh((r-RSO)/2./aSO),2) * ( aSO + r * tanh((r-RSO)/2./aSO) ) / 4 / aSO/ aSO / r / r;
    //derivative of Coulomb
    double dVc = 0;
    if( r > Rc ){
      dVc = - Z * ee / r / r;
    }else{
      dVc = -Z * ee * r / Rc / Rc ; 
    }
    return dWSCentral + dSO + dVc;
  }

  double FindBarrier(int L){

    if( L == 0 ) return 0;
    
    //finding the zero crossing point of derivative
    double step = 0.1;
    double rMax = R0;
    double t1 = 100, t2 = -1000;
    bool isFound = false;
    for( float r = R0; r < 20 * R0; r += step ) {

      //using tracing method
      t1 = Pot(r) + pow( hbarc, 2) * L *(L + 1.) / pow(r,2) / 2 / mu;
      if( t1 > t2 ) t2 = t1;

      //printf(" r : %f fm | t1 : %f | t2 : %f \n", r, t1 , t2);
      
      if( t1 < t2 ) {
	rMax = r;
	isFound = true;
	break;
      }
      
      /*// using 1st derivative 
      t1 = dPot(r)    -  pow( hbarc, 2) * L *(L + 1.) / pow(r,3) / mu;
      t2 = dPot(r + step) -  pow( hbarc, 2) * L *(L + 1.) / pow(r + step,3) / mu;
      double pp = t1 * t2;
      //printf(" r : %f fm | t1 : %f | t2 : %f \n", r, t1 , t2);
      if( pp < 0 ) {
	rMax = r;
	isFound = true;
	break;
      }
      */
    }

    double barrier = 0;

    if( isFound ) {
      //rMax = rMax + t1 / t2 * step; // when using 1st derivative
      barrier = Pot(rMax) + pow( hbarc, 2) * L *(L + 1.) / pow(rMax,2) / 2 / mu;
      if( barrier < 0 ) barrier = 0;
      
      //printf(" max position = %f fm | %f  \n", rMax, barrier);
      
    }else{
      //printf("max position not found for L = %d \n", L);
    }
    
    return  barrier;
    
  }
	
  double G(double r, double u, double T, double L, double Pot){
    return (2*mu/pow(hbarc,2))*(Pot-T)*u + L*(L+1)*u/pow(r,2);
  } 

  vector<double> SolveRadialFunction (double KE, int L){

    // return last u = SolU[last][0];
    vector<double> output;
    output.clear();
    output.push_back(-1000.); // for u[nStep]
    output.push_back(-1000.); // for uMax;
	  
    // output is a file "RK4.dat"

    //TODO using a array as potential

    const double scale = abs(V0);
    double rStart = 0.0001;
    double rEnd = rStart + dr * nStep;

    //printf("-------------- inputs\n");
    //printf(" KE: %6.2f MeV, L: %2d \n", KE, L);
    //printf("  V0: %6.2f MeV,  R0: %6.2f fm,  a0: %6.2f fm \n", V0, R0, a0);
    //printf(" VSO: %6.2f MeV, RSO: %6.2f fm, aSO: %6.2f fm \n", VSO, RSO, aSO);
    //printf(" start: %7.5f fm, dr:%5.3f, end: %10.5f fm, nStep: %3d \n", rStart, dr, rEnd, nStep);
    //printf(" --------> save in RK4.dat \n");
	  
    const int nStep1 = nStep + 1 ;
    double SolU[nStep1][2]; // SolU[i][0] is U, SolU[i][1] is du/dr
	  
    SolU[0][0] = 0;
    SolU[0][1] = 1;
	  
    FILE * paraOut = NULL;
    if( isSaveWaveFunction ){
      paraOut = fopen ("RK4.dat", "w");
      fprintf(paraOut, "# KE: %6.2f MeV, L: %2d \n", KE, L);
      fprintf(paraOut, "# V0: %6.2f MeV, R0: %6.2f fm, a0: %6.2f fm\n", V0, R0, a0);
      fprintf(paraOut, "# start: %7.5f fm, dr:%5.3f, end: %10.5f fm, nStep: %3d \n", rStart, dr, rEnd, nStep);
      fprintf(paraOut, "#%12s %12s %12s %12s %12s %12s %12s %12s\n", "r", "Pot", "(Pot+Cent.)", "u", "du/dr", "R", "dR/dr", "KE");
      fprintf(paraOut, "#----------\n");
      fprintf(paraOut, " %12.3E %12.3E %12.3E %12.3E %12.3E %12.3E %12.3E %12.3E\n", rStart, Pot(rStart), Pot(rStart) + L*(L+1)/pow(rStart,2), SolU[0][0], SolU[0][1], SolU[0][0]/rStart, SolU[0][1]/rStart - SolU[0][0]/pow(rStart,2), KE);
    }
	  
    double maxu = 0;  
    for(int i = 0; i <= nStep; i++){
      double r = rStart + i * dr;
      double pot = Pot(r);
      double potHalf = Pot(r+dr/2.);
      double u = SolU[i][0];
      double v = SolU[i][1];

      double ku1 = v;
      double kv1 = G(r, u, KE, L, pot);

      double ku2 = v + kv1*dr/2;    
      double kv2 = G(r + dr/2, u + ku1*dr/2, KE, L, potHalf); 

      double ku3 = v + kv2*dr/2;    
      double kv3 = G(r + dr/2, u + ku2*dr/2, KE, L, potHalf);

      double ku4 = v + kv3*dr;    
      double kv4 = G(r + dr, u + ku3*dr, KE, L, Pot(r+dr));
		 
      double du = dr/6*(ku1 + 2*ku2 + 2*ku3 + ku4);
      double dv = dr/6*(kv1 + 2*kv2 + 2*kv3 + kv4);

      //printf(" r:%8f, Pot:%12f, G:%12f, Pot1/2:%12f, u:%12f, v:%12f \n", r, pot, kv1, potHalf, u, v );
      //printf("            %12f, %12f, %12f, %12f, %12f, %12f, %12f, %12f, %12f, %12f \n", ku1, ku2, ku3, ku4, kv1, kv2, kv3, kv4, du, dv);
		 
      SolU[i + 1][1] = v + dv;
      SolU[i + 1][0] = u + du;

      if ( u > maxu) maxu = u;

      if( i == nStep ) output[0] = u;

      wf.push_back(u);
      
    }
	  
    output[1] = maxu;
    maxu = 1;
    //printf("------- normalized ul \n"); //imporer normalized

    for( int i = 0; i <= nStep; i++){
      double r = rStart + i*dr;
      double pot = Pot(r+dr);
      double poteff = pot + pow(hbarc/(r+dr),2)*L*(L+1)/2/mu;
      //if ( abs(poteff) > abs(V0) ) poteff = 0.0;
      SolU[i+1][1] = SolU[i+1][1]/maxu;
      SolU[i+1][0] = SolU[i+1][0]/maxu;
		 
      if( isSaveWaveFunction ){
	fprintf(paraOut, " %12f %12f %12f %12f %12f %12f %12f %12f\n", r+dr, pot, poteff , SolU[i+1][0], SolU[i+1][1], SolU[i+1][0]/r, SolU[i+1][1]/r - SolU[i+1][0]/pow(r,2), KE);
      }
    }

    if( isSaveWaveFunction) {
      fclose(paraOut);
      printf("----------saved in RK4.dat\n");
    }
	  
    return output;
  }

};

#endif
