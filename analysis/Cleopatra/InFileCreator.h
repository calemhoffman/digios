/***********************************************************************
 * 
 *  This is InFileCreator, To creator the in-file for Ptolemy
 *          only for (x,y), x or y = p, d, t, 3He
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

using namespace std;

vector<string> SplitStr(string tempLine, string splitter, int shift = 0){

  vector<string> output;

  size_t pos;
  do{
    pos = tempLine.find(splitter); // fine splitter
    if( pos == 0 ){ //check if it is splitter again
      tempLine = tempLine.substr(pos+1);
      continue;
    }

    string secStr;
    if( pos == string::npos ){
      secStr = tempLine;
    }else{
      secStr = tempLine.substr(0, pos+shift);
      tempLine = tempLine.substr(pos+shift);
    }

    //check if secStr is begin with space
    if( secStr.substr(0, 1) == " "){
      secStr = secStr.substr(1);
    }

    output.push_back(secStr);
    //printf(" |%s---\n", secStr.c_str());
    
  }while(pos != string::npos );

  return output;
}

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
    
    //for( int i = 0 ; i < str0.size() ; i++){
    //  printf(" str0[%d] %s \n", i,  str0[i].c_str());
    //}

    vector<string> str1 = SplitStr(str0[0], "(", 0);
    vector<string> str2 = SplitStr(str1[1], ")", 1);
    
    str2[0] = "(" + str2[0];
    
    //TODO use mass table for d, p, t, 3He
    int lenStr20 = str2[0].length();
    size_t posTemp1 = str2[0].find(",");
    string ma = str2[0].substr(1, posTemp1-1);
    size_t posTemp2 = str2[0].find(")");
    string mb = str2[0].substr(posTemp1+1, posTemp2-posTemp1-1);
    //printf(" ma : |%s| , mb : |%s| \n", ma.c_str(), mb.c_str());
    Isotope isotopea(ma);
    Isotope isotopeb(mb);
    
    bool isReactionSupported = false;
    if( isotopea.A <= 3 && isotopea.Z <= 2 && isotopeb.A <=3 && isotopeb.Z <=2 ) isReactionSupported = true; 

    //if( ma == "p" || ma == "d" || ma == "t" || ma == "3He" || mb == "n"){
    //  if( mb == "p" || mb == "d" || mb == "t" || mb == "3He" ||  mb == "n" ) isReactionSupported = true;
    //}

    if( isReactionSupported == false ){
      printf("  ===> Ignored. Reaction type not supported. \n"); 
      continue;
    }
    
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
    
    Isotope isotopeA(str1[0]);
    Isotope isotopeB(str2[1]);
    
    //check reaction valid by balancing the A and Z number;
    //printf("A: %d + %d = %d + %d \n", isotopeA.A, isotopea.A, isotopeB.A, isotopeb.A);
    //printf("Z: %d + %d = %d + %d \n", isotopeA.Z, isotopea.Z, isotopeB.Z, isotopeb.Z);

    if( isotopeA.A + isotopea.A != isotopeB.A + isotopeb.A || isotopeA.Z + isotopea.Z != isotopeB.Z + isotopeb.Z ) {
      printf("    ====> ERROR! A-number or Z-number not balanced. \n");
      continue;
    }
    
    bool isTransferReaction = true;
    if( ma == mb ) isTransferReaction = false;

    if( isTransferReaction && potential.length() != 2 ){
      printf("     ===> ERROR! Potential input should be 2 charaters! skipped. \n");
      continue;
    }
    
    string node = orbital.substr(0,1);
    string jValue = orbital.substr(2);
    string lValue = orbital.substr(1,1);
    int spdf = GetLValue(lValue);
    //printf(" j = %s, l = %s = %d, parity = %s \n", jValue.c_str(), lValue.c_str(), spdf, parity.c_str());
    if( spdf == -1 ){
      printf(" ===> skipped. Not reconginzed L-label. (user input : %s) \n", lValue.c_str());
      continue;
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
    //string beamParticle = reactionType.substr(1,1);
    int factor = 1;
    if( unit == "MeV/u") factor = isotopea.A;
    double totalBeamEnergy = atof(reactionEnergy.substr(0, pos+1).c_str()) * factor;
    
    //printf("unit : %s , %f\n", unit.c_str(), totalBeamEnergy);
    //printf("  target nucleus : %s \n", isoA.c_str());
    //printf("        reaction : %s \n", reactionType.c_str());
    //printf("          remain : %s \n", isoB.c_str());
    //printf(" reaction energy : %s \n", reactionEnergy.c_str());
    //printf("       Potential : %s \n", potential.c_str());
    //printf("         orbital : %s \n", orbital.c_str());
    //printf("        Ex [MeV] : %s \n", Ex.c_str());
    
    double Qvalue = isotopea.Mass + isotopeA.Mass - isotopeb.Mass  - isotopeB.Mass;
    //printf("Q-Value = %f MeV\n", Qvalue);
    
    //printf("A: %d, Z: %d, mass: %f MeV/c2 \n", isotopeA.A, isotopeA.Z, isotopeA.Mass);
    //printf("A: %d, Z: %d, mass: %f MeV/c2 \n", isotopeB.A, isotopeB.Z, isotopeB.Mass);
    if( isotopeA.Mass == -404 || isotopeB.Mass == -404 ){
      printf("  ===> Error! mass does not found. \n");
      continue;
    }
    
    //write ptolmey infile
    numOfReaction ++ ;
    
    if( isTransferReaction ){
      fprintf(file_out, "$============================================ Ex=%s(%s)%s\n", Ex.c_str(), orbital.c_str(), potential.c_str());
      fprintf(file_out, "reset\n");
      fprintf(file_out, "REACTION: %s%s%s(%s%s %s) ELAB=%7.3f\n", 
               isoA.c_str(), reactionType.c_str(), isoB.c_str(), spin.c_str(), parity.c_str(), Ex.c_str(),  totalBeamEnergy);
      if( isotopea.A <= 2 && isotopea.Z <= 1){
        fprintf(file_out, "PARAMETERSET dpsb r0target \n");
        fprintf(file_out, "$lstep=1 lmin=0 lmax=30 maxlextrap=0 asymptopia=50 \n");
        fprintf(file_out, "\n");
        fprintf(file_out, "PROJECTILE \n");
        fprintf(file_out, "wavefunction av18 \n");
        fprintf(file_out, "r0=1 a=0.5 l=0 rc0=1.2\n");
      }
      if( isotopea.A == 3 ){
        fprintf(file_out, "PARAMETERSET alpha3 r0target \n");
        fprintf(file_out, "$lstep=1 lmin=0 lmax=30 maxlextrap=0 asymptopia=50 \n");
        fprintf(file_out, "\n");
        fprintf(file_out, "PROJECTILE \n");
        fprintf(file_out, "wavefunction phiffer \n");
        fprintf(file_out, "nodes=0 l=0 jp=1/2 spfacp=1.31 v=179.94 r=0.54 a=0.68 param1=0.64 param2=1.13 rc=2.0\n");
      } 
      fprintf(file_out, ";\n");
      fprintf(file_out, "TARGET\n");
      fprintf(file_out, "JBIGA=%s\n", gsSpinA.c_str());
      fprintf(file_out, "nodes=%s l=%d jp=%s $node is n-1 \n", node.c_str(), spdf, jValue.c_str());
      fprintf(file_out, "r0=1.25 a=.65 \n");
      fprintf(file_out, "vso=6 rso0=1.10 aso=.65 \n");
      fprintf(file_out, "rc0=1.3 \n");
      fprintf(file_out, ";\n");
      
      string pot1Name = potential.substr(0,1);
      string pot1Ref = potentialRef(pot1Name);
      fprintf(file_out, "INCOMING $%s\n", pot1Ref.c_str());
      CallPotential(pot1Name, isotopeA.A, isotopeA.Z, totalBeamEnergy, isotopea.Z);
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
      CallPotential(pot2Name, isotopeB.A, isotopeB.Z, eBeam, isotopeb.Z); 
      fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
      fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
      fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
      fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
      fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
      fprintf(file_out, ";\n");
    }
    
    if( isTransferReaction == false ){
      if ( atof(Ex.c_str()) == 0.0 ) {
	fprintf(file_out, "$============================================ ELab=%5.2f(%s+%s)%s\n", 
		totalBeamEnergy, ma.c_str(), isoA.c_str(), potential.c_str());
	fprintf(file_out, "reset\n");
	fprintf(file_out, "CHANNEL %s + %s\n", ma.c_str(), isoA.c_str());
	fprintf(file_out, "ELAB = %f\n", totalBeamEnergy);
	fprintf(file_out, "JBIGA=%s\n", gsSpinA.c_str());
	string pot1Name = potential.substr(0,1);
	string pot1Ref = potentialRef(pot1Name);
	fprintf(file_out, "$%s\n", pot1Ref.c_str());
	CallPotential(pot1Name, isotopeA.A, isotopeA.Z, totalBeamEnergy, isotopea.Z);
	fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
	fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
	fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
	fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
	fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
	fprintf(file_out, "ELASTIC SCATTERING\n");
	fprintf(file_out, ";\n");
      }else{
	fprintf(file_out, "$============================================ Ex=%s(%s+%s|%s%s)%s,ELab=%5.2f\n", 
		Ex.c_str(), ma.c_str(), isoA.c_str(), spin.c_str(), parity.c_str(), potential.c_str(),totalBeamEnergy);
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
	CallPotential(pot1Name, isotopeA.A, isotopeA.Z, totalBeamEnergy, isotopea.Z);
	fprintf(file_out, "INCOMING\n");
	fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
	fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
	fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f  rc0 = %7.3f\n", vsi, rsi0, asi, rc0);
        //fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
	//fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
	fprintf(file_out, ";\n");
	
	fprintf(file_out, "OUTGOING\n");
	fprintf(file_out, "$%s\n", pot1Ref.c_str());
	CallPotential(pot1Name, isotopeA.A, isotopeA.Z, totalBeamEnergy - atof(Ex.c_str()), isotopea.Z);
	fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
	fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
	fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f  rc0 = %7.3f\n", vsi, rsi0, asi, rc0);
	//fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
	//fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
        //fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
	fprintf(file_out, ";\n");
      }
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
