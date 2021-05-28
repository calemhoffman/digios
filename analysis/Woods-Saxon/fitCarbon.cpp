/****************************************************************
  fitting Carbon ESPE
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2021-5-13
*****************************************************************/

#include <stdio.h>
#include <string>
#include <stdlib.h> //for atoi, atof
#include <cmath> //exp
#include <vector> 
#include <fstream> 

#include "TMath.h"
#include "RK4.h"
#include "WS.h"

using namespace std;

int main(int argc, char *argv[]){
  
  vector<double> fitEnergy;
  vector<string> fitorb;
  
  ///======== 12C
  fitEnergy.push_back(-16.359);  fitorb.push_back("0p3/2");
  fitEnergy.push_back( -9.422);  fitorb.push_back("0p1/2");
  fitEnergy.push_back( -1.871);  fitorb.push_back("1s1/2");
  fitEnergy.push_back( -1.215);  fitorb.push_back("0d5/2");
  
  ///======== 13C
  //fitEnergy.push_back( -1.617);  fitorb.push_back("1s1/2");
  //fitEnergy.push_back( -1.123);  fitorb.push_back("0d5/2");
  
  
  ///======== 14C
  fitEnergy.push_back(-16.927);  fitorb.push_back("0p3/2");
  fitEnergy.push_back( -8.829);  fitorb.push_back("0p1/2");
  fitEnergy.push_back( -1.485);  fitorb.push_back("1s1/2");
  fitEnergy.push_back( -1.066);  fitorb.push_back("0d5/2");
  
  ///======== 15C
  fitEnergy.push_back( -1.218);  fitorb.push_back("1s1/2");
  fitEnergy.push_back( -0.474);  fitorb.push_back("0d5/2");
  
  ///======== 16C
  //fitEnergy.push_back( -0.76);  fitorb.push_back("1s1/2");

  ///======== 16N
  //fitEnergy.push_back( -2.16);  fitorb.push_back("1s1/2");
  //fitEnergy.push_back( -2.32);  fitorb.push_back("0d5/2");
  
  
  vector<int>stateID = {4, 8, 10, 12, 13, 15};
  
  ///================= For only 12C, 14C, and 15C, V0 = -57, Vsym = 49, Vso = 21  %rms = 0.227118
  ///================= For only 12C, 13C, and 14C, V0 = -57, Vsym = 45, Vso = 22  %rms = 0.125987
  
  ///================= For only 12-16C, 16N,       V0 = -56, Vsym = 47, Vso = 24  %rms = 0.249321
  
  ///================= For all but only sd-shell,  V0 = -57, Vsym = 50, Vso = 21  %rms = 0.191864
  
  
  
  WoodsSaxon ws;
  
  ws.SetNucleus(12,6);
  ws.IsNeutron();
  ws.SetRange2(0.0001, 0.1, 200);
  
  double V0start = -57., V0end = -57.;
  double VsymStart = -49, VsymEnd = -49;
  double VsoStart = 21, VsoEnd = 21;
  
  double rmsMin = 1000;
  double rms2Min = 1000;
  
  double V0min, VsymMin, VsoMin;
  
  ///======= count number of trials;
  int totTrials = 0;
  for( double Vsym = VsymStart ; Vsym <= VsymEnd; Vsym += 1.0 ){
    for( double Vso = VsoStart ; Vso <= VsoEnd ; Vso += 1.0){  
      for( double V0 = V0start ; V0 <= V0end ; V0 += 1.0 ){  
        totTrials ++;
      }
    }
  }
  
  int nTrials = 0;
  for( double Vsym = VsymStart ; Vsym <= VsymEnd; Vsym += 1.0 ){
    for( double Vso = VsoStart ; Vso <= VsoEnd ; Vso += 1.0){  
      for( double V0 = V0start ; V0 <= V0end ; V0 += 1.0 ){
  
        vector<double> energy;
        nTrials ++;
        
        int A = 0;
        int id = 0;
        for( int AA = 12; AA <= 17 ; AA++ ){
          
          if( AA <  17 ) {
            A = AA;
            ws.SetNucleus(A, 6);
          }
          if( AA == 17 ) {
            A = 16;
            ws.SetNucleus(16, 7);
          }
        
          double V = V0 - Vsym*(ws.GetN() - ws.GetZ())/A;
          
          ws.SetMass( mn * A / (A+1) );
          ws.SetWSPars(V, 1.25, 0.67, Vso, 1.20, 0.67, 1.25);
          
          ws.CalWSEnergies();
          ///ws.PrintWSParas();
          ///ws.PrintEnergyLevels();
          
          
          for( int i = 0 ; i < (int) (ws.energy).size(); i ++){
            if( AA == 12 ) {
              for( int j = 0 ; j < stateID[id] ; j ++){
                if( fitorb[j] == (ws.orbString)[i] ) {
                  energy.push_back( (ws.energy)[i] ) ;
                } 
              }
            }
            if( AA == 13 ) {
              for( int j = stateID[id-1]; j < stateID[id] ; j ++){
                if( fitorb[j] == (ws.orbString)[i] ) {
                  energy.push_back( (ws.energy)[i] ) ;
                } 
              }
            }
            if( AA == 14 ) {
              for( int j = stateID[id-1]; j < stateID[id] ; j ++){
                if( fitorb[j] == (ws.orbString)[i] ) {
                  energy.push_back( (ws.energy)[i] ) ;
                } 
              }
            }
            if( AA == 15 ) {
              for( int j = stateID[id-1]; j < stateID[id] ; j ++){
                if( fitorb[j] == (ws.orbString)[i] ) {
                  energy.push_back( (ws.energy)[i] ) ;
                } 
              }
            }
            if( AA == 16 ) {
              for( int j = stateID[id-1]; j < stateID[id]; j ++){
                if( fitorb[j] == (ws.orbString)[i] ) {
                  energy.push_back( (ws.energy)[i] ) ;
                } 
              }
            }
            if( AA == 17 ) {
              for( int j = stateID[id-1]; j < stateID[id]; j ++){
                if( fitorb[j] == (ws.orbString)[i] ) {
                  energy.push_back( (ws.energy)[i] ) ;
                } 
              }
            }
          }
          
          id++;
          
        }
        
        
        ///======== compare the energy
        double rms = 0 , rms2 = 0;

        system("clear");
        printf("=========================== %d/ %d\n", nTrials, totTrials);  
        
        printf(" V0   : %f \n", V0);
        printf(" Vsym : %f \n", Vsym);
        printf(" Vso  : %f \n", Vso);
              
        for( int i = 0 ; i < (int) fitEnergy.size(); i++){
          rms  += pow((fitEnergy[i] - energy[i])/fitEnergy[i], 2);
          rms2 += pow((fitEnergy[i] - energy[i]), 2);
          printf("%2d | %10.6f, %10.6f | %10.6f, %10.6f \n", i, fitEnergy[i], energy[i],
           (fitEnergy[i] - energy[i])/fitEnergy[i],
           fitEnergy[i]-energy[i]);
        }
        
        rms = sqrt(rms/10.);
        rms2 = sqrt(rms2/10.);
        printf("-------------------------- rms : %f, %f \n", rms, rms2);
        
        if( rms < rmsMin ) {
          rmsMin = rms;
          rms2Min = rms2;
          V0min = V0;
          VsymMin = Vsym;
          VsoMin = Vso;
        }

        printf(" V0   : %f \n", V0min);
        printf(" Vsym : %f \n", VsymMin);
        printf(" Vso  : %f \n", VsoMin);  
        printf("-------------------------- Best rms : %f, %f \n", rmsMin, rms2Min);        
      }
    }
  }
  
  printf("================================== Best\n");
  printf(" V0   : %f \n", V0min);
  printf(" Vsym : %f \n", VsymMin);
  printf(" Vso  : %f \n", VsoMin);  
  printf(" rms  : %f \n", rmsMin);  
  

  for( int A = 12; A <= 16; A++){
    ws.SetNucleus(A,6);
    ws.SetMass( mn * A / (A+1) );
    ws.SetWSPars(V0min - VsymMin * (A-12) / A , 1.25, 0.67, VsoMin, 1.20, 0.67, 1.25);
    ws.CalWSEnergies();
    ws.PrintWSParas();
    ws.PrintEnergyLevels();
  }

  ws.SetNucleus(16,7);
  ws.SetMass( mn * 16 / 17 );
  ws.SetWSPars(V0min - VsymMin * (2) / 16 , 1.25, 0.67, VsoMin, 1.20, 0.67, 1.25);
  ws.CalWSEnergies();
  ws.PrintWSParas();
  ws.PrintEnergyLevels();
  
}
