/****************************************************************
  Rungu-Kuttan 4 method on solving 1-D Differential equation
	d^2u/dr^2 = G(r,u)
  
  In the Pot(r), the potential is for Wood-Saxon.
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-1-31
*****************************************************************/

#ifndef RK4
#define RK4

#include <stdio.h>
#include <stdlib.h> ///for atoi, atof
#include <cmath> ///exp
#include <vector> 

using namespace std;

class RKFourth {
  
public:

  RKFourth(){

    parC[0] = 0.;    parD[0] = 1./6;
    parC[1] = 1./2;  parD[1] = 2./6;
    parC[2] = 1./2;  parD[2] = 2./6;
    parC[3] = 1.;    parD[3] = 1./6;

    rStart = 0.0001;
    dr = 0.1;
    nStep = 200;

    sol0  = 0.;
    dsol0 = 1.;

    isSaveSolution = false;
    sol.clear();
  }

  ~RKFourth(){}

  void SetRange(double xStart, double xEnd, int Step);
  void SetRange2(double xStart, double dx, int Step);
  void SetBoundaryCondition(double y0, double dy0 );
  void SetSaveSolution(bool save);
  
  void SolveRK4(bool debug);
  void NormalizeSolution();

  int GetNStep()     {return nStep;}
  double GetrStart() {return rStart;}
  double Getdr()     {return dr;}


  vector<double> GetSolution()    {return sol;}
  double         GetPoint(int i ) { return (0 <= i && i <= nStep) ? sol[i] : -404; }
  double         GetLastPoint()   { return sol.back(); }
  double         GetMaxSolution() { return maxy; }

protected:

  double parC[4];
  double parD[4];

  double rStart; /// in fm
  double dr;     /// in fm
  int nStep;     /// number of step

  double sol0, dsol0; /// sol at 0, dsol at 0
  vector<double> sol; /// the solution
  double maxy;
  
  bool isSaveSolution;

  ///============= Potential Template
  virtual double Pot(double r){ return 0; }

  ///=============== The G-function, d^2y/dr^2 = G(r, y, dy/dr)
  virtual double G(double x, double y, double dy){ return  - 2 * x * dy - 2 * y ;  } /// solution of guassian

};


void RKFourth::SetRange(double xStart, double xEnd, int Step){
  this->rStart = xStart;
  this->nStep = Step;
  this->dr = (xEnd - xStart)/Step;
}

void RKFourth::SetRange2(double xStart, double dx, int Step){
  this->rStart = xStart;
  this->nStep = Step;
  this->dr = dx;
}

void RKFourth::SetBoundaryCondition(double y0, double dy0 ){
  this->sol0 = y0;
  this->dsol0 = dy0;
}


void RKFourth::SetSaveSolution(bool save = true){
  if( save == true ) {
    isSaveSolution = true;
  }else{
    isSaveSolution = false;
  }
}

//=============== This function output u[max] and uMax
void RKFourth::SolveRK4 (bool debug = false){ 

  double rEnd = rStart + dr * nStep;

  if( debug){
    printf("-------------- inputs\n");
    printf(" start: %7.5f fm, dr:%5.3f, end: %10.5f fm, nStep: %3d \n", rStart, dr, rEnd, nStep);
  }
  
  const int nStep1 = nStep + 1 ;
  double SolU[nStep1][2]; /// SolU[i][0] is U, SolU[i][1] is du/dr

  SolU[0][0] =  sol0; /// solution
  SolU[0][1] = dsol0; /// derivative of solution

  FILE * paraOut = NULL;
  if( isSaveSolution ){
    paraOut = fopen ("RK4.dat", "w");
    fprintf(paraOut, "# start: %7.5f fm, dr:%5.3f, end: %10.5f fm, nStep: %3d \n", rStart, dr, rEnd, nStep);
    fprintf(paraOut, "#%12s %12s %12s %12s %12s\n", "r", "Pot", "(Pot+Cent.)", "u", "du/dr");
    fprintf(paraOut, "#----------\n");
  }

  double dyy[5];
  double dzz[5];
  double r, y, z, dy, dz ;

  sol.clear();

  maxy = -1; 
  for(int i = 0; i <= nStep; i++){

    r = rStart + i * dr;
    y = SolU[i][0];
    z = SolU[i][1];

    dyy[0] = 1.;
    dzz[0] = 1.;
    for( int j = 1 ; j <= 4 ; j++){
      //dyy[j] = z + parC[j-1]*dzz[j-1]*dr;
      //dzz[j] = G(r + parC[j-1]*dr, y + parC[j-1]*dyy[j-1]*dr, z + parC[j-1]*dzz[j-1]*dr);
      dyy[j] = dr* (z + parC[j-1]*dzz[j-1]);
      dzz[j] = dr* G(r + parC[j-1]*dr, y + parC[j-1]*dyy[j-1], z + parC[j-1]*dzz[j-1]);
    }

    dy = 0;
    dz = 0;
    for( int j = 1; j <= 4; j++){
      //dy += dr * parD[j-1] * dyy[j];
      //dz += dr * parD[j-1] * dzz[j];
      dy += parD[j-1] * dyy[j];
      dz += parD[j-1] * dzz[j];
    }

    SolU[i + 1][0] = y + dy;
    SolU[i + 1][1] = z + dz;

    if ( y > maxy) maxy = y;

    if( i < 10 && debug ) {
      if ( i == 0 ) printf("%12s, %12s, %12s, %12s| %12s| %12s, %12s, %12s, %12s, %12s| %12s, %12s, %12s, %12s, %12s\n", "r", "G(r,y,z)", "y", "z", "Pot(r+dr/2)", "dy", "dyy[1]", "dyy[2]","dyy[3]", "dyy[4]", "dz", "dzz[1]", "dzz[2]", "dzz[3]", "dzz[4]");
      printf("%12f, %12f, %12f, %12f| %12f| %12f, %12f, %12f, %12f, %12f| %12f, %12f, %12f, %12f, %12f\n", r, G(r,y,z), y, z, Pot(r+dr/2.), dy, dyy[1], dyy[2], dyy[3], dyy[4], dz, dzz[1], dzz[2], dzz[3], dzz[4]);
    }
    
    if( isSaveSolution )   fprintf(paraOut, " %12.3E %12.3E %12.3E %12.3E %12.3E \n", rStart, Pot(rStart), G(rStart, SolU[0][0], Pot(rStart)), SolU[0][0], SolU[0][1]);

    sol.push_back(y);
    
  }

  if( isSaveSolution) fclose(paraOut);
  
}


void RKFourth::NormalizeSolution(){

  double norm = 0 ;
  for( int i = 0; i < nStep; i++){
    norm += sol[i]*sol[i] * dr;
  }
  norm = sqrt(norm);

  printf("norm : %f \n", norm);

  for( int i = 0; i < nStep; i++){
    sol[i] = sol[i]/norm;
  }

}


#endif
