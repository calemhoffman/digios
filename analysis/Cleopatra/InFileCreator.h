/***********************************************************************
 * 
 *  This is InFileCreator, To creator the in-file for Ptolemy
 *          only for (x,y), x or y = n, p, d, t, 3He
 * 
 *  It read a simple infile.in from reaction_setting file
 * 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ InFileCreator.C -o InFileCreator `root-config --cflags --glibs`
 *
 *------------------------------------------------------
 *  The reaction_setting file is simple like:
 *
 *  206Hg(d,p)207Hg(1s1/2 0.000) 10MeV/u AK
 *
 *  the first is similar to usual reaction setting, the word AK is a
 *     short name for Optical Potential, user can put as many line as
 *     they like, Cleopatra can create the suitable infile.in for Ptolomy
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <fstream>
#include <stdlib.h>     /* atof */
#include <cmath>
#include <vector>
#include "../Cleopatra/Isotope.h" // for geting Z
#include "potentials.h"
#include "../Armory/AnalysisLibrary.h"

using namespace std;

int GetLValue(string spdf){
  
  if( spdf == "s" ) return 0;
  if( spdf == "p" ) return 1;
  if( spdf == "d" ) return 2;
  if( spdf == "f" ) return 3;
  if( spdf == "g" ) return 4;
  if( spdf == "h" ) return 5;
  if( spdf == "i" ) return 6;
  if( spdf == "j" ) return 7;  
  return -1;
}


int InFileCreator(string readFile, string infile, double angMin, double angMax, double angStep) {
   
  //================= read infile. extract the reactions, write pptolemy infile for each reaction
  ifstream file_in;
  file_in.open(readFile.c_str(), ios::in);

  if( !file_in ){
    printf(" cannot read file. \n");
    return 0 ; 
  }
   
  printf("Save to infile : %s \n", infile.c_str()); 
  FILE * file_out;
  file_out = fopen (infile.c_str(), "w+");

  printf("Angle setting (%5.2f, %5.2f) deg | Step : %5.2f deg\n", angMin, angMax, angStep);
  printf("---------------------------\n");

  //extract information
  int numOfReaction = 0;
  while( file_in.good() ) {
    string tempLine;
    getline(file_in, tempLine );

    if( tempLine.substr(0, 1) == "#" ) continue;
    if( tempLine.size() < 5 ) continue;

    //split line using space
    vector<string> str0 = SplitStr(tempLine, " ");
    if ( str0.size() == 0 ) continue;

    printf("  %s\n", tempLine.c_str());
    
    ///for( int i = 0 ; i < str0.size() ; i++){
    ///  printf(" str0[%d] %s \n", i,  str0[i].c_str());
    ///}

    vector<string> str1 = SplitStr(str0[0], "(", 0);
    vector<string> str2 = SplitStr(str1[1], ")", 1);
    
    str2[0] = "(" + str2[0];
    
    int lenStr20 = str2[0].length();
    size_t posTemp1 = str2[0].find(",");
    string mass_a = str2[0].substr(1, posTemp1-1);
    size_t posTemp2 = str2[0].find(")");
    string mass_b = str2[0].substr(posTemp1+1, posTemp2-posTemp1-1);
    ///printf(" mass_a : |%s| , mass_b : |%s| \n", mass_a.c_str(), mass_b.c_str());
    Isotope iso_a(mass_a);
    Isotope iso_b(mass_b);
  
    // Check is the Reaction supported
    bool isReactionSupported = false;
    bool isTransferReaction = true;
    
    if( iso_a.A <= 4 && iso_a.Z <= 2 && iso_b.A <=4 && iso_b.Z <=2 ) isReactionSupported = true;     
    
    ///======= elastics-ish scattering
    if( iso_a.Mass == iso_b.Mass ) isTransferReaction = false;
    
    ///======= p/n-exchange is not supported
    if( iso_a.A == iso_b.A && iso_a.Z != iso_b.Z ) isReactionSupported = false;
    
    ///======= 3-nucleons transfer is not supported. e.g. (n,a), (p,a), (a,n), (a,p)
    int numNucleonsTransfer = iso_a.A - iso_b.A;
    if( abs(numNucleonsTransfer) >= 3 ) isReactionSupported = false;
    
    if( isReactionSupported == false ){
      printf("  ===> Ignored. Reaction type not supported. \n"); 
      continue;
    }
    
    // Continues to decode the input string
    string gsSpinA = str0[1];
    string orbital = str0[2];

    string spinParity = str0[3];
    int lenSpinParity = spinParity.length();
    string spin = spinParity.substr(0, lenSpinParity-1);
    string parity = spinParity.substr(lenSpinParity-1);

    string Ex = str0[4];
    string reactionEnergy = str0[5];
    string potential = str0[6];
    
    string isoA = str1[0];
    string isoB = str2[1];
    string reactionType = str2[0];
    
    Isotope iso_A(str1[0]);
    Isotope iso_B(str2[1]);
    
    /// check is iso_A or iso_B exist in the mass table
    if( iso_A.Mass == -404 || iso_B.Mass == -404 ){
      printf("  ===> Error! mass does not found. \n");
      continue;
    }
    
    /// check reaction valid by balancing the A and Z number;
    if( iso_A.A + iso_a.A != iso_B.A + iso_b.A || iso_A.Z + iso_a.Z != iso_B.Z + iso_b.Z ) {
      printf("====> ERROR! A-number or Z-number not balanced. \n");
      Isotope isotopeK(iso_A.A + iso_a.A - iso_b.A, iso_A.Z + iso_a.Z - iso_b.Z);
      printf("      try : %s(%s,%s)%s ??\n", iso_A.Name.c_str(), iso_a.Name.c_str(), iso_b.Name.c_str(), isotopeK.Name.c_str());
      continue;
    }

    if( isTransferReaction && potential.length() != 2 ){
      printf("====> ERROR! Potential input should be 2 charaters! skipped. \n");
      continue;
    }
    
    string node ;
    string jValue ;
    string lValue ;
    int spdf = -1;
    
    if( isTransferReaction ) {
      ///printf("------------ %d nucleon(s) transfer \n", abs(iso_a.A - iso_b.A));
      node = orbital.substr(0,1);
      
      // single nucleon transfer
      if( abs(iso_a.A - iso_b.A) == 1 ){
        lValue = orbital.substr(1,1);
        jValue = orbital.substr(2);
        ///printf(" l : %s, j : %s \n", lValue.c_str(), jValue.c_str());
        spdf = GetLValue(lValue);
      }
      
      // two-nucleons transfer
      if( abs(iso_a.A - iso_b.A) == 2 ){
        size_t posEq = orbital.find('=');
        lValue = orbital.substr(posEq+1,1);
        spdf=atoi(lValue.c_str());
      }
      
      if( abs(iso_a.A - iso_b.A) == 0 ){
        printf(" ===? skipped. p-n exchange reaction does not support. \n");
      }
      
      if( spdf == -1 ){
        printf(" ===> skipped. Not reconginzed orbital-label. (user input : l=%s | %s) \n", lValue.c_str(), orbital.c_str());
        continue;
      }
    }
    
    //get Beam energy, distingusih MeV or MeV/u
    int pos = reactionEnergy.length() - 1;
    for( int i = pos; i >= 0 ; i--){
      if( isdigit(reactionEnergy[i]) ) {
        pos = i; 
        break;
      }
    }
    string unit = reactionEnergy.substr(pos+1);
    int factor = 1;
    if( unit == "MeV/u") factor = iso_a.A;
    double totalBeamEnergy = atof(reactionEnergy.substr(0, pos+1).c_str()) * factor;
    
    ///printf("unit : |%s| , %f\n", unit.c_str(), totalBeamEnergy);
    ///printf("  target nucleus : %s \n", isoA.c_str());
    ///printf("        reaction : %s \n", reactionType.c_str());
    ///printf("          remain : %s \n", isoB.c_str());
    ///printf(" reaction energy : %s \n", reactionEnergy.c_str());
    ///printf("       Potential : %s \n", potential.c_str());
    ///printf("         orbital : %s \n", orbital.c_str());
    ///printf("        Ex [MeV] : %s \n", Ex.c_str());
    
    double Qvalue = iso_a.Mass + iso_A.Mass - iso_b.Mass  - iso_B.Mass;
    ///printf("Q-Value = %f MeV\n", Qvalue);
    
    //##########################################################
    //============ write ptolmey infile
    numOfReaction ++ ;
    
    //================ elastic-ish transfer
    if( isTransferReaction == false ){
      if ( atof(Ex.c_str()) == 0.0 ) {
        fprintf(file_out, "$============================================ ELab=%5.2f(%s+%s)%s\n", 
          totalBeamEnergy, mass_a.c_str(), isoA.c_str(), potential.c_str());
        fprintf(file_out, "reset\n");
        fprintf(file_out, "CHANNEL %s + %s\n", mass_a.c_str(), isoA.c_str());
        fprintf(file_out, "ELAB = %f\n", totalBeamEnergy);
        fprintf(file_out, "JBIGA=%s\n", gsSpinA.c_str());
        string pot1Name = potential.substr(0,1);
        string pot1Ref = potentialRef(pot1Name);
        fprintf(file_out, "$%s\n", pot1Ref.c_str());
        CallPotential(pot1Name, iso_A.A, iso_A.Z, totalBeamEnergy, iso_a.Z);
        fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
        fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
        fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
        fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
        fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
        fprintf(file_out, "ELASTIC SCATTERING\n");
        fprintf(file_out, ";\n");
      }else{
        fprintf(file_out, "$============================================ Ex=%s(%s+%s|%s%s)%s,ELab=%5.2f\n", 
          Ex.c_str(), mass_a.c_str(), isoA.c_str(), spin.c_str(), parity.c_str(), potential.c_str(),totalBeamEnergy);
        fprintf(file_out, "reset\n");
        fprintf(file_out, "REACTION: %s%s%s(%s%s %s) ELAB=%7.3f\n", 
                     isoA.c_str(), reactionType.c_str(), isoB.c_str(), spin.c_str(), parity.c_str(), Ex.c_str(),  totalBeamEnergy);
        fprintf(file_out, "PARAMETERSET ineloca2 r0target\n");
        fprintf(file_out, "JBIGA=%s\n", gsSpinA.c_str());
        if( str0.size() >= 8 ){
          fprintf(file_out, "BETA=%s\n", str0[7].c_str());  //deformation length
        }
        string pot1Name = potential.substr(0,1);
        string pot1Ref = potentialRef(pot1Name);
        fprintf(file_out, "$%s\n", pot1Ref.c_str());
        CallPotential(pot1Name, iso_A.A, iso_A.Z, totalBeamEnergy, iso_a.Z);
        fprintf(file_out, "INCOMING\n");
        fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
        fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
        fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f  rc0 = %7.3f\n", vsi, rsi0, asi, rc0);
        ///fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
        ///fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
        fprintf(file_out, ";\n");
        
        fprintf(file_out, "OUTGOING\n");
        fprintf(file_out, "$%s\n", pot1Ref.c_str());
        CallPotential(pot1Name, iso_A.A, iso_A.Z, totalBeamEnergy - atof(Ex.c_str()), iso_a.Z);
        fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
        fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
        fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f  rc0 = %7.3f\n", vsi, rsi0, asi, rc0);
        ///fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
        ///fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
        ///fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
        fprintf(file_out, ";\n");
      }
    }
    
    //================ Transfer reaction
    if( isTransferReaction ){
      fprintf(file_out, "$============================================ Ex=%s(%s)%s\n", Ex.c_str(), orbital.c_str(), potential.c_str());
      fprintf(file_out, "reset\n");
      fprintf(file_out, "REACTION: %s%s%s(%s%s %s) ELAB=%7.3f\n", 
               isoA.c_str(), reactionType.c_str(), isoB.c_str(), spin.c_str(), parity.c_str(), Ex.c_str(),  totalBeamEnergy);
      
      //-------- Projectile (the light particle)
      if( abs(numNucleonsTransfer) == 1 ){         
        if( iso_a.A <= 2 && iso_a.Z <= 1 && iso_b.A <=2 && iso_b.Z <= 1){ // incoming d or p
          fprintf(file_out, "PARAMETERSET dpsb r0target \n");
          fprintf(file_out, "lstep=1 lmin=0 lmax=30 maxlextrap=0 asymptopia=50 \n");
          fprintf(file_out, "\n");
          fprintf(file_out, "PROJECTILE \n");
          fprintf(file_out, "wavefunction av18 \n");
          fprintf(file_out, "r0=1 a=0.5 l=0 rc0=1.2\n");
        }
        
        if( (3 <= iso_a.A && iso_a.A <= 4) || (3 <= iso_b.A && iso_b.A <= 4) ){ 
          fprintf(file_out, "PARAMETERSET alpha3 r0target \n");
          fprintf(file_out, "lstep=1 lmin=0 lmax=30 maxlextrap=0 asymptopia=50 \n");
          fprintf(file_out, "\n");
          fprintf(file_out, "PROJECTILE \n");
          fprintf(file_out, "wavefunction phiffer \n");
          
          if( iso_a.Z + iso_b.Z == 2){ // (t,d) or (d,t)
            fprintf(file_out, "nodes=0 l=0 jp=1/2 spfacp=1.30 v=172.88 r=0.56 a=0.69 param1=0.64 param2=1.15 rc=2.0\n");
          }

          if( iso_a.Z + iso_b.Z == 3){ // (3He,d) or (d, 3He)
            fprintf(file_out, "nodes=0 l=0 jp=1/2 spfacp=1.31 v=179.94 r=0.54 a=0.68 param1=0.64 param2=1.13 rc=2.0\n");
          }
          
          if( iso_b.A == 4 ){
            fprintf(file_out, "nodes=0 l=0 jp=1/2 spfacp=1.61 v=202.21 r=.93 a=.66 param1=.81 param2=.87 rc=2.0 $ rc=2 is a quirk\n");
          } 
        }
      }else if( abs(numNucleonsTransfer) == 2 ){ // 2 nucleons transfer
        fprintf(file_out, "PARAMETERSET alpha3 r0target\n");
        fprintf(file_out, "lstep=1 lmin=0 lmax=30 maxlextrap=0 ASYMPTOPIA=40\n");
        fprintf(file_out, "\n");
        fprintf(file_out, "PROJECTILE\n");
        fprintf(file_out, "wavefunction phiffer\n");
        fprintf(file_out, "L = 0  NODES=0 R0 = 1.25  A = .65     RC0 = 1.25\n");
      } 
      fprintf(file_out, ";\n");
      
      //===== TARGET
      fprintf(file_out, "TARGET\n");
      ///check Ex is above particle threshold
      double nThreshold = iso_B.CalSp(0,1);
      double pThreshold = iso_B.CalSp(1,0);
      bool isAboveThreshold = false;
      double ExEnergy = atof(Ex.c_str());
      if( ExEnergy > nThreshold || ExEnergy > pThreshold ) {
        isAboveThreshold = true;
        printf("         Ex = %.3f MeV is above thresholds; Sp = %.3f MeV, Sn = %.3f MeV\n", ExEnergy, pThreshold, nThreshold);
      }
      
      if( abs(iso_a.A-iso_b.A) == 1 ){
        fprintf(file_out, "JBIGA=%s\n", gsSpinA.c_str());
        if( isAboveThreshold ) {
          fprintf(file_out, "nodes=%s l=%d jp=%s E=-.2 $node is n-1, set binding 200 keV \n", node.c_str(), spdf, jValue.c_str());
        }else{
          fprintf(file_out, "nodes=%s l=%d jp=%s $node is n-1 \n", node.c_str(), spdf, jValue.c_str());
        }
        fprintf(file_out, "r0=1.25 a=.65 \n");
        fprintf(file_out, "vso=6 rso0=1.10 aso=.65 \n");
        fprintf(file_out, "rc0=1.3 \n");
      }
      
      if( abs(iso_a.A-iso_b.A) == 2 ){
        fprintf(file_out, "JBIGA=%s\n", gsSpinA.c_str());
        if( isAboveThreshold ){
          fprintf(file_out, "nodes=%s L=%d E=-.2 $node is n-1, binding by 200 keV \n", node.c_str(), spdf);
        }else{
          fprintf(file_out, "nodes=%s L=%d  $node is n-1 \n", node.c_str(), spdf);
        }
      }
      
      fprintf(file_out, ";\n");
      
      //===== POTENTIAL
      string pot1Name = potential.substr(0,1);
      string pot1Ref = potentialRef(pot1Name);
      fprintf(file_out, "INCOMING $%s\n", pot1Ref.c_str());
      CallPotential(pot1Name, iso_A.A, iso_A.Z, totalBeamEnergy, iso_a.Z);
      fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
      fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
      fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
      fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
      fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
      fprintf(file_out, ";\n");
      
      string pot2Name = potential.substr(1,1);
      string pot2Ref = potentialRef(pot2Name);
      fprintf(file_out, "OUTGOING $%s\n", pot2Ref.c_str());
      //printf(" total Beam Energy : %f | Qvalue : %f | Ex : %f \n", totalBeamEnergy, Qvalue, atof(Ex.c_str()));
      double eBeam = totalBeamEnergy + Qvalue - atof(Ex.c_str());
      CallPotential(pot2Name, iso_B.A, iso_B.Z, eBeam, iso_b.Z); 
      fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
      fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
      fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
      fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
      fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
      fprintf(file_out, ";\n");
    }
    

    
    
    fprintf(file_out, "anglemin=%f anglemax=%f anglestep=%f\n", angMin, angMax, angStep);
    fprintf(file_out, ";\n");
    
  }
  
  printf("================= end of input. Number of Reaction : %d \n", numOfReaction);
  
  fprintf(file_out, "end $================================== end of input\n");
  file_in.close();
  fclose(file_out);
  
  return 1;

} 
