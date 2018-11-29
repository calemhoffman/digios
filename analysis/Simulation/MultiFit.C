#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TProfile.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <fstream>

int nPeaks = 16;

TString gate, gateB, gate_cm;

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

void MultiFit(){   

/**///======================================================== initial input
   
   const char* rootfile="transfer.root"; const char* treeName="tree";
   
   double xrange[2] = {-450, -100};
   double exrange[3] = {100, -1, 4}; // number of bin, eMin, eMax
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {3,2};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
   TCanvas * cMultiFit = new TCanvas("cMultiFit", "cMultiFit", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cMultiFit->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cMultiFit->cd(i)->SetGrid();
   }
   cMultiFit->cd(1);

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= Analysis

   //============ found the loop of e:z plot
   cMultiFit->cd(1);
   TH2F * ez = new TH2F("ez", "ez", 400, xrange[0], xrange[1], 400, 0, 20);
   tree->Draw("e:z >> ez", "hit == 1 && loop == 1 && ExID == 0 && ExAID == 0 && detID>0");
   
   TProfile * ezpx = new TProfile("ezpx", "ezpx", 400, xrange[0], xrange[1], 0, 20);
   ez->ProfileX("ezpx")->Draw("same");
   
   TF1 * fitE = new TF1("fitE", "pol1");
   
   ezpx->Fit("fitE");
   
   double a0 = fitE->GetParameter(0);
   double a1 = fitE->GetParameter(1);
   
   printf("a0 : %f, a1 : %f \n", a0, a1);
   
   //######################################
   // better to fix this number
   //a0 = -14.815453;
   //a1 = 0.038235;
      
   //=========== adjust energy
   cMultiFit->cd(2);
   TH2F * Ez = new TH2F("Ez", "Ez", 400, xrange[0], xrange[1], 400, exrange[1], exrange[2]);
   
   TString expression;
   expression.Form("%f * z + %f - e : z >> Ez", a1, a0);
   
   tree->Draw(expression, "hit == 1 && loop==1", "colz");


   //=========== energy
   cMultiFit->cd(3);
   
   TH1F * spec = new TH1F("spec", "energy", exrange[0], exrange[1], exrange[2]);
   
   expression.Form("%f * z + %f - e >> spec", a1, a0);
   
   tree->Draw(expression, "hit == 1 && loop==1 && detID == 5 && ExAID == 0", "colz");

   
   //=========== find peaks
   TSpectrum * specPeak = new TSpectrum(20);
   int nPeaks = specPeak->Search(spec, 1 ,"", 0.05);
   //float * xpos = specPeak->GetPositionX();
   double * xpos = specPeak->GetPositionX();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );  
   vector<double> energy;   
   printf("===== excitation energy found : %d \n", nPeaks);
   for( int j = 0; j < nPeaks; j++){
      printf(" %d , x: %8.3f \n", j, xpos[inX[j]]);
      energy.push_back(xpos[inX[j]]);
   }
   
   //---- load Ex energy
   vector<double> ExKnown;
   printf("----- loading excitation energy levels.");
   ifstream file;
   file.open("excitation_energies.txt");
   string isotopeName;
   if( file.is_open() ){
      string line;
      int i = 0;
      while( file >> line){
         //printf("%d, %s \n", i,  line.c_str());
         if( line.substr(0,2) == "//" ) continue;
         if( i == 0 ) isotopeName = line; 
         if ( i >= 1 ){
            ExKnown.push_back(atof(line.c_str()));
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      printf("========== %s\n", isotopeName.c_str());
      int n = ExKnown.size();
      for(int i = 0; i < n ; i++){
         printf("%d, %6.2f MeV \n", i, ExKnown[i]);
      }
   }else{
       printf("... fail\n");
   }
   
   // convert to real energy 
   cMultiFit->cd(4);
   int numPeak = ExKnown.size();
   if( energy.size() < ExKnown.size()){
      numPeak = energy.size();
   }else{
      numPeak = ExKnown.size();
   }
   TGraph * ga = new TGraph(numPeak, &energy[0], &ExKnown[0] );
   ga->Draw("*ap");
   
   TF1 * fit2 = new TF1("fit2", "pol1");
   
   ga->Fit("fit2", "");
   double eC0 = fit2->GetParameter(0);
   double eC1 = fit2->GetParameter(1);
   printf("====  eC0:%8.3f, eC1:%8.3f \n", eC0, eC1);
   
   vector<double> realEnergy;
   for( int j = 0; j < nPeaks; j++){
      realEnergy.push_back(energy[j] * eC1 + eC0);
      printf(" %d , e: %8.3f \n", j, realEnergy[j]);
   }
   
   //######################################
   // better to fix this number
   //eC0 = 0.0363021;
   //eC1 = 1.27094;
   
   
   //======= spectrum
   cMultiFit->cd(5);
   TH1F * spec2 = new TH1F("spec2", "spec2", exrange[0], exrange[1], exrange[2]);

   expression.Form("(%f * z + %f - e) * %f + %f >> spec2", a1, a0, eC1, eC0);
   tree->Draw(expression, "loop == 1 && detID == 5");
   
   
   TF1 * fit3 = new TF1("fit3", "gaus");
   
   spec2->Fit("fit3", "", "", -0.5,0.5 );
   
   printf("===========\n");
   printf("resolution : %f MeV \n", fit3->GetParameter(2));
   
   
   cMultiFit->cd(6);
   
   //tree->Draw("(0.03815* z - 14.76 - e)*1.3164 +0.0056 >> spec2(400, 4, 5.5)", "loop==1 && detID == 5", "colz");

   //spec2->Fit("gaus");
/*
   //========== Fitting 
   TSpectrum * peak = new TSpectrum(20);
   double threshold = 0.1;
   nPeaks  = peak->Search(spec2, 1, "", threshold);
   printf("======== found %d peaks \n", nPeaks);
   float * xpos = peak->GetPositionX();
   float * ypos = peak->GetPositionY();
   
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
      para[3*i+2] = 0.05;
   }
   
   TF1 * fit = new TF1("fit", fpeaks, -1 , 10, 3* nPeaks );
   fit->SetNpx(1000);
   fit->SetParameters(para);
   spec2->Fit("fit", "q");
   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double bw = spec2->GetBinWidth(1);
   
   double * ExPos = new double[nPeaks];
   
   //for(int i = 0; i < nPeaks ; i++){
   //   ExPos[i] = paraA[3*i+1];
   //}
   
   double meanSigma = 0;
   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
      printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
              i, 
              paraA[3*i] / bw,   paraE[3*i] /bw, 
              paraA[3*i+1], paraE[3*i+1],
              paraA[3*i+2], paraE[3*i+2]);
              
     meanSigma += paraA[3*i+2];
   }
   
   printf("======= mean Sigma : %f \n", meanSigma/nPeaks);
   
*/
   
}

