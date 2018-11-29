/***********************************************************************
 * 
 *  This is InFileCreator, To creator the in-file for Ptolemy
 *          only for (d,p), (d,p), (d,d), or (p,p)
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
#include "Isotope.h" // for geting Z
#include "potentials.h"

using namespace std;

vector<string> SplitStr(string tempLine, string splitter, int shift = 0){

  vector<string> output;

  size_t pos;
  do{
    pos = tempLine.find(splitter, 1); // fine space
    if( pos == 1 ){ //check if it is space again
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
  if( spdf == "i" ) return 5;
  if( spdf == "h" ) return 6;
  
  return -1;
}

int InFileCreator(string read, string infile, double angMin, double angMax, double angStep) {
   
  //================= read infile. extract the reactions, write pptolemy infile for each reaction
  ifstream file_in;
  file_in.open(read.c_str(), ios::in);

  if( !file_in ){
    printf(" cannot read file. \n");
    return 0 ; 
  }
   
  printf("Save to infile : %s \n", infile.c_str()); 
  FILE * file_out;
  file_out = fopen (infile.c_str(), "w+");

  printf("Angle setting (%5.2f, %5.2f) deg | Step : %5.2f deg\n", angMin, angMax, angStep);

  //extract information
  int numOfReaction = 0;
  while( file_in.good() ) {
    string tempLine;
    getline(file_in, tempLine );

    if( tempLine.substr(0, 1) == "#" ) continue;
    if( tempLine.size() < 5 ) continue;
    
    printf("  %s\n", tempLine.c_str());

    //split line using space
    vector<string> str0 = SplitStr(tempLine, " ");
    
    vector<string> str1 = SplitStr(str0[0], "(");
    vector<string> str2 = SplitStr(str1[1], ")", 1);

    if( !(str2[0] == "(d,d)" || str2[0] =="(p,p)" || str2[0] == "(p,d)" || str2[0] == "(d,p)") ){
      printf("  ===> Ignored. Reaction type not supported. \n"); 
      continue;
    }
    
    string gsSpinA = str0[1];
    string orbital = str0[2];
    string parity = (str0[3] == "+1" ? "+" : "-");
    string Ex = str0[4];
    string reactionEnergy = str0[5];
    string potential = str0[6];
    
    string isoA = str1[0];
    string isoB = str2[1];
    string reactionType = str2[0];
    
    if( potential.length() != 2 ){
      printf("  ===> ERROR! Potential input should be 2 charaters! skipped. \n");
      continue;
    }
    
    string node = orbital.substr(0,1);
    string jValue = orbital.substr(2);
    string lValue = orbital.substr(1,1);
    int spdf = GetLValue(lValue);
    //printf(" j = %s, l = %s = %d, parity = %s \n", jValue.c_str(), lValue.c_str(), spdf, parity.c_str());
    if( spdf == -1 ){
      printf(" ===> skipped. Not reconginzed L-label. \n");
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
    string beamParticle = reactionType.substr(1,1);
    int factor = 1;
    if( unit == "MeV/u" && beamParticle == "d") factor = 2;
    double totalBeamEnergy = atof(reactionEnergy.substr(0, pos+1).c_str()) * factor;
    //printf("unit : %s , %f\n", unit.c_str(), totalBeamEnergy);
    
    
    //printf("  target nucleus : %s \n", isoA.c_str());
    //printf("        reaction : %s \n", reactionType.c_str());
    //printf("          remain : %s \n", isoB.c_str());
    //printf(" reaction energy : %s \n", reactionEnergy.c_str());
    //printf("       Potential : %s \n", potential.c_str());
    //printf("         orbital : %s \n", orbital.c_str());
    //printf("        Ex [MeV] : %s \n", Ex.c_str());
    
    Isotope isotopeA(str1[0]);
    Isotope isotopeB(str2[1]);
    
    double Qvalue = 1875.6129 + isotopeA.Mass - 938.272 - isotopeB.Mass; //(d,p) reaction
    //printf("Q-Value = %f MeV\n", Qvalue);
    
    //printf("A: %d, Z: %d, mass: %f MeV/c2 \n", isotopeA.A, isotopeA.Z, isotopeA.Mass);
    //printf("A: %d, Z: %d, mass: %f MeV/c2 \n", isotopeB.A, isotopeB.Z, isotopeB.Mass);
    if( isotopeA.Mass == -404 || isotopeB.Mass == -404 ){
      printf("  ===> Error! mass does not found. \n");
      continue;
    }
    
    //write ptolmey infile
    numOfReaction ++ ;
    fprintf(file_out, "$============================================ Ex=%s(%s)%s\n", Ex.c_str(), orbital.c_str(), potential.c_str());
    fprintf(file_out, "reset\n");
    
    fprintf(file_out, "REACTION: %s%s%s(%s%s %s) ELAB=%7.3f\n", 
             isoA.c_str(), reactionType.c_str(), isoB.c_str(), jValue.c_str(), parity.c_str(), Ex.c_str(),  totalBeamEnergy);
    fprintf(file_out, "PARAMETERSET dpsb r0target \n");
    fprintf(file_out, "$lstep=1 lmin=0 lmax=30 maxlextrap=0 asymptopia=50 \n");
    fprintf(file_out, "\n");
    fprintf(file_out, "PROJECTILE \n");
    fprintf(file_out, "wavefunction av18 \n");
    fprintf(file_out, "r0=1 a=0.5 l=0 \n");
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
    CallPotential(pot1Name, isotopeA.A, isotopeA.Z, totalBeamEnergy);
    fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
    fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
    fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
    fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
    fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
    fprintf(file_out, ";\n");
    
    string pot2Name = potential.substr(1,1);
    string pot2Ref = potentialRef(pot2Name);
    fprintf(file_out, "OUTGOING $%s\n", pot2Ref.c_str());
    CallPotential(pot2Name, isotopeB.A, isotopeB.Z, totalBeamEnergy + Qvalue - atof(Ex.c_str())); 
    fprintf(file_out, "v    = %7.3f    r0 = %7.3f    a = %7.3f\n", v, r0, a);
    fprintf(file_out, "vi   = %7.3f   ri0 = %7.3f   ai = %7.3f\n", vi, ri0, ai);
    fprintf(file_out, "vsi  = %7.3f  rsi0 = %7.3f  asi = %7.3f\n", vsi, rsi0, asi);
    fprintf(file_out, "vso  = %7.3f  rso0 = %7.3f  aso = %7.3f\n", vso, rso0, aso);
    fprintf(file_out, "vsoi = %7.3f rsoi0 = %7.3f asoi = %7.3f  rc0 = %7.3f\n", vsoi, rsoi0, asoi, rc0);
    fprintf(file_out, ";\n");
    
    
    fprintf(file_out, "anglemin=%f anglemax=%f anglestep=%f\n", angMin, angMax, angStep);
    fprintf(file_out, ";\n");
    
  }
  
  printf("================= end of input. Number of Reaction : %d \n", numOfReaction);
  
  fprintf(file_out, "end $================================== end of input\n");
  file_in.close();
  fclose(file_out);
  
  return 1;

} 
