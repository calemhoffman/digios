#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLine.h"
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
 
void SetCanvas(int divX, int divY, int padSize, TCanvas * cScript);
void Aux(int cutID, TTree *tree0, TCutG ** cut_in, TCanvas * cScript, int startPadID, TObjArray * fxList, TObjArray * gList);

TChain * tree0 ;
TString gate_RDT;
TString gate_time;

TFile * fileCut;
TObjArray * cutList;
TCutG ** cut_in;

void LoadRDTCut(TString rdtCutFile, TString str){
   
   if( fileCut != NULL ) delete fileCut;
   if( cutList != NULL ) delete cutList;
   if( cut_in != NULL ) delete cut_in;
   
   
   fileCut = new TFile(rdtCutFile);
   
   if( fileCut->IsOpen() ){
      printf("====== File : %s \n", fileCut->GetName());
      cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut_in = new TCutG * [numCut];
      
      for( int i = 0 ; i < numCut; i++){
         cut_in[i] = (TCutG* ) cutList->At(i);
         TString name= cut_in[i]->GetName();
         cut_in[i]->SetName(str + name);
         cut_in[i]->SetLineWidth(3);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut_in[i]->GetName(), cut_in[i]->GetVarX(), cut_in[i]->GetVarY()); 
      }
   }
}

TString SetRDTGate(TString str){
   return Form("(%scut0 || %scut1 || %scut2 || %scut3)", str.Data(), str.Data(), str.Data(), str.Data());
}

void script_2(){

   TString fileName = "A_gen_run018-040.root";

   //========================== input file/cut/simulation   
   tree0 = new TChain("tree");
   tree0->Add(fileName);
   
   tree0->GetListOfFiles()->Print();
   
   //========================== Gate
   gate_RDT = "(cut0 || cut1 || cut2 || cut3)";
   gate_time =  "-10<coin_t && coin_t < 25"; 
   //========================== Canvas
      
   int div[2] = {2,1}; // x,y
   int padSize[2] = {800,800};
   
   TCanvas * cScript = new TCanvas("cScript", "cScript : " + fileName, 0, 0, padSize[0] * div[0], padSize[1] * div[1]);
   cScript->Divide(div[0], div[1]);
   
   cScript->cd(1)->Divide(1,2);
   cScript->cd(2)->Divide(2,2);
   
   
   gStyle->SetOptStat("");
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   //########################################### Plot

   
   ///SetCanvas(1,2, 800, cScript);
   
   TH1F * h1 = new TH1F("h1", "E_{x}; E_{x}[ MeV]; Count / 20 keV", 700, -1, 13);
   TH1F * h2 = new TH1F("h2", "E_{x}; E_{x}[ MeV]; Count / 20 keV", 700, -1, 13);
   TH1F * h3 = new TH1F("h3", "E_{x}; E_{x}[ MeV]; Count / 20 keV", 700, -1, 13);
   h2->SetLineColor(2);
   h3->SetLineColor(4);
   
   
   TH2F * rdt1 = new TH2F("rdt1", "rdt[1]-rdt[0]; E ; dE", 500, 1500, 2600,500, 280, 800); 
   TH2F * rdt2 = new TH2F("rdt2", "rdt[3]-rdt[2]; E ; dE", 500, 1600, 3000,500, 140, 420); 
   TH2F * rdt3 = new TH2F("rdt3", "rdt[5]-rdt[4]; E ; dE", 500, 1500, 2400,500, 330, 800); 
   TH2F * rdt4 = new TH2F("rdt4", "rdt[7]-rdt[6]; E ; dE", 500, 1500, 2600,500, 160, 500); 
   
   cScript->cd(2)->cd(1); tree0->Draw("rdt[1]:rdt[0]>>rdt1", gate_time, "colz");
   cScript->cd(2)->cd(2); tree0->Draw("rdt[3]:rdt[2]>>rdt2", gate_time, "colz");
   cScript->cd(2)->cd(3); tree0->Draw("rdt[5]:rdt[4]>>rdt3", gate_time, "colz");
   cScript->cd(2)->cd(4); tree0->Draw("rdt[7]:rdt[6]>>rdt4", gate_time, "colz");
   
   
   LoadRDTCut("rdtCuts_B.root", "B_");
   gate_RDT = SetRDTGate("B_");
   gROOT->cd();   
   cScript->cd(1)->cd(1); tree0->Draw("Ex>>h1", gate_RDT + " && " + gate_time, "");
   cScript->cd(2)->cd(1); cut_in[0]->Draw("same");
   cScript->cd(2)->cd(2); cut_in[1]->Draw("same");
   cScript->cd(2)->cd(3); cut_in[2]->Draw("same");
   cScript->cd(2)->cd(4); cut_in[3]->Draw("same");
   
   LoadRDTCut("rdtCuts_C.root", "C_");   
   gate_RDT = SetRDTGate("C_");
   gROOT->cd();
   cScript->cd(1)->cd(1); tree0->Draw("Ex>>h2", gate_RDT + " && " + gate_time, "");
   cScript->cd(2)->cd(1); cut_in[0]->Draw("same");
   cScript->cd(2)->cd(2); cut_in[1]->Draw("same");
   cScript->cd(2)->cd(3); cut_in[2]->Draw("same");
   cScript->cd(2)->cd(4); cut_in[3]->Draw("same");
   
   LoadRDTCut("rdtCuts_E.root", "E_");   
   gate_RDT = SetRDTGate("E_");
   gROOT->cd();
   cScript->cd(1)->cd(1); tree0->Draw("Ex>>h3", gate_RDT + " && " + gate_time, "");
   cScript->cd(2)->cd(1); cut_in[0]->Draw("same");
   cScript->cd(2)->cd(2); cut_in[1]->Draw("same");
   cScript->cd(2)->cd(3); cut_in[2]->Draw("same");
   cScript->cd(2)->cd(4); cut_in[3]->Draw("same");

   gROOT->cd();
   cScript->cd(1)->cd(1);
   h1->Draw();
   h2->Draw("same");
   h3->Draw("same");
   
   
   TLatex text;
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   
   text.SetTextColor(1); text.DrawLatex(0.15, 0.8, "Bound ^{21}Ne");
   text.SetTextColor(2); text.DrawLatex(0.15, 0.75, "Unbound ^{21}Ne, neutron decay");
   text.SetTextColor(4); text.DrawLatex(0.15, 0.7, "Unbound ^{21}Ne, #alpha decay");
   
   double yMax = h1->GetMaximum();
   TLine* lineSn = new TLine(6.76, 0, 6.76, yMax);
   TLine* lineSa = new TLine(7.35, 0, 7.35, yMax);
   
   lineSn->SetLineColor(2);
   lineSa->SetLineColor(4);
   
   lineSn->Draw("same");
   lineSa->Draw("same");
   
   text.SetNDC(0);
   text.SetTextColor(2); text.SetTextSize(0.04); text.DrawLatex(6.76, yMax, "S_{n}");
   text.SetTextColor(4); text.SetTextSize(0.04); text.DrawLatex(7.35, yMax, "S_{#alpha}");
   
   cScript->cd(1)->cd(2);
   
   LoadRDTCut("rdtCuts.root", "BC_");   
   gate_RDT = SetRDTGate("BC_");
   gROOT->cd();
   TH2F * hExTheta = new TH2F("hExTheta", "#theta_{CM} vs E_{x} ( gate_B+C, -10 < coinTime < 25 ); E_{x} [MeV] ; #theta_{CM}", 700, -1, 13, 400, 5, 45); 
   tree0->Draw("thetaCM:Ex>>hExTheta",gate_RDT + " && " + "-10<coin_t && coin_t < 25", "colz");
   
}

void SetCanvas(int divX, int divY, int padSize, TCanvas * cScript){
   
   cScript->Clear();
   cScript->SetWindowSize(divX* padSize, divY * padSize);
   cScript->Divide(divX, divY);
   for( int i = 1; i <= divX * divY ; i++){
      cScript->cd(i)->SetGrid();
      ///cScript->cd(i)->SetLogz();
   }
}
