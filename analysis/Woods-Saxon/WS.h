
/****************************************************************
  Eigen-energies for Wood-Saxon potential with Spin-Orbital.
  Using Rungu-Kuttan-4 method to solve the wavefunction numerically

  When the energy is not eigen-energyies, the wavefunction is diverge. 
  So, by checking the wavefunction at far-away, the eigen-energy can be found.
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-1-31
*****************************************************************/

//TODO convert to class

#include <stdio.h>
#include <string>
#include <stdlib.h> //for atoi, atof
#include <cmath>
#include <vector> 
#include "RK4.h"

using namespace std;

class WoodsSaxon : public RKFourth{

public:
  WoodsSaxon(){}
  ~WoodsSaxon(){}
	
  //=== output
  vector<double> energy;
  vector<string> orbString;
  vector<double> errorU; 
  vector<double> errorE; 
  vector<double> errorUratio; 

  int A, N; // mass number, neutron number
  //charge number already in class RKFourth
  double r0, rSO, rc; // reduced mass

  void IsNeutron(){ this->mu = mn;}
  void IsProton(){ this->mu = mp;}

  void ClearVector(){
    energy.clear();
    orbString.clear();
    errorU.clear();
    errorE.clear();
    errorUratio.clear();
    wf.clear();
  }
  
  void SetZ(int Z) {
    this->Z = Z;
    if( Z != 0) {
      this->mu = mp;
    }else{
      this->mu = mn;
    }
  }

  void SetMass(double mass){
    this->mu = mass;
  }

  void CalRadius(){
    this->R0  = r0  * pow(A, 1./3.);
    this->RSO = rSO * pow(A, 1./3.);
    this->Rc  = rc  * pow(A, 1./3.);
  }
  
  void SetWSRadius(int A, int Z,  double r0, double rSO, double rc){
    this->A = A;
    this->Z = Z;
    this->r0 = r0;
    this->rSO = rSO;
    this->rc = rc;
		
    this->R0  = r0  * pow(A, 1./3.);
    this->RSO = rSO * pow(A, 1./3.);
    this->Rc  = rc  * pow(A, 1./3.);

    if( Z != 0) {
      this->mu = mp;
    }else{
      this->mu = mn;
    }
  }

  double GetFreeWaveNumber(double Energy){
    if( Energy < 0 ) return 0;
    return sqrt(2*mn*Energy)/hbarc; 
  }

  void PrintEnergyLevels(){
    printf("================ result\n");
    printf("  | %8s,  %12s | %12s | %12s | %12s\n", "orbital", "energy", "errorU", "errorE", "errorUratio");  
    for( int i = 0; i < (int) energy.size() ; i++){
      printf("%2d| %8s,  %12.6f | %12.5f | %12.4E | %12f\n",i, orbString[i].c_str(), energy[i], errorU[i], errorE[i], errorUratio[i]);  
    }
  }

  void PrintWSParas(){
    printf("================ Woods-Saxon parameters \n");
    printf(" Reduced Mass : %f MeV/c2\n", mu);
    printf("  A: %d, N: %d, dr:%5.3f fm, nStep: %3d, range: %5.3f fm \n", A, N, dr, nStep, dr * nStep);
    printf("-------------------------------\n");
    printf(" V0: %8.4f MeV,  R0: %8.4f(%4.2f) fm,  a0: %8.4f fm \n", V0,   R0,  r0 , a0);
    printf("VSO: %8.4f MeV, RS0: %8.4f(%4.2f) fm, aS0: %8.4f fm \n", VSO,  RSO, rSO, aSO);
    printf("  Z: %8.0f,      Rc: %8.4f(%4.2f) fm\n", Z, Rc, rc);
    printf("================================\n");
  }

  // the potential parameters are global values
  int CalWSEnergies(bool useBarrier = false, int maxL = 7, double torr = 500, double eTorr = 0.001, int maxLoop = 300, double dKE = 0.2){
	  
    energy.clear();
    orbString.clear();
    errorU.clear();
    errorE.clear();
    errorUratio.clear();
	  
    double u = 0;
    double uOld = 0;
	  
    for( int L = 0 ; L <= maxL; L++ ){

      string orbital;

      if( L == 0 ) orbital = "s";
      if( L == 1 ) orbital = "p";
      if( L == 2 ) orbital = "d";
      if( L == 3 ) orbital = "f";
      if( L == 4 ) orbital = "g";
      if( L == 5 ) orbital = "h";
      if( L == 6 ) orbital = "i";
      if( L == 7 ) orbital = "j";
      if( L == 8 ) orbital = "k";

      //printf("----------- L : %d \n", L);

      for( float J = L + 0.5 ;  J >= abs(L - 0.5) ; J = J - 1. ){

	if( VSO == 0. &&  J < L + 0.5 ) continue;

	LS = (J*(J+1) - L*(L+1) - 0.5*(1.5))/2.;
	//printf(" ----------------  L, J = %d, %3.1f | LS : %f \n", L, J, LS); 
	int nValue = 0;
	for( float KE = V0 ; KE < 0 ; KE = KE + dKE ){
				
	  vector<double> outputRK = SolveRadialFunction(KE, L); // RKfouth will output the wavefunction at the furtherst distant.
			  
	  u = outputRK[0];

	  //printf(" ====  KE , uOdl,  u : %f , %f,  %f \n", KE, uOld,  u);

	  if( KE == V0 ) {
	    uOld = u;
	    continue;
	  }

	  //check crossing point 
	  if( u * uOld < 0 ) {
	    //use bi-section method to find zero point 
	    double e1 = KE - dKE;
	    double e2 = KE;
	    double w , wr; 
	    double e , de;
	    int loop = 0;

	    do{
	      loop ++;
	      e = (e1 + e2)/2.;
	      de = abs( e-e1);
	      vector<double> out = SolveRadialFunction(e, L);
	      w = out[0];
	      wr = out[0]/out[1];
	      if( loop > maxLoop ) break;

	      //printf(" %d|  %f< e = %f< %f |  w : %f\n", loop,  e1, e, e2,  w);

	      if( uOld * w < 0 ) {
		e2 = e;
		continue;
	      }

	      if( w * u < 0 ){
		e1 = e;
		continue;
	      }
	    }while( abs(w) > torr || de > eTorr );

	    char buffer[100];
	    int nn = sprintf(buffer, "%d%s%d/2", nValue, orbital.c_str(), J > L ? 2*L+1 : 2*L-1);

	    string temp = buffer;
	    orbString.push_back(temp);
	    if( useBarrier ) {
	      double barrier = FindBarrier(L);
	      //printf(" J=%3.1f, L = %d,  barrier = %f \n", J, L, barrier); 
	      energy.push_back(e - barrier);
	    }else{
	      energy.push_back(e);
	    }
	    errorU.push_back(w);
	    errorE.push_back(de);
	    errorUratio.push_back(wr); // It seems that wRatio is useless
				 
	    if( loop < maxLoop || (abs(w) < torr && de < eTorr && wr < 0.01)) nValue++;
				 
	    //printf(" %s : %12.6f (Last u = %f ), loop %d, de %f, wr %f \n", temp.c_str(),  e, w , loop, de, wr);
	  }

	  uOld = u;

	} // end of KE-loop
      } // end of J-loop
    } // end of L-loop
	  
    //sorting energy, dubble sort
    int i, j ;
    bool swapped;
    int size = energy.size();
    for (i = 0; i < size-1; i++) { 
      swapped = false; 
      for (j = 0; j < size-i-1; j++) { 
	if (energy[j] > energy[j+1]) { 
	  double temp = energy[j+1];
	  energy[j+1] = energy[j];
	  energy[j] = temp;

	  string tempstr = orbString[j+1];
	  orbString[j+1] = orbString[j];
	  orbString[j] = tempstr;

	  temp = errorU[j+1];
	  errorU[j+1] = errorU[j];
	  errorU[j] = temp;
			  
	  temp = errorE[j+1];
	  errorE[j+1] = errorE[j];
	  errorE[j] = temp;
			  
	  temp = errorUratio[j+1];
	  errorUratio[j+1] = errorUratio[j];
	  errorUratio[j] = temp;

	  swapped = true; 
	} 
      } 

      // IF no two elements were swapped by inner loop, then break 
      if (swapped == false) 
	break; 
    } 
	  
	  
    //Erase incorrect energy
    for( int i = energy.size() - 1 ; i >=0  ; i--){
      //printf("%s, energy: %f, errorU : |%f|,  torr :  %f \n", orbString[i].c_str(), energy[i],  errorU[i], torr); 
			
      //if( errorU[i] > torr || errorU[i] < -torr || errorE[i] > eTorr){
      if( abs(errorU[i]) > torr || errorE[i] > eTorr){
			 
	orbString.erase(orbString.begin() + i);
	energy.erase(energy.begin() + i);
	errorU.erase(errorU.begin() + i);
	errorE.erase(errorE.begin() + i);
	errorUratio.erase(errorUratio.begin() + i);
			
      }
    }
	  
    return 0;
	  
  }

};



