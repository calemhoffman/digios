#include "../Cleopatra/HELIOS_LIB.h"
#include "../Cleopatra/Isotope.h"
#include "../Armory/AnalysisLibrary.h"

#include "TLorentzVector.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TF2.h"
#include "TGraph2D.h"

#include <vector>

TH2D * hDataHRes = NULL;
Double_t paceDist(Double_t *x, Double_t *par){

   Double_t result = 0;
   
   par = NULL;

   if( hDataHRes == NULL ) return 0;
   
   int binX = hDataHRes->GetXaxis()->FindBin(x[0]);
   int binY = hDataHRes->GetYaxis()->FindBin(x[1]);
   
   result = hDataHRes->GetBinContent(binX, binY);
    
   return result;
}


void PACE4(){
   
   //============ load data
   string fileName = "pace4_82Se_12C_short.txt";
   
   printf("----- loading  %s\n", fileName.c_str());
   ifstream file_in;
   file_in.open(fileName.c_str(), ios::in);

   if( !file_in ){
      printf(" cannot read file. \n");
      return ; 
   }
   
   vector<array<double,2>> TList; 
   vector<array<double, 18>> yield;
   
   printf("%10s |", "energy[MeV]");
   for( int i = 0; i < 18; i++){
      printf("%6d ", 10*i);
   }
   printf("\n");
   
   int lineNum = -1;
   while( file_in.good() ) {
      string tempLine;
      lineNum++;
      getline(file_in, tempLine );
      
      if( lineNum < 6 ) continue;
      
      if( tempLine.substr(0, 16) == "| Above   30.0 |") continue;
      if( tempLine.substr(0, 6) == "|-----") break;
      
      //printf("%2d aaa%s\n", lineNum, tempLine.c_str());
      vector<string> str = SplitStr(tempLine, "|");
      
      ///===== get TList
      size_t pos = str[0].find_first_of("-");
      array<double,2> en = {atof(str[0].substr(0, pos).c_str()), atof(str[0].substr(pos+1).c_str()) };
      TList.push_back(en);
      printf(" %4.1f, %4.1f |", TList.back()[0], TList.back()[1]);
      
      array<double, 18> ang;
      for( int i = 0; i < 18; i++){
         ang[i] = atof(str[i+1].c_str()) + 0.1;
      }
      yield.push_back(ang);
      
      for( int i = 0 ; i < 18; i++){
         printf("%6.1f ", yield.back()[i]); 
      }
      printf("\n");
   }
   
   file_in.close();  
   
   int nEnergy = (int) TList.size();
   
   //=========== smoothing the data
   TFile * fileOut = new TFile("PACE4.root", "RECREATE" );
   
   TCanvas * c1 = new TCanvas("c1", "c1", 2400, 1600);
   c1->Divide(3,2);
   
   c1->cd(1);
   TH2F * hData = new TH2F("hData", "fission evaporation distribution (PACE4); Lab [deg]; Energy [MeV]", 18, 0, 180, nEnergy, 0, nEnergy);
   for( int i = 0; i < nEnergy; i++){
      for( int j = 0; j < 18; j++){
         hData->Fill( 10*(j) + 5, (TList[i][1]+TList[i][0])/2., yield[i][j]);
      }
   }
   hData->Draw("colz");

   c1->cd(2);
   TGraph2D * gData = new TGraph2D();
   gData->SetTitle("fission evaporation distribution (PACE4); Lab [deg]; Energy [MeV]; count");
   int nPt = 0;
   //for( int j = 0; j < 180; j++){
   //   gData->SetPoint(nPt, j, 0., 0.1);
   //   nPt++;
   //}
   for( int i = 0; i < nEnergy; i++){
      for( int j = 0; j < 18; j++){
         gData->SetPoint( nPt, 10*j + 5, (TList[i][1]+TList[i][0])/2., yield[i][j]);
         nPt ++;
      }
   }
   gData->SetNpx(500);
   gData->SetNpy(500);
   
   gData->Draw("TRI2"); 
   
   hDataHRes = gData->GetHistogram();
   hDataHRes->SetNameTitle("hData2", "high Res.");  
   hDataHRes->Draw("colz");
   
   
   //c1->cd(3);
   TF2 * dataDist = new TF2("paceDist", paceDist, 0, 180, 0, 30);
   //dataDist->Draw("");
   
   
   //=========== generate events, and produce e-z plot
   TransferReaction reaction;
   reaction.SetReactionFromFile("reactionConfig.txt");
   
   HELIOS helios;
   helios.SetDetectorGeometry("detectorGeo.txt");
   
   double mp = 938.272; // proton mass
   
   
   printf("==================================== E-Z plot slope\n");
   double beta = reaction.GetReactionBeta() ;
   double gamma = reaction.GetReactionGamma();
   double mb = reaction.GetMass_b();
   double pCM = reaction.GetMomentumbCM();
   double q = TMath::Sqrt(mp*mp + pCM*pCM);
   double slope = 299.792458 * 1 * abs(helios.GetBField()) / TMath::TwoPi() * beta / 1000.; // MeV/mm
   printf("                       e-z slope : %f MeV/mm\n", slope);   
   double intercept = q/gamma - mp; // MeV
   printf("    e-z intercept (ground state) : %f MeV\n", intercept); 
  
   printf("%-15.4f  //%s\n", reaction.GetMass_b(), "mass_b");
   printf("%-15.8f  //%s\n", reaction.GetReactionBeta(), "betaCM");
   printf("%-15.4f  //%s\n", reaction.GetCMTotalEnergy(), "Ecm");
   printf("%-15.4f  //%s\n", reaction.GetMass_B(), "mass_B");
   printf("%-15.4f  //%s\n", slope/beta, "alpha=slope/beta");

   

   ///====== Tree;
   TTree * tree =  new TTree("tree","tree");
   
   int hit;
   double KE, theta, zPos, Ex, thetaCM;
   tree->Branch("hit", &hit,"hit/I"); 
   tree->Branch("e", &KE,"KE/D"); 
   tree->Branch("theta" ,  &theta, "theta/D");
   tree->Branch("z" ,  &zPos, "z/D");
   tree->Branch("Ex" ,  &Ex, "Ex/D");
   tree->Branch("thetaCM" ,  &thetaCM, "thetaCM/D");
   
   
   TLorentzVector p;
   
   TH2F * kGen        = new TH2F("kGen", "Generated; theta[deg]; KE [MeV]", 100, 0, 180, 100, 0, 30);
   TH2F * hEZ         = new TH2F("hEZ",  "E-Z; Z [mm]; E [MeV]", 500, -600, -150, 100, 0, 10);
   TH2F * hExThetaCM  = new TH2F("hExThetaCM",  "Ex-thetaCM; thetaCM [deg]; Ex [MeV]", 500, 0, 50, 100, -1, 5);
   TH1F * hEx         = new TH1F("hEx",  "Ex; Ex [MeV] ; count / 0.1 MeV", 100, -1, 5);
   
   gErrorIgnoreLevel = 6001;
   
   int nEvent = 10000000;
   for ( int i = 0 ; i < nEvent; i++){
      
      if( i% 1000000 == 0 ) printf("===== %d \n", i);
      
      dataDist->GetRandom2( theta, KE);
      
      kGen->Fill(theta, KE);
      
      //if( theta < 90 ) continue ;
      
      double k = sqrt( (mp + KE)*(mp + KE) - mp*mp);
      p.SetXYZM(0, 0, k, mp + KE);
      p.RotateY(theta * TMath::DegToRad());
      
      //p.Print();
      
      helios.CalArrayHit(p, 1);
      hit = helios.DetAcceptance();
      trajectory trj = helios.GetTrajectory_b();
      
      if( hit == 1){      
         
         zPos = trj.z;
         
         reaction.CalExThetaCM(KE, zPos, helios.GetBField(), helios.GetDetRadius());
         Ex = reaction.GetEx();
         thetaCM = reaction.GetThetaCM();
         
         hEZ->Fill(zPos, KE); 
         
         if( thetaCM > 10 ){
            hEx->Fill(Ex);
            hExThetaCM->Fill(thetaCM, Ex);
         }
      }else{
         zPos = trj.z0;
         Ex = TMath::QuietNaN();
         thetaCM = TMath::QuietNaN();

      }
      
      tree->Fill();
      
   }
   
   c1->cd(3);
   kGen->Draw("colz");
   
   c1->cd(4);
   hEZ->Draw("colz");
   
   c1->cd(5);
   hExThetaCM->Draw("colz");
   
   c1->cd(6);
   hEx->Draw();
   
   fileOut->Write();
   //fileOut->Close();
   
   printf("=========== done.\n");
   
   //=========== calculate Ex 
   
   
   
}
