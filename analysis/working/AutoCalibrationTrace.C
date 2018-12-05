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
#include "../Simulation/transfer.C"
#include "../Armory/Cali_littleTree_trace.h"
#include "../Armory/Check_e_x.C"
#include "../Armory/Cali_e_single.C"
#include "../Armory/Cali_xf_xn.C"
#include "../Armory/Cali_xf_xn_to_e.C"
#include "../Armory/Cali_scale_x.C"
#include "../Armory/Cali_compareF.C"
#include "../Armory/Cali_e_trace.h"
#include "../Armory/Cali_e_trace.h"
#include "../Armory/GetCoinTimeCorrectionCutG.C"

//==========================================
//         This file show the steps for calibration 
//         //TODO detect branch te_t and trdt_t exist or not, if exist, calibrate with coinTime
//==========================================

void AutoCalibrationTrace(){
   
   int option;
   printf(" ================================================== \n");
   printf(" ========= Auto Calibration w/ Trace ============== \n");
   printf(" ================================================== \n");
   printf("      Please Edit runsList.txt  \n");
   //printf(" ------ GeneralSortTrace.C output : sorted.root --- \n");
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
   printf(" 7 = run Simulation/transfer.C\n");
   printf(" ================================================== \n");
   printf(" Choose action : ");
   int temp = scanf("%d", &option);
   printf(" -------------------------------------------------- \n");
   printf(" ..... loading runsList.txt........ \n");
//==================================================== data files
   
   //======== alpha data
   TChain * chainAlpha = new TChain("gen_tree");
   
   //======== experimental sorted data
   TChain * chain = new TChain("gen_tree");
   
   string dataList="runsList.txt";
   ifstream file;
   file.open(dataList.c_str());
   
   int nBranchAlpha = 0;
   int nBranch = 0;
   
   if( file.is_open() ){
      string line;
      while( file >> line){
         //printf("%s \n", line.c_str());
         int tempNBranchAlpha = 0;
         int tempNBranch      = 0;
         
         
         if( line.substr(0,2) == "//" ) continue;
         if( line.substr(0,1) == "-" ) {
            chainAlpha->Add(line.substr(1).c_str());
            
            //Check size of Branch, if not consistence with the first, return error.
            int entry = chainAlpha->GetEntries();
            chainAlpha->GetEntry(entry);
            if( nBranchAlpha == 0 ) {
               nBranchAlpha = chainAlpha->GetNbranches(); 
            }else{
               tempNBranchAlpha = chainAlpha->GetNbranches();
               
               if( tempNBranchAlpha != nBranchAlpha ){
                  printf(" tree structure inconistant at %s \n", line.substr(1).c_str());
                  gROOT->ProcessLine(".q");
                  return;
               }
            }
         }
         
         if( line.substr(0,1) == "+" ){
            chain->Add(line.substr(1).c_str());
            
            //Check size of Branch, if not consistence with the first, return error.
            int entry = chain->GetEntries();
            chain->GetEntry(entry);
            if( nBranch == 0 ) {
               nBranch = chain->GetNbranches(); 
            }else{
               tempNBranch = chain->GetNbranches();
               
               if( tempNBranch != nBranch ){
                  printf(" tree structure inconistant at %s \n", line.substr(1).c_str());
                  gROOT->ProcessLine(".q");
                  return;
               }
            }
         }
      }
      file.close();
   }else{
      printf("!!!!! ERRRO!!! Missing runsList.txt\n");
      gROOT->ProcessLine(".q");
      return; 
   }
   
   printf("                              .......... done\n");
   printf(" -------------------------------------------------- \n");
   
/**///=========================================== Calibration
   if( option > 7 || option < 0 ) {
     printf(" --- bye!----\n");
     gROOT->ProcessLine(".q");
     return;
   }
   
   if( option == 0 || option == 4 || option == 5 ){
      printf(" ============================= alpha source files :  \n");
      chainAlpha->GetListOfFiles()->Print();
      printf(" ================================================== \n");
   }
   if( 1 <= option && option <= 3){
      printf(" ================ files :  \n");
      chain->GetListOfFiles()->Print();
      printf(" ================================================== \n");
   }    
   
   if( option == 0 ) {
      Cali_xf_xn(chainAlpha);
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
      printf(" Step 2) Generate kinematics line using Simulation/transfer.C    \n");
      printf("         ** make sure you have correct A) basicReactoinConfig.txt    \n");
      printf("                                       B) excitation_energies.txt    \n");
      printf(" Step 3) Run the Calibration using Armory/compare_F.C   \n");
      printf("=================================================================\n");
      int proceedFlag = 0;
      printf(" Proceed ? (1 = Yes / 0 = No) ");
      temp = scanf("%d", &proceedFlag);
      
      if( proceedFlag == 0 ) {
         printf(" ------ bye bye !------- \n");
         gROOT->ProcessLine(".q");
         return;
      }
      printf("#######################################################\n");
      printf("Step 1) ========= creating smaller tree.\n");
      chain->Process("../Armory/Cali_littleTree_trace.C+");
      double eThreshold = 300;
      Check_e_x("temp.root", eThreshold);
      
      TFile *caliFile = new TFile ("temp.root", "read");
      if( !caliFile->IsOpen() ){
         printf("!!!!!!!!!!! no temp.root, please run step 2.!!!!!!!\n");
         gROOT->ProcessLine(".q");
         return;
      }
      TTree * caliTree = (TTree*) caliFile->Get("tree");
      
      printf("#######################################################\n");
      printf("Step 2) =============== creating transfer.C\n");

      int nextFlag = 0; 
      printf("(1 = Yes / 0 = No ) ? ");
      temp = scanf("%d", &nextFlag);
      
      if( nextFlag == 0 ){
         printf(" ------ bye bye !------- \n");
         gROOT->ProcessLine(".q");
         return;
      }
      transfer();
      
      TString rootfileSim="transfer.root";
      TFile *fs = new TFile (rootfileSim, "read"); 
      if(!fs->IsOpen()){
         printf("!!!!! cannot open transfer.root !!!!! \n");
         gROOT->ProcessLine(".q");
         return;
      }
      
      printf("#######################################################\n");
      printf("Step 3) =============== Calibrate\n");
      float energyThreshold = 300;
      printf(" Energy Threshold (default = 300 ch, -1 to stop): ");
      temp = scanf("%f", &energyThreshold);
      if( energyThreshold < 0 ) {
         printf(" ------ bye bye !------- \n");
         gROOT->ProcessLine(".q");
         return;
      }
      
      int eCdet = -1; 
      printf(" Choose detID (-1 for all & make new root): ");
      temp = scanf("%d", &eCdet);
      Cali_compareF(caliTree, fs, eCdet, energyThreshold);
      
      if( eCdet == -1) {
         chain->Process("../Armory/Cali_e_trace.C+");
         gROOT->ProcessLine(".q");
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
      Cali_e_single(chainAlpha, det);
      //gROOT->ProcessLine(".q");
   }
   
   if( option == 5 ) {
      Cali_scale_x(chainAlpha);
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
   
    if( option == 7 ) {
      printf("========= Simulation/transfer.C ========= \n");
      printf("== Please check :\n");
      printf("        1) detectorGeo.txt\n");
      printf("        2) basicReactionConfig.txt\n");
      printf("        3) excitation_energies.txt\n");
      printf("========================================= \n");
      int nextFlag = 0; 
      printf("Prceed (1 = Yes / 0 = No ) ? ");
      temp = scanf("%d", &nextFlag);
      
      if( nextFlag == 0 ){
         printf(" ------ bye bye !------- \n");
         gROOT->ProcessLine(".q");
         return;
      }
      transfer();
      gROOT->ProcessLine(".q");
   }
}

