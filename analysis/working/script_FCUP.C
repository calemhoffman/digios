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
  //duration, startTime
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

void script_FCUP(int runID = 41){

   
   TFile * file0 = new TFile("A_gen_run025.root");
   TFile * file1 = new TFile("FCUP_user2.dat.root");
   TFile * file2 = new TFile("FCUP_beamline.dat.root");
 
   TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   TTree * tree2 = (TTree*) file2->FindObjectAny("tree");

   //========================== Gate   
   TString gate_D = Form("1000 < elum && elum < 1250 && run==%d ", runID);
   
   double fScale = 1.0;

   if( runID == 0) {
     gate_D = "1000 < elum && elum < 1250";
     fScale = 1.;
   }

   //========================== Canvas
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, 800, 700);
   cScript->Divide(1,3);
   for( int i = 1; i <= 3 ; i++){
      cScript->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat(11111111);
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   //========================== Plot
   double tPadding = 0.01;
   double tRange[2] = {runTime[runID - 25][1] - tPadding, runTime[runID - 25][1] + runTime[runID - 25][0] + tPadding };
   int tBin = (tRange[1] - tRange[0])*24*60;

   if( runID == 0 ){
     tRange[0] = 0.9;
     tRange[1] = 5.3;
     tBin = 6*24*60;
   }

   printf("run-%d | startTime : %f [day], endTime : %f [day]\n", runID, tRange[0], tRange[1]);

   TH1F * h1 = new TH1F("h1", Form("ELUM Deuteron - RUN:%d", runID ), tBin, tRange[0], tRange[1]);
   TH2F * h2 = new TH2F("h2", "FCUP", tBin, tRange[0], tRange[1], 500, 0, 30);
   TProfile * h2_pfx = new TProfile("h2_pfx", "h2_pfx", tBin, tRange[0], tRange[1]);
   TH2F * h3 = new TH2F("h3", "FCUP-ISOLDE", tBin, tRange[0], tRange[1], 500, 0, 30);

   cScript->cd(1);
   // offset to experiment date time
   tree0->Draw("elum_t/1e8/60/60/24. - 0.261936341558169 >>h1", gate_D, "");
   h1->SetYTitle("count / min ");
   h1->SetXTitle("day");

   //find bin
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

   cScript->cd(2);
   tree1->Draw(Form("%f*f: f_t/1e8/60./60./24 - 0.261936341558169>>h2", fScale), "", "colz");
   h2->SetYTitle("FCUP [pA]");
   h2->SetXTitle("day");


   cScript->cd(3);
   tree2->Draw(Form("%f*f: f_t/1e8/60./60./24 - 0.261936341558169>>h3", fScale), "", "colz");
   

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
   
}
