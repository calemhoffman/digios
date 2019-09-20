#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"
#include "TLegend.h"


int nPeaks = 16;
Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1);
   }
   return result;
}

void script_Ex(){
   
   double ExRange[3] = {350, -1, 6};
	double eRange[3]  = {400, 0, 10};
	double thetaCMRange[3]  = {40, 0, 40};

   
   TFile * file0 = new TFile("A_gen_run60_99.root");
   TFile * file1 = new TFile("transfer.root");
   
   TFile * fileCut = new TFile("rdtCuts.root");

   TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   
   TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray*) file1->FindObjectAny("gList");
   
   TObjArray * cutList = NULL;
   TCutG ** cut = NULL;
   if( fileCut->IsOpen() ){
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut = new TCutG * [numCut];
      
      printf(" ======== found %d cuts in %s \n", numCut, fileCut->GetName());
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
      }
   }
   
   //============= Gate
   //TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && -20 < coin_t && coin_t < 40 && rdtMultiHit == 2 && arrayRDT == 0 && multiHit == 1 ";
   
   TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && -20 < coin_t && coin_t < 40";
   
   //TString gate_RDT = "cut3 && -20 < coin_t && coin_t < 40 && rdtMultiHit == 2 && arrayRDT == 0 && multiHit == 1 ";
   
   TString detGate = "&& detID != 11";
   
   
   //============== Plot
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, 1600, 900);
   cScript->Divide(2,3);
   for( int i = 1; i <= 4 ; i++){
      cScript->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat("neiou");
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   cScript->cd(1);
   tree0->Draw("rdtID:detID>>w(24, 0, 24, 8, 0, 8)", gate_RDT, "colz");
   
   cScript->cd(3);
   tree0->Draw("e:z >> h(400, -550,  -100, 400, 0, 10)", gate_RDT , "colz");
   
   gList->At(0)->Draw("same");
   //gList->At(15)->Draw("same");
   //gList->At(20)->Draw("same");
   
   fxList->At(0)->Draw("same"); // 0.000
   //fxList->At(1)->Draw("same"); // 0.591
   //fxList->At(2)->Draw("same"); // 1.095
   //fxList->At(3)->Draw("same"); // 1.431
   //fxList->At(4)->Draw("same"); // 1.638
   //fxList->At(5)->Draw("same"); // 2.500
   //fxList->At(6)->Draw("same"); // 2.615
   //fxList->At(7)->Draw("same"); // 3.227
   //fxList->At(8)->Draw("same"); // 4.280
   //fxList->At(9)->Draw("same"); // 6.00
   //fxList->At(10)->Draw("same"); // 8.00
   

   cScript->cd(2);
   gStyle->SetLabelSize(0.05, "XY");
   TString titleH;
   titleH.Form("excitation energy; Ex [MeV]; Count / %3.0f keV", (ExRange[2] - ExRange[1])*1000/ExRange[0]);
   TH1F * hEx = new TH1F("hEx", titleH, ExRange[0], ExRange[1], ExRange[2]);
   hEx->GetXaxis()->SetTitleSize(0.06);
   hEx->GetYaxis()->SetTitleSize(0.06);
   hEx->GetXaxis()->SetTitleOffset(0.7);
   hEx->GetYaxis()->SetTitleOffset(0.6);
   tree0->Draw("Ex >> hEx", gate_RDT + detGate); 
   
   
   TH1F * specS = (TH1F*) hEx->Clone();
   titleH.Form("fitted spectrum; Ex [MeV]; Count / %3.0f keV", (ExRange[2] - ExRange[1])*1000/ExRange[0]);
   specS->SetTitle(titleH);   
   specS->SetName("specS");
   specS->GetXaxis()->SetTitleSize(0.06);
   specS->GetYaxis()->SetTitleSize(0.06);
   specS->GetXaxis()->SetTitleOffset(0.7);
   specS->GetYaxis()->SetTitleOffset(0.6);
   
   //=================== find peak and fit
   printf("============= estimate background and find peak\n");
   TSpectrum * peak = new TSpectrum(50);
   nPeaks = peak->Search(hEx, 1, "", 0.1);
   TH1 * h1 = peak->Background(hEx,40);
   h1->Draw("same");
   
   
   cScript->cd(4)->SetGrid();
   cScript->cd(4);
   
   specS->Add(h1, -1.);
   specS->Sumw2();
   specS->Draw();
   
   //========== Fitting 
   printf("============= Fit.....");
   printf(" found %d peaks \n", nPeaks);
   
   //float * xpos =  peak->GetPositionX();
   //float * ypos =  peak->GetPositionY();
	// in root-6, 
   double * xpos = peak->GetPositionX();
   double * ypos = peak->GetPositionY();
   
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
      para[3*i+2] = 0.08;
   }

   TF1 * fit = new TF1("fit", fpeaks, ExRange[1], ExRange[2], 3* nPeaks );
   fit->SetLineWidth(1);
   fit->SetNpx(1000);
   fit->SetParameters(para);
   specS->Fit("fit", "q");

   printf("============= display\n");   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double bw = specS->GetBinWidth(1);
   
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
   cScript->Update();
   
   //================================ thetaCM distribution
   cScript->cd(5);
   gStyle->SetOptStat(0);
   
   TH1F ** hTheta = new TH1F *[nPeaks];
   double yMax = 0;
   
   TLegend * legend = new TLegend(0.1,0.9,0.5,1-0.35);
   legend->SetName("legend");
   
   //for(int i = 0; i < nPeaks; i++){
   for(int i = 0; i < 3; i++){
      TString name, title, gate, expression;
      name.Form("hTheta%d", i);
      title.Form("thetaCM +-3Sigma; thetCM [deg]; count / %3.1f deg", (thetaCMRange[2]-thetaCMRange[1])/thetaCMRange[0]);
      hTheta[i] = new TH1F(name, title, thetaCMRange[0], thetaCMRange[1], thetaCMRange[2]);
      hTheta[i]->SetLineColor(2*i+2);
      expression.Form("thetaCM >> hTheta%d", i);
      gate.Form("%f > Ex && Ex > %f", ExPos[i] + 3*ExSigma[i], ExPos[i] - 3*ExSigma[i] );
      if( i == 0 )tree0->Draw(expression, gate_RDT + detGate + "&&" + gate);
      if( i > 0 )tree0->Draw(expression, gate_RDT + detGate + "&&" + gate, "same");
      
      TString lTitle;
      lTitle.Form("Ex = %6.2f MeV, %3.0f keV", ExPos[i], ExSigma[i]*1000.);
      legend->AddEntry(name,lTitle, "l");
   
      if( hTheta[i]->GetMaximum() > yMax ) yMax = hTheta[i]->GetMaximum();
      hTheta[0]->GetYaxis()->SetRangeUser(1, yMax*1.5);
      
      cScript->SetLogy();
      cScript->Update();
   }
   
   legend->Draw();
   
   
   
}
