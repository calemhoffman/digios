#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraph.h>
#include <TLine.h>
#include <TSpectrum.h>

#include "../Armory/AnalysisLibrary.h"
#include "../Armory/AutoFit.C"

void Cali_coinTime_alpha(TTree * tree){
/**///======================================================== initial input
   
   const int rowDet = 4;
   const int colDet = 6;
   
/**///========================================================  load tree

   printf("============================================================= \n");
   printf("================ Cali_coinTime_alpha.C ====================== \n");
   printf("============================================================= \n");
   printf("==   calibration for PSD detectors using alpha souce. \n");
   printf("------------------------------------------------------------- \n");
   printf("=========== Total #Entry: %10lld \n", tree->GetEntries());
   
/**///======================================================== check 
   
   TBranch * br = (TBranch *) tree->GetListOfBranches()->FindObject("te_t");
   if( br == NULL ){
      printf("No trace data. abort.\n");
      return;
   }
   
/**///======================================================== Canvas

   const int numDet = rowDet * colDet;

   Int_t Div[2] = {colDet,rowDet};  //x,y
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cAlpha = new TCanvas("cAlpha", "cAlpha", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAlpha->Divide(Div[0],Div[1]);
   
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAlpha->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(0);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
   if(cAlpha->GetShowEditor()  )cAlpha->ToggleEditor();
   if(cAlpha->GetShowToolBar() )cAlpha->ToggleToolBar();
   
/**///========================================================= Analysis

   //############################################################ trigger time vs x 
   printf("############## trigger times vs x \n");
   TH2F ** q = new TH2F*[numDet];
   TString * gate = new TString[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("q%d", i);
      q[i] = new TH2F(name, name, 400, -1.1, 1.1, 400, 50, 150);
      q[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("te_t[%d]:x[%d] >> q%d" ,i, i, i);
      
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "colz");
      cAlpha->Update();
      gSystem->ProcessEvents();
   }
   
   printf("============== fit and correct [Yes = 1, No = 0 ]? ");
   int isFit = 0;
   int temp = scanf("%d", &isFit);
   if( isFit == 0 ) return;
   
   //############################################################ fit pol7
   printf("############### fit with pol-7 \n");
   
   TF1 * fit = new TF1("fit", "pol7");
   
   double para[numDet][8];
   
   for( int i = 0; i < numDet; i ++){
      
      if( i == 11 ) {
         
         for( int j = 0; j < 8; j++){
            para[i][j] = 0.0;
         }
         
         continue;
      }
      
      cAlpha->cd(i+1);

      q[i]->Fit("fit", "q");

      cAlpha->Update();
      gSystem->ProcessEvents();
      
      ///const Double_t* paraA = fit->GetParameters();
      
      for( int j = 0; j < 8; j++){
         para[i][j] = fit->GetParameter(j);
      }
      
   }
   
   printf("############### correction \n");
   
   TH2F ** p = new TH2F*[numDet];
   
   FILE * paraOut;
   TString filename;
   filename.Form("correction_coinTime.dat");
   paraOut = fopen (filename.Data(), "w+");
   
   for( int i = 0; i < numDet; i ++){
      cAlpha->cd(i+1);
      
      TString name;
      name.Form("p%d", i);
      p[i] = new TH2F(name, name, 400, -1.1, 1.1, 400, -10, 10);

      TString expression;
      expression.Form("te_t[%d] - %f - %f*x[%d] - %f*TMath::Power(x[%d],2) - %f*TMath::Power(x[%d],3) - %f*TMath::Power(x[%d],4) - %f*TMath::Power(x[%d],5) - %f*TMath::Power(x[%d],6) - %f*TMath::Power(x[%d],7) :x[%d] >> p%d" ,i, 
                 para[i][0], 
                 para[i][1], i,
                 para[i][2], i,
                 para[i][3], i,
                 para[i][4], i,
                 para[i][5], i,
                 para[i][6], i,
                 para[i][7], i,
                 i, i);
      
      tree->Draw(expression, "", "colz");
      cAlpha->Update();
      gSystem->ProcessEvents();
      
      fprintf(paraOut, "%d\t", i);
      for( int j = 0; j < 8; j++){
         fprintf(paraOut, "%11.6f\t", para[i][j]);
      }
      fprintf(paraOut, "0.00\n");

      fflush(paraOut);
      
   }
   
   fclose(paraOut);   
   
   printf("paramaters saved to %s \n", filename.Data());
   
   return ;
   
}
