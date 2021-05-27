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
void Aux(int cutID, TTree *tree0, TCutG ** cut_in, TCanvas * cScript, int startPadID, TObjArray * fxList, TObjArray * gList);

TChain * tree0 ;
TString gate_RDT;
TString gate_time;

void script(){

   //========================== input file/cut/simulation   
   tree0 = new TChain("tree");
   ///tree0->Add("A_trace_run055_noCoinCorr.root");
   ///tree0->Add("A_trace_run101_noCoinCorr.root");
   tree0->Add("A_trace_run055-057_059-085_087-099.root");
   tree0->Add("A_trace_run101-144_147-165_167-214.root");
   
   tree0->GetListOfFiles()->Print();
   
   TString fileName = "run056 - run099";
   
   TFile * file1 = new TFile("transfer.root");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray*) file1->FindObjectAny("gList");
   
   TString rdtCutFile = "rdtCuts_15C_tight.root";
   //TString rdtCutFile = "rdtCuts_14C.root";
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
   
   
   TString ezCutFile = "EZCut.root";
   TFile * fileEZCut = new TFile(ezCutFile);
   TCutG * ezCut = (TCutG *) fileEZCut->FindObjectAny("cutez"); 
   
   //========================== Gate
   gate_RDT = "(cut0 || cut1 || cut2 || cut3) && multiHit == 1 && rdtdEMultiHit == 1";
   gate_RDT = "(cut0 || cut1 || cut2 || cut3) ";
   gate_time = "abs(coinTime-15)<9 && Ex > -2";  ///change time gate
 //   TString gate_time = "abs(coinTime)<200";  
   //========================== Canvas
   
   
   int div[2] = {1,1}; // x,y
   int padSize = 600;
   
   TCanvas * cScript = new TCanvas("cScript", "cScript : " + fileName + "|" + rdtCutFile, 0, 0, padSize * div[0], padSize * div[1]);
   cScript->Divide(div[0], div[1]);
   for( int i = 1; i <= div[0] * div[1] ; i++){
      cScript->cd(i)->SetGrid();
      ///cScript->cd(i)->SetLogz();
   }
   
   gStyle->SetOptStat("neiou");
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   //########################################### Plot
   
   tree0->Process("Analyzer.C+");
   
   ///tree0->Process("Tree4NN.C+");
   
   //SetCanvas(2,1, 400, cScript);
   //
   //cScript->cd(1); tree0->Draw("e:detID>>h1(30, 0, 30, 400, 8, 12)",  "11>e && e>9", "colz");
   //cScript->cd(2); tree0->Draw("det>>h2(30, 0, 30)", "11>e && e>9 && detID%5==0", "colz");
   
   //cScript->cd(2); tree0->Draw("e:z>>h2(400, 700, 1000, 400, 8, 12)", "11>e && e>9 && 10 > detID && detID >=  5", "colz");
   //cScript->cd(3); tree0->Draw("e:z>>h3(400, 700, 1000, 400, 8, 12)", "11>e && e>9 && 15 > detID && detID >= 10", "colz");
   //cScript->cd(4); tree0->Draw("e:z>>h4(400, 700, 1000, 400, 8, 12)", "11>e && e>9 && 20 > detID && detID >= 15", "colz");
   //cScript->cd(5); tree0->Draw("e:z>>h5(400, 700, 1000, 400, 8, 12)", "11>e && e>9 && 25 > detID && detID >= 20", "colz");
   //cScript->cd(6); tree0->Draw("e:z>>h6(400, 700, 1000, 400, 8, 12)", "11>e && e>9 && 30 > detID && detID >= 25", "colz");
   
   
   //======================= recoi-ID vs det-ID
   
   ///SetCanvas(4,1, 400, cScript);
   ///
   ///cScript->cd(1); tree0->Draw("detID:rdtID>>h1(8, 0, 8, 30, 0, 30)", gate_time + " && cut0" , "colz");
   ///cScript->cd(2); tree0->Draw("detID:rdtID>>h2(8, 0, 8, 30, 0, 30)", gate_time + " && cut1" , "colz");
   ///cScript->cd(3); tree0->Draw("detID:rdtID>>h3(8, 0, 8, 30, 0, 30)", gate_time + " && cut2" , "colz");
   ///cScript->cd(4); tree0->Draw("detID:rdtID>>h4(8, 0, 8, 30, 0, 30)", gate_time + " && cut3" , "colz");
   
   
   //======================= Trace of recoil
   
   ///SetCanvas(4, 2, 400, cScript);
   ///
   ///cScript->cd(1);  tree0->Draw("rdt[1]:rdt[0]>>rdt1(300, 0, 4000, 300, 0, 6000)", "abs(coinTime-17)<9 && thetaCM > 10", "colz"); if( fileCut->IsOpen()){cut_in[0]->Draw("same");}
   ///cScript->cd(2);  tree0->Draw("rdt[3]:rdt[2]>>rdt2(300, 0, 4000, 300, 0, 6000)", "abs(coinTime-17)<9 && thetaCM > 10", "colz"); if( fileCut->IsOpen()){cut_in[1]->Draw("same");}
   ///cScript->cd(3);  tree0->Draw("rdt[5]:rdt[4]>>rdt3(300, 0, 4000, 300, 0, 6000)", "abs(coinTime-17)<9 && thetaCM > 10", "colz"); if( fileCut->IsOpen()){cut_in[2]->Draw("same");}
   ///cScript->cd(4);  tree0->Draw("rdt[7]:rdt[6]>>rdt4(300, 0, 4000, 300, 0, 6000)", "abs(coinTime-17)<9 && thetaCM > 10", "colz"); if( fileCut->IsOpen()){cut_in[3]->Draw("same");}
   ///
   ///cScript->cd(5); tree0->Draw("Ex>>h1(400, -1,2)", "abs(coinTime-17)<9 && thetaCM>10");
   
   
   ///cScript->cd(1);  tree0->Draw("rdt[1]:rdt[0]>>rdt1(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[0]->Draw("same");}
   ///cScript->cd(2);  tree0->Draw("rdt[3]:rdt[2]>>rdt2(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[1]->Draw("same");}
   ///cScript->cd(3);  tree0->Draw("rdt[5]:rdt[4]>>rdt3(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[2]->Draw("same");}
   ///cScript->cd(4);  tree0->Draw("rdt[7]:rdt[6]>>rdt4(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[3]->Draw("same");}
   /// 
   ///cScript->cd(5);  tree0->Draw("trdt[1]:trdt[0]>>trdt1(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[0]->Draw("same");}
   ///cScript->cd(6);  tree0->Draw("trdt[3]:trdt[2]>>trdt2(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[1]->Draw("same");}
   ///cScript->cd(7);  tree0->Draw("trdt[5]:trdt[4]>>trdt3(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[2]->Draw("same");}
   ///cScript->cd(8);  tree0->Draw("trdt[7]:trdt[6]>>trdt4(300, 0, 4000, 300, 0, 6000)", "", "colz"); if( fileCut->IsOpen()){cut_in[3]->Draw("same");}
   ///
   ///cScript->cd(9);  tree0->Draw("trdt[1]:trdt[0]>>trdt1g(300, 0, 4000, 300, 0, 6000)", "trdt_r[0]<20", "colz");
   ///cScript->cd(10); tree0->Draw("trdt[3]:trdt[2]>>trdt2g(300, 0, 4000, 300, 0, 6000)", "trdt_r[2]<20", "colz");
   ///cScript->cd(11); tree0->Draw("trdt[5]:trdt[4]>>trdt3g(300, 0, 4000, 300, 0, 6000)", "trdt_r[4]<20", "colz");
   ///cScript->cd(12); tree0->Draw("trdt[7]:trdt[6]>>trdt4g(300, 0, 4000, 300, 0, 6000)", "trdt_r[6]<20", "colz");
   
   ///SetCanvas(2, 2, 400, cScript);
   ///
   ///cScript->cd(1);    
   ///tree0->Draw("trdt[1]:trdt[0]>>trdt1(300, 0, 4000, 300, 0, 6000)", gate_time , "colz");
   ///cScript->cd(2);    
   ///tree0->Draw("trdt[1]:trdt[0]>>trdt1g(300, 0, 4000, 300, 0, 6000)", gate_time + " && cut0", "colz");
   ///
   ///cScript->cd(3);    
   ///tree0->Draw("trdt[1]:trdt[0]>>trdt2(300, 0, 4000, 300, 0, 6000)", gate_time + " && 5 <= detID && detID <= 9", "colz");
   ///cScript->cd(4);    
   ///tree0->Draw("trdt[1]:trdt[0]>>trdt2g(300, 0, 4000, 300, 0, 6000)", gate_time + "  && 5 <= detID && detID <= 9 && cut0", "colz");
   
   ///cScript->cd(2);    
   ///tree0->Draw("trdt[3]:trdt[2]>>trdt2(300, 0, 4000, 300, 0, 6000)", gate_time , "colz");
   ///tree0->Draw("trdt[3]:trdt[2]>>trdt2g(300, 0, 4000, 300, 0, 6000)", gate_time + " && cut1", "same");
   ///
   ///cScript->cd(3); 
   ///tree0->Draw("trdt[5]:trdt[4]>>trdt3(300, 0, 4000, 300, 0, 6000)", gate_time , "colz");
   ///tree0->Draw("trdt[5]:trdt[4]>>trdt3g(300, 0, 4000, 300, 0, 6000)", gate_time + " && cut2", "same");
   ///
   ///cScript->cd(4); 
   ///tree0->Draw("trdt[7]:trdt[6]>>trdt4(300, 0, 4000, 300, 0, 6000)", gate_time , "colz");
   ///tree0->Draw("trdt[7]:trdt[6]>>trdt4g(300, 0, 4000, 300, 0, 6000)", gate_time + " && cut3", "same");
   
   //======================== coinTime, RDT
   
   ///cScript->cd(2); tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "(cut0 || cut1 || cut2 || cut3) && detID==15", "colz");
   
   
   ///SetCanvas(3,2, 400, cScript);
   
   ///cScript->cd(2); tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "int(detID/5)==1", "colz");
   ///cScript->cd(3); tree0->Draw("coinTime:z>>h2(400, 700, 1000, 400, -50, 200)", "int(detID/5)==2", "colz");
   ///cScript->cd(6); tree0->Draw("coinTime:z>>h3(400, 700, 1000, 400, -50, 200)", "int(detID/5)==3", "colz");
   ///cScript->cd(5); tree0->Draw("coinTime:z>>h4(400, 700, 1000, 400, -50, 200)", "int(detID/5)==4", "colz");
   ///cScript->cd(4); tree0->Draw("coinTime:z>>h5(400, 700, 1000, 400, -50, 200)", "int(detID/5)==5", "colz");
   
   
   ///cScript->cd(2); tree0->Draw("coinTime:z>>h1(400, 700, 1000, 400, -50, 200)", "(cut0 || cut1 || cut2 || cut3) && int(detID/5)==1", "colz");
   ///cScript->cd(3); tree0->Draw("coinTime:z>>h2(400, 700, 1000, 400, -50, 200)", "(cut0 || cut1 || cut2 || cut3) && int(detID/5)==2", "colz");
   ///cScript->cd(6); tree0->Draw("coinTime:z>>h3(400, 700, 1000, 400, -50, 200)", "(cut0 || cut1 || cut2 || cut3) && int(detID/5)==3", "colz");
   ///cScript->cd(5); tree0->Draw("coinTime:z>>h4(400, 700, 1000, 400, -50, 200)", "(cut0 || cut1 || cut2 || cut3) && int(detID/5)==4", "colz");
   ///cScript->cd(4); tree0->Draw("coinTime:z>>h5(400, 700, 1000, 400, -50, 200)", "(cut0 || cut1 || cut2 || cut3) && int(detID/5)==5", "colz");
   
   
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
   
   //SetCanvas(6,4, 300, cScript);
   //
   //Aux(0, tree0, cut_in, cScript,  1, fxList, gList);
   //Aux(1, tree0, cut_in, cScript,  7, fxList, gList);
   //Aux(2, tree0, cut_in, cScript, 13, fxList, gList);
   //Aux(3, tree0, cut_in, cScript, 19, fxList, gList);

   //==========================================  E-Z and Ex
   
   ///SetCanvas(3,1, 500, cScript);
   ///cScript->cd(1); tree0->Draw("e:z>>h1(400, 700, 1000, 400, 0, 20)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17)< 9 && Ex > -2 && thetaCM > 10", "colz");
   ///ezCut->Draw("same");
   ///fxList->At(0)->Draw("same");
   ///fxList->At(1)->Draw("same");
   ///gList->At(0)->Draw("same");
   ///cScript->cd(2); tree0->Draw("Ex:thetaCM>>h2(400, 0, 50, 400, -2, 5)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17)< 9 && Ex > -2 && thetaCM > 10", "colz");
   ///cScript->cd(3); tree0->Draw("Ex>>h3(70, -2, 5)", "(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17)< 9 && Ex > -2 && thetaCM > 10", "colz");
   
   ///TString gate ;
   ///TString express ;
   ///for( int i = 5; i < 30 ; i++){
   ///  cScript->cd(i-4); 
   ///  express.Form("Ex>>h%d(200, -2, 8)", i);
   ///  gate.Form("(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17)< 9 && Ex > -2 && thetaCM > 10 && detID == %d", i);
   ///  tree0->Draw(express, gate, "");
   ///}
   
   ///SetCanvas(2,2, 500, cScript);
   ///cScript->cd(1); tree0->Draw("e:z>>h1(100, 700, 770, 100, 3, 6)", "(cut0) && Ex < 0.5 && " + gate_time, ""); fxList->At(0)->Draw("same");
   ///cScript->cd(2); tree0->Draw("e:z>>h2(100, 700, 770, 100, 3, 6)", "(cut1) && Ex < 0.5 && " + gate_time, ""); fxList->At(0)->Draw("same");
   ///cScript->cd(3); tree0->Draw("e:z>>h3(100, 700, 770, 100, 3, 6)", "(cut2) && Ex < 0.5 && " + gate_time, ""); fxList->At(0)->Draw("same");
   ///cScript->cd(4); tree0->Draw("e:z>>h4(100, 700, 770, 100, 3, 6)", "(cut3) && Ex < 0.5 && " + gate_time, ""); fxList->At(0)->Draw("same");
   
   ///cScript->cd(1); tree0->Draw("Ex>>h1(100, -2, 3)", "(cut0) && " + gate_time, "");
   ///cScript->cd(2); tree0->Draw("Ex>>h2(100, -2, 3)", "(cut1) && " + gate_time, "");
   ///cScript->cd(3); tree0->Draw("Ex>>h3(100, -2, 3)", "(cut2) && " + gate_time, "");
   ///cScript->cd(4); tree0->Draw("Ex>>h4(100, -2, 3)", "(cut3) && " + gate_time, "");
   
   //========================================= EZ-cut
   
   ///SetCanvas( 4, 4, 400, cScript);
   
   ///cScript->cd(1); tree0->Draw("trdt[1]:trdt[0]>>trdt1(300, 0, 4000, 300, 0, 6000)", "", "colz");
   ///cScript->cd(5); tree0->Draw("trdt[3]:trdt[2]>>trdt2(300, 0, 4000, 300, 0, 6000)", "", "colz");
   ///cScript->cd(9); tree0->Draw("trdt[5]:trdt[4]>>trdt3(300, 0, 4000, 300, 0, 6000)", "", "colz");
   ///cScript->cd(13); tree0->Draw("trdt[7]:trdt[6]>>trdt4(300, 0, 4000, 300, 0, 6000)", "", "colz");
   ///
   ///cScript->cd(2);  tree0->Draw("trdt[1]:trdt[0]>>trdt1g(300, 0, 4000, 300, 0, 6000)", "abs(coinTime - 17)< 9", "colz"); cut_in[0]->Draw("same");
   ///cScript->cd(6);  tree0->Draw("trdt[3]:trdt[2]>>trdt2g(300, 0, 4000, 300, 0, 6000)", "abs(coinTime - 17)< 9", "colz"); cut_in[1]->Draw("same");
   ///cScript->cd(10); tree0->Draw("trdt[5]:trdt[4]>>trdt3g(300, 0, 4000, 300, 0, 6000)", "abs(coinTime - 17)< 9", "colz"); cut_in[2]->Draw("same");
   ///cScript->cd(14); tree0->Draw("trdt[7]:trdt[6]>>trdt4g(300, 0, 4000, 300, 0, 6000)", "abs(coinTime - 17)< 9", "colz"); cut_in[3]->Draw("same");
   ///
   ///
   ///cScript->cd(3); tree0->Draw("trdt[1]:trdt[0]>>trdt1a(300, 0, 4000, 300, 0, 6000)", "cutez", "colz"); cut_in[0]->Draw("same");
   ///cScript->cd(7); tree0->Draw("trdt[3]:trdt[2]>>trdt2a(300, 0, 4000, 300, 0, 6000)", "cutez", "colz"); cut_in[1]->Draw("same");
   ///cScript->cd(11); tree0->Draw("trdt[5]:trdt[4]>>trdt3a(300, 0, 4000, 300, 0, 6000)", "cutez", "colz"); cut_in[2]->Draw("same");
   ///cScript->cd(15); tree0->Draw("trdt[7]:trdt[6]>>trdt4a(300, 0, 4000, 300, 0, 6000)", "cutez", "colz"); cut_in[3]->Draw("same");
   ///
   ///cScript->cd(4);  tree0->Draw("trdt[1]:trdt[0]>>trdt1ga(300, 0, 4000, 300, 0, 6000)", "cutez & abs(coinTime - 17)< 9", "colz"); cut_in[0]->Draw("same");
   ///cScript->cd(8);  tree0->Draw("trdt[3]:trdt[2]>>trdt2ga(300, 0, 4000, 300, 0, 6000)", "cutez & abs(coinTime - 17)< 9", "colz"); cut_in[1]->Draw("same");
   ///cScript->cd(12); tree0->Draw("trdt[5]:trdt[4]>>trdt3ga(300, 0, 4000, 300, 0, 6000)", "cutez & abs(coinTime - 17)< 9", "colz"); cut_in[2]->Draw("same");
   ///cScript->cd(16); tree0->Draw("trdt[7]:trdt[6]>>trdt4ga(300, 0, 4000, 300, 0, 6000)", "cutez & abs(coinTime - 17)< 9", "colz"); cut_in[3]->Draw("same");
   
   
   
   ///cScript->cd(1);  tree0->Draw("coinTime:Ex>>h1(100, -5, 8, 400, -20, 60)", "", "colz");
   ///cScript->cd(5);  tree0->Draw("coinTime:Ex>>h2(100, -5, 8, 400, -20, 60)", "", "colz");
   ///cScript->cd(9);  tree0->Draw("coinTime:Ex>>h3(100, -5, 8, 400, -20, 60)", "", "colz");
   ///cScript->cd(13); tree0->Draw("coinTime:Ex>>h4(100, -5, 8, 400, -20, 60)", "", "colz");
   ///
   ///cScript->cd(2);  tree0->Draw("coinTime:Ex>>h1g(100, -5, 8, 400, -20, 60)", "cut0", "colz");
   ///cScript->cd(6);  tree0->Draw("coinTime:Ex>>h2g(100, -5, 8, 400, -20, 60)", "cut1", "colz");
   ///cScript->cd(10); tree0->Draw("coinTime:Ex>>h3g(100, -5, 8, 400, -20, 60)", "cut2", "colz");
   ///cScript->cd(14); tree0->Draw("coinTime:Ex>>h4g(100, -5, 8, 400, -20, 60)", "cut3", "colz");
   ///
   ///cScript->cd(3);  tree0->Draw("coinTime:Ex>>h1a(100, -5, 8, 400, -20, 60)", "cutez", "colz");
   ///cScript->cd(7);  tree0->Draw("coinTime:Ex>>h2a(100, -5, 8, 400, -20, 60)", "cutez", "colz");
   ///cScript->cd(11); tree0->Draw("coinTime:Ex>>h3a(100, -5, 8, 400, -20, 60)", "cutez", "colz");
   ///cScript->cd(15); tree0->Draw("coinTime:Ex>>h4a(100, -5, 8, 400, -20, 60)", "cutez", "colz");
   ///
   ///cScript->cd(4);  tree0->Draw("coinTime:Ex>>h1ga(100, -5, 8, 400, -20, 60)", "cutez & cut0", "colz");
   ///cScript->cd(8);  tree0->Draw("coinTime:Ex>>h2ga(100, -5, 8, 400, -20, 60)", "cutez & cut1", "colz");
   ///cScript->cd(12); tree0->Draw("coinTime:Ex>>h3ga(100, -5, 8, 400, -20, 60)", "cutez & cut2", "colz");
   ///cScript->cd(16); tree0->Draw("coinTime:Ex>>h4ga(100, -5, 8, 400, -20, 60)", "cutez & cut3", "colz");
   

   ///cScript->cd(1);  tree0->Draw("coinTime>>h1(200, -20, 60)", "", "colz");
   ///cScript->cd(5);  tree0->Draw("coinTime>>h2(200, -20, 60)", "", "colz");
   ///cScript->cd(9);  tree0->Draw("coinTime>>h3(200, -20, 60)", "", "colz");
   ///cScript->cd(13); tree0->Draw("coinTime>>h4(200, -20, 60)", "", "colz");
   ///                                     
   ///cScript->cd(2);  tree0->Draw("coinTime>>h1g(200, -20, 60)", "cut0", "colz");
   ///cScript->cd(6);  tree0->Draw("coinTime>>h2g(200, -20, 60)", "cut1", "colz");
   ///cScript->cd(10); tree0->Draw("coinTime>>h3g(200, -20, 60)", "cut2", "colz");
   ///cScript->cd(14); tree0->Draw("coinTime>>h4g(200, -20, 60)", "cut3", "colz");
   ///                                     
   ///cScript->cd(3);  tree0->Draw("coinTime>>h1a(200, -20, 60)", "cutez", "colz");
   ///cScript->cd(7);  tree0->Draw("coinTime>>h2a(200, -20, 60)", "cutez", "colz");
   ///cScript->cd(11); tree0->Draw("coinTime>>h3a(200, -20, 60)", "cutez", "colz");
   ///cScript->cd(15); tree0->Draw("coinTime>>h4a(200, -20, 60)", "cutez", "colz");
   ///                                     
   ///cScript->cd(4);  tree0->Draw("coinTime>>h1ga(100, -20, 60)", "cutez & cut0", "colz");
   ///cScript->cd(8);  tree0->Draw("coinTime>>h2ga(100, -20, 60)", "cutez & cut1", "colz");
   ///cScript->cd(12); tree0->Draw("coinTime>>h3ga(100, -20, 60)", "cutez & cut2", "colz");
   ///cScript->cd(16); tree0->Draw("coinTime>>h4ga(100, -20, 60)", "cutez & cut3", "colz");
   
   
   //======================================== Ex vs thetaCM
   
   ///SetCanvas( 4, 1, 400, cScript);
   
   ///cScript->cd(1); tree0->Draw("Ex:z>>h1(400, 700, 1000, 400, -2, 5)", "cut0 && " + gate_time, "");
   ///cScript->cd(2); tree0->Draw("Ex:z>>h2(400, 700, 1000, 400, -2, 5)", "cut1 && " + gate_time, "");
   ///cScript->cd(3); tree0->Draw("Ex:z>>h3(400, 700, 1000, 400, -2, 5)", "cut2 && " + gate_time, "");
   ///cScript->cd(4); tree0->Draw("Ex:z>>h4(400, 700, 1000, 400, -2, 5)", "cut3 && " + gate_time, "");
   
   ///cScript->cd(1); tree0->Draw("Ex>>k1(70, -2, 5)", "cut0 && " + gate_time, "");
   ///cScript->cd(2); tree0->Draw("Ex>>k2(70, -2, 5)", "cut1 && " + gate_time, "");
   ///cScript->cd(3); tree0->Draw("Ex>>k3(70, -2, 5)", "cut2 && " + gate_time, "");
   ///cScript->cd(4); tree0->Draw("Ex>>k4(70, -2, 5)", "cut3 && " + gate_time, "");
   

   //========================================= histogram offset

   ///For H072_16N
   
   /**
   div[0] = 5; // x
   div[1] = 5; // y
   TCanvas * cJaJaJa = new TCanvas("cJaJaJa", "cJaJaJa : " + fileName + "|" + rdtCutFile, 0, 0, 200 * div[0], 200 * div[1]);
   cJaJaJa->Divide(div[0], div[1]);
   for( int i = 1; i <= div[0] * div[1] ; i++){
      cJaJaJa->cd(i)->SetGrid();
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
      ///gate.Form("(cut0 || cut1 || cut2 || cut3 ) && abs(coinTime - 17)< 9 && Ex > -2 && thetaCM > 10 && detID == %d", i);
      gate.Form("(cut3 ) && abs(coinTime - 17)< 9 && Ex > -2 && thetaCM > 10 && detID == %d", i);
      tree0->Draw(express, gate, "");
      legend->AddEntry(hh[i]);
   }
   
   THStack *hs = new THStack("hs","");
   int yMax = 0;
   for( int i = 29; i > 4 ; i--){
      if( ExOffset[i] > -1 ) {
         hs->Add(hh[i]);
         if( hh[i]->GetMaximum() > yMax ) yMax = hh[i]->GetMaximum();
      }
   }
   yMax = yMax * 1.2;
   
   for( int i = 5; i < 30 ; i++){
      cJaJaJa->cd(i-4); 
      hh[i]->SetMaximum(yMax);
      hh[i]->Draw("");
   }
   
   TCanvas * hahaha = new TCanvas( "hahaha", "Stacked Ex histogram & all", 1600, 600);
   hahaha->Divide(2,1);
   hahaha->cd(1);
   ///gStyle->SetPalette(kRainbow);
   hs->Draw("pfc");
   gStyle->SetOptStat("000000");
   legend->Draw();

   /**
   gStyle->SetOptStat("neiour");   
   TH1F * hAll = new TH1F("hAll", Form("%s | %s", fileName.Data(), rdtCutFile.Data()), (ExRange[1] - ExRange[0])/resol, ExRange[0], ExRange[1]);
   
   for( int i = 5; i < 30 ; i++){
      if( ExOffset[i] == -1 ) continue;
      hAll->Add(hh[i]);      
   }
   
   hahaha->cd(2);
   hAll->SetLineColor(4);
   hAll->SetLineWidth(3);
   hAll->SetXTitle(Form("Ex [%.1f keV]", resol * 1000));   
   hAll->Draw("same");

   /**
      
   TF1 * fit  = new TF1("fit" , "gaus(0)+gaus(3)",-1.,2.);  
   fit->SetLineColor(kRed);
   fit->SetParameter(0, 10);
   fit->SetParameter(1, 0);
   fit->SetParameter(2, 0.1);
   fit->SetParameter(3, 100);
   fit->SetParameter(4, 1.0);
   fit->SetParameter(5, 0.2);
   
   Double_t * par = new Double_t[6];
   Double_t * par_err = new Double_t[6];
   
   hAll->Fit("fit","R+");
   
   par = fit->GetParameters();
   par_err = (Double_t*) fit->GetParErrors();

   TF1 * fit1 = new TF1("fit1", "gaus", -1.,2.) ;
   TF1 * fit2 = new TF1("fit2", "gaus", -1.,2.); 

   fit1->SetLineColor(kBlack);
   fit2->SetLineColor(kBlack);
   
   fit1->SetParameter(0, par[0]);
   fit1->SetParameter(1, par[1]);
   fit1->SetParameter(2, par[2]);
   
   fit2->SetParameter(0, par[3]);
   fit2->SetParameter(1, par[4]);
   fit2->SetParameter(2, par[5]);
   
   TLatex text;
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.03);
   text.SetTextColor(2);
    
   text.DrawLatex(0.6, 0.65, " g.s ");
   text.DrawLatex(0.6, 0.60, Form(" mean : %5.3f(%5.3f) MeV", par[1], par_err[1]));
   text.DrawLatex(0.6, 0.55, Form("sigma : %5.3f(%5.3f) MeV", par[2], par_err[2]));
   text.DrawLatex(0.6, 0.50, Form(" FWHM : %5.3f(%5.3f) MeV", par[2]*2.355, par_err[2]*2.355)); 
    
   text.DrawLatex(0.6, 0.45, " isomeric ");
   text.DrawLatex(0.6, 0.40, Form(" mean : %5.3f(%5.3f) MeV", par[4], par_err[4]));
   text.DrawLatex(0.6, 0.35, Form("sigma : %5.3f(%5.3f) MeV", par[5], par_err[5]));
   text.DrawLatex(0.6, 0.30, Form(" FWHM : %5.3f(%5.3f) MeV", par[5]*2.355, par_err[5]*2.355));
   /***/
  
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

 
void Aux(int cutID, TTree *tree0, TCutG ** cut_in, TCanvas * cScript, int startPadID, TObjArray * fxList, TObjArray * gList){
   
   TString expression, gate= "";
   
   int coinTimeGate[2] = {17, 5}; //mean, half-width
   
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
   expression.Form("coinTime:Ex>>h%d(100, -5, 8, 400, -20, 60)", startPadID+1);
   tree0->Draw(expression, gate , "colz");
   
   ////========
   cScript->cd(startPadID+2); 
   expression.Form("trdt[%d]:trdt[%d]>>h%d(400, 0, 4000, 400, 0, 5000)", cutID*2+1, cutID*2, startPadID+2); 
   ///gate.Form("abs(coinTime-17)<9 && Ex > -2 && trdt_r[%d]<20", cutID*2);
   gate.Form("abs(coinTime-%d)<%d && Ex > -2 ", coinTimeGate[0], coinTimeGate[1]);
   //gate.Form("abs(coinTime-36)<10");//crh
   tree0->Draw(expression, gate, "box"); 
   cut_in[cutID]->Draw("same");
   
   ////========
   cScript->cd(startPadID+3); 
   ///gate.Form("cut%d  && trdt_r[%d]<20 ", cutID, cutID*2);
   gate.Form("cut%d ", cutID);
   //gate.Form("cut%d && abs(coinTime-14)<10 ", cutID);
   expression.Form("coinTime:Ex>>h%d(100, -5, 8, 400, -20, 60)", startPadID+3);
   tree0->Draw(expression, gate , "box");
   TBox* box = new TBox(-5, coinTimeGate[0]-coinTimeGate[1], 8, coinTimeGate[0]+coinTimeGate[1]);
   //box->SetFillColorAlpha(kRed, 0.1);
   box->SetFillStyle(0);
   box->SetLineColor(kRed);
   box->SetLineWidth(2);
   box->Draw("same");
   
   ////========
   cScript->cd(startPadID+4); 
   ///gate.Form("cut%d && abs(coinTime -17)< 9 && trdt_r[%d]<20 ", cutID, cutID*2);
   gate.Form("cut%d && abs(coinTime - %d)< %d", cutID, coinTimeGate[0],coinTimeGate[1]);
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
