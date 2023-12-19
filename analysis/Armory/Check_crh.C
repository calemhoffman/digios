#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <fstream>
#include <TCutG.h>

#include "../Armory/AnalysisLibrary.h"
using namespace std;

// int nPeakss = 16;
//TTree *tr = NULL;

bool doEZ=true;
bool doEx=true;
bool doRDT=false;
bool doEx2d=true;
bool doAngs=true;
bool doCoinT=false;
bool doCSV=false;

bool RDTCUT=true;
bool RINGCUT=false;
bool XCUT=true;
bool TIMECUT=true;
bool THETACUT=true;

// Double_t fpeaks(Double_t *x, Double_t *par) {
//    Double_t result = 0;
//    for (Int_t p=0;p<nPeakss;p++) {
//       Double_t norm  = par[3*p+0];
//       Double_t mean  = par[3*p+1];
//       Double_t sigma = par[3*p+2];
//       result += norm * TMath::Gaus(x[0],mean,sigma, 1);
//    }
//    return result;
// }

void Check_crh(TString rootfile){  
/**///======================================================== User input
   //const char* rootfile="A_gen_run107.root"; 
   const char* treeName="tree";
   const char* simfile="transfer.root"; const char* treeNameS="tree";

   int Div[2] = {5,6};  //x,y
   
   double ExRange[3] = {180, -1, 6};
	double eRange[3]  = {400, 0, 10};
   
   bool showFx = true;
   bool showTx = false;

   TString drawOption ="colz"; 
   //============================= extra calibrations
   // RDT
   double rdtCorr[8] = {1.0,1.4,1.0,1.22,1.0,1.2,1.0,1.2};//match DE to E
   double rdtOff[8] = {0.0,-115.,0.0,104.0,0.0,-36.,0.0,2.0};//offset to match
   Float_t rdtrCorr[8] = {0.72,0.1246,0.31,-0.0183,0.325,0.06642,0.578,0.05544};
   // Float_t rdtrCorr[8] = {0,0,0,0,0,0,0,0};

   //============================= Set Gates!


   TFile * fileCut = new TFile("rdtCuts.root");
   
   TObjArray * cutList = NULL;
   TCutG ** cut = NULL;
   
   TString gate_RDT = "";
   
   if( fileCut->IsOpen() ){
      
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut = new TCutG * [numCut];
      printf(" ======== found %d cuts in %s \n", numCut, fileCut->GetName());
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
      }
      if (RDTCUT){
         gate_RDT = "&& ((cut0) || (cut1) || (cut2) || (cut3))";
      }
   }
   
   TString detGate = "x>-10";
   if (RINGCUT) {
      detGate = detGate + "&& ring > -100 && ring < 100";
   }
   if (XCUT) {
      detGate = detGate + " && x>=-0.98 && x<=0.98";
   }
   TString timeGate = "";
   if (TIMECUT) {
      timeGate = " && coinTime > -18 && coinTime < 18";
   }

   TString thetaGate = "";
   if (THETACUT) {
      thetaGate = " && thetaCM > 15";
   }

/**///======================================================== read tree   
   printf("################### Check_crh.C ######################\n");
   
   printf("det Gate : %s \n", detGate.Data());
   
   printf("======================================== \n");
   
   
   TFile *file0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)file0->Get(treeName);
   tree->Print();
   printf("=====> /// %20s //// is loaded. Total #Entry: %10lld \n", rootfile.Data(),  tree->GetEntries());
   
   TFile *file1 = new TFile (simfile, "read"); 
   TTree *sTree = (TTree*)file1->Get(treeNameS);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10lld \n", simfile,  sTree->GetEntries());

   gStyle->SetOptStat(10001);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.4);
   gStyle->SetStatH(0.2);
   gStyle->SetLabelSize(0.05, "XY");
   gStyle->SetTitleFontSize(0.1);

//========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   int numDet;
   int rDet = 5; // number of detector at different position, row-Det
   int cDet = 6; // number of detector at same position, column-Det

   double xnCorr[30]; // xn correction for xn = xf
   double xfxneCorr[30][2]; //xf, xn correction for e = xf + xn
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   DetGeo detGeo;
   TMacro * haha = new TMacro();
   if( haha->ReadFile(detGeoFileName.c_str()) > 0 ) {
      detGeo = LoadDetectorGeo(haha);
      PrintDetGeo(detGeo);
      rDet = detGeo.nDet;
      cDet = detGeo.mDet;
      printf("... done.\n");
   }else{
      printf("... fail\n");
      return;
   }
   
   numDet = rDet * cDet;
   double zRange[3]= {400, detGeo.zMin-20, detGeo.zMax+20}; // nBin, min, max


/**///======================================================== Analysis
// #include <iostream>
// #include <fstream>
// using namespace std;

// int main() {
//   ofstream myfile;
//   myfile.open ("example.txt");
//   myfile << "Writing this to a file.\n";
//   myfile.close();
//   return 0;
// }
/**///======================================================== csv
   gStyle->SetOptStat("i");
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.35);
   gStyle->SetStatW(0.25);
   gStyle->SetStatH(0.10);
   gStyle->SetLabelSize(0.035, "XY");
   gStyle->SetTitleFontSize(0.035);


   ofstream myfile;
   myfile.open ("evz_data.csv");
   float e[30];
   float z[30];
   float x[30];
   float coinTime = 0;
   float thetaCM = 0;
   tree->SetBranchAddress("e", e);
   tree->SetBranchAddress("z", z);
   tree->SetBranchAddress("x", x);
   tree->SetBranchAddress("coinTime",&coinTime);
   tree->SetBranchAddress("thetaCM",&thetaCM);
   myfile << "z,e\n";
   int DETID = 14;
   if (doCSV) {
      for (int k=0;k<tree->GetEntries()-1;k++) {
         tree->GetEntry(k);
         float tempp = (0.7/50.*(280. + z[DETID]) + 5);//2.8);
         if ((e[DETID] > tempp) && e[DETID]<9 && coinTime>-18 && coinTime<18 && thetaCM > 15. && x[DETID]>=-0.98 && x[DETID]<=0.98) {
            // printf("%f %f\n",z[0],e[0]);
            myfile << z[DETID] << "," << e[DETID] << endl;
         }
      }
   }
   /**///======================================================== e vs z
   if (doEZ) {
      TCanvas * cCheck1 = new TCanvas("cCheck1", "cCheck1", 700, 50,  1800, 1400);
      cCheck1->ToggleEditor();cCheck1->ToggleToolBar();
      cCheck1->SetGrid(); cCheck1->Divide(3,2);
      TH2F * hEZ[6];
      for (int i=0;i<6;i++) {
         cCheck1->cd(i+1);
         TString detIDGate;
         detIDGate.Form("detID >= %d && detID < %d && ",i*5,i*5+5);
         hEZ[i] = new TH2F(Form("hEZ%d",i), Form("e:z [det %d - %d]; z [mm]; e [MeV]",i*5,i*5+5), zRange[0], zRange[1], zRange[2], eRange[0], eRange[1], eRange[2]);
         hEZ[i]->SetMarkerStyle(7);hEZ[i]->SetMarkerSize(5);
         tree->Draw(Form("e:z >> hEZ%d",i),  "hitID>=0 && " + detIDGate + detGate + gate_RDT + timeGate + thetaGate  , drawOption);
         hEZ[i]->Draw();
      
         if( showFx ) {
            TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
            int numFx = fxList->GetEntries();
            for( int i = 0; i < numFx ; i++){
               fxList->At(i)->Draw("same");
            }
         }
      }
      cCheck1->Update();
   }
   /**///======================================================== CoinT

   if (doCoinT) {
      TH2F *hCoinTx[30];
      TCanvas * cCheckCoinT = new TCanvas("cCheckCoinT", "cCheckCoinT", 100, 0,  1800, 1600);
      cCheckCoinT->ToggleEditor();cCheckCoinT->ToggleToolBar();
      cCheckCoinT->SetGrid(); cCheckCoinT->Divide(3,2);

      for (int i=0;i<30;i++) {
         // cCheckCoinT->cd(i+1);
         TString detIDGate;
         // for (int j=0;j<5;j++) {
            detIDGate.Form("detID==%d &&",i);
         // }
         hCoinTx[i] = new TH2F(Form("hCoinTx%d",i),Form("hCoinTx%d",i),100,-1,1,100,-50,50);
         tree->Draw(Form("coinTime:x>>hCoinTx%d",i),detIDGate+detGate,"");
      }

      for (int j=0;j<5;j++) {

      }
   }

      /**///======================================================== Ex2d
   if (doEx2d) {
      TCanvas * cCheckEx2d = new TCanvas("cCheckEx2d", "cCheckEx2d", 100, 0,  1800, 1600);
      cCheckEx2d->ToggleEditor();cCheckEx2d->ToggleToolBar();
      cCheckEx2d->SetGrid(); cCheckEx2d->Divide(3,2);
      TCanvas * cCheckEx2db = new TCanvas("cCheckEx2db", "cCheckEx2db", 0, 0,  1800, 1600);
      cCheckEx2db->ToggleEditor();cCheckEx2db->ToggleToolBar();
      cCheckEx2db->SetGrid(); cCheckEx2db->Divide(2,2);
      TH2F * hExCoinT;
      TH2F * hExRDT[4];
      TH2F * hExRDTg[4];
      TH2F * hExtRDT[4];
      TH2F * hExRDTde[4];
      TString exGate("&& Ex<6.6");
      cCheckEx2d->cd(1);
      hExCoinT = new TH2F("hExCoinT","hExCoinT; Ex; coinTime",100,-100,100,700,-1,13);
      tree->Draw("Ex:coinTime>>hExCoinT","hitID>=0 && " + detGate + gate_RDT, "colz");
      for (int i=0;i<4;i++) {
         cCheckEx2d->cd(i+3);
         // hExRDTde[i] = new TH2F(Form("hExRDTde%d",i), Form("ExRDTde [rdt %d]; TOTE[]; Ex [MeV]",i), 1000,0,2200,700,-1,13); //20 keV/ch
         // tree->Draw(Form("Ex:rdt[%d] >> hExRDTde%d",2*i+1,i), "hitID>=0 && " + detGate + timeGate+ thetaGate, drawOption);
         hExRDT[i] = new TH2F(Form("hExRDT%d",i), Form("ExRDT [rdt %d]; TOTE; Ex [MeV]",i), 1000,2500,5000,700,-1,13); //20 keV/ch
         tree->Draw(Form("Ex:rdte[%d] >> hExRDT%d",i,i), "hitID>=0 && " + detGate + timeGate+ thetaGate, drawOption);
         hExRDTg[i] = new TH2F(Form("hExRDTg%d",i), Form("ExRDTg [rdt %d]; TOTE; Ex [MeV]",i), 1000,2500,5000,700,-1,13); //20 keV/ch
         tree->Draw(Form("Ex:rdte[%d] >> hExRDTg%d",i,i), "hitID>=0 && " + detGate + gate_RDT + timeGate+ thetaGate , "same");
         hExRDTg[i]->SetMarkerStyle(7); hExRDTg[i]->Draw("same");
         cCheckEx2db->cd(i+1);
         hExtRDT[i] = new TH2F(Form("hExtRDT%d",i), Form("ExtRDT [rdt %d]; tRDT; Ex [MeV]",i), 1000,2500,5000,700,-1,13); //20 keV/ch
         tree->Draw(Form("Ex:trdte[%d]>> hExtRDT%d",i,i), "hitID>=0 && " + detGate + timeGate+ thetaGate, drawOption);

         // tree->Draw(Form("Ex:trdt_r[%d] >> hExRDTr%d",2*i+1,i), "hitID>=0 && " + detGate + timeGate+ thetaGate, drawOption);
      }
      cCheckEx2d->Update();      
      // cCheckEx2db->Update();

   }

   /**///======================================================== Ex
   if (doEx) {
      TCanvas * cCheck2 = new TCanvas("cCheck2", "cCheck2", 700, 50,  1400, 1200);
      cCheck2->ToggleEditor();cCheck2->ToggleToolBar();
      cCheck2->SetGrid(); cCheck2->Divide(5,6);
      TH1F * hEx[30];
      TH1F * hExSum;
      TH1F * hExSum2;
      TH2F * hExAng[30];
      TH2F * hExAngSum;
      hExSum = new TH1F(Form("hExSum"), Form("Ex [sum]; Ex [MeV]"), 700,-1,13); //20 keV/ch
      hExSum2 = new TH1F(Form("hExSum2"), Form("Ex2 [sum]; Ex [MeV]"), 700,-1,13); //20 keV/ch
      hExAngSum = new TH2F(Form("hExAngSum"), Form("ExAngSum [sum]; Ex [MeV] ; Ang [deta]"), 500,0,60,700,-1,13); //20 keV/ch
      for (int i=0;i<30;i++) {
         cCheck2->cd(i+1);
         TString detIDGate;
         detIDGate.Form("detID == %d &&",i);

         // hExAng[i] = new TH2F(Form("hExAng%d",i), Form("ExAng%d [sum]; Ex [MeV] ; Ang [deta]",i), 500,0,60,700,-1,13); //20 keV/ch
         // tree->Draw(Form("Ex*%f+%f:thetaCM >> hExAng%d",p[i][0],p[i][1],i),  "hitID>=0 && " + detGate + gate_RDT + timeGate+ thetaGate , "colz");
         // hExAngSum->Add(hExAng[i]);

         hEx[i] = new TH1F(Form("hEx%d",i), Form("Ex [det %d]; Ex [MeV]",i), 700,-1,13); //20 keV/ch
         tree->Draw(Form("Ex >> hEx%d",i),  "hitID>=0 && " + detIDGate + detGate + gate_RDT + timeGate+ thetaGate , drawOption);
         hExSum->Add(hEx[i]);
         }
      // draw / cut by x < 0.5 < x for angles / columns (SUM & IND)
      // cCheck2->Clear(); cCheck2->Divide(3,2);
      // for (int i=0;i<30;i++) {
      //    if (i<5) {cCheck->cd(1); hEx[i]-Draw("same")}
      // }
      TCanvas * cCheck2b = new TCanvas("cCheck2b", "cCheck2b", 700, 50,  1000, 800);
      cCheck2b->Clear();
      
      // // tree->Draw(Form("Ex >> hExSum"),  "hitID>=0 && " + detGate + timeGate , drawOption);
      hExSum->Draw();
      // tree->Draw(Form("Ex >> hExSum2"),  "hitID>=0 && " + detGate + gate_RDT + timeGate, "same");
      // cCheck2b->Update();

      // TCanvas * cCheck2c = new TCanvas("cCheck2c", "cCheck2c", 700, 50,  1000, 800);
      // cCheck2c->Clear();
      // tree->Draw("Ex:thetaCM >> hExAngSum", "hitID>=0 && " + detGate + gate_RDT + timeGate+ thetaGate ,"colz");

   /**///======================================================== Angular Distribution Stuff
   if (doAngs) {
   TCanvas * cAngs = new TCanvas("cAngs", "cAngs", 700, 50,  1800, 1600);
      cAngs->ToggleEditor();cAngs->ToggleToolBar();
      cAngs->SetGrid(); cAngs->Divide(3,2);

      TH1F *hCols[5];
      for (int i=0;i<5;i++) {
         hCols[i] = new TH1F(Form("hCols%d",i),Form("hCols%d;Ex [MeV]",i),700,-1,13);
         for (int j=0;j<6;j++){      hCols[i]->Add(hEx[j*5+i]); }
         cAngs->cd(i+1);hCols[i]->Rebin(); hCols[i]->Draw();
         TString fit_name("AutoFit_para0.txt");
         if (i==1) fitNGauss(hCols[i],100,fit_name);
      }

      cAngs->cd(6);tree->Draw("Ex:thetaCM >> hExAngSum", "hitID>=0 && " + detGate + gate_RDT + timeGate+ thetaGate ,"colz");
      hExAngSum->Draw("colz");
   }

   }
    /**///======================================================== RDT
   if (doRDT) {
      TCanvas * cCheck3 = new TCanvas("cCheck3", "cCheck3", 700, 50,  1800, 1600);
      cCheck3->ToggleToolBar();cCheck3->Divide(2,2);
      TCanvas * cCheck3b = new TCanvas("cCheck3b", "cCheck3b", 70, 50,  1800, 1600);
      cCheck3b->ToggleToolBar();cCheck3b->Divide(2,2);
      TH2F * hRDT[8];
      TH2F * hRDTg[8];
      TH2F * hRDTr[8];
      TH1F * hRDTe[4];
      TH1F * hRDTte[4];
      TString exGate("&& Ex<6.6");
      for (int i=0;i<4;i++) {
         cCheck3->cd(i+1);
         hRDTe[i] = new TH1F(Form("hRDTe%d",i), Form("RDTe %d; ETOT [ch]",i), 500,3500,4200);
         hRDTte[i] = new TH1F(Form("hRDTte%d",i), Form("RDTte %d; ETOT [ch]",i), 500,3500,4200);
         tree->Draw(Form("rdte[%d]>>hRDTe%d",i,i));
         TLine *line1 = new TLine(3900,0,4000,1e4); line1->Draw("same");
         TLine *line2 = new TLine(4000,0,4000,1e4); line2->Draw("same");
         tree->Draw(Form("trdte[%d]>>hRDTte%d",i,i),"","same");
      }
      for (int i=0;i<4;i++) {
         cCheck3->cd(i+1);
         hRDT[i] = new TH2F(Form("hRDT%d",i), Form("RDT  [de=%d, e=%d]; ETOT [MeV]; DE [MeV]",2*i+1,2*i), 500,1500,5500,1000,0,2200);
         tree->Draw(Form("rdt[%d]:rdte[%d] >> hRDT%d",2*i+1,i,i),  "hitID>=0", drawOption);
         hRDTg[i] = new TH2F(Form("hRDT%dg",2*i+1), Form("RDTg  [de=%d, e=%d]; ETOT [MeV]; DE [MeV]",2*i+1,2*i), 500,1500,5500,1000,0,2200);
         tree->Draw(Form("rdt[%d]:rdte[%d]>> hRDTg%d",2*i+1,i,i),  "hitID>=0 && " + detGate + timeGate + exGate, "same");
         hRDTg[i]->SetMarkerStyle(7);hRDTg[i]->Draw("same");
      }

      for (int i=0;i<4;i++) {
         cCheck3b->cd(i+1);
         hRDTr[i] = new TH2F(Form("hRDTr%d",i), Form("TRDT  [de=%d, e=%d]; ETOT [MeV]; DE [MeV]",2*i+1,2*i), 500,1500,5500,1000,0,2200);
         tree->Draw(Form("trdt[%d]:trdte[%d] >> hRDTr%d",2*i+1,i,i),  "hitID>=0", drawOption);
      }
      // draw / cut by x < 0.5 < x for angles / columns (SUM & IND)
      // cCheck3->Clear(); cCheck3->Divide(3,2);
      // for (int i=0;i<30;i++) {
      //    if (i<5) {cCheck->cd(1); hEx[i]-Draw("same")}
      // }
      cCheck3->Update();
   }

/**///======================================================== e vs x
   // Int_t size[2] = {230,230}; //x,y
   // TCanvas * cCheck1 = new TCanvas("cCheck1", "cCheck1", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   // if(cCheck1->GetShowEditor() )cCheck1->ToggleEditor();
   // if(cCheck1->GetShowToolBar() )cCheck1->ToggleToolBar();
   // cCheck1->Divide(Div[0],Div[1]);
   // for( int i = 1; i <= Div[0]*Div[1] ; i++){
   //    cCheck1->cd(i)->SetGrid();
   // }
   
   // TH2F ** hEX = new TH2F*[numDet];
   // TString expression, name, gate;
   // for( int idet = 0 ; idet < numDet ; idet ++){
   //    cCheck1->cd(idet+1);  
   //    name.Form("hEX%02d", idet);
   //    hEX[idet] = new TH2F(name, name, 400, -1.3, 1.3, eRange[0], eRange[1], eRange[2]);
   //    expression.Form("e:x>>hEX%02d", idet);
   //    gate.Form("detID == %d && hitID >= 0 && ring[%d]>-100 && ring[%d]<100", idet,idet,idet);
   //    tree->Draw(expression, gate, "colz");
   //    cCheck1->Update();
   // }
   
/**///======================================================== Ex and fit
   
   // TCanvas * cCheck3 = new TCanvas("cCheck3", "cCheck3", 50, 50,  600, 400);
   // if(cCheck3->GetShowEditor() )cCheck3->ToggleEditor();
   // if(cCheck3->GetShowToolBar() )cCheck3->ToggleToolBar();
   // cCheck3->Divide(1,2);
   // cCheck3->cd(1)->SetGrid();
   // cCheck3->cd(1);
   // gStyle->SetOptStat(0);
   // gStyle->SetLabelSize(0.05, "XY");
   // TString titleH;
   // titleH.Form("excitation energy; Ex [MeV]; Count / %3.0f keV", (ExRange[2] - ExRange[1])*1000/ExRange[0]);
   // TH1F * hEx = new TH1F("hEx", titleH, ExRange[0], ExRange[1], ExRange[2]);
   // hEx->GetXaxis()->SetTitleSize(0.06);
   // hEx->GetYaxis()->SetTitleSize(0.06);
   // hEx->GetXaxis()->SetTitleOffset(0.7);
   // hEx->GetYaxis()->SetTitleOffset(0.6);
   // tree->Draw("Ex >> hEx", detGate + gate_RDT); 
   
   
   // TH1F * specS = (TH1F*) hEx->Clone();
   // titleH.Form("fitted spectrum; Ex [MeV]; Count / %3.0f keV", (ExRange[2] - ExRange[1])*1000/ExRange[0]);
   // specS->SetTitle(titleH);   
   // specS->SetName("specS");
   // specS->GetXaxis()->SetTitleSize(0.06);
   // specS->GetYaxis()->SetTitleSize(0.06);
   // specS->GetXaxis()->SetTitleOffset(0.7);
   // specS->GetYaxis()->SetTitleOffset(0.6);
   // //=================== find peak and fit
   // printf("============= estimate background and find peak\n");
   // TSpectrum * peak = new TSpectrum(50);
   // nPeakss = peak->Search(hEx, 1, "", 0.1);
   // TH1 * h1 = peak->Background(hEx,10);
   // h1->Draw("same");
   
   // cCheck3->cd(2)->SetGrid();
   // cCheck3->cd(2);
   
   // specS->Add(h1, -1.);
   // specS->Sumw2();
   // specS->Draw();
   
   // //========== Fitting 
   // printf("============= Fit.....");
   // printf(" found %d peaks \n", nPeakss);
   
   // //float * xpos =  peak->GetPositionX();
   // //float * ypos =  peak->GetPositionY();
	// // in root-6, 
   // double * xpos = peak->GetPositionX();
   // double * ypos = peak->GetPositionY();
   
   // int * inX = new int[nPeakss];
   // TMath::Sort(nPeakss, xpos, inX, 0 );
   // vector<double> energy, height;
   // for( int j = 0; j < nPeakss; j++){
   //    energy.push_back(xpos[inX[j]]);
   //    height.push_back(ypos[inX[j]]);
   // }
   
   // const int  n = 3 * nPeakss;
   // double * para = new double[n]; 
   // for(int i = 0; i < nPeakss ; i++){
   //    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
   //    para[3*i+1] = energy[i];
   //    para[3*i+2] = 0.08;
   // }

   // TF1 * fit = new TF1("fit", fpeaks, ExRange[1], ExRange[2], 3* nPeakss );
   // fit->SetLineWidth(1);
   // fit->SetNpx(1000);
   // fit->SetParameters(para);
   // specS->Fit("fit", "q");

   // printf("============= display\n");   
   // const Double_t* paraE = fit->GetParErrors();
   // const Double_t* paraA = fit->GetParameters();
   
   // double bw = specS->GetBinWidth(1);
   
   // double * ExPos = new double[nPeakss];
   // double * ExSigma = new double[nPeakss];   
   // for(int i = 0; i < nPeakss ; i++){
   //    ExPos[i] = paraA[3*i+1];
   //    ExSigma[i] = paraA[3*i+2];
   //    printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
   //            i, 
   //            paraA[3*i] / bw,   paraE[3*i] /bw, 
   //            paraA[3*i+1], paraE[3*i+1],
   //            paraA[3*i+2], paraE[3*i+2]);
   // }
   // cCheck3->Update();


/**///======================================================== thetaCM vs Ex
   // gStyle->SetStatY(0.9);
   // gStyle->SetStatX(0.9);
   // TCanvas * cCheck4 = new TCanvas("cCheck4", "cCheck4", 700, 500,  600, 400);
   // if(cCheck4->GetShowEditor() )cCheck4->ToggleEditor();
   // if(cCheck4->GetShowToolBar() )cCheck4->ToggleToolBar();
   // cCheck4->SetGrid();
   // TH2F * hExTheta = new TH2F("hExTheta", "Ex:thetaCM:z; thetaCM [deg]; Ex [MeV]", 400, 0, 45, ExRange[0], ExRange[1], ExRange[2]);
   // tree->Draw("Ex:thetaCM >> hExTheta", detGate + gate_RDT,  drawOption); 
   
   // if( showTx ) {
   //    TObjArray * txList = (TObjArray*)  file1->FindObjectAny("txList");
   //    int numFx = txList->GetEntries();
   //    for( int i = 0; i < numFx ; i++){
   //       txList->At(i)->Draw("same");
   //    }
   // }
   // cCheck4->Update();

/**///======================================================== Ex vs z 
   // gStyle->SetStatY(0.35);
   // gStyle->SetStatX(0.9);
   // gStyle->SetStatW(0.25);
   // gStyle->SetStatH(0.10);
   // TCanvas * cCheck5 = new TCanvas("cCheck5", "cCheck5", 50, 500,  600, 400);
   // if(cCheck5->GetShowEditor() )cCheck5->ToggleEditor();
   // if(cCheck5->GetShowToolBar() )cCheck5->ToggleToolBar();
   // cCheck5->SetGrid();
   // TH2F * hExZ = new TH2F("hExZ", "z:Ex; Ex [MeV]; z [mm]", ExRange[0], ExRange[1], ExRange[2], zRange[0], zRange[1], zRange[2]);
   // tree->Draw("z : Ex >> hExZ", detGate + gate_RDT, drawOption); 
   // cCheck5->Update();


/**///======================================================== thetaCM   
   // TCanvas * cCheck6 = new TCanvas("cCheck6", "cCheck6", 1300, 50,  600, 400);
   // if(cCheck6->GetShowEditor() )cCheck6->ToggleEditor();
   // if(cCheck6->GetShowToolBar() )cCheck6->ToggleToolBar();
   // cCheck6->SetGrid();
   // gStyle->SetOptStat(0);
   
   // TH1F ** hTheta = new TH1F *[nPeakss];
   // double yMax = 0;
   
   // TLegend * legend = new TLegend(0.6,0.1,0.9,0.35);
   // legend->SetName("legend");
   
   // for(int i = 0; i < nPeakss; i++){
   //    TString name, title, gate, expression;
   //    name.Form("hTheta%d", i);
   //    title.Form("thetaCM +-3Sigma; thetCM * sin() [deg]; count / 0.5 deg");
   //    hTheta[i] = new TH1F(name, title, 100, 0, 45);
   //    hTheta[i]->SetLineColor(2*i+2);
   //    expression.Form("thetaCM >> hTheta%d", i);
   //    gate.Form("%f > Ex && Ex > %f", ExPos[i] + 3*ExSigma[i], ExPos[i] - 3*ExSigma[i] );
   //    if( i == 0 )tree->Draw(expression, gate + "&&" + detGate + gate_RDT);
   //    if( i > 0 )tree->Draw(expression, gate + "&&" + detGate + gate_RDT, "same");
      
   //    TString lTitle;
   //    lTitle.Form("Ex = %6.2f MeV, %3.0f keV", ExPos[i], ExSigma[i]*1000.);
   //    legend->AddEntry(name,lTitle, "l");
   
   //    if( hTheta[i]->GetMaximum() > yMax ) yMax = hTheta[i]->GetMaximum();
   //    hTheta[0]->GetYaxis()->SetRangeUser(1, yMax*1.5);
      
   //    //cCheck6->SetLogy();
   //    cCheck6->Update();
   // }
   
   // legend->Draw();
   // cCheck6->Update();
   
   //========================== save histograms and fit-result
   // TFile * saveFile = new TFile("checkResult.root", "recreate");
   
   // TObjArray * hEXList = new TObjArray();
   // for(int i = 0 ; i < numDet; i++){
	// 	hEXList->Add(hEX[i]);
	// }
	// hEXList->Write("hEXList", TObject::kSingleKey);
	
	// hEx->Write();
	// h1->Write();
	// specS->Write();
	// fit->Write();
	
	// hEZ->Write();
	// hExZ->Write();
	// hExTheta->Write();
	
	// TObjArray * hThetaList = new TObjArray();
   // for(int i = 0 ; i < nPeakss; i++){
	// 	hThetaList->Add(hTheta[i]);
	// }
	// hThetaList->Write("hThetaList", TObject::kSingleKey);
	// legend->Write();
	
	// saveFile->Write();
   
   // printf("=========> saved histograms in checkResult.root \n");
}
