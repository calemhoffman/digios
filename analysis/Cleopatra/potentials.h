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

/// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
/// 1 1 1 1 0 0 1 1 0 0 1 1 1 0 0 1 1 0 0 0 0 1 0 1 1 1
string potentialRef(string name){
  
  //======== Deuteron 
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
  
  if( name == "Z" ){
    return "Zhang, Pang, Lou (2016) 5 < E < 170  | A < 18 | https://doi.org/10.1103/PhysRevC.94.014619";
  }
   
  //======= Proton 
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

/// a b c d e f g h i j k l m n o p q r s t u v w x y z
/// 1 1 1 0 0 1 0 1 0 0 0 1 0 0 0 1 0 0 1 1 0 0 0 1 0 0
  //====== A = 3
  if( name == "x" ){
    return "XU, GUO, HAN, SHEN (2011) E < 250 | 20 < A < 209 | http://dx.doi.org/10.1007/s11433-011-4488-5";
  }
  if( name == "l" ){
    return "Liang, Li, Cai (2009) E < 270 | All masses | http://dx.doi.org/10.1088/0954-3899/36/8/085104";
  }
  if( name == "p" ){
    return "Pang et al., (2009) All E | All masses | Isospin dep. | http://dx.doi.org/10.1103/PhysRevC.79.024615";
  }
  if( name == "c" ){
    return "Li, Liang, Cai, (2007) E < 40 | All masses | 48 < A < 232 | Tritons   | http://dx.doi.org/10.1016/j.nuclphysa.2007.03.004";
  }
  if( name == "t" ){
    return "Trost et al., (1987) 10 < E < 220 | 10 < A < 208 | http://dx.doi.org/10.1016/0375-9474(87)90551-3";
  }
  if( name == "h" ){
    return "Hyakutake et al., (1980) 90 < E < 120 | About 58 < A < 92 | http://dx.doi.org/10.1016/0375-9474(80)90013-5";
  }
  if( name == "b" ){
    return "Becchetti and Greenlees, (1971) E < 40 | 40 < A | Iso. Dep.";
  }
  
  //======= alpha
  if( name == "s"){
    return "Su and Han, (2015) E < 398 | 20 < A < 209 | http://dx.doi/org/10.1142/S0218301315500925";
  }
  if( name == "a"){
    return "Avrigeanu et al., (2009) E ??? | A ??? | http://dx.doi/org/10.1016/j.adt.2009.02.001";
  }
  if( name == "f"){
    return "(FIXED) Bassani and Picard, (1969) 24 < E < 31 | A = 90 | https://doi.org/10.1016/0375-9474(69)90601-0";
  }
  
  //====== custom
  if( name == "Y"){
    return "Bardayan Parameters PRC 78 052801(R) (2008)";
  }
  if( name == "X"){
    return "Bardayan Parameters PRC 78 052801(R) (2008)";
  }
   
  return "";
}


bool CustomXPotential(int A, int Z, double E){

v = 54.19    ;
r0 = 1.25    ;
a = 0.65     ;
vi = 0.0     ;
ri0 = 1.25   ;
ai = 0.65    ;
vsi = 13.5   ;
rsi0 = 1.25  ;
asi = 0.47   ;
vso = 7.5    ;
rso0 = 1.25  ;
aso = 0.47   ;
vsoi = 0.0   ;
rsoi0 = 1.25 ;
asoi = 0.47 ;
rc0 = 1.25  ;

  return true;
}

bool CustomYPotential(int A, int Z, double E){

  v = 85.31;
  r0 = 1.15;
  a = 0.81;
  
  vi = 0.0;
  ri0 = 1.15;
  ai = 0.81;
  
  vsi = 16.0;
  rsi0 = 1.34;
  asi = 0.68;
  
  vso = 0.0;
  rso0 = 1.0;
  aso = 1.0;

  vsoi = 0.0;
  rsoi0 = 1.0;
  asoi = 1.0;
   
  rc0 = 1.15;

  return true;
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

  return true;
}

bool ZhangPangLouPotential(int A, int Z, double E, double Zproj){
  
  vso = 0;
  rso0 = 0;
  aso = 0;
  vsoi = 0;
  rsoi0 = 0;
  rc0 = 1.3;
    
  double A3 = pow(A, 1./3.);

  double RC = rc0 * A3;  
  double Ec = 6 * Zproj * Z * 1.44 / 5 / RC;
  
  if( A == 6 && Z == 3 ){
    r0  = 1.62 - 0.0122 * (E - Ec) / A3;
    ri0 = 2.83 - 0.0911 * (E - Ec) / A3;
    rsi0 = ri0;
    
    a   = 0.876;
    ai  = 0.27;
    asi = ai;
    
    v   = 47.9 + 2.37 * (E - Ec);
    vi  = 0;
    vsi = 11.3 + 3.44 * (E - Ec); 

  }else if( A == 7 && Z == 3 ){
    r0  = 1.45  + 0.097 * (E - Ec) / A3;
    ri0 = 2.12  + 0.022 * (E - Ec) / A3;
    rsi0 = ri0;
    
    a   = 0.844;
    ai  = 0.261;
    asi = ai;
    
    v   = 26.1 + 1.19 * (E - Ec);
    vi  = 0;
    vsi = 215.0 - 16.1 * (E - Ec);     
    
  }else{ 
    r0   = 1.11  - 0.167 / A3 + 0.00117 * (E - Ec) / A3;
    ri0  = 0.561 + 3.07  / A3 - 0.00449 * (E - Ec) / A3;
    rsi0 = ri0;
    
    a   = 0.776;
    ai  = 0.744;
    asi = ai;
    
    v   = 98.9 - 0.279 * (E - Ec);
    vi  = 11.5 / ( 1 + exp((18.1 - (E - Ec))/5.97));
    vsi = 7.56 / ( 1 + exp(((E - Ec) - 14.3)/4.55)); 
    
  }
    
  return true;
}

//======================== proton 
bool KoningPotential(int A, int Z, double E, double Zproj){
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
  double vn4 = vp4;
  
  double wp1 = 14.667 + 0.009629*A;
  double wn1 = 12.195 + 0.0167*A;
  
  double wp2 = 73.55 + 0.0795*A; // = wn2
  double wn2 = wp2;
  
  double dp1 = 16 + 16.*(N-Z)/A;
  double dn1 = 16 - 16.*(N-Z)/A;
  
  double dp2 = 0.018 + 0.003802/(1 + exp((A-156.)/8)); // = dn2
  double dn2 = dp2;
  
  double dp3 = 11.5 ; // = dn3
  double dn3 = dp3;
 
  double vso1 = 5.922 + 0.003 * A;
  double vso2 = 0.004;
  
  double wso1 = -3.1;
  double wso2 = 160;
  
  double epf = -8.4075 + 0.01378 *A;
  double enf = -11.2814 + 0.02646 *A;
  
  double rc = 1.198 + 0.697/pow(A3,2) + 12.995/pow(A3,5);
  double vc = 1.73/rc * Z / A3;

  v  = vp1*(1 - vp2*(E-epf) + vp3*pow(E-epf,2) - vp4*pow(E-epf,3)) + vc * vp1 * (vp2 - 2*vp3*(E-epf) + 3*vp4*pow(E-epf,2));
  //neutron
  if( Zproj == 0 ) v  = vn1*(1 - vn2*(E-enf) + vn3*pow(E-enf,2) - vn4*pow(E-enf,3));


  r0 = 1.3039 - 0.4054 / A3;
  a  = 0.6778 - 0.000148 * A;

  vi  = wp1 * pow(E-epf,2)/(pow(E-epf,2) + pow(wp2,2));
  if( Zproj == 0 ) vi  = wn1 * pow(E-enf,2)/(pow(E-enf,2) + pow(wn2,2));
  
  ri0 = 1.3039 - 0.4054 / A3;
  ai  = 0.6778 - 0.000148 * A;

  vsi  = dp1 * pow(E-epf,2)/(pow(E-epf,2)+pow(dp3,2)) * exp(-dp2*(E-epf));
  if( Zproj == 0 )   vsi  = dn1 * pow(E-enf,2)/(pow(E-enf,2)+pow(dn3,2)) * exp(-dn2*(E-enf));

  rsi0 = 1.3424 - 0.01585 * A3;
  asi  = 0.5187 + 0.0005205 * A;
  if( Zproj == 0 ) asi = 0.5446 - 0.0001656 * A;

  vso  = vso1 * exp(-vso2 * (E-epf));
  if( Zproj == 0 ) vso = vso1 * exp(-vso2 * (E-enf));

  rso0 = 1.1854 - 0.647/A3;
  aso  = 0.59;

  vsoi  = wso1 * pow(E-epf,2)/(pow(E-epf,2)+pow(wso2,2));
  if ( Zproj == 0 )   vsoi  = wso1 * pow(E-enf,2)/(pow(E-enf,2)+pow(wso2,2));

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

  vsi = 11.8 - 0.258 * E + 12. * (N-Z)/A;
  if( vsi < 0 ) {
    vsi = 0;
  }
  rsi0 = 1.320;
  asi  = 0.51 + 0.7 * (N-Z)/A;

  vso  = 6.2;
  rso0 = 1.1;
  aso  = 0.75;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.3;
  
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

  return true;
}

bool XuPotential(int A, int Z, double E){

  int N   = A-Z;
  double A3 = pow(A, 1./3.);

  double vsiTest = 33.26647 - 0.16975 * E - 12.0 * (N-Z)/A;
  double viTest = -2 + 0.10645*E - 0.00016156 * pow(E,2);
  
  v  = 136.34988 - 0.20315 * E - 0.00030147 * pow(E,2) - 24.0 * (N-Z) / A + 0.4 * Z / A3;
  r0 = 1.14963;
  a  = 0.78836;

  vi = 0.0; if( viTest > 0 ) vi = viTest;
  ri0 = 1.61807;
  ai  = 0.66485;

  vsi = 0.0; if( vsiTest > 0 ) vsi = vsiTest;
  rsi0 =  1.20655;
  asi  = 0.73593;

  vso  = 3.0;
  rso0 = 1.26864;
  aso  = 0.89999;

  vsoi  = 0;
  rsoi0 = 0;
  asoi  = 0;

  rc0 = 1.25;
  
  return true;
}

bool LiangPotential(int A, int Z, double E){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 118.36 - 0.2071 * E + 0.000063961 * pow(E,2) + 26.001 * (N-Z) / A + 0.5668 * Z / A3;
  r0 = 1.1657 + 0.0401 / A3;
  a  = 0.6641 + 0.0305 * A3;

  vi  = -6.8871 + 0.3115 * E - 0.00068096 * pow(E,2);
  ri0 = 1.4022 + 0.0418 / A3;
  ai  = 0.7732 + 0.0219 * A3;

  vsi  = 20.119 - 0.1626 * E - 5.4067 * (N-Z) / A + 1.2087 * A3;
  rsi0 = 1.1802 + 0.0587 / A3;
  asi  = 0.6292 + 0.0657 * A3;

  vso  = 2.0491 + 0.0099804 * A3;
  rso0 = 0.7211 + 0.0586 / A3;
  aso  = 0.7643 + 0.0535 * A3;

  vsoi  = -1.1591;
  rsoi0 = 0.7211 + 0.0586 * A3;
  asoi  = 0.7643 + 0.0535 * A3;

  rc0 = 1.289;
  
  return true;
}

bool PangPotential(int A, int Z, double E, int Zproj){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  double rc = 1.24 * A3 + 0.12;
  double EC = 1.728 * Z * Zproj / rc;
  double ETA = 1.0 * (N-Z) / A;
  double vsiAsym = 35.0 - 34.2 * ETA;
  if( Zproj == 2.0 ) vsiAsym = 35 + 34.2* ETA;
   
  v  = 118.3 - 0.13 * (E - EC);
  r0 = 1.3  - 0.48 / A3;
  a  = 0.82;

  vi  = 38.5/(1.0 + exp( (156.1 - E + EC)/52.4 ));
  ri0 = 1.31 - 0.13 / A3;
  ai  = 0.84;

  vsi  = vsiAsym / ( 1.0 + exp( (E - EC - 30.8) / 106.4 ));
  rsi0 = 1.31 - 0.13 / A3;
  asi  = 0.84;

  vso  = 0;
  if( E < 85 ) vso = 1.7 - 0.02 * E;
  rso0 = 0.64 + 1.18 / A3;
  aso  = 0.13;

  vsoi  = 0.;
  rsoi0 = 0.;
  asoi  = 0.;

  rc0 = rc/ A3;
  
  return true;
}

bool LiLiangCaiPotential(int A, int Z, double E){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 137.6 - 0.1456 * E + 0.0436 * pow(E,2) + 4.3751 * (N-Z) / A + 1.0474 * Z / A3;
  r0 = 1.1201 - 0.1504 / A3;
  a  = 0.6833 + 0.0191 * A3;

  vi  = 7.383 + 0.5025 * E - 0.0097 * pow(E,2);
  ri0 = 1.3202 - 0.1776 / A3;
  ai  = 1.119  + 0.01913 * A3;

  vsi  = 37.06 - 0.6451 * E - 47.19 * (N-Z) / A;
  rsi0 = 1.251 - 0.4622 / A3;
  asi  = 0.8114 + 0.01159 * A3;

  vso  = 1.9029;
  rso0 = 0.46991 + 0.1294 / A3;
  aso  = 0.3545 - 0.0522 * A3;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.422;
  
  return true;
}

bool TrostPotential(int A, int Z, double E){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  double JR = 272.33 * (1.0 + 0.002029 * A) * ( 1.0 - 0.001453 * E ) * (1.0 + 3.4931 * pow(A3,-2)) * ( 1.0+(-0.825165+exp(0.92059-0.079154*A))*exp(-0.065066*E) );
  double AP = 3 ;

  r0 = 1.150;
  double RR = A3 * r0;  
  a  = 0.64*(1+(0.0004*A))*(1+0.25*(1-exp(-0.2*A))*(1-exp(-0.06*E)));
  double pi = 3.141592653589793;
  v  = 3.0/(4 * pi) * JR * AP * A * pow(RR, -3.0) / (1 + pow((pi * a)/RR,2) );
  
  vi  = 0.0;
  ri0 = 0.0;
  ai  = 0.0;

  vsi  = 24.5*(1+(1-(0.011*A))*(-0.0018)*E)*(1-exp(-(1.0+(0.003*E))*0.1*A))*(1-exp(-0.1*E));
  rsi0 = 1.26*(1-(0.00055*E))*(1+exp(-0.9163-(0.005*A))*exp(-0.09*E));
  asi  = 0.8;

  vso  = 0.0;
  rso0 = 0.0;
  aso  = 0.0;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.4;
  
  return true;
}

bool HyakutakePotential(int A, int Z, double E){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 111.4 - 0.173 * E + 14.9 * (N-Z) / A + 1.1 * Z / A3;
  r0 = 1.21;
  a  = 0.76;

  vi  = 0.0;
  ri0 = 0.0;
  ai  = 0.0;

  vsi  = 24.8 - 0.028 * E;
  rsi0 = 1.17;
  asi  = 0.754 + 0.78 * (N-Z) / A;

  vso  = 0.0;
  rso0 = 0.0;
  aso  = 0.0;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.300;
  
  return true;
}

bool BecchettiA3Potential(int A, int Z, double E, int Zproj){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  v  = 165 - 0.17 * E - 6.4 * (N-Z) / A;
  if( Zproj == 2 ) v = 151.9 - 0.17 * E + 50. * (N-Z) / A;
  
  r0 = 1.20;
  a  = 0.72;

  vi  = 46.0 - 0.33 * E - 110. * (N-Z) / A;
  if( Zproj == 2 ) vi = 41.7 - 0.33 * E + 44.0 * (N-Z) / A;
  ri0 = 1.4;
  ai  = 0.84;
  if( Zproj == 2 ) ai = 0.88;

  vsi  = 0.0;
  rsi0 = 0.0;
  asi  = 0.0;

  vso  = 2.5;
  rso0 = 1.2;
  aso  = 0.72;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.300;
  
  return true;
}

//=============================== alpha

bool SuAndHanPotential(int A, int Z, double E){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  double vsiCOND = 27.5816 - 0.0797 * E + 48.0*(N-Z)/A;
  double  viCOND = -4.0174 + 0.1409 * E ;
  
  v  = 175.0881 - 0.6236 * E + 0.0006*E*E + 30.*(N-Z)/A - 0.236 * Z/A3;
  r0 = 1.3421;
  a  = 0.6578;

  vi  = viCOND; if( viCOND < 0 ) vi = 0.0;
  ri0 = 1.4259;
  ai  = 0.6578;

  vsi  = vsiCOND; if (vsiCOND < 0 ) vsi = 0.0;
  rsi0 = 1.2928;
  asi  = 0.6359;

  vso  = 0.0;
  rso0 = 1.2686;
  aso  = 0.85;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.350;
  
  return true;
}

bool AvrigeanuPotential(int A, int Z, double E){
  
  int N   = A-Z;
  double A3 = pow(A, 1./3.);
  
  double e3 = 23.6 + 0.181 * Z/A3;
  double e2 = (2.59 + 10.4/A)*Z/(2.66+1.36*A3);
  double e1 = -3.03 + 0.762 * A3 + 0.24 + e2;
  
  v  = 116.5 + 0.337 * Z /A3 + 0.453*E;
  if( E < e3 ) v = 168 + 0.733 * Z / A3 - 2.64 *E;
  r0 = 0.00;
  a  = 0.00;

  vi  = 2.73 - 2.88 * A3 + 1.11*E;
  ri0 = 0.00;
  ai  = 0.00;

  vsi  = 0.0;
  rsi0 = 0.0;
  asi  = 0.0;

  vso  = 0.0;
  rso0 = 0.0;
  aso  = 0.0;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.300;
  
  return true;
}

bool BassaniPicardPotential(int A, int Z, double E){
  
  v  = 207;
  r0 = 1.3;
  a  = 0.65;

  vi  = 28;
  ri0 = 1.3;
  ai  = 0.52;

  vsi  = 0.0;
  rsi0 = 0.0;
  asi  = 0.0;

  vso  = 0.0;
  rso0 = 0.0;
  aso  = 0.0;

  vsoi  = 0.0;
  rsoi0 = 0.0;
  asoi  = 0.0;

  rc0 = 1.400;
  
  return true;
}

bool CallPotential(string potName, int A, int Z, double E, int Zproj){
  bool okFlag = false;
  
  if( potName == "A") okFlag = AnCaiPotential(A, Z, E);
  if( potName == "H") okFlag = HSSPotential(A, Z, E);
  if( potName == "B") okFlag = BojowaldPotential(A, Z, E);
  if( potName == "D") okFlag = DaehnickPotential(A, Z, E);
  if( potName == "L") okFlag = LohrPotential(A, Z, E);
  if( potName == "Q") okFlag = PereyPereyPotential(A, Z, E);
  if( potName == "Z") okFlag = ZhangPangLouPotential(A, Z, E, Zproj);
  
  if( potName == "K") okFlag = KoningPotential(A, Z, E, Zproj);
  if( potName == "V") okFlag = VarnerPotential(A, Z, E);
  if( potName == "M") okFlag = MenetPotential(A, Z, E);
  if( potName == "G") okFlag = BecchettiPotential(A, Z, E);
  if( potName == "P") okFlag = PereyPotential(A, Z, E);
  
  if( potName == "x") okFlag = XuPotential(A, Z, E);
  if( potName == "l") okFlag = LiangPotential(A, Z, E);
  if( potName == "p") okFlag = PangPotential(A, Z, E, Zproj);
  if( potName == "c") okFlag = LiLiangCaiPotential(A, Z, E);
  if( potName == "t") okFlag = TrostPotential(A, Z, E);
  if( potName == "h") okFlag = HyakutakePotential(A, Z, E);
  if( potName == "b") okFlag = BecchettiA3Potential(A, Z, E, Zproj);
  
  if( potName == "s") okFlag = SuAndHanPotential(A, Z, E);
  if( potName == "a") okFlag = AvrigeanuPotential(A, Z, E);
  if( potName == "f") okFlag = BassaniPicardPotential(A, Z, E);

  if( potName == "X") okFlag = CustomXPotential(A, Z, E);  
  if( potName == "Y") okFlag = CustomYPotential(A, Z, E);
  
  //printf(" Potenital : %s | A : %d | Z : %d | E : %f\n", potName.c_str(), A, Z, E);
  //PrintPotential();
  
  return okFlag;
}
