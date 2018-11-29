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

//######################################################
//   To get the coincident Time correction parameter
//   require processed by Cali_e_trace.C
//######################################################

void GetCoinTimeCorrectionCutG(TString fileName, int detID){

   int numGauss = 1;
   if( detID%6 == 4) numGauss = 2;
   if( detID%6 == 5) numGauss = 3;

   //====================================================== read file, canvas, histogram
   TFile * f1 = new TFile(fileName, "READ");
   TTree * tree = (TTree*) f1->Get("tree");
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);

   TBranch * br = (TBranch *) tree->GetListOfBranches()->FindObject("coinTimeUC");
   if( br == NULL ){
      printf(" Branch <coinTimeUC> not exist!!!!! exit. \n");
      return;
   }


   int Div[2] = {1,1};
   int size[2] = {600,600}; //x,y
   TCanvas * cAna = new TCanvas("cAna", "cAna", 500, 0, size[0]*Div[0], size[1]*Div[1]);
   if( cAna->GetShowEditor() )  cAna->ToggleEditor();
   if( cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   cAna->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAna->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(0);
   if(cAna->GetShowEditor() )   cAna->ToggleEditor();
   if(!cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   
   TString name, expression, gate;

   TH2F * hTX   = new TH2F("hTX",   "time vs X; x; coinTime [ch]", 300, -1.5, 1.5, 300, -30, 40);
   TH2F * hTXg  = new TH2F("hTXg",  "time vs X; x; coinTime [ch]", 300, -1.5, 1.5, 300, -30, 40);
   TH2F * hTXc2 = new TH2F("hTXc2", "time vs X; x; coinTime [ch]", 300, -1.5, 1.5, 300, -30, 40);
   TH1F * hT = new TH1F("hT", "", 100, -5, 10);
   TProfile * hp = new TProfile("hp", "time Profile", 400, -1.5,1.5);
   TSpectrum * spec = new TSpectrum(5);
   
   TCutG * cut = NULL;
   
   bool isBranchDetIDExist = false;
   br = (TBranch *) tree->GetListOfBranches()->FindObject("detID");
   if( br == NULL ){
      printf(" detected as sorted.root file. \n");
   }else{
      printf(" detected as little.root file. \n");
      isBranchDetIDExist = true;
   }
   
   //====================================================== open savefile
   FILE * paraOut;
   TString filename;
   filename.Form("correction_coinTime.dat");
   paraOut = fopen (filename.Data(), "a+");
   
   //====================================================== processing      
   printf("============ detID: %d\n", detID);

   expression.Form("coinTimeUC:x>>hTX");
   if( isBranchDetIDExist ) {   
      gate.Form("detID==%d", detID);
   }else{
      gate.Form("Iteration$==%d", detID);
   }
   name.Form("time vs X (detID-%d); x; coinTime [ch]", detID);
   hTX->SetTitle(name);
   tree->Draw(expression, gate, "colz");
   int entries = hTX->Integral();
   printf("entries : %d \n", entries);
   if( entries < 100 ){
      printf("set bin = 30\n");
      hTX->SetBins(30, -1.5, 1.5, 50, -10, 40);
      hTXg->SetBins(30, -1.5, 1.5, 50, -10, 40);
      hTXc2->SetBins(30, -1.5, 1.5, 50, -10, 40);
      hp->SetBins(30, -1.5, 1.5); 
      tree->Draw(expression, gate, "colz");
   }else if( 100 <= entries && entries < 500 ){
      printf("set bin = 100\n");
      hTX->SetBins(100, -1.5, 1.5, 100, -10, 40);
      hTXg->SetBins(100, -1.5, 1.5, 100, -10, 40);
      hTXc2->SetBins(100, -1.5, 1.5, 100, -10, 40);
      hp->SetBins(100, -1.5, 1.5);
      tree->Draw(expression, gate, "colz");
   }else if ( 500 <= entries ){
      printf("set bin = 400\n");
      hTX->SetBins(400, -1.5, 1.5, 400, -10, 40);
      hTXg->SetBins(400, -1.5, 1.5, 400, -10, 40);
      hTXc2->SetBins(400, -1.5, 1.5, 400, -10, 40);
      hp->SetBins(400, -1.5, 1.5);
      tree->Draw(expression, gate, "colz");
   }      
   cAna->Update();
   
   //==== create cut and draw
   printf("---------- create TCutG\n");
   cAna->WaitPrimitive("CUTG", "TCutG");
   cut = (TCutG*) gROOT->FindObjectAny("CUTG");
   cut->SetName("cut1");
   cut->SetVarX("x");
   cut->SetVarY("coinTimeUC");
   if( isBranchDetIDExist ) {   
      gate.Form("cut1 && detID==%d", detID);
   }else{
      gate.Form("cut1 && Iteration$==%d", detID);
   }
   
   expression.Form("coinTimeUC:x>>hTXg");   
   tree->Draw(expression, gate, "colz");
   cut->Draw("same");
   
   //==== create profile and fit
   hTXg->ProfileX("hp");
   hp->Draw("same");
   
   TF1 * fit7 = new TF1("fit7", "pol7", -2, 2);
   hp->Fit("fit7", "");
   
   printf("---------- double click for flatten.\n");
   cAna->WaitPrimitive();
   
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
   expression.Form("coinTimeUC - %f - %f*x - %f*TMath::Power(x,2) - %f*TMath::Power(x,3)- %f*TMath::Power(x,4)- %f*TMath::Power(x,5)- %f*TMath::Power(x,6)- %f*TMath::Power(x,7) :x>>hTXc2", q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
   tree->Draw(expression, gate, "colz");
   
   printf("---------- double click for 1D plot.\n");
   cAna->WaitPrimitive();
   
   //==== 1-D plot
   expression.Form("coinTimeUC - %f - %f*x - %f*TMath::Power(x,2) - %f*TMath::Power(x,3)- %f*TMath::Power(x,4)- %f*TMath::Power(x,5)- %f*TMath::Power(x,6)- %f*TMath::Power(x,7) >>hT", q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
   tree->Draw(expression, gate, "colz");
   
   int nPeak = spec->Search(hT);
   printf("find %d peaks\n", nPeak);
   
   //Float_t * xpos =  spec->GetPositionX();
   //Float_t * ypos =  spec->GetPositionY();

   Double_t * xpos =  spec->GetPositionX();
   Double_t * ypos =  spec->GetPositionY();
   
   TF1* fit2g = NULL;
   
   if( numGauss == 1) fit2g = new TF1("fit2g", "gaus(0)");      
   if( numGauss == 2) fit2g = new TF1("fit2g", "gaus(0) + gaus(3)");      
   if( numGauss == 3) fit2g = new TF1("fit2g", "gaus(0) + gaus(3) + gaus(6)");
   
   fit2g->SetParameter(0, ypos[0]);
   fit2g->SetParameter(1, xpos[0]);
   fit2g->SetParameter(2, 0.5);
   if( numGauss >= 2){
      fit2g->SetParameter(3, ypos[1]);
      fit2g->SetParameter(4, xpos[1]);
      fit2g->SetParameter(5, 0.5);
   }
   if( numGauss >= 3){
      fit2g->SetParameter(6, ypos[2]);
      fit2g->SetParameter(7, xpos[2]);
      fit2g->SetParameter(8, 0.5);
   }
   
   hT->Fit("fit2g", "q");
   
   Double_t time[numGauss], eTime[numGauss];
   Double_t resol[numGauss], eResol[numGauss];
   
   printf("================================= in nano-sec\n");
   
   for(int i = 0; i < numGauss; i ++){
      time[i]   = fit2g->GetParameter(3*i+1) * 10.;
      eTime[i]  = fit2g->GetParError(3*i+1) * 10.;
      resol[i]  = fit2g->GetParameter(3*i+2) * 10.;
      eResol[i] = fit2g->GetParError(3*i+2) * 10.;
      
      printf("Peak: %6.2f(%4.2f), Sigma: %6.4f(%6.4f)\n", time[i], eTime[i], resol[i], eResol[i]);   
   }
   
   printf("==== time difference: %6.2f(%5.2f) ns \n", 
                     (time[1] - time[0]), 
                     TMath::Sqrt( eTime[0]*eTime[0] + eTime[1]*eTime[1]));
   
   //cAna->WaitPrimitive();

   //====================================================== save parameter
   printf("=========== save parameters to %s \n", filename.Data());
   fprintf(paraOut, "%d\t", detID);
   for( int i = 0; i < 8; i++){
      fprintf(paraOut, "%11.6f\t", q[i]);
   }
   fprintf(paraOut, "%11.6f\n", time[0]/10.);

   
   fflush(paraOut);
   fclose(paraOut);
   
   printf("========= double click to exit.\n");
   cAna->WaitPrimitive();
   
   if( !isBranchDetIDExist ) gROOT->ProcessLine(".q");
}
