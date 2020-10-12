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
 
void SetCanvas(int divX, int divY, int padSize, TCanvas * cScript);

const int numDet = 1;
//int detID[numDet] = {0,1,2,6}; 
//TString detName[numDet] = {"HPGe-1","HPGe-2","HPGe-3", "NaI"} ;
int detID[numDet] = {1}; 
TString detName[numDet] = {"HPGe-2"} ;
int eRange[2] = { 40, 1000 }; // keV
double cali[numDet][2]= { //{ -0.44862490, 0.62456335},
                          {  -3.20496468, 0.64990118}};
                          //{ -1.86186218, 0.67175540},
                          //{-17.00628161, 0.70740610}};
                          
int BGRunID=247;

void script_Gamma(){

   //========================== input file/cut/simulation   
   TChain * tree = new TChain("gen_tree");
   //tree->Add("../root_data/gen_run23[1-9].root"); //Cali-Source
   //tree->Add("../root_data/gen_run24[0-6].root"); //Cali-Source
   //tree->Add("../root_data/gen_run247.root"); // BG
   //tree->Add("../root_data/gen_run248.root"); // 152Eu
   //tree->Add("../root_data/gen_run249.root"); // 152Eu, outside
   //tree->Add("../root_data/gen_run25[0-9].root"); // Cali-source, outside
   //tree->Add("../root_data/gen_run26[0-3].root"); // Cali-source, outside
   //tree->Add("../root_data/gen_run266.root"); // 133Ba, 10 mins
   //tree->Add("../root_data/gen_run269.root"); // 133Ba, 10 mins
   //tree->Add("../root_data/gen_run270.root"); 
   //tree->Add("../root_data/gen_run271.root"); 
   //tree->Add("../root_data/gen_run272.root"); 
   //tree->Add("../root_data/gen_run306.root"); 
   //tree->Add("../root_data/gen_run307.root"); 
   tree->Add("../root_data/gen_run30[8-9].root"); //Al, 140 enA
   tree->Add("../root_data/gen_run31[0-9].root"); 
   tree->Add("../root_data/gen_run32[0-9].root"); 
   tree->Add("../root_data/gen_run33[0-3].root"); 
   //tree->Add("../root_data/gen_run334.root"); //BG
   
   tree->Add("../root_data/gen_run34[6-9].root"); //Al, 50 enA
   tree->Add("../root_data/gen_run35[0-4].root"); //Al, 50 enA
   
   tree->GetListOfFiles()->Print();
   
   double a0[numDet];
   double a1[numDet];
   for( int i = 0; i < numDet; i++){
      a0[i] = cali[i][0];
      a1[i] = cali[i][1];
   }
   
   float energyRange[3];
   energyRange[1] = eRange[0];
   energyRange[2] = eRange[1];
   energyRange[0] = eRange[1] - eRange[0];
   
   //========================== Canvas
   
   int div[2] = {1,1}; // x,y
   int padSize = 400;
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, padSize *2 * div[0], padSize * div[1]);
   cScript->Divide(div[0], div[1]);
   for( int i = 1; i <= div[0] * div[1] ; i++){
      cScript->cd(i)->SetGrid();
      ///cScript->cd(i)->SetLogz();
   }
   
   gStyle->SetOptStat("neiou");
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   //########################################### Plot
   
   
   TH1F ** q = new TH1F*[numDet];
   TH1F ** p = new TH1F*[numDet];
   TString * gate1 = new TString[numDet];   
   TString * gate2 = new TString[numDet];
   

   for( int i = 0; i < numDet; i ++){
      int id = detID[i];

      if( id != 6 ){
         q[i] = new TH1F(Form("q%d", id), detName[i], energyRange[0]*2, energyRange[1], energyRange[2]);
         p[i] = new TH1F(Form("p%d", id), detName[i] + "| BG", energyRange[0]*2, energyRange[1], energyRange[2]);
         q[i]->SetYTitle("count / 0.5 keV ");
      }else{
         q[i] = new TH1F(Form("q%d", id), detName[i], energyRange[0]/1., energyRange[1], energyRange[2]);
         p[i] = new TH1F(Form("p%d", id), detName[i] + "| BG", energyRange[0]/1., energyRange[1], energyRange[2]);
         q[i]->SetYTitle("count / 1 keV ");
      }
      q[i]->SetXTitle("gamma-energy[keV]");
      q[i]->SetLineColor(2);
      

      cScript->cd(i+1);
      //cScript->cd(i+1)->SetLogy();   
   
      TString expression;
   
      expression.Form("rdt[%d] * %.8f + %.8f >> q%d", id, a1[i], a0[i], id);      
      gate1[i].Form("!TMath::IsNaN(rdt[%d]) ", id);   
      tree->Draw(expression, gate1[i] , "");
      
      expression.Form("(rdt[%d] * %.8f + %.8f) >> p%d", id, a1[i], a0[i], id);      
      gate2[i].Form("!TMath::IsNaN(rdt[%d])  && runID == 334", id);
      tree->Draw(expression, gate2[i] , "same");
      
      //p[i]->Scale(0.817);
      p[i]->Scale(2.0);
      
      p[i]->Draw("same hist");

      cScript->Update();
      gSystem->ProcessEvents();
   }
   
   
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 600, padSize*2 * div[0], padSize * div[1]);
   cAux->Divide(div[0], div[1]);
   
   TH1F ** h = new TH1F*[numDet];
   for( int i = 0; i < numDet; i++){
      cAux->cd(i+1)->SetGrid();
      int id = detID[i];
      
      h[i] = (TH1F*) q[i]->Clone();
      h[i]->SetName(Form("h%d", id));
      
      //h[i]->Divide(p[i]);
      h[i]->Add(p[i], -1);
      
      cAux->cd(i+1);
      h[i]->Draw("hist");
      
      cScript->Update();
      gSystem->ProcessEvents();
      
   }
   
   
   ///TFile * fout = new TFile("Mixed.root", "RECREATE");
   ///fout->cd();
   ///for( int i = 0; i < numDet; i++){
   ///   q[i]->Write();
   ///}
   ///fout->Write();
   ///fout->Close();
   
  
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

 
