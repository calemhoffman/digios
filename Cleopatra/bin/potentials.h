/***********************************************************************
 * 
 *  This is potenital.h, To calculate optical potenital
 *  It contains various optical potential for proton and deuteron
 *  It is as same as Kay Ben's globals_beta_v5
 * 
 *-------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/
 
//golbal varibale for Optical Potential
double v, r0, a;
double vi, ri0, ai;
double vsi, rsi0, asi;
double vso, rso0, aso;
double vsoi, rsoi0, asoi, rc0;

void PrintPotential(){
  printf("v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
  printf("vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
  printf("vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
  printf("vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
  printf("vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
}

string potentialRef(string name){
   
  if( name == "A" ){
    return "An and Cai (2006) E < 183 | 12 < A < 238 | http://dx.doi.org/10.1103/PhysRevC.73.054605";
  }
  if( name == "H" ){
    return "Han, Shi, Shen (2006) E < 200 | 12 < A < 209 | http://dx.doi.org/10.1103/PhysRevC.74.044615";
  }
  if( name == "B" ){
    return "Bojowald et al.(1988) 50 < E < 80 | 27 < A < 208 | http://dx.doi.org/10.1103/PhysRevC.38.1153";
  }
  if( name == "D" ){
    return "Daehnick, Childs, Vrcelj (1980) 11.8 < E < 80 | 27 < A < 238 (REL) | http://dx.doi.org/10.1103/PhysRevC.21.2253";
  }
  if( name == "C" ){
    return "Daehnick, Childs, Vrcelj (1980) 11.8 < E < 80 | 27 < A < 238 (NON-REL) | http://dx.doi.org/10.1103/PhysRevC.21.2253";
  }
  if( name == "L" ){
    return "Lohr and Haeberli (1974) 9 < E < 13 | 40 < A | http://dx.doi.org/10.1016/0375-9474(74)90627-7";
  }
  if( name == "Q" ){
    return "Perey and Perey (1963) 12 < E < 25  | 40 < A | http://dx.doi.org/10.1016/0370-1573(91)90039-O";
  }
   
   
  if( name == "K" ){
    return "Koning and Delaroche (2009) 0.001 < E < 200 | 24 < A < 209 | Iso. Dep. | http://dx.doi.org/10.1016/S0375-9474(02)01321-0";
  }
  if( name == "V" ){
    return "Varner et al., (CH89) (1991) 16 < E < 65 | 4 < A < 209 | http://dx.doi.org/10.1016/0370-1573(91)90039-O";
  }
  if( name == "M" ){
    return "Menet et al. (1971) 30 <  E < 60 | 40 < A | http://dx/doi.org/10.1016/0092-640X(76)90007-3";
  }
  if( name == "G" ){
    return "Becchetti and Greenlees (1969) E < 50 | 40 < A | http://dx.doi.org/10.1103/PhysRev.182.1190";
  }
  if( name == "P" ){
    return "Perey (1963) E < 20 | 30 < A < 100 | http://dx/doi.org/10.1016/0092-640X(76)90007-3";
  }
   
  return "";
}

//======================== deuteron 
bool AnCaiPotential(int A, int Z, double E){
  // d + A(Z)
  // E < 183 or 81.5 MeV/u
  // 12 < A < 238
  // http://dx.doi.org/10.1103/PhysRevC.73.054605
  
  //if( !(12 <= A &&  A <= 238 ) ) return false;
  //if( E > 183 ) return false;

  double A3 = pow(A, 1./3.);

  v  = 91.85 - 0.249*E + 0.000116*pow(E,2) + 0.642 * Z / A3;
  r0 = 1.152 - 0.00776 / A3;
  a  = 0.719 + 0.0126 * A3;

  vi  = 1.104 + 0.0622 * E;
  ri0 = 1.305 + 0.0997 / A3;
  ai  = 0.855 - 0.1 * A3;

  vsi  = 10.83 - 0.0306 * E;
  rsi0 = 1.334 + 0.152 / A3;
  asi  = 0.531 + 0.062 * A3;

  vso  = 3.557;
  rso0 = 0.972;
  aso  = 1.011;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.303;

  return true;
}

bool HSSPotential(int A, int Z, double E){
  // d + A(Z)
  // E < 200 or 100 MeV/u
  // 12 < A < 209
  // http://dx.doi.org/10.1103/PhysRevC.73.054605
  
  //if( !(12 <= A &&  A <= 209 ) ) return false;
  //if( E > 200 ) return false;

  int N = A-Z;
  double A3 = pow(A, 1./3.);
  
  double VIcond = -4.916 + (0.0555*E) + 0.0000442 * pow(E,2) +  35. * (N-Z)/A;

  v  = 82.18 - 0.148 * E - 0.000886 * pow(E,2) - 34.811*(N-Z)/A + 1.058*Z/A3;
  r0 = 1.174;
  a  = 0.809;

  vi  = VIcond > 0 ? VIcond : 0 ;
  ri0 = VIcond > 0 ? 1.563 : 0;
  ai  = VIcond > 0 ? 0.7 + 0.045 * A3 : 0;

  vsi  = 20.968 - 0.0794 * E - 43.398 * (N-Z)/A;
  rsi0 = 1.328;
  asi  = 0.465 + 0.045*A3;

  vso  = 3.703;
  rso0 = 1.234;
  aso  = 0.813;

  vsoi  = -0.206;
  rsoi0 = 1.234;
  asoi  = 0.813;

  rc0 = 1.698;
  
  //PrintPotential();

  return true;
}

bool BojowaldPotential(int A, int Z, double E){
  // d + A(Z)
  // 50 < E < 80 or 25, 40 MeV/u
  // 27 < A < 208
  // http://dx.doi.org/10.1103/PhysRevC.38.1153
  
  //if( !(27 <= A &&  A <= 208 ) ) return false;
  //if( E > 80 || E < 27) return false;

  int N = A-Z;
  double A3 = pow(A, 1./3.);
  
  double VIcond = 0.132 * (E - 45.);

  v  = 81.33 - 0.24 * E + 1.43 * Z / A3;
  r0 = 1.18;
  a  = 0.636 + 0.035 * A3;

  vi  = VIcond > 0 ? VIcond : 0 ;
  ri0 = 1.27;
  ai  = 0.768 + 0.021 * A3;

  vsi  = 7.8 + 1.04 * A3 - 0.712 * vi;
  rsi0 = 1.27;
  asi  = 0.768 + 0.021 * A3;

  vso  = 6.0;
  rso0 = 0.78 + 0.038 * A3;
  aso  = 0.78 + 0.038 * A3;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.3;
  
  //PrintPotential();

  return true;
}

bool DaehnickPotential(int A, int Z, double E){
  // d + A(Z)
  // 11.8 < E < 80 or 5.4, 40 MeV/u
  // 27 < A < 238
  // http://dx.doi.org/10.1103/PhysRevC.21.2253
  
  //if( !(27 <= A &&  A <= 238 ) ) return false;
  //if( E > 80 || E < 11.8) return false;

  int N = A-Z;
  double A3 = pow(A, 1./3.);
  
  double beta = -1.* pow(E/100.,2); 
  
  double MU1 = exp(-1*pow((8. - N)/2.,2));
  double MU2 = exp(-1*pow((20. - N)/2.,2));
  double MU3 = exp(-1*pow((28. - N)/2.,2));
  double MU4 = exp(-1*pow((50. - N)/2.,2));
  double MU5 = exp(-1*pow((82. - N)/2.,2));
  double MU6 = exp(-1*pow((126. - N)/2.,2));
  
  double MU = MU1 + MU2 + MU3 + MU4 + MU5 + MU6;
  
  v  = 88. - 0.283 * E + 0.88 * Z / A3;
  r0 = 1.17;
  a  = 0.717 + 0.0012 * E;

  vi  = (12. + 0.031 * E )*(1 - exp(beta));
  ri0 = 1.376 - 0.01 * sqrt(E);
  ai  = 0.52 + 0.07 * A3  - 0.04 * MU;

  vsi  = (12. + 0.031 * E) * exp(beta);
  rsi0 = ri0;
  asi  = ai;

  vso  = 7.2 - 0.032 * E;
  rso0 = 1.07;
  aso  = 0.66;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.3;
  
  //PrintPotential();

  return true;
}

bool LohrPotential(int A, int Z, double E){
  // d + A(Z)
  // 9 < E < 13 or 4.5, 6.5 MeV/u
  // 40 < A 
  // http://dx.doi.org/10.1016/0375-9474(74)90627-7
  
  //if( !(40 <= A ) ) return false;
  //if( E > 13 || E < 9) return false;

  int N = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 91.13 + 2.2 * Z / A3;
  r0 = 1.05;
  a  = 0.86;

  vi  = 0;
  ri0 = 0;
  ai  = 0;

  vsi  = 218./pow(A,2./3.);
  rsi0 = 1.43;
  asi  = 0.5 + 0.013 * pow(A,2./3.);

  vso  = 7;
  rso0 = 0.75;
  aso  = 0.5;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.3;
  
  //PrintPotential();

  return true;
}

bool PereyPereyPotential(int A, int Z, double E){
  // d + A(Z)
  // 12 < E < 25 or 6, 12.5 MeV/u
  // 40 < A 
  // http://dx.doi.org/10.1016/0375-9474(74)90627-7
  
  //if( !(40 <= A ) ) return false;
  //if( E > 25 || E < 12) return false;

  int N = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 81 - 0.22*E + 2 * Z / A3;
  r0 = 1.15;
  a  = 0.81;

  vi  = 0;
  ri0 = 0;
  ai  = 0;

  vsi  = 14.4 + 0.24 * E;
  rsi0 = 1.34;
  asi  = 0.68;

  vso  = 0.;
  rso0 = 0.;
  aso  = 0.;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.15;
  
  //PrintPotential();

  return true;
}


//======================== proton 
bool KoningPotential(int A, int Z, double E){
  // p + A(Z)
  // E < 200 or 200 MeV/u
  // 24 < A < 209
  // http://dx.doi.org/10.1016/S0375-9474(02)01321-0
  
  //if( !(24 <= A &&  A <= 209 ) ) return false;
  //if( E > 200 ) return false;

  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  double vp1 = 59.3 + 21.*(N-Z)/A - 0.024*A;
  double vn1 = 59.3 - 21.*(N-Z)/A - 0.024*A;
  
  double vp2 = 0.007067 + 0.00000423*A;
  double vn2 = 0.007228 - 0.00000148*A;
  
  double vp3 = 0.00001729 + 0.00000001136 * A;
  double vn3 = 0.00001994 - 0.00000002 * A;
  
  double vp4 = 7e-9; // = vn4
  
  double wp1 = 14.667 + 0.009629*A;
  double wn1 = 12.195 + 0.0167*A;
  
  double wp2 = 73.55 + 0.0795*A; // = wn2
  
  double dp1 = 16 + 16.*(N-Z)/A;
  double dn2 = 16 - 16.*(N-Z)/A;
  
  double dp2 = 0.018 + 0.003802/(1 + exp((A-156.)/8)); // = dn2
  
  double dp3 = 11.5 ; // = dn3
  
  double vso1 = 5.922 + 0.003 * A;
  double vso2 = 0.004;
  
  double wso1 = -3.1;
  double wso2 = 160;
  
  double epf = -8.4075 + 0.01378 *A;
  double enf = -11.2814 + 0.02646 *A;
  
  double rc = 1.198 + 0.697/pow(A3,2) + 12.995/pow(A3,5);
  double vc = 1.73/rc * Z / A3;

  v  = vp1*(1 - vp2*(E-epf) + vp3*pow(E-epf,2) + vp4*pow(E-epf,3)) + vc * vp1 * (vp2 - 2*vp3*(E-epf) + 3*vp4*pow(E-epf,2));
  r0 = 1.3039 - 0.4054 / A3;
  a  = 0.6778 - 0.000148 * A;

  vi  = wp1 * pow(E-epf,2)/(pow(E-epf,2) + pow(wp2,2));
  ri0 = 1.3039 - 0.4054 / A3;
  ai  = 0.6778 - 0.000148 * A;

  vsi  = dp1 * pow(E-epf,2)/(pow(E-epf,2)+pow(dp3,2)) * exp(-dp2*(E-epf));
  rsi0 = 1.3424 - 0.01585 * A3;
  asi  = 0.5187 + 0.0005205 * A;

  vso  = vso1 * exp(-vso2 * (E-epf));
  rso0 = 1.1854 - 0.647/A3;
  aso  = 0.59;

  vsoi  = wso1 * pow(E-epf,2)/(pow(E-epf,2)+pow(wso2,2));
  rsoi0 = 1.1854 - 0.647/A3;
  asoi  = 0.59;

  rc0 = rc;

  return true;
}

bool VarnerPotential(int A, int Z, double E){
  // p + A(Z)
  // 16 < E < 65 or 65 MeV/u
  // 4 < A < 209
  // http://dx.doi.org/10.1016/0370-1573(91)90039-O
  
  //if( !(4 <= A &&  A <= 209 ) ) return false;
  //if( E > 65 || E < 16 ) return false;

  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  double Rc = 1.24 * A3 + 0.12;
  double EC = 1.73 * Z / Rc;
  double ETA = 1.0*(N-Z)/A;
  
  v  = 52.9 + 13.1 * (N-Z)/A - 0.299 * (E - EC);
  r0 = 1.25 - 0.225/A3;
  a  = 0.690;

  vi  = 7.8/(1 + exp((35. - E + EC)/16.) );
  ri0 = 1.33 - 0.42/A3;
  ai  = 0.69;

  vsi  = (10 + 18.*(N-Z)/A)/(1+exp((E-EC - 36.)/37.));
  rsi0 = 1.33 - 0.42/A3;
  asi  = 0.69;

  vso  = 5.9;
  rso0 = 1.34 - 1.2/A3;
  aso  = 0.63;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = Rc/A3;
  
  //PrintPotential();

  return true;
}

bool MenetPotential(int A, int Z, double E){
  // p + A(Z)
  // 30 < E < 60 or 60 MeV/u
  // 40 < A 
  // http://dx.doi.org/10.1016/0370-1573(91)90039-O
  
  //if( !(40 <= A ) ) return false;
  //if( E > 60 || E < 30 ) return false;

  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 49.9 - 0.22 * E + 26.4 * (N-Z) / A + 0.4 * Z / A3;
  r0 = 1.16;
  a  = 0.75;

  vi  = 1.2 + 0.09 * E;
  ri0 = 1.37;
  ai  = 0.74 - 0.008 * E + 1.*(N-Z)/A;

  vsi  = 4.2 - 0.05 * E + 15.5 * (N-Z)/A;
  rsi0 = 1.37;
  asi  = 0.74 - 0.008 * E + 1.*(N-Z)/A;

  vso  = 6.04;
  rso0 = 1.064;
  aso  = 0.78;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.25;
  
  //PrintPotential();

  return true;
}

bool BecchettiPotential(int A, int Z, double E){
  // p + A(Z)
  // E < 50 or 60 MeV/u
  // 40 < A 
  // http://dx.doi.org/10.1103/PhysRev.182.1190
  
  //if( !(40 <= A ) ) return false;
  //if( E > 60 || E < 30 ) return false;

  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 54 - 0.32 * E + 24. * (N-Z) / A + 0.4 * Z / A3;
  r0 = 1.17;
  a  = 0.75;

  vi  = 0.22 * E - 2.7 < 0 ? 0 : 0.22 * E - 2.7;
  ri0 = 1.32;
  ai  = 0.51 + 0.7 * (N-Z)/A;

  vsi  = 4.2 - 0.05 * E + 15.5 * (N-Z)/A;
  rsi0 = 11.8 - 0.258 * E + 12. * (N-Z)/A;
  if( rsi0 < 0 ) {
    rsi0 = 0;
  }
  asi  = 0.51 + 0.7 * (N-Z)/A;

  vso  = 6.2;
  rso0 = 1.1;
  aso  = 0.75;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.3;
  
  //PrintPotential();

  return true;
}

bool PereyPotential(int A, int Z, double E){
  // p + A(Z)
  // E < 20 or 20 MeV/u
  // 30 < A < 100 
  // http://dx.doi.org/10.1103/PhysRev.182.1190
  
  //if( !(40 <= A && A <= 100 ) ) return false;
  //if( E > 20 ) return false;

  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 53.3 - 0.55 * E + 27. * (N-Z) / A + 0.4 * Z / A3;
  r0 = 1.25;
  a  = 0.65;

  vi  = 0.;
  ri0 = 0.;
  ai  = 0.;

  vsi  = 13.5;
  rsi0 =  1.25;
  asi  = 0.47;

  vso  = 7.5;
  rso0 = 1.25;
  aso  = 0.47;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.25;
  
  //PrintPotential();

  return true;
}


bool CallPotential(string potName, int A, int Z, double E){
  bool okFlag = false;
  
  //printf(" Potenital : %s \n", potName.c_str());

  if( potName == "A") okFlag = AnCaiPotential(A, Z, E);
  if( potName == "H") okFlag = HSSPotential(A, Z, E);
  if( potName == "B") okFlag = BojowaldPotential(A, Z, E);
  if( potName == "D") okFlag = DaehnickPotential(A, Z, E);
  if( potName == "L") okFlag = LohrPotential(A, Z, E);
  if( potName == "Q") okFlag = PereyPereyPotential(A, Z, E);
  
  if( potName == "K") okFlag = KoningPotential(A, Z, E);
  if( potName == "V") okFlag = VarnerPotential(A, Z, E);
  if( potName == "M") okFlag = MenetPotential(A, Z, E);
  if( potName == "G") okFlag = BecchettiPotential(A, Z, E);
  if( potName == "P") okFlag = PereyPotential(A, Z, E);
  
  return okFlag;
}
