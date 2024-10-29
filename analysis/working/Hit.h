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

  float te_t;
  float te_r;
  float te;

  // recoil
  double dE[4];
  double EE[4];
  unsigned long long dE_t[4];
  unsigned long long EE_t[4];
  float tdE_t[4];
  float tdE_r[4];
  float tEE_t[4];
  float tEE_r[4];
  unsigned short nRDT;

  double tDiff;
  double tDiffFine;
  double tDiffFineC; // corrected

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
  te_t = std::nan("");
  te_r = std::nan("");
  te   = std::nan("");

  nRDT = 0;
  for( int i = 0; i < 4; i++){
    dE[i] = std::nan("");
    EE[i] = std::nan("");
    dE_t[i] = 0;
    EE_t[i] = 0;
    tdE_t[i] = std::nan("");
    tEE_t[i] = std::nan("");
    tdE_r[i] = std::nan("");
    tEE_r[i] = std::nan("");
  }

  Ex = std::nan("");
  thetaCM = std::nan("");

  tDiff = std::nan("");
  tDiffFine = std::nan("");
  tDiffFineC = std::nan("");

}

inline void Hit::CalRDTMultiplicity(){

  nRDT = 0 ;
  int ID = -1;
  for( int i = 0; i < 4 ; i++ ){
    if( std::isnan(dE[i]) || std::isnan(EE[i]) ) continue;
    if( dE[i] < 0  || EE[i] < 0 ) continue;
    nRDT ++;
    ID = i;
  }

  if(nRDT == 1 ){
    if( e_t >= dE_t[ID] ){
      tDiff = (e_t - dE_t[ID])*(1.);
    }else{
      tDiff = (dE_t[ID] - e_t)*(-1.);
    }

    // double traceTime = tdE_t[ID]+ tdE_r[ID]*TMath::Log(100./(100.-50.));
    double traceTime = tdE_t[ID];
    tDiffFine = tDiff + (te_t - traceTime);
    tDiffFineC = tDiffFine;

  }

}

#endif