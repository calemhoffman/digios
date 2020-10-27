#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"
#include "TLatex.h"

double runTime [40][2] = {
  //duration, startTime, in day
{0.0375,	0.9659722}, //run25,  0
{0.0778,	1.0069444}, //run26,  1
{0.1255,	1.0888542}, //run27,  2
{0.0815,	1.2178704}, //run28,  3
{0.0576,	1.3021759}, //run29,  4
{0.0050,	1.3717477}, //run30,  5
{0.1212,	1.3787500}, //run31,  6
{0.0580,	1.5013426}, //run32,  7
{0.1230,	1.5835532}, //run33,  8
{0.1315,	1.7088426}, //run34,  9
{0.1201,	1.8418750}, //run35, 10
{0.1168,	1.9644444}, //run36, 
{0.1255,	2.0912153}, //run37,
{0.1252,	2.2180324}, //run38,
{0.0545,	2.3446412}, //run39,
{0.0693,	2.4154630}, //run40, 15
{0.1040,	2.4933912}, //run41,
{0.1752,	2.6011458}, //run42,
{0.1255,	2.7920718}, //run43,
{0.1147,	2.9273727}, //run44,
{0.0824,	3.0434722}, //run45, 20
{0.0202,	3.1264468}, //run46,
{0.1311,	3.2014468}, //run47,
{0.1056,	3.3332639}, //run48,
{0.0181,	3.4628472}, //run49,
{0.1248,	3.4823148}, //run50,
{0.1267,	3.6155787}, //run51,
{0.1314,	3.7435069}, //run52,
{0.1370,	3.8843171}, //run53,
{0.1301,	4.0231019}, //run54,
{0.1259,	4.1545139}, //run55, 30
{0.1242,	4.2816782}, //run56
{0.1213,	4.4128588}, //run57
{0.0222,	4.5356713}, //run58
{0.1297,	4.5592940}, //run59
{0.1408,	4.6904514}, //run60
{0.1281,	4.8321065}, //run61
{0.1208,	4.9770370}, //run62
{0.1257,	5.0992245}, //run63
  {0.1326,	5.2259954}  //run64
};

void script_FCUP_min(int runID = 41){

   
   TFile * file0 = new TFile("A_gen_run025.root");
   TFile * file1 = new TFile("FCUP_user2.dat.root");
   TFile * file2 = new TFile("FCUP_beamline.dat.root");
 
   TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   TTree * tree2 = (TTree*) file2->FindObjectAny("tree");

   //========================== Gate   
   TString gate_D = Form("1045 < elum && elum < 1245 && run==%d ", runID);
   TString gate_BG = Form("800 < elum && elum < 1000 && run==%d ", runID);
   
   double fScale = 1.0;

   if( runID == 0) {
     gate_D = "1045 < elum && elum < 1245";
     gate_BG = "800 < elum && elum < 1000";
     fScale = 1.;
   }

   //========================== Canvas
   TCanvas * cScript = new TCanvas("cScript", "cScript", 50, 100, 1800, 900);
   if( runID == 0 ){
    cScript->Divide(1,4);
    for( int i = 1; i <= 4 ; i++){
      cScript->cd(i)->SetGrid();
    }
   }else{
    cScript->Divide(1,3);
    for( int i = 1; i <= 3 ; i++){
      cScript->cd(i)->SetGrid();
    }
   }
   
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat(11111111);
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   //========================== Plot
   double tPadding = 0.01;
   double tRange[2];
   int tBin;

   if( runID == 0 ){
      tRange[0] = 0.9*24*60;
      tRange[1] = 5.3*24*60;
      tBin = (tRange[1] - tRange[0]);
      printf("All RUNs | startTime : %f [min], endTime : %f [min]\n", tRange[0], tRange[1]);
   }else{
      tRange[0] = runTime[runID - 25][1] - tPadding; // in day
      tRange[1] = runTime[runID - 25][1] + runTime[runID - 25][0] + tPadding; // in day
      tRange[0] = tRange[0] * 24 * 60 ; // in Min 
      tRange[1] = tRange[1] * 24 * 60 ; // in Min
      printf("run-%d | startTime : %f [min], endTime : %f [min]\n", runID, tRange[0], tRange[1]);
   }

   TH2F * hRUN = new TH2F("hRun", "RunID; time [min]; run-ID", tBin, tRange[0], tRange[1], 60, 20, 70);
   TProfile * hRUN_pfx = new TProfile("hRUN_pfx", "RUNID; time [min]; FCUP mean", tBin, tRange[0], tRange[1]);
   
   TH1F * h1 = new TH1F("h1", Form("ELUM Deuteron - RUN:%d ; time [min]; count / min", runID ), tBin, tRange[0], tRange[1]);
   TH1F * hBG = new TH1F("hBG", Form("ELUM BG - RUN:%d ; time [min]; count / min", runID ), tBin, tRange[0], tRange[1]);

   TH2F * h2 = new TH2F("h2", "FCUP", tBin, tRange[0], tRange[1], 500, 0, 30);
   TProfile * h2_pfx = new TProfile("h2_pfx", "FCUP; time [min]; FCUP mean", tBin, tRange[0], tRange[1]);
   TH2F * h3 = new TH2F("h3", "FCUP-ISOLDE", tBin, tRange[0], tRange[1], 500, 0, 15);
   TH2F * hElum = new TH2F("hElum", "Elum vs time", tBin, tRange[0], tRange[1], 600, 700, 1300);

   int padID = 1;
   if( runID == 0 ){
      cScript->cd(padID);
      tree0->Draw("run : elum_t/1e8/60. - 0.261936341558169*60.*24 >>hRun", gate_D, "colz");
      hRUN->ProfileX("hRUN_pfx");
      padID ++;
   }

   cScript->cd(padID);
   // offset to experiment date time
   tree0->Draw("elum_t/1e8/60. - 0.261936341558169*60.*24 >>h1", gate_D, "");
   tree0->Draw("elum_t/1e8/60. - 0.261936341558169*60.*24 >>hBG", gate_BG, "");

   TH1F * hD = new TH1F("hD", Form("ELUM Deuteron (est. BG-subtracted) - RUN:%d ; time [min]; count / min", runID ), tBin, tRange[0], tRange[1]);

   hD->Add(h1);
   hD->Add(hBG, -1);

   hD->Draw();
   gSystem->ProcessEvents();
   padID++;
  
   //find bin for 
   int bin[2];
   bool flag = false;
   for( int i = 1; i <= tBin ; i++){
     double y = h1->GetBinContent(40);
     if( flag == false && y > 0 ) {
       bin[0] = i;
       flag = true;
     }
     if( flag == true && y == 0.){
       bin[1] = i;
       break;
     }
   }

   //cScript->cd(padID);
   //tree0->Draw("elum: elum_t/1e8/60. - 0.261936341558169*60.*24>>hElum", "", "colz");
   //padID++;


   cScript->cd(padID);
   tree1->Draw(Form("%f*f: f_t/1e8/60. - 0.261936341558169*60.*24>>h2", fScale), "", "colz");
   h2->SetYTitle("FCUP [pA]");
   h2->SetXTitle("time [min]");
   h2->ProfileX("h2_pfx");
   h2_pfx->Draw("hist");
   padID++;
   gSystem->ProcessEvents();


   cScript->cd(padID);

   TH1F * hTD = new TH1F("hTD", Form("Target Degrade - RUN:%d ; time [min]; count / min", runID ), tBin, tRange[0], tRange[1]);

   hTD->Divide(hD, h2_pfx);
   hTD->Draw();
   gSystem->ProcessEvents();

   //cScript->cd(padID);
   //tree2->Draw(Form("%f*f: f_t/1e8/60. - 0.261936341558169*60.*24>>h3", fScale), "", "colz");

   ///=================== Save to txt file
   int nBin = h1->GetNbinsX();
   
   FILE * paraOut;
   TString fileName = "TD.txt";
   paraOut = fopen (fileName.Data(), "w+");
   printf("=========== save histogram to %s \n", fileName.Data());
   
   for( int i = 1; i <= nBin ; i++){
      
      double x     = h1->GetBinCenter(i);
      double y1    = h1->GetBinContent(i);
      double yRun  = hRUN_pfx->GetBinContent(i);
      double yBG   = hBG->GetBinContent(i);
      double yFCup = h2_pfx->GetBinContent(i);
      
      fprintf(paraOut, "%14.8f\t%9.6f\t%3.0f\t%9.6f\t%9.6f\n", x, y1, yRun, yBG, yFCup);
   }

   fflush(paraOut);
   fclose(paraOut);



   /*
   if( runID != 0 ){
     h2->ProfileX("h2_pfx");
     h2_pfx->SetLineColor(2);

     double duration = runTime[runID - 25][0] * 24 * 60; // min
     printf("Duration: %f [min] \n",  duration );
     double ddCount = h1->GetEntries();
     printf("Total (d,d) count : %f \n", ddCount ); 
     double fcuppA = h2_pfx->Integral(bin[0], bin[1]) ; // in pico-A
     printf("Integral FCUP: %f [pC]\n", fcuppA * 60.);

     printf("-------------- Average \n");
     cScript->cd(1);
     TF1 * fit0 = new TF1("fit0", "pol0", tRange[0], tRange[1]);
     h1->Fit("fit0", "", "Rq",  tRange[0] + 1.5*tPadding, tRange[1] - 3*tPadding);

     double ddCountA  = fit0->GetParameter(0);
     double ddCountAE = fit0->GetParError(0);

     cScript->cd(2);
     TF1 * fit1 = new TF1("fit1", "pol0", tRange[0], tRange[1]);
     h2_pfx->Fit("fit1", "", "Rq",  tRange[0] + 1.5*tPadding, tRange[1] - 3*tPadding);

     double fCuppAA  = fit1->GetParameter(0);
     double fCuppAAE = fit1->GetParError(0);

     printf("(d,d) count per min          : %f (%f)\n", ddCountA, ddCountAE );
     printf("Average Integral FCUP per min: %f (%f)[pA]\n", fCuppAA, fCuppAAE );
   
     h2_pfx->Draw("same");
   }

   return;
   
   cScript->cd(2);
   TH1F * h1copy = (TH1F*) h1->Clone("h1copy");
   h1copy->Divide(h2_pfx);
   h1copy->SetTitle("ELUM/FCUP");
   h1copy->SetYTitle("");
   h1copy->Draw();

   if( runID == 0 ) return;

   TF1 * fit = new TF1("fit", "pol1", tRange[0], tRange[1]);
   h1copy->Fit("fit", "", "Rq", tRange[0] + 1.5*tPadding, tRange[1] - 3*tPadding);
   
   TLatex text;
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.06);
   
   double chi2 = fit->GetChisquare();
   int ndf = fit->GetNDF();
   text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

   double p0 = fit->GetParameter(0);
   double p1 = fit->GetParameter(1);

   double e0 = fit->GetParError(0);
   double e1 = fit->GetParError(1);

   text.DrawLatex(0.15, 0.7, Form("%5.2f(%5.2f) + %5.2f(%5.2f) t", p0, e0, p1, e1));


   //*/
}
