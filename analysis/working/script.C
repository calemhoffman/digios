#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"

/*********************************************
 * 
 * The script is for temperary analysis,
 * 
 * plot some graphs, apply simple gate or cut.
 * 
 * 
 * ******************************************/

void script(){

   //========================== input file/cut/simulation
   
   TString fileName = "A_trace_run051-070.root";
   TFile * file0 = new TFile(fileName);
   TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   
   TFile * file1 = new TFile("transfer.root");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray*) file1->FindObjectAny("gList");
   
   TString rdtCutFile = "rdtCuts_trace_C2.root";
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
   
   //========================== Canvas
   int div[2] = {4,2}; // x,y
   TCanvas * cScript = new TCanvas("cScript", "cScript : " + fileName + "|" + rdtCutFile, 0, 0, 400 * div[0], 400 * div[1]);
   cScript->Divide(div[0], div[1]);
   for( int i = 1; i <= div[0] * div[1] ; i++){
      cScript->cd(i)->SetGrid();
      ///cScript->cd(i)->SetLogz();
   }
   
   gStyle->SetOptStat("neiou");
   if(!cScript->GetShowEditor() )cScript->ToggleEditor();
   if(!cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   //========================== Plot
   
   /****
   cScript->cd(1);
   tree0->Draw("trdt[1]:trdt[0]>>rdt1(300, 0, 4000, 300, 0, 8000)", "", "colz");
   cut[0]->Draw("same");
   
   cScript->cd(2);
   tree0->Draw("trdt[3]:trdt[2]>>rdt2(300, 0, 4000, 300, 0, 8000)", "", "colz");
   cut[1]->Draw("same");
   
   cScript->cd(3);
   tree0->Draw("trdt[5]:trdt[4]>>rdt3(300, 0, 4000, 300, 0, 8000)", "", "colz");
   cut[2]->Draw("same");
  
   cScript->cd(4);
   tree0->Draw("trdt[7]:trdt[6]>>rdt4(300, 0, 4000, 300, 0, 8000)", "", "colz");
   cut[3]->Draw("same");
   
   //===================================
   cScript->cd(5);
   //tree0->Draw("e:z >> k1(400, -500,  0, 400, 0, 10)", gate_RDT + "&& detID < 6", "colz");
   //tree1->Draw("e:z>> k1", "hit==1 && ExID <= 2 && loop == 1", "colz");
   
   tree0->Draw("e:z >> h1(400, 590, 900, 400, 0, 20)", "", "colz");
   //gList->At(10)->Draw("same");
   //gList->At(15)->Draw("same");
   //gList->At(20)->Draw("same");
   //tree0->Draw("rdtID:detID>>w(24, 0, 24, 8, 0, 8)", gate_RDT + "&& detID > -1 && rdtID >= 0", "colz");
   
   
   cScript->cd(6);
   //tree0->Draw("e:z >> k2(100, -500, 0, 400, 0, 10)", gate_RDT + "&& 6 <= detID && detID < 12", "colz");
   tree0->Draw("e:z >> h(400, 590,  900, 400, 0, 20)", gate_RDT , "colz");
   //tree1->Draw("e:z>> k1", "hit==1 && ExID == 0", "colz");
   
   //gList->At(10)->Draw("same");
   //gList->At(15)->Draw("same");
   //gList->At(20)->Draw("same");
   
   //fxList->At(0)->Draw("same"); // 0.000
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
   tree0->Draw("Ex >> h2(180, -4, 8)", gate_RDT , "");
   
   cScript->cd(8);
   //tree0->Draw("e:coin_t >> j(100, -100, 100, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");
   tree0->Draw("coinTime:z >> j1(400, 590, 900, 400, -100, 200)", gate_RDT + "&& int(detID/5) == 1", "colz");
   
   cScript->cd(9);
   //tree0->Draw("e:coin_t >> j(100, -100, 100, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");
   tree0->Draw("coinTime:z >> j2(400, 590, 900, 400, -100, 200)", gate_RDT + "&& int(detID/5) == 2", "colz");
   
   cScript->cd(10);
   //tree0->Draw("e:coin_t >> j(100, -100, 100, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");
   tree0->Draw("coinTime:z >> j3(400, 590, 900, 400, -100, 200)", gate_RDT + "&& int(detID/5) == 3", "colz");
   
   cScript->cd(11);
   //tree0->Draw("e:coin_t >> j(100, -100, 100, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");
   tree0->Draw("coinTime:z >> j4(400, 590, 900, 400, -100, 200)", gate_RDT + "&& int(detID/5) == 4", "colz");
   
   cScript->cd(12);
   //tree0->Draw("e:coin_t >> j(100, -100, 100, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");
   tree0->Draw("coinTime:z >> j5(400, 590, 900, 400, -100, 200)", gate_RDT + "&& int(detID/5) == 5", "colz");
   
   /**/
   
   
   //======================== coinTime, RDT
   
   ///cScript->cd(2); tree0->Draw("coinTime:z>>h1(400, 590, 900, 400, -100, 200)", "int(detID/5)==1", "colz");
   ///cScript->cd(3); tree0->Draw("coinTime:z>>h2(400, 590, 900, 400, -100, 200)", "int(detID/5)==2", "colz");
   ///cScript->cd(6); tree0->Draw("coinTime:z>>h3(400, 590, 900, 400, -100, 200)", "int(detID/5)==3", "colz");
   ///cScript->cd(5); tree0->Draw("coinTime:z>>h4(400, 590, 900, 400, -100, 200)", "int(detID/5)==4", "colz");
   ///cScript->cd(4); tree0->Draw("coinTime:z>>h5(400, 590, 900, 400, -100, 200)", "int(detID/5)==5", "colz");
   
   
   ///cScript->cd(1);  tree0->Draw("coinTimeUC:x>>j1(400, -2, 2, 400, -100, 200)", "detID == 5 && cut1", "colz");
   ///
   ///TObjArray * cfList = (TObjArray *) file0->FindObjectAny("fList");
   ///cfList->At(5)->Draw("same");
   ///cfList->At(5)->Print();
   ///TF1 * fit = (TF1*) cfList->At(5);
   ///for( int i = 0; i < 8; i++){
   ///   printf("a%i: %f \n", i, fit->GetParameter(i));
   ///}
   
   
   cScript->cd(1); tree0->Draw("trdt[1]:trdt[0]>>h1(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[0]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 
   cScript->cd(2); tree0->Draw("trdt[3]:trdt[2]>>h2(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[2]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 
   cScript->cd(3); tree0->Draw("trdt[5]:trdt[4]>>h3(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[4]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 
   cScript->cd(4); tree0->Draw("trdt[7]:trdt[6]>>h4(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[6]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 


   cScript->cd(5); tree0->Draw("trdt_r[0]:trdt[0]>>k1(400, 0, 4000, 400, 0, 100)", " rdtdEMultiHit ==1 && trdt_r[0]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 
   cScript->cd(6); tree0->Draw("trdt_r[2]:trdt[2]>>k2(400, 0, 4000, 400, 0, 100)", " rdtdEMultiHit ==1 && trdt_r[2]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 
   cScript->cd(7); tree0->Draw("trdt_r[4]:trdt[4]>>k3(400, 0, 4000, 400, 0, 100)", " rdtdEMultiHit ==1 && trdt_r[4]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 
   cScript->cd(8); tree0->Draw("trdt_r[6]:trdt[6]>>k4(400, 0, 4000, 400, 0, 100)", " rdtdEMultiHit ==1 && trdt_r[6]<50 && abs(coinTime-17.5)<8 && abs(Ex) < 1", "colz"); 

   
   
   ///cScript->cd(1); tree0->Draw("trdt[1]:trdt[0]>>h1(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[0]<50 ", "colz"); 
   ///cScript->cd(2); tree0->Draw("trdt[3]:trdt[2]>>h2(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[2]<50 ", "colz"); 
   ///cScript->cd(3); tree0->Draw("trdt[5]:trdt[4]>>h3(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[4]<50 ", "colz"); 
   ///cScript->cd(4); tree0->Draw("trdt[7]:trdt[6]>>h4(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[6]<50 ", "colz"); 

   ///cScript->cd(1); tree0->Draw("trdt[1]:trdt[0]>>h1(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[0]<50 && cut0 && abs(coinTime-17.5)<8", "colz"); 
   ///cScript->cd(2); tree0->Draw("trdt[3]:trdt[2]>>h2(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[2]<50 && cut1 && abs(coinTime-17.5)<8", "colz"); 
   ///cScript->cd(3); tree0->Draw("trdt[5]:trdt[4]>>h3(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[4]<50 && cut2 && abs(coinTime-17.5)<8", "colz"); 
   ///cScript->cd(4); tree0->Draw("trdt[7]:trdt[6]>>h4(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[6]<50 && cut3 && abs(coinTime-17.5)<8", "colz"); 


   ///cScript->cd(1); tree0->Draw("trdt[1]:trdt[0]>>h1(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[0]<50 && abs(coinTime-17.5)<8 ", "colz"); 
   ///cScript->cd(2); tree0->Draw("trdt[3]:trdt[2]>>h2(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[2]<50 && abs(coinTime-17.5)<8 ", "colz"); 
   ///cScript->cd(3); tree0->Draw("trdt[5]:trdt[4]>>h3(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[4]<50 && abs(coinTime-17.5)<8 ", "colz"); 
   ///cScript->cd(4); tree0->Draw("trdt[7]:trdt[6]>>h4(400, 0, 4000, 400, 0, 7000)", " rdtdEMultiHit ==1 && trdt_r[6]<50 && abs(coinTime-17.5)<8 ", "colz"); 

   
   ///cScript->cd(1); tree0->Draw("coinTime>>h1(400, -50, 200)", " cut0", ""); 
   ///cScript->cd(2); tree0->Draw("coinTime>>h2(400, -50, 200)", " cut1", ""); 
   ///cScript->cd(3); tree0->Draw("coinTime>>h3(400, -50, 200)", " cut2", ""); 
   ///cScript->cd(4); tree0->Draw("coinTime>>h4(400, -50, 200)", " cut3", ""); 
   
   ///cScript->cd(1); tree0->Draw("coinTime>>h1(400, -50, 200)", "int(detID/5)==1", ""); 
   ///cScript->cd(2); tree0->Draw("coinTime>>h2(400, -50, 200)", "int(detID/5)==2", ""); 
   ///cScript->cd(3); tree0->Draw("coinTime>>h3(400, -50, 200)", "int(detID/5)==3", ""); 
   ///cScript->cd(4); tree0->Draw("coinTime>>h4(400, -50, 200)", "int(detID/5)==4", ""); 
   ///cScript->cd(5); tree0->Draw("coinTime>>h5(400, -50, 200)", "int(detID/5)==5", ""); 
   
   
   ///cScript->cd(1);  tree0->Draw("coinTime:z>>j1(400, 590, 900, 400, -100, 200)", "int(detID/5)==1", "colz");
   ///cScript->cd(2);  tree0->Draw("coinTime:z>>j2(400, 590, 900, 400, -100, 200)", "int(detID/5)==2", "colz");
   ///cScript->cd(3);  tree0->Draw("coinTime:z>>j3(400, 590, 900, 400, -100, 200)", "int(detID/5)==3", "colz");
   ///cScript->cd(4);  tree0->Draw("coinTime:z>>j4(400, 590, 900, 400, -100, 200)", "int(detID/5)==4", "colz");
   ///cScript->cd(5);  tree0->Draw("coinTime:z>>j5(400, 590, 900, 400, -100, 200)", "int(detID/5)==5", "colz");
   
   ///cScript->cd(1);  tree0->Draw("coinTime:z>>h1(400, 590, 900, 400, -100, 200)", "int(detID/5)==1 && cut1", "colz");
   ///cScript->cd(2);  tree0->Draw("coinTime:z>>h2(400, 590, 900, 400, -100, 200)", "int(detID/5)==2 && cut1", "colz");
   ///cScript->cd(3);  tree0->Draw("coinTime:z>>h3(400, 590, 900, 400, -100, 200)", "int(detID/5)==3 && cut1", "colz");
   ///cScript->cd(4);  tree0->Draw("coinTime:z>>h4(400, 590, 900, 400, -100, 200)", "int(detID/5)==4 && cut1", "colz");
   ///cScript->cd(5);  tree0->Draw("coinTime:z>>h5(400, 590, 900, 400, -100, 200)", "int(detID/5)==5 && cut1", "colz");
   
   ///cScript->cd(1); tree0->Draw("coinTime:z>>k1(400, 590, 900, 400, -100, 200)", "int(detID/5)==1 && cut2", "colz");
   ///cScript->cd(2); tree0->Draw("coinTime:z>>k2(400, 590, 900, 400, -100, 200)", "int(detID/5)==2 && cut2", "colz");
   ///cScript->cd(3); tree0->Draw("coinTime:z>>k3(400, 590, 900, 400, -100, 200)", "int(detID/5)==3 && cut2", "colz");
   ///cScript->cd(4); tree0->Draw("coinTime:z>>k4(400, 590, 900, 400, -100, 200)", "int(detID/5)==4 && cut2", "colz");
   ///cScript->cd(5); tree0->Draw("coinTime:z>>k5(400, 590, 900, 400, -100, 200)", "int(detID/5)==5 && cut2", "colz");
   
   ///cScript->cd(1); tree0->Draw("coinTime:z>>p1(400, 590, 900, 400, -100, 200)", "int(detID/5)==1 && cut3", "colz");
   ///cScript->cd(2); tree0->Draw("coinTime:z>>p2(400, 590, 900, 400, -100, 200)", "int(detID/5)==2 && cut3", "colz");
   ///cScript->cd(3); tree0->Draw("coinTime:z>>p3(400, 590, 900, 400, -100, 200)", "int(detID/5)==3 && cut3", "colz");
   ///cScript->cd(4); tree0->Draw("coinTime:z>>p4(400, 590, 900, 400, -100, 200)", "int(detID/5)==4 && cut3", "colz");
   ///cScript->cd(5); tree0->Draw("coinTime:z>>p5(400, 590, 900, 400, -100, 200)", "int(detID/5)==5 && cut3", "colz");
   
   
   ///cScript->cd(6);
   ///TH1F * h1 = new TH1F("h1", "det-5 coinTime & cut1", 400, -100, 200);
   ///h1->SetLineColor(2);
   ///tree0->Draw("coinTime>>h1", "detID==5 && cut1", "");   
   
   //==========================================  E-Z and Ex
   
   ///cScript->cd(1); tree0->Draw("e:z>>h1(400, 590, 900, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17.5)< 8 && Ex > -1 && thetaCM > 10", "colz");
   ///fxList->At(0)->Draw("same");
   ///fxList->At(1)->Draw("same");
   ///fxList->At(2)->Draw("same");
   ///gList->At(0)->Draw("same");
   ///
   ///cScript->cd(2); tree0->Draw("Ex>>j1(200, -2, 9)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17.5)< 8", "colz");
   
   ///cScript->cd(2); tree0->Draw("Ex>>j1(200, -2, 9)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17.5)< 8 && int(detID/5)==1", "colz");
   ///cScript->cd(3); tree0->Draw("Ex>>j2(200, -2, 9)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17.5)< 8 && int(detID/5)==2", "colz");
   ///cScript->cd(4); tree0->Draw("Ex>>j3(200, -2, 9)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17.5)< 8 && int(detID/5)==3", "colz");
   ///cScript->cd(5); tree0->Draw("Ex>>j4(200, -2, 9)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17.5)< 8 && int(detID/5)==4", "colz");
   ///cScript->cd(6); tree0->Draw("Ex>>j5(200, -2, 9)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17.5)< 8 && int(detID/5)==5", "colz");
   
   ///cScript->cd(1); tree0->Draw("e:z>>h1(400, 590, 900, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && int(detID/5)==1 && abs(coinTime)< 13", "colz");
   ///cScript->cd(2); tree0->Draw("e:z>>h2(400, 590, 900, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && int(detID/5)==2 && abs(coinTime)< 13", "colz");
   ///cScript->cd(3); tree0->Draw("e:z>>h3(400, 590, 900, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && int(detID/5)==3 && abs(coinTime)< 13", "colz");
   ///cScript->cd(4); tree0->Draw("e:z>>h4(400, 590, 900, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && int(detID/5)==4 && abs(coinTime)< 13", "colz");
   ///cScript->cd(5); tree0->Draw("e:z>>h5(400, 590, 900, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && int(detID/5)==5 && abs(coinTime)< 13", "colz");
   
   
   
}
