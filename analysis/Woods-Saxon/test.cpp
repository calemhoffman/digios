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

int main(int argc, char *argv[]){

  double Energy = 2.0;
  if( argc == 2 ) Energy = atof(argv[1]);
  
  WoodsSaxon ws;

  ws.V0 = -43;
  ws.r0 = 1.30;
  ws.a0 = 0.71;
  
  ws.VSO = 27;
  ws.rSO = 1.20;
  ws.aSO = 0.68;
  
  ws.Z = 0; 
  ws.Rc = 1.25; 
  
  ws.nStep = 1000; 
  ws.dr = 0.1; 

  ws.IsNeutron();

  ws.A = 16;
  
  ws.ClearVector();
  ws.CalRadius();

  ws.PrintWSParas();

  double J = 2.5;
  double L = 2;
  
  ws.LS = (J*(J+1) - L*(L+1) - 0.5*(1.5))/2.;

  printf("Barrier height : %f MeV \n", ws.FindBarrier(L));  


  double waveNumber = sqrt(2*mn*Energy)/hbarc;
  double period  = 2*3.1415925 / waveNumber;
  printf("Free scattering wave number = %f / fm \n", waveNumber ); 
  printf("                     period = %f  fm \n", period ); 
    
  ws.SaveWaveFunction();
  ws.SolveRadialFunction( Energy, L);
  
  vector<double> wf = ws.wf;

  //find wave function amplitude
  int n = wf.size();
  double maxU = 0;
  for( int i = 400 ; i < 400 + 2* period / ws.dr; i++){
    //printf(" %d  %f \n", i, wf[i]);
    if( wf[i] > maxU ) maxU = wf[i];
  }

  printf(" max U = %f \n", maxU );

  //fitting with A*sin(x + phi) to get the phase shift


  TApplication theApp("App",&argc, argv);
  
  
  TGraph * gr = new TGraph();
  for(int i = 400; i < n; i++){
    gr->SetPoint(i-400, ws.dr*i, wf[i]);
  }

  gr->Draw("A*");
  
  TF1 * fit = new TF1 ("fit", "[0]*sin([1]*x + [2])");
  fit->FixParameter(1, waveNumber);

  gr->Fit(fit);

  double p0 = fit->GetParameter(0);
  double p1 = fit->GetParameter(2);

  printf(" p0: %f, p1: %f \n", p0, p1 * TMath::RadToDeg() );
  
  
  //.CalWSEnergies();
  //print all energy
  //ws.PrintEnergyLevels();
  //printf("========== end of program ======= \n");

  theApp.Run();
  
  return 0;
}

