#ifndef HIT_h
#define HIT_h

#include <vector>
#include <cmath> // Required for std::nan

class Hit{

public:
  Hit(){
    Clear();
  }
  ~Hit(){};

  int detID;
  double e; // in MeV
  double x; // in mm
  double z; // in mm
  unsigned long long e_t;

  // recoil
  double dE[4];
  double EE[4];
  unsigned long long dE_t[4];
  unsigned long long EE_t[4];
  unsigned short nRDT;

  double Ex;
  double thetaCM;

  void Clear();
  void CalRDTMultiplicity();

private:


};

//^#######################################
inline void Hit::Clear(){
  detID = -1;
  e = std::nan("");
  x = std::nan("");
  z = std::nan("");
  e_t = 0;

  nRDT = 0;
  for( int i = 0; i < 4; i++){
    dE[i] = std::nan("");
    EE[i] = std::nan("");
    dE_t[i] = 0;
    EE_t[i] = 0;
  }

  Ex = std::nan("");
  thetaCM = std::nan("");

}

inline void Hit::CalRDTMultiplicity(){

  nRDT = 0 ;
  for( int i = 0; i < 4 ; i++ ){
    if( std::isnan(dE[i]) || std::isnan(EE[i]) ) continue;
    if( dE[i] < 0  || EE[i] < 0 ) continue;
    nRDT ++;
  }

}

#endif