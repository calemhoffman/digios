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

#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"
#include "TApplication.h"

using namespace std;

double FindValue(vector<double> vec, double x, double dx = 0.1){

  int n = x / dx + 1;

  int size = vec.size();

  //printf("x: %f,  n: %d, size : %d\n",x, n, size);

  if( n > size ) return 0;
  
  double l = x - n * dx;
  
  double y1 = vec[n];
  double y2 = vec[n+1];

  
  //printf(" x : %f, y1 : %f, y2 : %f, l: %f \n", x, y1, y2, l);
  
  return y1 + (y2-y1) * l / dx;
  
}

int main(int argc, char *argv[]){

  double Energy = 2.0;
  //if( argc == 2 ) Energy = atof(argv[1]);
  
  WoodsSaxon ws;
  
  ws.A = 197;

  double J = 1.5;
  double L = 2;
  
  ws.N = 127;

  ws.V0 = -50.25 * ( 1 - 0.67 * ( 2 * ws.N - ws.A) / ws.A ) ;
  ws.r0 = 1.30;
  ws.a0 = 0.71;
  
  ws.VSO = 27;
  ws.rSO = 1.20;
  ws.aSO = 0.68;
  
  ws.Z = 0; 
  ws.Rc = 1.25; 
  
  ws.nStep = 2000; 
  ws.dr = 0.1; 

  ws.IsNeutron();

  ws.ClearVector();
  ws.CalRadius();

  ws.PrintWSParas();

  ws.LS = (J*(J+1) - L*(L+1) - 0.5*(1.5))/2.;

  TApplication theApp("App",&argc, argv);
  
  printf("Barrier height : %f MeV \n", ws.FindBarrier(L));

  TGraph * g1 = new TGraph();
  int count = 0;
  for( Energy = 0.05; Energy <= .5; Energy += 0.05 ){

    if( argc == 2) Energy = atof(argv[1]);
    
    printf("---------------- Enegry :%f \n", Energy);
    double waveNumber = sqrt(2*mn*Energy)/hbarc;
    double period  = 2*3.1415925 / waveNumber;
    printf("Free scattering wave number = %f / fm \n", waveNumber ); 
    printf("                     period = %f  fm \n", period ); 
    
    if( argc == 2 ) ws.SaveWaveFunction();
    ws.ClearVector();
    ws.SolveRadialFunction( Energy, L);
  
    vector<double> wf = ws.wf;

    //find wave function amplitude
    int n = wf.size();
    int nStart = 1000;
    double maxU = 0;
    for( int i = nStart ; i < nStart + 2 * period / ws.dr; i++){
      //printf(" %d  %f \n", i, wf[i]);
      if( wf[i] > maxU ) maxU = wf[i];
    }

    printf(" max U = %f \n", maxU );

    //fitting with A*sin(x + phi) to get the phase shift
  
    TGraph * gr = new TGraph();
    for(int i = nStart; i < n; i++){
      gr->SetPoint(i - nStart, ws.dr*i, wf[i]);
    }

    gr->Draw("AC*");
  
    TF1 * fit = new TF1 ("fit", "[0]*sin([1]*x + [2])");
    fit->SetParameter(0, maxU);
    fit->FixParameter(1, waveNumber);
    fit->SetParameter(2, 0);

    fit->SetParLimits(2, -3.142, 3.142 );

    gr->Fit(fit, "Q");

    double p0 = fit->GetParameter(0);
    double p1 = fit->GetParameter(2) * TMath::RadToDeg();

    double e0 = fit->GetParError(0);
    double e1 = fit->GetParError(2) * TMath::RadToDeg();

    printf(" p0: %f (%f) \n", p0, e0 );
    printf(" p1: %f (%f) \n", p1, e1 );

    if( argc == 2) break;
    
    g1->SetPoint( count , Energy, p1);
    count++;
  }

  if( argc == 1 ) g1->Draw("AC*");
    
    
  //.CalWSEnergies();
  //print all energy
  //ws.PrintEnergyLevels();
  //printf("========== end of program ======= \n");

  /*
  printf("===================== Find value at n * PI() / k \n");
  int maxI = ws.nStep * ws.dr * waveNumber / 3.1415925 ;
  for( int i = 1; i < maxI ; i++){
    double pos = i * 3.1415925 / waveNumber;

    printf("i=%2d, pos=%8.3f, %9.3f \n", i, pos, FindValue(wf, pos));

  }
  */
  
  theApp.Run();
  
  return 0;
}

