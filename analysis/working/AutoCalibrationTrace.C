#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <fstream>
#include <TProof.h>
#include "../Armory/Cali_littleTree_trace.h"
#include "../Armory/Check_e_x.C"
#include "../Armory/Cali_e_single.C"
#include "../Armory/Cali_xf_xn.C"
#include "../Armory/Cali_xf_xn_to_e.C"
#include "../Armory/Cali_scale_x.C"
#include "../Armory/Cali_compareF.C"
#include "../Armory/Cali_e_trace.h"
#include "../Armory/GetCoinTimeCorrectionCutG.C"

//==========================================
//         This file show the steps for calibration 
//         //TODO detect branch te_t and trdt_t exist or not, if exist, calibrate with coinTime
//         //TODO rdtCut is in runslist.txt
//==========================================

int temp ;   


int PrintManual(){

   printf(" ================================================== \n");
   printf(" ========= Auto Calibration w/ Trace ============== \n");
   printf(" ================================================== \n");
   printf(" \e[31m     Please Edit runsList.txt  \e[0m\n");
   printf(" ================================================== \n");
   printf(" 0 = alpha source calibration for e and xf - xn.\n");
   printf(" 1 = xf+xn to e calibration. \n");
   printf(" 2 = Auto-Calibration by Kinematics.\n");
   printf(" --------- transfer.root required -----------------\n");
   printf(" | 3 = Generate new root with calibrated data.    |\n");
   printf(" --------- other programs  ------------------------ \n");
   printf(" 4 = e calibration for single detector (alpha)\n");
   printf(" 5 = x - scaling to full (-1,1) (alpha)\n");
   printf(" 6 = coinTimeUC calibration. (MANUAL) \n");
   printf(" 7 = run Cleopatra/Transfer\n");
   printf(" ================================================== \n");
   printf(" Choose action : ");
   int option;
   temp = scanf("%d", &option);
   printf(" -------------------------------------------------- \n");
   
   return option;
   
}

void toTransferReaction(){
   
   printf("========= Cleopatra/Transfer.C ========= \n");
   printf("== Please check :\n");
   printf("        1) detectorGeo.txt\n");
   printf("        2) reactionConfig.txt\n");
   printf("        3) Ex.txt\n");
   printf("========================================= \n");
   int nextFlag = 0; 
   printf("Prceed (1 = Yes / 0 = No ) ? ");
   temp = scanf("%d", &nextFlag);

   if( nextFlag == 0 ){
      printf(" ------ bye bye !------- \n");
      gROOT->ProcessLine(".q");
      return;
   }
   system("../Cleopatra/Transfer");
   //gROOT->ProcessLine(".q");

}


//TODO in a while-loop that don't need to run the macro again
void AutoCalibrationTrace(){

   int option = PrintManual();
   
//====================================================  Transfer calculation
   if( option == 7 ) {
      toTransferReaction();
   }
   

//==================================================== data files

   //======== experimental sorted data
   TChain * chain = new TChain("gen_tree");
   
   string dataList="ChainMonitors.C";
   printf(" ..... loading %s..", dataList.c_str());
   ifstream file;
   file.open(dataList.c_str());
   
   if( file.is_open() ){
      string line;
      bool startFlag = false;
      while( std::getline(file,line)){

         line.erase(remove(line.begin(), line.end(), ' '), line.end());
         ///printf("%s\n", line.c_str());
         
         if( line == "///**********startMarkerforAutoCalibration." ) {
            startFlag = true;
            continue;
         }
         if( line == "///**********endMarkerforAutoCalibration." ) break;
         if( !startFlag) continue; 
         if( line.length() == 0 ) continue;
         if( line.substr(0,2) == "//" ) continue;
         
         if( line.substr(0, 11) == "chain->Add(" ) {
            
            size_t first, last;
            
            first = line.find_first_of('"');
            last = line.find_last_of('"');
            
            ///printf("%s\n", line.substr(first+1, last-first-1).c_str());
            
            chain->Add(line.substr(first+1, last-first-1).c_str());
         }
         
      }
      file.close();
      gErrorIgnoreLevel = kUnset ; //go back to default ignore level
   }else{
      printf("!!!!! ERRRO!!! Missing %s\n", dataList.c_str());
      gROOT->ProcessLine(".q");
      return; 
   }
   
   printf(".......... done\n");
   printf(" -------------------------------------------------- \n");
   
/**///=========================================== Calibration
   if( option > 7 || option < 0 ) {
     printf(" --- bye!----\n");
     gROOT->ProcessLine(".q");
     return;
   }
   
   if( 0 <= option && option <= 5){
      printf(" ================ files :  \033[0;33m\n");
      chain->GetListOfFiles()->Print();
      printf("\033[0m\n");
   }    
   
   if( option == 0 ) {
      Cali_xf_xn(chain);
      return ;
   }
   
   if( option == 1 ) {
      Cali_xf_xn_to_e(chain);
      gROOT->ProcessLine(".q");
   }
      
   if( option == 2 ) {
      
      printf("=============== Auto-Calibration Subrountine ====================\n");
      printf(" Step 1) Generate smaller root file to speed thing up.           \n");
      printf("         ** aware of the Gate in Armory/Cali_little_tree_trace.C \n");
      printf(" Step 2) Generate kinematics line using Cleopatra/Transfer    \n");
      printf("         ** make sure you have correct A) reactoinConfig.txt    \n");
      printf("                                       B) detGeometry.txt    \n");
      printf("                                       C) Ex.txt    \n");
      printf(" Step 3) Run the Calibration using Armory/Cali_compare_F.C   \n");
      printf("=================================================================\n");
      int proceedFlag = 0;
      printf(" Proceed ? (1 = Yes / 0 = No / 3 = to step 3) ");
      temp = scanf("%d", &proceedFlag);
      
      if( proceedFlag == 0 ) {
         printf(" ------ bye bye !------- \n");
         gROOT->ProcessLine(".q");
         return;
      }
      
      TFile * caliFile = NULL;
      TTree * caliTree = NULL;
      TFile *fs = NULL;
      TString rootfileSim;
      
      if( proceedFlag == 1 ){
        printf("#######################################################\n");
        printf("Step 1) ========= creating smaller tree.\n");
        chain->Process("../Armory/Cali_littleTree_trace.C+");
        double eThreshold = 300;
        Check_e_x("temp.root", eThreshold);
        
        caliFile = new TFile ("temp.root", "read");
        if( !caliFile->IsOpen() ){
           printf("!!!!!!!!!!! no temp.root, please run step 1.!!!!!!!\n");
           gROOT->ProcessLine(".q");
           return;
        }
        caliTree = (TTree*) caliFile->Get("tree");
        
        printf("#######################################################\n");
        printf("Step 2) =============== creating transfer.C\n");

        int nextFlag = 0; 
        printf("(1 = Yes / 0 = No ) ? ");
        temp = scanf("%d", &nextFlag);
        
        rootfileSim="transfer.root";
        if( nextFlag == 1 ){
           toTransferReaction();
        }
        
        fs = new TFile (rootfileSim, "read"); 
        if(!fs->IsOpen()){
           printf("!!!!! cannot open transfer.root !!!!! \n");
           gROOT->ProcessLine(".q");
           return;
        }
      }
      
      if ( proceedFlag == 1 || proceedFlag == 3){
        if( proceedFlag == 3 ){
          caliFile = new TFile ("temp.root", "read");
          if( !caliFile->IsOpen() ){
             printf("!!!!!!!!!!! no temp.root, please run step 1.!!!!!!!\n");
             gROOT->ProcessLine(".q");
             return;
          }
          caliTree = (TTree*) caliFile->Get("tree");
          rootfileSim="transfer.root";
          fs = new TFile (rootfileSim, "read"); 
        }
        printf("#######################################################\n");
        printf("Step 3) =============== Calibrate\n");
        printf("           Please edit the gate on Armory/CompareF.C if needed\n");
        float energyThreshold = 300;
        printf(" Energy Threshold (default = 300 ch, -1 to stop, -9 to exist root): ");
        temp = scanf("%f", &energyThreshold);
        if( energyThreshold == -1 ){
           return;
        }
        if( energyThreshold == -9 ) {
           printf(" ------ bye bye !------- \n");
           gROOT->ProcessLine(".q");
           return;
        }
        
        int eCdet = -1; 
        do{
           printf(" Choose detID (-1 for all, -9 exit ): ");
           temp = scanf("%d", &eCdet);
           if( eCdet >= - 1) {
              double a1Min = 220;
              double a1Max = 320;
              double a0Min = -1.0;
              double a0Max = 1.0;
              int useDefaultRange = 1;
              printf("    Use default range ? a1 = (%.0f, %.0f), a0 = (%.1f, %.1f) [ 1 = Yes, 0 = No ]: ", a1Min, a1Max, a0Min, a0Max);
              temp = scanf("%d", &useDefaultRange);
              if( useDefaultRange == 0 ) {
                  printf("    a1 Min : " ); temp = scanf("%lf", &a1Min);
                  printf("    a1 Max : " ); temp = scanf("%lf", &a1Max);
                  printf("    a0 Min : " ); temp = scanf("%lf", &a0Min);
                  printf("    a1 Max : " ); temp = scanf("%lf", &a0Max);
              }
              int nTrials = 800;
              printf("    Number of Trials? [%d] : ", nTrials);
              temp = scanf("%d", &nTrials);
              Cali_compareF(caliTree, fs, a1Min, a1Max, a0Min, a0Max, nTrials, eCdet, energyThreshold);
            }
         }while( eCdet > -1  );
         
         
        if( eCdet == -9) {
           //chain->Process("../Armory/Cali_e_trace.C+");
           gROOT->ProcessLine(".q");
        }
      }
   }
   
   if( option == 3 ) {
      chain->Process("../Armory/Cali_e_trace.C+");
      gROOT->ProcessLine(".q");
      return;
   }

   
   if( option == 4 ) {
      int det = -1; 
      printf(" Choose detID (-1 to exit): ");
      temp = scanf("%d", &det);
      Cali_e_single(chain, det);
      //gROOT->ProcessLine(".q");
   }
   
   if( option == 5 ) {
      Cali_scale_x(chain);
      //gROOT->ProcessLine(".q");
   }
   
   if( option == 6 ) {
      int det = -1; 
      printf(" Choose detID (-1 for all): ");
      temp = scanf("%d", &det);
      if( det > -1 ){
         GetCoinTimeCorrectionCutG("temp.root", det);
      }else{
         for(int iDet = 0; iDet < 24; iDet ++){
            GetCoinTimeCorrectionCutG("temp.root", iDet);
         }
      }
      gROOT->ProcessLine(".q");
   }
}

