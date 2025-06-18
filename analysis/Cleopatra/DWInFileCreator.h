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
#include <complex>
#include <tuple>

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

struct TNA{
    float Ex;
    std::vector<float> overlap1;
    std::vector<float> overlap2;
    std::vector<float> amplitude;
};

std::tuple<std::vector<TNA>, std::vector<std::vector<int>>> parseTNAFile(string inputFile){
    ifstream file_in;
    file_in.open(inputFile.c_str(), ios::in);

    if( !file_in ){
      printf(" cannot read file. \n");
        //return;
    }
    //extract information
      int numOfReaction = 0;
      int skipFirstLine = 0;
      int record = 0;
      std::vector<std::vector<int>> orbitals;
      std::vector<float> o1;
      std::vector<float> o2;
      std::vector<float> amp;
      std::vector<TNA> allTNAs;
      float Ex = 0;
      while( file_in.good() ) {
          string tempLine;
          getline(file_in, tempLine );
          
          if( tempLine.substr(0, 1) == "!" ) continue;
          if( tempLine.size() < 5 ) continue;
          if( skipFirstLine == 0) {
              skipFirstLine++;
              continue;
          }
          
          
          //printf("  %s\n", tempLine.c_str());
          string kk = "k = ";
          string orbs = "2j =";
          size_t found = tempLine.find(kk);
          size_t forbs = tempLine.find(orbs);
          if (found != string::npos){
              string s1 = tempLine.substr(found+5,found+6);
              string s2 = tempLine.substr(forbs+8,forbs+9);
              string n = s2.substr(0,2);
              string l = s2.substr(2,4);
              string j = s2.substr(4,5);
              int nVal = std::stoi(n);
              int lVal = std::stoi(l);
              int jVal = std::stoi(j);
              s1 = s1.substr(0,2);
              int kVal = std::stoi(s1);
              orbitals.push_back({kVal,nVal,lVal,jVal});
          }
          else{
              if( tempLine.size() < 10 ){
                  record = 0;
                  allTNAs.push_back({Ex, o1, o2, amp});
                  continue;
              }
              if(record == 1){
                  string p1 = tempLine.substr(0,5);
                  string p2 = tempLine.substr(5,8);
                  string p3 = tempLine.substr(21,22);
                  float p1int = std::stof(p1);
                  float p2int = std::stof(p2);
                  float p3int = std::stof(p3);
                  o1.push_back(p1int);
                  o2.push_back(p2int);
                  amp.push_back(p3int);
                  
              }
              if(tempLine.size() > 60){
                  Ex = 0;
                  o1.clear();
                  o2.clear();
                  amp.clear();
                  record = 1;
                  string ex = tempLine.substr(54,56);
                  Ex = std::stof(ex);
              }
          }
      }
      
      /*for(int i=0;i<allTNAs.size();i++){
          std::cout<<allTNAs[i].Ex<<std::endl;
          for(int j=0;j<allTNAs[i].overlap1.size();j++){
              std::cout<<allTNAs[i].overlap1[j]<<" "<<allTNAs[i].overlap2[j]<<" "<<allTNAs[i].amplitude[j]<<std::endl;
          }
      }*/
    
    /*for(int i=0;i<orbitals.size();i++){
        for(int j=0;j<orbitals[i].size();j++){
            std::cout<<orbitals[i][j]<<" ";
        }
        std::cout<<std::endl;
    }*/
      
      

    file_in.close();
    
    return {allTNAs, orbitals};

    
}


int DWInFileCreator(string readFile, string infile, double angMin, double angMax, double angStep, string TNAinfile) {
   
  //================= read infile. extract the reactions, write pptolemy infile for each reaction
  ifstream file_in;
  file_in.open(readFile.c_str(), ios::in);

  if( !file_in ){
    printf(" cannot read file. \n");
    return 0 ; 
  }
    
    ifstream TNAfile_in;
    TNAfile_in.open(TNAinfile.c_str(), ios::in);

    if( !TNAfile_in ){
      printf(" cannot read TNA file. \n");
      return 0 ;
    }
    TNAfile_in.close();
   
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
      if( iso_a.Mass == iso_b.Mass ) {
          isTransferReaction = false;
          isReactionSupported = false;
      }
    
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
    int spin_int = std::stoi(spin);
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
    
      //Read TNA file *******************************************************
      //string readFile = "/Users/nwatwood/Downloads/si32tnazip/tfew00_ey00.tna";
      auto [allTNAs, orbitals] = parseTNAFile(TNAinfile);
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
        double ExEnergy = atof(Ex.c_str());
        fprintf(file_out, "1200000030000000    %s%s%s(%s%s %s) ELAB=%7.3f\n", isoA.c_str(), reactionType.c_str(), isoB.c_str(), spin.c_str(), parity.c_str(), Ex.c_str(),  totalBeamEnergy);
        fprintf(file_out, "+%2.0f.    +%02.0f.    +%03.2f\n", (angMax-angMin)/angStep, angMin, angStep);
        fprintf(file_out, "+20+01+0%s+0%i\n", spin.c_str(), 2*spin_int);
        fprintf(file_out, "+00.1   +00.    +15.    +00.\n");
        
        //Incoming Potential
        string pot1Name = potential.substr(0,1);
        string pot1Ref = potentialRef(pot1Name);
        CallPotential(pot1Name, iso_A.A, iso_A.Z, totalBeamEnergy, iso_a.Z);
        
        fprintf(file_out, "%+.1f   +%02d.    +%02d.    +%d.    +%02d.    +%05.2f                  +01.\n",totalBeamEnergy,iso_a.A,iso_a.Z,iso_A.A,iso_A.Z, rc0);
        fprintf(file_out, "+01.    %+06.1f  %+07.3f %+07.3f +00.    %+05.1f   %+07.3f %+07.3f +00.\n",-1*v,r0,a,-1*vi,ri0,ai); //volume
        fprintf(file_out, "+02.    %+06.2f  %+07.3f %+07.3f +00.    %+05.1f   %+07.3f %+07.3f +00.\n",0.0,ri0,ai,4*vsi,rsi0,asi); //surface
        fprintf(file_out, "-04.    %+06.2f  %+07.3f %+07.3f +00.    %+05.1f   %+07.3f %+07.3f +00.\n",vso,rso0,aso,vsoi,rsoi0,asoi); //spin-orbit
        
        //Outgoing Potential
        string pot2Name = potential.substr(1,1);
        string pot2Ref = potentialRef(pot2Name);
        double eBeam = totalBeamEnergy + Qvalue - atof(Ex.c_str());
        CallPotential(pot2Name, iso_B.A, iso_B.Z, eBeam, iso_b.Z);
        
        fprintf(file_out, "%+06.3f  %+03d.    %+03d.    %+03d.    %+03d.    %+06.2f                  +01.\n",Qvalue-ExEnergy,iso_b.A,iso_b.Z,iso_B.A,iso_B.Z,rc0);
        fprintf(file_out, "+01.    %+06.1f  %+07.3f %+07.3f +00.    %+05.1f   %+07.3f %+07.3f +00.\n",-1*v,r0,a,-1*vi,ri0,ai); //volume
        fprintf(file_out, "+02.    %+06.2f  %+07.3f %+07.3f +00.    %+05.1f   %+07.3f %+07.3f +00.\n",0.0,ri0,ai,4*vsi,rsi0,asi); //surface
        fprintf(file_out, "-04.    %+06.2f  %+07.3f %+07.3f +00.    %+05.1f   %+07.3f %+07.3f +00.\n",vso,rso0,aso,vsoi,rsoi0,asoi); //spin-orbit
        
        //TNAs
        float closestEx=100;
        int closestExpos=0;
        for(int i=0;i<allTNAs.size();i++){
            if(abs(allTNAs[i].Ex-ExEnergy)<abs(closestEx-ExEnergy)){
                closestEx = allTNAs[i].Ex;
                closestExpos = i;
            }
        }
        std::cout<<"Using TNAs for energy closest to Ex = "<<ExEnergy<<" is TNA Ex = "<<closestEx<<std::endl;
        
        int TNA_n = 0;
        int TNA_l = 1;
        int TNA_j = 2;
        int TNA_n2 = 0;
        int TNA_l2 = 1;
        int TNA_j2 = 3;
        double n2Threshold = iso_B.CalSp(0,2);
        for(int i=0;i<allTNAs[closestExpos].amplitude.size();i++){
            
            for(int j=0;j<orbitals.size();j++){
                if (allTNAs[closestExpos].overlap1[i] == orbitals[j][0]){
                    TNA_n = orbitals[j][1];
                    TNA_l = orbitals[j][2];
                    TNA_j = orbitals[j][3];
                }
                if (allTNAs[closestExpos].overlap2[i] == orbitals[j][0]){
                    TNA_n2 = orbitals[j][1];
                    TNA_l2 = orbitals[j][2];
                    TNA_j2 = orbitals[j][3];
                }
                
            }
            
            int fact_nushellx = 1 ; //std::pow(-1,(TNA_j*0.5) - (TNA_j2*0.5)); // !!! If you are not using nushellx output, set this value to 1.
            int fact = (TNA_l+TNA_l2+2)-spin_int;
            std::complex<double> factor=std::pow(1i,fact);
            double total_factor = factor.real()*fact_nushellx;
            
            if(factor.real()<1 && factor.real()>-1){
                std::cout<<"!!! DWUCK TNA phase calculations yield imaginary numbers. Check to make sure your input spin and TNA file is correct"<<std::endl;
                std::cout<<std::endl;
                return 0;
            }
            
            
            if(allTNAs[closestExpos].overlap1[i]==allTNAs[closestExpos].overlap2[i]){
                if(i==allTNAs[closestExpos].amplitude.size()-1){
                    fprintf(file_out, "-01.    +05.    +00.    %+7.3f\n", allTNAs[closestExpos].amplitude[i]*total_factor);
                } else {
                    fprintf(file_out, "+01.    +05.    +00.    %+7.3f\n", allTNAs[closestExpos].amplitude[i]*total_factor);
                }
                
                fprintf(file_out, "%+06.2f  +01.    +00.    %+03d.    %+03d.    +01.25\n",-1*((n2Threshold-ExEnergy)/2), iso_A.A,iso_A.Z);
                fprintf(file_out, "-01.    -01.    +01.26  +00.60  +25.\n");
                fprintf(file_out, "+%02i.    +%02i.    +%02i.    +01.    +55.\n",TNA_n,TNA_l,TNA_j);
            }
            else {
                if(i==allTNAs[closestExpos].amplitude.size()-1){
                    fprintf(file_out, "-02.    +05.    +00.    %+7.3f\n", allTNAs[closestExpos].amplitude[i]*total_factor);
                } else {
                    fprintf(file_out, "+02.    +05.    +00.    %+7.3f\n", allTNAs[closestExpos].amplitude[i]*total_factor);
                }
                
                fprintf(file_out, "%+06.2f  +01.    +00.    %+03d.    %+03d.    +01.25\n",-1*((n2Threshold-ExEnergy)/2), iso_A.A,iso_A.Z);
                fprintf(file_out, "-01.    -01.    +01.26  +00.60  +25.\n");
                fprintf(file_out, "+%02i.    +%02i.    +%02i.    +01.    +55.\n",TNA_n,TNA_l,TNA_j);
                fprintf(file_out, "%+06.2f  +01.    +00.    %+03d.    %+03d.    +01.25\n",-1*((n2Threshold-ExEnergy)/2), iso_A.A,iso_A.Z);
                fprintf(file_out, "-01.    -01.    +01.26  +00.60  +25.\n");
                fprintf(file_out, "+%02i.    +%02i.    +%02i.    +01.    +55.\n",TNA_n2,TNA_l2,TNA_j2);
            }
        }
        
        fprintf(file_out, "9                    END OF DATA");
        
    }
    
  }
  

  file_in.close();
  fclose(file_out);
  
  return 1;

} 
