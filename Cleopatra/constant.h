/***********************************************************************
 * 
 *  This is constant.h, to provide various physical constants.
 * 
 *-------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/


#ifndef constant
#define constant
#include <cmath>

const double pi   = acos(-1.0);
const double E    = 2.718281828459 ;
const double hbar_SI = 1.054571628e-34;   //Js
const double kB   = 1.3806504e-23;     //JK^-1
const double e    = 1.602176487e-19;       //C
const double c_SI    = 299792458;         //ms^-1
const double me_SI   = 9.10938215e-31 ;   //kg
const double mp_SI   = 1.672621637e-27 ;  //kg
const double mn_SI   = 1.67492729e-27 ;   //kg
const double NA   = 6.022141e+23 ;     //mol^-1

const double deg2rad = pi/180 ;
const double rad2deg = 180/pi ;


//======================================================================
const double amu  = 931.494; // MeV/c^2
const double hbarc = 197.272; // MeV fm;
const double c = 299.792458; // mm/ns; 

//======================================================================
double kg2MeV(double m){
    return m*c_SI*c_SI/e/1e6;
}

double T2Brho(double mass, int Z, int A, double T){
    //mass in MeV
    // Z in e
    // T in MeV/A
    double gamma = (T*A + mass)/mass;
    double beta = sqrt(1-1/gamma/gamma);
    return mass*beta*gamma/Z/c;
}

double Brho2T(double mass, int Z, int A, double Brho){
    //mass in MeV
    // Z in e
    return (sqrt(pow(Brho*Z*c,2)+mass*mass)-mass)/A;
}

double T2beta(double mass, int A, double T){
	double gamma = 1.0 + T*A/mass;
	return sqrt(1-1/gamma/gamma);
}

double ev2nm(double eV){
  // photon energy to nm
  return hbarc/2/pi/eV;
}

//======================================================================
const double mp = kg2MeV(mp_SI);
const double mn = kg2MeV(mn_SI);


#endif
