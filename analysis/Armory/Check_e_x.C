#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TF1.h"
#include "TLatex.h"
#include <fstream>

void Check_e_x( TString rootFile = "temp.root",double eThreshold = 400){
/**///======================================================== User input
   
   const char* treeName="tree";
   double eRange[3]  = {400, 0, 4000};

/**///======================================================== read tree and create Canvas
   printf("################### Check_e_x.C ######################\n");
   
   TFile *file0 = new TFile (rootFile, "read"); 
   TTree *tree = (TTree*)file0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10lld \n", rootFile.Data(),  tree->GetEntries());
   
   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {200,230}; //x,y
   
   TCanvas * cCheck = new TCanvas("cCheck", "cCheck", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cCheck->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCheck->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(0);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   gStyle->SetLabelSize(0.05, "X");
   gStyle->SetLabelSize(0.05, "Y");
   gStyle->SetTitleFontSize(0.1);
   
   TFile * cutFile = new TFile("cut.root", "read");
   TCutG * cut = NULL;
   if( cutFile->IsOpen() ){
      cut = (TCutG *) cutFile->FindObjectAny("cutEZ");
      if( cut != NULL ) {
         printf("======== loaded TCutG with name : %s, X: %s, Y: %s \n", cut->GetName(), cut->GetVarX(), cut->GetVarY()); 
         cut->SetLineColor(4);
         cut->SetLineWidth(2);   
      }
   }

//========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   int numDet;
   int rDet = 6; // number of detector at different position, row-Det
   int cDet = 4; // number of detector at same position, column-Det
   vector<double> pos;
   double length = 50.5;
   double firstPos = -110;
   double xnCorr[24]; // xn correction for xn = xf
   double xfxneCorr[24][2]; //xf, xn correction for e = xf + xn
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) cDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      rDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < rDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < rDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       return;
   }
   
   numDet = rDet * cDet;
   
   double zRange[3];
   zRange[0] = 400; // number of bin
   
   if( firstPos > 0 ){
      zRange[1] = pos[0]-50;
      zRange[2] = pos[rDet-1] + length + 50;
   }else{
      zRange[1] = pos[0]- length - 50;
      zRange[2] = pos[rDet-1] + 50;
   }

/**///======================================================== Analysis
   
   if( cCheck->GetShowEditor() )cCheck->ToggleEditor();
   if( cCheck->GetShowToolBar())cCheck->ToggleToolBar();
   
   TLine * line = new TLine(-1, eThreshold, 1, eThreshold);
   line->SetLineColor(2);
   
   TH2F ** hEX = new TH2F*[numDet];
   TString expression, name, gate;
   for( int idet = 0 ; idet < numDet ; idet ++){
      cCheck->cd(idet+1);  
      name.Form("hEX%02d", idet);
      hEX[idet] = new TH2F(name, name, 400, -1.3, 1.3, eRange[0], eRange[1], eRange[2]);
      expression.Form("e:x>>hEX%02d", idet);
      
      gate.Form("detID == %d", idet);

      tree->Draw(expression, gate, "box");
      line->Draw("same");
      cCheck->Modified();
      cCheck->Update();
       gSystem->ProcessEvents();
   }
   
/**///======================================================== e vs z
   TCanvas * cCheck2 = new TCanvas("cCheck2", "cCheck2", 700, 50,  600, 400);
   cCheck2->SetGrid();
   
   gStyle->SetOptStat(11111);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.35);
   gStyle->SetStatW(0.25);
   gStyle->SetStatH(0.10);
   gStyle->SetLabelSize(0.035, "X");
   gStyle->SetLabelSize(0.035, "Y");
   gStyle->SetTitleFontSize(0.035);
   
   if( cCheck2->GetShowEditor() )cCheck2->ToggleEditor();
   if(!cCheck2->GetShowToolBar())cCheck2->ToggleToolBar();
   
   TH2F * hEZ = new TH2F("hEZ", "e:z; z [mm]; e [MeV]", zRange[0], zRange[1], zRange[2], eRange[0], eRange[1]-100, eRange[2]);
   
   tree->Draw("e:z >> hEZ", "", "colz" );
   if( cut != NULL ) cut->Draw("same");
   
   TLine * line2 = new TLine(zRange[1], eThreshold, zRange[2], eThreshold);
   line2->SetLineColor(2);
   line2->Draw("same");
   
   TLatex latex;
   latex.SetTextSize(0.04);
   latex.SetTextAlign(11);  //align bottom
   latex.SetTextColor(2);
   latex.DrawLatex(zRange[1]+30,eThreshold,"e-Threshold");

   cCheck2->Modified();
   cCheck2->Update();
   gSystem->ProcessEvents();
   
   printf("=========== you may want to make a TCutG and rename it as cutEZ, and save into cut.root.\n");
   
/**///======================================================== multi
   TCanvas * cCheck3 = new TCanvas("cCheck3", "cCheck3", 700, 600,  600, 300);
   cCheck3->SetGrid();
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatH(0.3);
   if(!cCheck3->GetShowToolBar() )cCheck3->ToggleToolBar(); // force show tool bar
   cCheck3->SetLogy();
   TH1F * hMulti = new TH1F("hMulti", "Multi", 20, 0, 20);
   tree->Draw("multiHit >> hMulti", "" );
   cCheck3->Modified();
   cCheck3->Update();

   gSystem->ProcessEvents(); 
   
   
}
