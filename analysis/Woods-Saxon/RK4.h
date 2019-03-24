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

using namespace std;

double mp = 938.272;
double mn = 939.5654133;
double hbarc = 197.326979;
double amu = 931.5;

double mu = mn;

bool isSaveWaveFunction = false;

//Central potential 
typedef struct {
 double V0;
 double R0;
 double a0;
 
 double VSO;
 double RSO;
 double aSO;
 
 double LS;
}WSPara;

//WSPara wsp;

void PrintWSParas(int A, double dr, int step, WSPara wsp){
  printf("================ Woods-Saxon parameters \n");
  printf("   A: %d, dr:%5.3f fm, nStep: %3d, range: %5.3fm \n", A, dr, step, dr * step);
  double fact = pow(A, 1./3);
  printf(" V0: %8.4f MeV,  R0: %8.4f(%4.2f) fm,  a0: %8.4f fm \n",wsp.V0,  wsp.R0, wsp.R0/fact, wsp.a0);
  printf("VSO: %8.4f MeV, RS0: %8.4f(%4.2f) fm, aS0: %8.4f fm \n",wsp.VSO,  wsp.RSO, wsp.RSO/fact, wsp.aSO);
  printf("================================\n");
}

double Pot(double r, WSPara wsp){
  // Wood-Saxon
  double WS = wsp.V0/(1+exp((r-wsp.R0)/wsp.a0));
  // spin-orbital 
  double SO = - wsp.LS * wsp.VSO * exp((r-wsp.RSO)/wsp.aSO) / pow(1+exp((r-wsp.RSO)/wsp.aSO), 2) / wsp.aSO/ r ;
  return WS + SO;
}

double G(double r, double u, double T, double L, double Pot){
  return (2*mu/pow(hbarc,2))*(Pot-T)*u + L*(L+1)*u/pow(r,2);
} 

vector<double> RKfouth (const double KE, int L, int nStep, double dr, WSPara wsp){

  // return last u = SolU[last][0];
  vector<double> output;
  output.clear();
  output.push_back(-1000.); // for u[nStep]
  output.push_back(-1000.); // for uMax;
  
  // output is a file "RK4.dat"

  //TODO using a array as potential

  const double scale = abs(wsp.V0);
  double rStart = 0.0001;
  double rEnd = rStart + dr * nStep;

  //printf("-------------- inputs\n");
  //printf(" KE: %6.2f MeV, L: %2d \n", KE, L);
  //printf(" V0: %6.2f MeV, R0: %6.2f fm, a0: %6.2f fm \n", V0, R0, a0);
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
    fprintf(paraOut, "# V0: %6.2f MeV, R0: %6.2f fm, a0: %6.2f fm\n", wsp.V0, wsp.R0, wsp.a0);
    fprintf(paraOut, "# start: %7.5f fm, dr:%5.3f, end: %10.5f fm, nStep: %3d \n", rStart, dr, rEnd, nStep);
    fprintf(paraOut, "#%12s %12s %12s %12s %12s %12s %12s %12s\n", "r", "Pot", "(Pot+Cent.)", "u", "du/dr", "R", "dR/dr", "KE");
    fprintf(paraOut, "#----------\n");
    fprintf(paraOut, " %12.3E %12.3E %12.3E %12.3E %12.3E %12.3E %12.3E %12.3E\n", rStart, Pot(rStart, wsp), Pot(rStart, wsp) + L*(L+1)/pow(rStart,2), SolU[0][0], SolU[0][1], SolU[0][0]/rStart, SolU[0][1]/rStart - SolU[0][0]/pow(rStart,2), KE);
  }
  
  double maxu = 0;  
  for(int i = 0; i <= nStep; i++){
    double r = rStart + i * dr;
    double pot = Pot(r, wsp);
    double potHalf = Pot(r+dr/2., wsp);
    double u = SolU[i][0];
    double v = SolU[i][1];

    double ku1 = v;
    double kv1 = G(r, u, KE, L, pot);

    double ku2 = v + kv1*dr/2;    
    double kv2 = G(r + dr/2, u + ku1*dr/2, KE, L, potHalf); 

    double ku3 = v + kv2*dr/2;    
    double kv3 = G(r + dr/2, u + ku2*dr/2, KE, L, potHalf);

    double ku4 = v + kv3*dr;    
    double kv4 = G(r + dr, u + ku3*dr, KE, L, Pot(r+dr, wsp));
    
    double du = dr/6*(ku1 + 2*ku2 + 2*ku3 + ku4);
    double dv = dr/6*(kv1 + 2*kv2 + 2*kv3 + kv4);

    //printf(" r:%8f, Pot:%12f, G:%12f, Pot1/2:%12f, u:%12f, v:%12f \n", r, pot, kv1, potHalf, u, v );
    //printf("            %12f, %12f, %12f, %12f, %12f, %12f, %12f, %12f, %12f, %12f \n", ku1, ku2, ku3, ku4, kv1, kv2, kv3, kv4, du, dv);
    
    SolU[i + 1][1] = v + dv;
    SolU[i + 1][0] = u + du;

    if ( u > maxu) maxu = u;

    if( i == nStep ) output[0] = u;
  }
  
  output[1] = maxu;
  maxu = 1;
  //printf("------- normalized ul \n"); //imporer normalized

  for( int i = 0; i <= nStep; i++){
    double r = rStart + i*dr;
    double pot = Pot(r+dr, wsp);
    double poteff = pot - pow(hbarc/(r+dr),2)*L*(L+1)/2/mu;
    //if ( abs(poteff) > abs(V0) ) poteff = 0.0;
    SolU[i+1][1] = SolU[i+1][1]/maxu;
    SolU[i+1][0] = SolU[i+1][0]/maxu;
    
    if( isSaveWaveFunction ){
      fprintf(paraOut, " %12f %12f %12f %12f %12f %12f %12f %12f\n", r+dr, pot, poteff , SolU[i+1][0], SolU[i+1][1], SolU[i+1][0]/r, SolU[i+1][1]/r - SolU[i+1][0]/pow(r,2), KE);
    }
  }

  if( isSaveWaveFunction) {
    fclose(paraOut);
    printf("----------done.\n");
  }
  
  return output;
}

#endif
