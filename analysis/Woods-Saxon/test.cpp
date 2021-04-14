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

  RKFourth rk; // the default rk solve give gaussian 

  rk.SetBoundaryCondition(1.,0.);

  rk.SetRange(0.0001, 4., 200);
  
  rk.SolveRK4(0);

  rk.NormalizeSolution();

  vector<double> u = rk.GetSolution();

  double dr = rk.Getdr();
  double rStart = rk.GetrStart();

  TApplication theApp("App",&argc, argv);

  TGraph * g = new TGraph();
  for( int i = 0; i <= rk.GetNStep(); i++){
    
    g->SetPoint(i, rStart + i*dr, u[i]);

  }

  g->Draw("Al");

  theApp.Run();
};
