#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"


void script_RDT(){

   
   TFile * file0 = new TFile("../root_data/trace_run061.root");
   //TFile * file1 = new TFile("transfer.root");
   
   //TFile * fileCut = new TFile("rdtCuts.root");

   TTree * tree0 = (TTree*) file0->FindObjectAny("gen_tree");
   //TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   
   //TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
   //TObjArray * gList = (TObjArray*) file1->FindObjectAny("gList");
   
   /*
   TObjArray * cutList = NULL;
   TCutG ** cut = NULL;
   if( fileCut->IsOpen() ){
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut = new TCutG * [numCut];
      
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
      }
   }
   */
   //========================== Gate
   //TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && detID != 12 && -20 < coin_t && coin_t < 40 && rdtMultiHit == 2 && arrayRDT == 0 && multiHit == 1 ";   
   
   //TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && -20 < coin_t && coin_t < 40 ";
   
   //========================== Canvas
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 1000, 1200, 1000);
   cScript->Divide(5,4);
   for( int i = 1; i <= 16 ; i++){
      cScript->cd(i)->SetGrid();
      cScript->cd(i)->SetLogz();
   }
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat(11111111);
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   //========================== Plot
   TString exp, gate;
   for( int i = 0; i < 8; i++){
     cScript->cd( 5*(i/2) + i%2 +1);
     exp.Form("trdt[%d]-rdt[%d]:trdt_r[%d]>>h%d(400, 0, 500, 400, -1000, 5000)", i, i, i, i);
     tree0->Draw(exp, "", "colz");
   }

   for( int i = 0; i < 4; i++){
     cScript->cd(5*i+3);
     exp.Form("trdt[%d]:trdt[%d]>>k%d(400, 0, 5000, 400, 0, 10000)", 2*i+1, 2*i, i);
     tree0->Draw(exp, "", "colz");
     cScript->cd(5*i+4);
     exp.Form("rdt[%d]:rdt[%d]>>p%d(400, 0, 5000, 400, 0, 10000)", 2*i+1, 2*i, i);
     tree0->Draw(exp, "", "colz");
     cScript->cd(5*i+5);
     exp.Form("trdt[%d]:trdt[%d]>>q%d(400, 0, 5000, 400, 0, 10000)", 2*i+1, 2*i, i);
     gate.Form("trdt_r[%d] < 50", 2*i);
     tree0->Draw(exp, gate, "colz");
   }
}
