#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <fstream>
#include <TCutG.h>

void Check_Transfer(TString filename = "transfer.root"){

//========================================== User Input
  TString gate = "hit == 1 && rhoBHit > 10 && loop == 1";
  //TString gate = "hit == 1 && loop == 1";

  TString gate2 = "rhoHit1 < 50  && rhoHit2 > 60 "; // elum

//============================================== 
  TFile * file = new TFile(filename, "read");
  TTree * tree = (TTree*) file->Get("tree");

  TObjArray * fxList = (TObjArray *) file->FindObjectAny("fxList");
  TObjArray * txList = (TObjArray *) file->FindObjectAny("txList");

  const int nExID = fxList->GetLast()+1;

   gStyle->SetOptStat("");
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.4);
   gStyle->SetStatH(0.2);
   gStyle->SetLabelSize(0.05, "XY");
   gStyle->SetTitleFontSize(0.1);


//========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   int numDet;
   int rDet = 5; // number of detector at different position, row-Det
   int cDet = 6; // number of detector at same position, column-Det
   vector<double> pos;
   double length = 50.5;
   double firstPos = -110;
   double rhoRecoil = 50;
   double posRecoil = 400;
   double posRecoil1 = 400;
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream detFile;
   detFile.open(detGeoFileName.c_str());
   int i = 0;
   if( detFile.is_open() ){
      string x;
      while( detFile >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 5 ) length   = atof(x.c_str());
	 if( i == 6 ) posRecoil = atof(x.c_str());
	 if( i == 7 ) rhoRecoil = atof(x.c_str());
	 if( i == 9 ) posRecoil1 = atof(x.c_str());
	 if( i == 14 ) firstPos = atof(x.c_str());
         if( i == 17 ) cDet = atoi(x.c_str());
         if( i >= 18 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      rDet = pos.size();
      detFile.close();
      printf("... done.\n");
      
      for(int id = 0; id < rDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < rDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %10.2f mm - %10.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %10.2f mm - %10.2f mm \n", i, pos[i] - length , pos[i]);
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

   //===================================================
   Int_t Div[2] = {4,2}; // x,y
   Int_t size[2] = {400,400}; //x,y
   TCanvas * cCheck = new TCanvas("cCheck", "cCheck", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(cCheck->GetShowEditor() )cCheck->ToggleEditor();
   if(cCheck->GetShowToolBar() )cCheck->ToggleToolBar();
   cCheck->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCheck->cd(i)->SetGrid();
   }

   printf("============================== Gate\n");
   printf("gate : %s\n", gate.Data());
   printf("gate2: %s\n", gate2.Data());

   printf("=======================meaning of Hit ID\n");
   printf("  1 = light recoil hit array & heavy recoil hit recoil\n");
   printf("  0 = when no detector\n");
   printf(" -1 = light recoil blocked by recoil detector\n");
   printf(" -2 = heavy recoil missed recoil detector\n");
   printf(" -3 = light recoil looped over 10 loops\n");
   printf(" -4 = light recoil over shoot array \n");
   printf(" -5 = light recoil hit array from inside, becasue of math\n");
   printf(" -6 = light recoil blocked by the structure between target and array detector\n");
   printf(" -7 = light recoil hit the gap between array detectors (along z)\n");
   printf(" -8 = light recoil hit the gap between array detectors (along xy)\n");
   printf(" -9 = light recoil don't hit any array detectors (exclude above hit-ID)\n");
   printf("-10 = light recoil hit HELIOS wall or go to opposite direction.\n");
   printf("            (e.g. upstream array, light particle go downstream)\n");
   printf("===========================================\n");
   
   cCheck->cd(1);
   cCheck->cd(1)->SetLogy();
   TH1F * hHit = new TH1F("hHit", "hit; hit-ID; count", 13, -11, 2);
   tree->Draw("hit>>hHit", "", "");
      

   cCheck->cd(2);
   TH2F * hez = new TH2F("hez", Form("e-z [gated] @ %5.0f mm; z [mm]; e [MeV]", firstPos), zRange[0], zRange[1], zRange[2], 400, 0, 20);
   tree->Draw("e:z>>hez", gate, "colz");
   for( int i = 0; i < nExID ; i++){
     fxList->At(i)->Draw("same");
   }

   cCheck->cd(3);
   TH2F * hRecoilXY = new TH2F("hRecoilXY", Form("RecoilXY [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil ), 400, -rhoRecoil, rhoRecoil, 400,-rhoRecoil, rhoRecoil);
   tree->Draw("ryHit:rxHit>>hRecoilXY", gate, "colz");
   
   cCheck->cd(4);
   TH2F * hRecoilRThetaCM = new TH2F("hRecoilRThetaCM", "RecoilR - thetaCM [gated]; thetaCM [deg]; RecoilR [mm]", 400, 0, 60, 400,0, rhoRecoil);
   tree->Draw("rhoBHit:thetaCM>>hRecoilRThetaCM", gate, "colz");

   cCheck->cd(5);
   TH2F * hRecoilRZ = new TH2F("hRecoilRZ", "RecoilR - Z [gated]; z [mm]; RecoilR [mm]",  zRange[0], zRange[1], zRange[2], 400,0, rhoRecoil);
   tree->Draw("rhoBHit:z>>hRecoilRZ", gate, "colz");

   cCheck->cd(6);
   TH1F * hExCal = new TH1F("hExCal", "calculated Ex [gated]; Ex [MeV]; count",  500, -1, 6);
   tree->Draw("ExCal>>hExCal", gate, "");

   cCheck->cd(7);
   cCheck->cd(7)->SetGrid(0,0);
   
   TH1F * hThetaCM[nExID];
   double maxCount = 0;
   for( int i = 0; i < nExID; i++){
     hThetaCM[i] = new TH1F(Form("hThetaCM%d", i), Form("thetaCM [gated] (ExID=%d); thetaCM [deg]; count", i), 200, 0, 50);
     hThetaCM[i]->SetLineColor(i+1);
     hThetaCM[i]->SetFillColor(i+1);
     hThetaCM[i]->SetFillStyle(3000+i);
     tree->Draw(Form("thetaCM>>hThetaCM%d", i), gate + Form("&& ExID==%d", i), "");
     double max = hThetaCM[i]->GetMaximum();
     if( max > maxCount ) maxCount = max;
   }

   for( int i = 0; i < nExID; i++){
     hThetaCM[i]->GetYaxis()->SetRangeUser(0, maxCount * 1.2);
     if( i == 0 ) {
       hThetaCM[i]->Draw();
     }else{
       hThetaCM[i]->Draw("same");
     }
   }

   cCheck->cd(8);
   TH2F *hThetaCM_Z = new TH2F("hThetaCM_Z","ThetaCM vs Z ; Z [mm]; thetaCM [deg]",zRange[0], zRange[1], zRange[2], 200,0,50);
   tree->Draw("thetaCM:z>>hThetaCM_Z",gate,"col");
   for( int i = 0; i < nExID ; i++){
     txList->At(i)->Draw("same");
   }

   /*
   cCheck->cd(9);
   TH2F * hRecoilXY1 = new TH2F("hRecoilXY1", Form("RecoilXY [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil1 ), 400, -rhoRecoil, rhoRecoil, 400,-rhoRecoil, rhoRecoil);
   tree->Draw("ryHit1:rxHit1>>hRecoilXY1", gate, "colz");

   cCheck->cd(10);
   */
}
