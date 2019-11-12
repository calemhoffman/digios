#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"
#include "THStack.h"

/*********************************************
 * 
 * The script is for temperary analysis,
 * 
 * plot some graphs, apply simple gate or cut.
 * 
 * 
 * ******************************************/
 
 
void Aux(int cutID, TTree *tree0, TCutG ** cut_in, TCanvas * cScript, int startPadID, TObjArray * fxList, TObjArray * gList);

void script1(){

   //========================== input file/cut/simulation
   //TString fileName = "A_trace_run008-013.root"; // 15N degraded
   //TString fileName = "A_trace_run023-034.root"; // 500ug, 16N, not well tuned.
   //TString fileName = "A_trace_run056-057_059-085_087-099.root"; //226ug, 16N
   //TString fileName = "A_trace_run101-144_147-170.root"; //366ug, 16N
   //TString fileName = "A_trace_run056-057_059-085_088-099_101-112.root"; // all 16N
   //TFile * file0 = new TFile(fileName);
   //TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   
   TChain * tree0 = new TChain("tree");
   //tree0->Add("A_trace_run056-057_059-085_087-099.root");
   tree0->Add("A_trace_run101-144_147-194.root");
   //TString fileName = "run056 - run194";
   TString fileName = "run101 - run194";
   
   TFile * file1 = new TFile("transfer.root");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray*) file1->FindObjectAny("gList");
   
   TString rdtCutFile = "rdt_16N.root";
   TFile * fileCut = new TFile(rdtCutFile);
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
   
   //========================== Gate
   ///TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && detID != 12 && -20 < coin_t && coin_t < 40 && rdtMultiHit == 2 && arrayRDT == 0 && multiHit == 1 ";   
   
   ///TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && -20 < coin_t && coin_t < 40 ";

   TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && multiHit == 1 && rdtdEMultiHit == 1";
   
   TString gate_time = "abs(coinTime-19)<9 && Ex > -2";  ///change time gate
 //   TString gate_time = "abs(coinTime)<200";  
   //========================== Canvas
   int div[2] = {6,4}; // x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript : " + fileName + "|" + rdtCutFile, 0, 0, 400 * div[0], 400 * div[1]);
   cScript->Divide(div[0], div[1]);
   for( int i = 1; i <= div[0] * div[1] ; i++){
      cScript->cd(i)->SetGrid();
      ///cScript->cd(i)->SetLogz();
   }
   
   gStyle->SetOptStat("neiou");
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   //########################################### Plot
   
   //======================= Trace of recoil
   
   ///canvas size = 4 x 3
   ///cScript->cd(1);  tree0->Draw("rdt[1]:rdt[0]>>rdt1(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///cScript->cd(2);  tree0->Draw("rdt[3]:rdt[2]>>rdt2(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///cScript->cd(3);  tree0->Draw("rdt[5]:rdt[4]>>rdt3(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///cScript->cd(4);  tree0->Draw("rdt[7]:rdt[6]>>rdt4(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///
   ///
   ///cScript->cd(5);  tree0->Draw("trdt[1]:trdt[0]>>trdt1(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///cScript->cd(6);  tree0->Draw("trdt[3]:trdt[2]>>trdt2(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///cScript->cd(7);  tree0->Draw("trdt[5]:trdt[4]>>trdt3(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///cScript->cd(8);  tree0->Draw("trdt[7]:trdt[6]>>trdt4(300, 0, 4000, 300, 0, 8000)", "", "colz");
   ///
   ///cScript->cd(9);  tree0->Draw("trdt[1]:trdt[0]>>trdt1g(300, 0, 4000, 300, 0, 8000)", "trdt_r[0]<20", "colz");
   ///cScript->cd(10); tree0->Draw("trdt[3]:trdt[2]>>trdt2g(300, 0, 4000, 300, 0, 8000)", "trdt_r[2]<20", "colz");
   ///cScript->cd(11); tree0->Draw("trdt[5]:trdt[4]>>trdt3g(300, 0, 4000, 300, 0, 8000)", "trdt_r[4]<20", "colz");
   ///cScript->cd(12); tree0->Draw("trdt[7]:trdt[6]>>trdt4g(300, 0, 4000, 300, 0, 8000)", "trdt_r[6]<20", "colz");
   

   
   //======================== coinTime, RDT
   
   /**
   cScript->cd(2); tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "int(detID/5)==1", "colz");
   cScript->cd(3); tree0->Draw("coinTime:z>>h2(400, 700, 1000, 400, -50, 200)", "int(detID/5)==2", "colz");
   cScript->cd(6); tree0->Draw("coinTime:z>>h3(400, 700, 1000, 400, -50, 200)", "int(detID/5)==3", "colz");
   cScript->cd(5); tree0->Draw("coinTime:z>>h4(400, 700, 1000, 400, -50, 200)", "int(detID/5)==4", "colz");
   cScript->cd(4); tree0->Draw("coinTime:z>>h5(400, 700, 1000, 400, -50, 200)", "int(detID/5)==5", "colz");
   */
   
   ///cScript->cd(1);  tree0->Draw("coinTimeUC:x>>j1(400, -2, 2, 400, -100, 200)", "detID == 5 && cut1", "colz");
   ///
   ///TObjArray * cfList = (TObjArray *) file0->FindObjectAny("fList");
   ///cfList->At(5)->Draw("same");
   ///cfList->At(5)->Print();
   ///TF1 * fit = (TF1*) cfList->At(5);
   ///for( int i = 0; i < 8; i++){
   ///   printf("a%i: %f \n", i, fit->GetParameter(i));
   ///}
   
   ///cScript->cd(1);  tree0->Draw("trdt[1]:trdt[0]>>k1(400, 0, 4000, 400, 0, 7000)", "", "colz");
   ///cut_in[0]->Draw("same");
   ///cScript->cd(2);  tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "int(detID/5)==1 && cut0", "colz");
   ///cScript->cd(3);  tree0->Draw("coinTime:z>>h2(400, 700, 1000, 400, -50, 200)", "int(detID/5)==2 && cut0", "colz");
   ///cScript->cd(4);  tree0->Draw("coinTime:z>>h3(400, 700, 1000, 400, -50, 200)", "int(detID/5)==3 && cut0", "colz");
   ///cScript->cd(5);  tree0->Draw("coinTime:z>>h4(400, 700, 1000, 400, -50, 200)", "int(detID/5)==4 && cut0", "colz");
   ///cScript->cd(6);  tree0->Draw("coinTime:z>>h5(400, 700, 1000, 400, -50, 200)", "int(detID/5)==5 && cut0", "colz");
   
   ///cScript->cd(1);  tree0->Draw("trdt[3]:trdt[2]>>k1(400, 0, 4000, 400, 0, 7000)", "", "colz");
   ///cut_in[1]->Draw("same");
   ///cScript->cd(2);  tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "int(detID/5)==1 && cut1", "colz");
   ///cScript->cd(3);  tree0->Draw("coinTime:z>>h2(400, 700, 1000, 400, -50, 200)", "int(detID/5)==2 && cut1", "colz");
   ///cScript->cd(4);  tree0->Draw("coinTime:z>>h3(400, 700, 1000, 400, -50, 200)", "int(detID/5)==3 && cut1", "colz");
   ///cScript->cd(5);  tree0->Draw("coinTime:z>>h4(400, 700, 1000, 400, -50, 200)", "int(detID/5)==4 && cut1", "colz");
   ///cScript->cd(6);  tree0->Draw("coinTime:z>>h5(400, 700, 1000, 400, -50, 200)", "int(detID/5)==5 && cut1", "colz");
   ///
   ///cScript->cd(1);  tree0->Draw("trdt[5]:trdt[4]>>k1(400, 0, 4000, 400, 0, 7000)", "", "colz");
   ///cut_in[2]->Draw("same");
   ///cScript->cd(2); tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "int(detID/5)==1 && cut2", "colz");
   ///cScript->cd(3); tree0->Draw("coinTime:z>>h2(400, 700, 1000, 400, -50, 200)", "int(detID/5)==2 && cut2", "colz");
   ///cScript->cd(4); tree0->Draw("coinTime:z>>h3(400, 700, 1000, 400, -50, 200)", "int(detID/5)==3 && cut2", "colz");
   ///cScript->cd(5); tree0->Draw("coinTime:z>>h4(400, 700, 1000, 400, -50, 200)", "int(detID/5)==4 && cut2", "colz");
   ///cScript->cd(6); tree0->Draw("coinTime:z>>h5(400, 700, 1000, 400, -50, 200)", "int(detID/5)==5 && cut2", "colz");
   
   ///cScript->cd(1);  tree0->Draw("trdt[7]:trdt[6]>>k1(400, 0, 4000, 400, 0, 7000)", "", "colz");
   ///cut_in[3]->Draw("same");
   ///cScript->cd(2); tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "int(detID/5)==1 && cut3", "colz");
   ///cScript->cd(3); tree0->Draw("coinTime:z>>h2(400, 700, 1000, 400, -50, 200)", "int(detID/5)==2 && cut3", "colz");
   ///cScript->cd(4); tree0->Draw("coinTime:z>>h3(400, 700, 1000, 400, -50, 200)", "int(detID/5)==3 && cut3", "colz");
   ///cScript->cd(5); tree0->Draw("coinTime:z>>h4(400, 700, 1000, 400, -50, 200)", "int(detID/5)==4 && cut3", "colz");
   ///cScript->cd(6); tree0->Draw("coinTime:z>>h5(400, 700, 1000, 400, -50, 200)", "int(detID/5)==5 && cut3", "colz");
   
   
   //========================================== 
   
   ///Canvas size is 6 x 4
   
   gStyle->SetPalette(kBird);
   
   Aux(0, tree0, cut_in, cScript,  1, fxList, gList);
   Aux(1, tree0, cut_in, cScript,  7, fxList, gList);
   Aux(2, tree0, cut_in, cScript, 13, fxList, gList);
   Aux(3, tree0, cut_in, cScript, 19, fxList, gList);

   //==========================================  E-Z and Ex
   
   ///cScript->cd(1); tree0->Draw("e:z>>h1(400, 700, 1000, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 19)< 8 && Ex > -2 && thetaCM > 10", "colz");
   ///fxList->At(0)->Draw("same");
   ///fxList->At(1)->Draw("same");
   ///gList->At(0)->Draw("same");
   ///cScript->cd(2); tree0->Draw("Ex:thetaCM>>h2(400, 0, 50, 400, -2, 5)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 19)< 8 && Ex > -2 && thetaCM > 10", "colz");
   ///cScript->cd(3); tree0->Draw("Ex>>h3(280, -2, 5)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 19)< 8 && Ex > -2 && thetaCM > 10", "colz");
   ///
   ///TString gate ;
   ///TString express ;
   ///for( int i = 5; i < 30 ; i++){
   ///   cScript->cd(i-4); 
   ///   express.Form("Ex>>h%d(200, -2, 8)", i);
   ///   gate.Form("(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 19)< 10 && Ex > -2 && thetaCM > 10 && detID == %d", i);
   ///   tree0->Draw(express, gate, "");
   ///}

   //========================================= histogram offset
   
   
   /// Canvas size 5 x 5
   
   ///For H072_16N
   
   
   div[0] = 5; // x
   div[1] = 5; // y
   TCanvas * cJaJaJa = new TCanvas("cJaJaJa", "cJaJaJa : " + fileName + "|" + rdtCutFile, 0, 0, 200 * div[0], 200 * div[1]);
   cJaJaJa->Divide(div[0], div[1]);
   for( int i = 1; i <= div[0] * div[1] ; i++){
      cJaJaJa->cd(i)->SetGrid();
      ///cScript->cd(i)->SetLogz();
   }
   
   
   ///2nd calibration
   ///double ExOffset[30] ={     -1,     -1,     -1,     -1,     -1,
   ///                        0.005,     -1,     -1,     -1,     -1,
   ///                        0.112,     -1,  0.331,  0.360,     -1,
   ///                        0.014,  0.126,  0.224,  0.293,  7.110-6.589,
   ///                       -0.007,  0.082,  0.245,  1.083,  0.589,
   ///                        0.259,  0.264,  0.432,  0.456,     -1};
   
   double ExOffset[30];
   for( int i = 0; i < 30; i++) ExOffset[i] = 0;
   
   // testing
   //double ExOffset[30] ={     -1,     -1,     -1,     -1,     -1,
   //                            0,      0,      0,      0,     -1,
   //                            0,     -1,      0,      0,     -1,
   //                            0,      0,      0,      0,      0,
   //                         -0.1,   -0.1,   -0.1,   -0.1,      0,
   //                         -0.2,   -0.2,   -0.2,   -0.2,     -1};
      
                          
   TString gate ;
   TString express ;
   TH1F ** hh = new TH1F *[30];
   double resol = 0.1;
   int ExRange [2] = {-3, 7};
   TLegend * legend = new TLegend(0.85,0.1,.99,0.99);
   
   
   for( int i = 5; i < 30 ; i++){
      cJaJaJa->cd(i-4); 
      hh[i] = new TH1F( Form("hh%d", i), Form("hh%d", i), (ExRange[1] - ExRange[0])/resol, ExRange[0], ExRange[1]);
      express.Form("Ex - %f>>hh%d", ExOffset[i], i);
      gate.Form("(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 19)< 10 && Ex > -2 && thetaCM > 10 && detID == %d", i);
      tree0->Draw(express, gate, "");
      legend->AddEntry(hh[i]);

   }
                          
   THStack *hs = new THStack("hs","");
   for( int i = 29; i > 4 ; i--){
      if( ExOffset[i] > -1 ) hs->Add(hh[i]);
   }
   
   TCanvas * hahaha = new TCanvas( "hahaha", "Stacked Ex histogram & all", 1600, 600);
   hahaha->Divide(2,1);
   hahaha->cd(1);
   //gStyle->SetPalette(kRainbow);
   hs->Draw("pfc");
   gStyle->SetOptStat("000000");
   legend->Draw();


   gStyle->SetOptStat("neiour");   
   TH1F * hAll = new TH1F("hAll", Form("%s | %s", fileName.Data(), rdtCutFile.Data()), (ExRange[1] - ExRange[0])/resol, ExRange[0], ExRange[1]);
   
   for( int i = 5; i < 30 ; i++){
      if( ExOffset[i] == -1 ) continue;

      hAll->Add(hh[i]);
      
   }
   hAll->Add(hh[10],-1);
   
   hahaha->cd(2);
   hAll->SetLineColor(4);
   hAll->SetLineWidth(3);
   hAll->SetXTitle(Form("Ex [%.1f keV]", resol * 1000));   
   hAll->Draw("same");
   
   
   //TF1 * fit = new TF1("fit", "gaus");
   //hAll->Fit("fit", "", "R",0, 1.0 );
   
   TF1 * fit = new TF1("fit", "gaus", 0.,1.1) ;
   TF1 * fit1 = new TF1("fit1", "gaus",-0.7,0.3); 
   TF1 * fit3 = new TF1("fit3", "gaus",0.2,0.5);
   TF1 * fit2 = new TF1("fit2", "gaus(0)+gaus(3)+gaus(6)",-0.7,1.1);  
   
   double par[100];
   double par_err[100];
   fit->SetLineColor(kBlack);
   fit1->SetLineColor(kBlack);
   fit3->SetLineColor(kBlack);
   
   //fit3->FixParameter(1,0.35);
   //fit3->FixParameter(2,0.166);


   hAll->Fit("fit","R+");
   hAll->Fit("fit1","R+");
   //fit3->FixParameter(1,0.35);                                                             
   //fit3->FixParameter(2,0.166); 
   hAll->Fit("fit3","R+");
   
   
   fit->GetParameters(&par[0]);
   fit1->GetParameters(&par[3]);
   
   fit3->GetParameters(&par[6]);

   
   fit2->SetParameters(par);
   fit2->FixParameter(5,0.166);
   fit2->FixParameter(2,0.166);
   fit2->FixParameter(8,0.08xc);

   fit2->FixParameter(7,0.35);
   fit2->FixParameter(6,15);


   
   fit2->SetLineColor(kRed);
   fit2->SetLineWidth(5);
   hAll->Fit("fit2","R+");
   
   fit2->GetParameters(&par[9]);
   for(int i=0;i<9;i++){
      par_err[i]=fit2->GetParError(i);
   }
   TLatex text;
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   text.SetTextColor(2);
  /*
   text.DrawLatex(0.4, 0.60, Form(" mean : %5.3f(%5.3f) MeV", fit->GetParameter(1), fit->GetParError(1)));
   text.DrawLatex(0.4, 0.55, Form("sigma : %5.3f(%5.3f) MeV", fit->GetParameter(2), fit->GetParError(2)));
   text.DrawLatex(0.4, 0.50, Form(" FWHM : %5.3f(%5.3f) MeV", fit->GetParameter(2) *2.355, fit->GetParError(2)*2.355));
*/
   text.DrawLatex(0.6, 0.65, " g.s ");
   text.DrawLatex(0.6, 0.60, Form(" mean : %5.3f(%5.3f) MeV", par[10], par_err[1]));
   text.DrawLatex(0.6, 0.55, Form("sigma : %5.3f(%5.3f) MeV", par[11], par_err[2]));
   text.DrawLatex(0.6, 0.50, Form(" FWHM : %5.3f(%5.3f) MeV", par[11]*2.355, par_err[2]*2.355)); 
    
   text.DrawLatex(0.6, 0.45, " isomeric ");
   text.DrawLatex(0.6, 0.40, Form(" mean : %5.3f(%5.3f) MeV", par[13], par_err[4]));
   text.DrawLatex(0.6, 0.35, Form("sigma : %5.3f(%5.3f) MeV", par[14], par_err[5]));
   text.DrawLatex(0.6, 0.30, Form(" FWHM : %5.3f(%5.3f) MeV", par[14]*2.355, par_err[5]*2.355));
    
   /**/
}

 
void Aux(int cutID, TTree *tree0, TCutG ** cut_in, TCanvas * cScript, int startPadID, TObjArray * fxList, TObjArray * gList){
   
   TString expression, gate= "";
   
   ////========
   cScript->cd(startPadID); 
   ///gate.Form("trdt_r[%d]<20", cutID*2);
   gate ="";
   expression.Form("trdt[%d]:trdt[%d]>>h%d(400, 0, 4000, 400, 0, 5000)", cutID*2+1, cutID*2, startPadID); 
   tree0->Draw(expression, gate, "colz"); 
   
   ////========
   cScript->cd(startPadID+1); 
   ///gate.Form(" 4000 > trdt[%d] && trdt[%d] > 1000 && trdt_r[%d]<20", cutID*2+1,cutID*2+1, cutID*2);
   gate.Form(" 4000 > trdt[%d] && trdt[%d] > 1000", cutID*2+1,cutID*2+1);
   gate.Form(" 4000 > trdt[%d] && trdt[%d] > 1000", cutID*2+1,cutID*2+1);
   expression.Form("coinTime:Ex>>h%d(100, -5, 8, 400, -50, 100)", startPadID+1);
   tree0->Draw(expression, gate , "colz");
   
   ////========
   cScript->cd(startPadID+2); 
   expression.Form("trdt[%d]:trdt[%d]>>h%d(400, 0, 4000, 400, 0, 5000)", cutID*2+1, cutID*2, startPadID+2); 
   ///gate.Form("abs(coinTime-19)<8 && Ex > -2 && trdt_r[%d]<20", cutID*2);
   gate.Form("abs(coinTime-19)<10 && Ex > -2 ");
   //gate.Form("abs(coinTime-36)<10");//crh
   tree0->Draw(expression, gate, "box"); 
   cut_in[cutID]->Draw("same");
   
   ////========
   cScript->cd(startPadID+3); 
   ///gate.Form("cut%d  && trdt_r[%d]<20 ", cutID, cutID*2);
   gate.Form("cut%d ", cutID);
   //gate.Form("cut%d && abs(coinTime-14)<10 ", cutID);
   expression.Form("coinTime:Ex>>h%d(100, -5, 8, 400, -50, 100)", startPadID+3);
   tree0->Draw(expression, gate , "box");
   TBox* box = new TBox(-5, 19-8, 8, 19+8);
   box->SetFillColorAlpha(kRed, 0.1);
   box->Draw("same");
   
   ////========
   cScript->cd(startPadID+4); 
   ///gate.Form("cut%d && abs(coinTime -19)< 8 && trdt_r[%d]<20 ", cutID, cutID*2);
   gate.Form("cut%d && abs(coinTime -19)<10", cutID);
   //gate.Form("cut%d && abs(coinTime-36)<10", cutID);//crh
   expression.Form("e:z>>h%d(400, 700, 1000, 400, 1, 15)", startPadID+4);
   tree0->Draw(expression, gate, "box");
   fxList->At(0)->Draw("same");
   gList->At(0)->Draw("same");
   
   ////========
   cScript->cd(startPadID+5); 
   expression.Form("Ex>>h%d(100, -5, 8)", startPadID+5);
   tree0->Draw(expression, gate);
   

   
}
