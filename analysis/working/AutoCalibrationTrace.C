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
   printf("      Please Edit dataList.txt  \n");
   //printf(" ------ GeneralSortTrace.C output : sorted.root --- \n");
   printf(" ================================================== \n");
   printf(" 0 = alpha source calibration for e and xf - xn.\n");
   printf(" 1 = xf+xn to e calibration. \n");
   printf(" 2 = Auto-Calibration by Kinematics.\n");
   printf(" --------- transfer.root required below  ----------\n");
   printf(" 3 = Generate new root with calibrated data. \n");
   printf(" --------- other programs  ------------------------ \n");
   printf(" 4 = e calibration for single detector (alpha)\n");
   printf(" 5 = x - scaling to full (-1,1) (alpha)\n");
   printf(" 6 = coinTimeUC calibration. (MANUAL) \n");
   printf(" ================================================== \n");
   
//==================================================== data files
   
   //======== alpha data
   TChain * chainAlpha = new TChain("gen_tree");
   
   //======== experimental sorted data
   TChain * chain = new TChain("gen_tree");
   
   string dataList="runsList.txt";
   ifstream file;
   file.open(dataList.c_str());
   if( file.is_open() ){
      string line;
      while( file >> line){
         //printf("%s \n", line.c_str());
         if( line.substr(0,2) == "//" ) continue;
         if( line.substr(0,1) == "-" ) {
            chainAlpha->Add(line.substr(1).c_str());
         }
         
         if( line.substr(0,1) == "+" ){
            chain->Add(line.substr(1).c_str());
         }
      }
      file.close();
   }else{
      printf("!!!!! ERRRO!!! Missing runsList.txt\n");
      gROOT->ProcessLine(".q");
      return; 
   }
   
   
   printf(" Choose action : ");
   int temp = scanf("%d", &option);
   
/**///=========================================== Calibration
   if( option > 6 || option < 0 ) {
     printf(" --- bye!----\n");
     gROOT->ProcessLine(".q");
     return;
   }
   
   if( option == 0 || option == 6 || option == 7 ){
      printf(" ============================= alpha source files :  \n");
      chainAlpha->GetListOfFiles()->Print();
      printf(" ================================================== \n");
   }else{
      printf(" ================ files :  \n");
      chain->GetListOfFiles()->Print();
      printf(" ================================================== \n");
   }    
   
   if( option == 0 ) {
      Cali_xf_xn(chainAlpha);
      return ;
   }
   
      

   
   if( option == 1 ) Cali_xf_xn_to_e(chain);
   
   TString rootfileSim="transfer.root";
      
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
      printf(" Proceed ? (1 = Yes / 0 = No");
      temp = scanf("%d", &proceedFlag);
      
      if( proceedFlag == 0 ) {
         printf(" ------ bye bye !------- \n");
         return;
      }
      
      printf("Step 1) ========= creating smaller tree.\n");
      chain->Process("../Armory/Cali_littleTree_trace.C+");
      double eThreshold = 300;
      Check_e_x("temp.root", eThreshold);
      
      TFile *caliFile = new TFile ("temp.root", "read");
      if( !caliFile->IsOpen() ){
         printf("!!!!!!!!!!! no temp.root, please run step 2.!!!!!!!\n");
         return;
      }
      TTree * caliTree = (TTree*) caliFile->Get("tree");
      
      printf("Step 2) =============== creating transfer.C\n");
      transfer();
      
      
      TFile *fs = new TFile (rootfileSim, "read"); 
      if(!fs->IsOpen()){
         printf("!!!!! cannot open transfer.root !!!!! \n");
         return;
      }
      
      
      printf("Step 3) =============== Calibrate\n");
      float energyThreshold = 300;
      printf(" Energy Threshold (default = 300 ch): ");
      temp = scanf("%f", &energyThreshold);
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
}

