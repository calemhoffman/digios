#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"


void script(){

   
   TFile * file0 = new TFile("A_gen_run16.root");
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
      
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
      }
   }
   
   //========================== Gate
   //TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && detID != 12 && -20 < coin_t && coin_t < 40 && rdtMultiHit == 2 && arrayRDT == 0 && multiHit == 1 ";   
   
   TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && -20 < coin_t && coin_t < 40 ";
   
   //========================== Canvas
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, 1600, 600);
   cScript->Divide(4,2);
   for( int i = 1; i <= 4 ; i++){
      cScript->cd(i)->SetGrid();
      //cScript->cd(i)->SetLogz();
   }
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat(11111111);
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   //========================== Plot
   
   cScript->cd(1);
   tree0->Draw("rdt[0]:rdt[4]>>rdt1(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[0]->Draw("same");
   
   cScript->cd(2);
   tree0->Draw("rdt[1]:rdt[5]>>rdt2(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[1]->Draw("same");
   
   cScript->cd(3);
   tree0->Draw("rdt[2]:rdt[6]>>rdt3(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[2]->Draw("same");
   
   cScript->cd(4);
   tree0->Draw("rdt[3]:rdt[7]>>rdt4(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[3]->Draw("same");
   
   //===================================
   cScript->cd(5);
   //tree0->Draw("e:z >> k1(400, -500,  0, 400, 0, 10)", gate_RDT + "&& detID < 6", "colz");
   //tree1->Draw("e:z>> k1", "hit==1 && ExID <= 2 && loop == 1", "colz");
   
   tree0->Draw("e:z >> h1(400, -550, -100, 400, 0, 10)", "", "colz");
   //gList->At(10)->Draw("same");
   //gList->At(15)->Draw("same");
   //gList->At(20)->Draw("same");
   //tree0->Draw("rdtID:detID>>w(24, 0, 24, 8, 0, 8)", gate_RDT + "&& detID > -1 && rdtID >= 0", "colz");
   
   
   cScript->cd(6);
   //tree0->Draw("e:z >> k2(100, -500, 0, 400, 0, 10)", gate_RDT + "&& 6 <= detID && detID < 12", "colz");
   tree0->Draw("e:z >> h(400, -550,  -100, 400, 0, 10)", gate_RDT , "colz");
   //tree1->Draw("e:z>> k1", "hit==1 && ExID == 0", "colz");
   
   gList->At(10)->Draw("same");
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
   
   cScript->cd(7);
   //tree0->Draw("e:z >> k3(100, -500, 0, 400, 0, 10)", gate_RDT + "&& 12 <= detID && detID < 18", "colz");
   tree0->Draw("Ex >> h2(180, -1, 8)", gate_RDT , "");
   
   cScript->cd(8);
   //tree0->Draw("e:coin_t >> j(100, -100, 100, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");
   tree0->Draw("e:coin_t >> j(100, -100, 100, 400, 0, 10)", gate_RDT, "colz");
   
   /**/
}
