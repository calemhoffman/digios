#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TCutG.h>
#include <TProfile.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TMath.h>

#include "../Armory/AutoFit.C"

//######################################################
//   To get the coincident Time correction parameter
//   require processed by Cali_e_trace.C
//######################################################

/*
int nPeaks = 16;
Double_t nGauss(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1); // normalized Gaussian
   }
   return result;
}
*/

TH2F * hTX;   
TH2F * hTXg;  
TH2F * hTXc2; 
TH1F * hT; 
   
void GetCoinTimeCorrectionCutG(TString A_fileName_TChain="root_data/s005_32Si_trace_tp.root",int detID=4){
   detID = 5;
   int timeRange[2] ={-100, 120};
   TString rdtCutName = "rdtCuts.root";

   //====================================================== read file, canvas, histogram
   
   TChain * tree = new TChain("tree");
   tree->Add(A_fileName_TChain);
   tree->GetListOfFiles()->Print();
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);

   // TBranch * br = (TBranch *) tree->GetListOfBranches()->FindObject("coinTimeUC");
   // if( br == NULL ){
   //    printf(" Branch <coinTimeUC> not exist!!!!! exit. \n");
   //    return;
   // }
   TBranch * br = (TBranch *) tree->GetListOfBranches()->FindObject("te_t");
   if( br == NULL ){
      printf(" Branch <coinTimeUC> not exist!!!!! exit. \n");
      return;
   }
   
   TFile * fileCut = new TFile(rdtCutName);
   TObjArray * cutList = NULL;
   TCutG ** cut_in = NULL;
   if( fileCut->IsOpen() ){
      printf("====== File : %s \n", fileCut->GetName());
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut_in = new TCutG * [numCut];
      
      for( int i = 0 ; i < numCut; i++){
         cut_in[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut_in[i]->GetName(), cut_in[i]->GetVarX(), cut_in[i]->GetVarY()); 
      }
   }

   int Div[2] = {4,1};
   int size[2] = {600,600}; //x,y
   TCanvas * cAna = new TCanvas("cAna", "cAna", 100, 100, size[0]*Div[0], size[1]*Div[1]);
   if( cAna->GetShowEditor() )  cAna->ToggleEditor();
   if( cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   cAna->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAna->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(0);
   if(cAna->GetShowEditor() )   cAna->ToggleEditor();
   if(!cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   
   TString name, expression, gate, gateCut;

   hTX   = new TH2F("hTX",   "time vs X; x; te_t [ch]", 200, -1.1, 1.1, 200, 95, 115 );
 hTXg  = new TH2F("hTXg",  "time vs X; x; te_t [ch]", 200, -1.1, 1.1, 200, 95, 115);
hTXc2 = new TH2F("hTXc2", "time vs X; x; te_t [ch]", 200, -1.1, 1.1, 200, -10, 10);
hT = new TH1F("hT", "", 500, -25, 25);
   TProfile * hp = new TProfile("hp", "time Profile", 400, -1.5,1.5);
   TSpectrum * spec = new TSpectrum(5);
   
   TCutG * cut = NULL;
   
   bool isBranchDetIDExist = false;
   br = (TBranch *) tree->GetListOfBranches()->FindObject("detID");
   if( br == NULL ){
      printf(" not detected detID branch. \n");
   }else{
      printf(" detected detID branch. \n");
      isBranchDetIDExist = true;
   }
   
   //====================================================== open savefile
   FILE * paraOut;
   TString filename;
   filename.Form("correction_coinTime.dat");
   paraOut = fopen (filename.Data(), "a+");
   
   //====================================================== processing      
   printf("============ detID: %d\n", detID);

   //expression.Form("coinTimeUC:x>>hTX");
   expression.Form("te_t:x>>hTX");
   if( isBranchDetIDExist ) {   
      gate.Form("detID==%d", detID);
   }else{
      gate.Form("Iteration$==%d", detID);
   }
   
   ///***************
   //if( fileCut->IsOpen() ) gate = "(cut0 || cut1 || cut2 || cut3) && " + gate;
   //if( fileCut->IsOpen() ) gate = "cut0 && " + gate;
   
   printf("%s \n", gate.Data());
   // name.Form("time vs X (detID-%d); x; coinTimeUC [ch]", detID);
   name.Form("time vs X (detID-%d); x; te_t [ch]", detID);
   hTX->SetTitle(name);
   cAna->cd(1);
   tree->Draw(expression, gate, "colz");
   int entries = hTX->Integral();
   printf("entries : %d \n", entries);
   
   /**
   if( entries < 100 ){
      printf("set bin = 30\n");
      hTX->SetBins(30,   -1.5, 1.5, 50, timeRange[0], timeRange[1] );
      hTXg->SetBins(30,  -1.5, 1.5, 50, timeRange[0], timeRange[1] );
      hTXc2->SetBins(30, -1.5, 1.5, 50, timeRange[0], timeRange[1] );
      hp->SetBins(30, -1.5, 1.5); 
      tree->Draw(expression, gate, "colz");
   }else if( 100 <= entries && entries < 500 ){
      printf("set bin = 100\n");
      hTX->SetBins(100,   -1.5, 1.5, 100, timeRange[0], timeRange[1] );
      hTXg->SetBins(100,  -1.5, 1.5, 100, timeRange[0], timeRange[1] );
      hTXc2->SetBins(100, -1.5, 1.5, 100, timeRange[0], timeRange[1] );
      hp->SetBins(100, -1.5, 1.5);
      tree->Draw(expression, gate, "colz");
   }else if ( 500 <= entries ){
      printf("set bin = 400\n");
      hTX->SetBins(400,   -1.5, 1.5, 400, timeRange[0], timeRange[1] );
      hTXg->SetBins(400,  -1.5, 1.5, 400, timeRange[0], timeRange[1] );
      hTXc2->SetBins(400, -1.5, 1.5, 400, timeRange[0], timeRange[1] );
      hp->SetBins(400, -1.5, 1.5);
      tree->Draw(expression, gate, "colz");
   }*/      
   cAna->Update();
   
   //==== create cut and draw
   printf("---------- create TCutG\n");
   cAna->WaitPrimitive("CUTG", "TCutG");
   cut = (TCutG*) gROOT->FindObjectAny("CUTG");
   cut->SetName("cutG");
   cut->SetVarX(Form("x[%d]",detID));
   // cut->SetVarY("coinTimeUC");
   cut->SetVarY(Form("te_t[%d]",detID));
   printf("Got the TCut.\n");
   
   if( isBranchDetIDExist ) {   
      gate.Form("cutG && detID==%d", detID);
   }else{
      gate.Form("cutG && Iteration$==%d", detID);
   }
   //if( fileCut->IsOpen() ) gate = "(cut0 || cut1 || cut2 || cut3) && " + gate;

   cAna->cd(2);
   // expression.Form("coinTimeUC:x>>hTXg");
   expression.Form("te_t:x>>hTXg");
   tree->Draw(expression, gate, "colz");
   cut->Draw("same");
   gSystem->ProcessEvents();
   
   //==== create profile and fit
   hTXg->ProfileX("hp");
   hp->Draw("same");
   
   TF1 * fit7 = new TF1("fit7", "pol7", -2, 2);
   hp->Fit("fit7", "");
   gSystem->ProcessEvents();
   
   ///printf("---------- double click for flatten.\n");
   ///cAna->WaitPrimitive();
   
   //==== create substract
   double q[8];
   q[0] = fit7->GetParameter(0);
   q[1] = fit7->GetParameter(1);
   q[2] = fit7->GetParameter(2);
   q[3] = fit7->GetParameter(3);
   q[4] = fit7->GetParameter(4);
   q[5] = fit7->GetParameter(5);
   q[6] = fit7->GetParameter(6);
   q[7] = fit7->GetParameter(7);
 
   if( isBranchDetIDExist ) {   
      gate.Form("detID==%d", detID);
   }else{
      gate.Form("Iteration$==%d", detID);
   } 
   
   ///***************
   cAna->cd(3);   
   expression.Form("te_t - %f - %f*x - %f*TMath::Power(x,2) - %f*TMath::Power(x,3)- %f*TMath::Power(x,4)- %f*TMath::Power(x,5)- %f*TMath::Power(x,6)- %f*TMath::Power(x,7) :x>>hTXc2", q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
   tree->Draw(expression, gate, "colz");
   gSystem->ProcessEvents();
   ///printf("---------- double click for 1D plot.\n");
   ///cAna->WaitPrimitive();
   
   //==== 1-D plot
   cAna->cd(4);
   expression.Form("te_t - %f - %f*x - %f*TMath::Power(x,2) - %f*TMath::Power(x,3)- %f*TMath::Power(x,4)- %f*TMath::Power(x,5)- %f*TMath::Power(x,6)- %f*TMath::Power(x,7)>>hT", q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
   tree->Draw(expression, gate, "colz");
   gSystem->ProcessEvents();
   
   nPeaks = spec->Search(hT);
   printf("find %d peaks\n", nPeaks);

   // ROOT 5
   //Float_t * xpos =  spec->GetPositionX();
   //Float_t * ypos =  spec->GetPositionY();

   Double_t * xpos =  spec->GetPositionX();
   Double_t * ypos =  spec->GetPositionY();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );
   vector<double> energy, height;
   for( int j = 0; j < nPeaks; j++){
      energy.push_back(xpos[inX[j]]);
      height.push_back(ypos[inX[j]]);
   }
   
   const int  n = 3 * nPeaks;
   double * para = new double[n]; 
   for(int i = 0; i < nPeaks ; i++){
      para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
      para[3*i+1] = energy[i];
      para[3*i+2] = 5;
   }
   
   TF1 * fit = new TF1("fit", nGauss, timeRange[0], timeRange[1], 3* nPeaks );
   fit->SetLineWidth(2);
   fit->SetLineColor(2);
   fit->SetNpx(1000);
   fit->SetParameters(para);
   hT->Fit("fit", "q");
   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
  
   printf("============= Fit Result: \n");  

   double bw = hT->GetBinWidth(1);

   double * ExPos = new double[nPeaks];
   double * ExSigma = new double[nPeaks];   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
      ExSigma[i] = paraA[3*i+2];
      printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
   }
   cAna->Update();

   
   //====================================================== save parameter
   printf("=========== saved parameters to %s \n", filename.Data());
   fprintf(paraOut, "%d\t", detID);
   for( int i = 0; i < 8; i++){
      fprintf(paraOut, "%11.6f\t", q[i]);
   }
   fprintf(paraOut, "%11.6f\n", ExPos[0]);

   
   fflush(paraOut);
   fclose(paraOut);
   
   ///printf("========= double click to exit.\n");
   ///cAna->WaitPrimitive();
   ///gROOT->ProcessLine(".q");
}
