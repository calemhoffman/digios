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
#include "../AutoCali/Cali_littleTree_trace.h"
#include "../AutoCali/Check_e_x.C"
#include "../AutoCali/Cali_compareF.C"
#include "../AutoCali/Cali_xf_xn.C"
#include "../AutoCali/Cali_xf_xn_to_e.C"
#include "../AutoCali/Cali_e_trace.h"
#include "../AutoCali/GetCoinTimeCorrectionCutG.C"

//==========================================
//         This file show the steps for calibration 
//         //TODO detect branch te_t and trdt_t exist or not, if exist, calibrate with coinTime
//==========================================

void AutoCalibrationTrace(double eThreshold = 300){
   
   int option;
   printf(" ========= Auto Calibration w/ Trace ============== \n");
   printf(" ================================================== \n");
   printf(" --------- GeneralSortTrace.C --> sorted.root------  \n");
   printf(" ================================================== \n");
   printf(" 0 = alpha source calibration for xf - xn.\n");
   printf(" 1 = xf+xn to e calibration. \n");
   printf(" 2 = Generate smaller root file with e, x, z, detID, coinTimeUC (aware of Gate)\n");
   printf(" 3 = coinTimeUC calibration. (MANUAL) \n");
   printf(" --------- transfer.root require below ------------\n");
   printf(" 4 = e calibration by compare with simulation.\n");
   printf(" 5 = Generate new root with calibrated data. \n");
   printf(" ================================================== \n");
   printf(" Choose action : ");
   int temp = scanf("%d", &option);
   
//==================================================== data files
   
   //======== alpha data
   TChain * chainAlpha = new TChain("gen_tree");
   
   //======== experimental sorted data
   TChain * chain = new TChain("gen_tree");
   
   string dataList="dataList.txt";
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
   }
   
   //printf("====================== alpha data. \n");
   //chainAlpha->GetListOfFiles()->Print();
   //printf("====================== data. \n");
   //chain->GetListOfFiles()->Print();
   
/**///=========================================== Calibration
   if( option > 5 || option < 0 ) return;
   
   if( option == 0 ) {
      printf(" ================ alpha source files :  \n");
      chainAlpha->GetListOfFiles()->Print();
      printf(" ================================================== \n");
      Cali_xf_xn(chainAlpha);
      return ;
   }
   
      
   printf(" ================ files :  \n");
   chain->GetListOfFiles()->Print();
   printf(" ================================================== \n");

   
   if( option == 1 ) Cali_xf_xn_to_e(chain);
   
   TString rootfileSim="transfer.root";
      
   if( option == 2 ) {
      printf("=============== creating smaller tree.\n");
      chain->Process("../AutoCali/Cali_littleTree_trace.C+");
      Check_e_x("temp.root", eThreshold);
   }
   
   if( option == 3 ) {
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
   
   if( option == 4 ) {
      TFile *caliFile = new TFile ("temp.root", "read");
      if( !caliFile->IsOpen() ){
			printf("!!!!!!!!!!! no temp.root, please run step 2.!!!!!!!\n");
			return;
		}
      TTree * caliTree = (TTree*) caliFile->Get("tree");
      
      TFile *fs = new TFile (rootfileSim, "read"); 
      
      if(fs->IsOpen()){
         
         int eCdet = -1; 
         printf(" Choose detID (-1 for all & make new root): ");
         temp = scanf("%d", &eCdet);
         Cali_compareF(caliTree, fs, eCdet, eThreshold);
         
         if( eCdet == -1) {
            chain->Process("../AutoCali/Cali_e_trace.C+");
            gROOT->ProcessLine(".q");
         }
      }else{
         printf("!!!!! cannot open transfer.root !!!!! \n");
         return;
      }
   }
   
   if( option == 5 ) {
      chain->Process("../AutoCali/Cali_e_trace.C+");
      gROOT->ProcessLine(".q");
   }
   
}

