#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TTree.h>
#include <TFile.h>
#include <TSelector.h>
#include <TMath.h>
#include <TBenchmark.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraph2D.h>
#include <TGraph.h>
#include <TCutG.h>
#include <TRandom.h>
#include <string>
#include <fstream>
#include <math.h>
#include <TRandom.h>
#include <TDatime.h>
#include <TObjArray.h>

//use the fx in refTree, use fx->Eval(x) as e

void Cali_compareF(TTree *expTree, TFile *refFile, int option = -1, double eThreshold = 400){
/**///======================================================== User Input

   double a1Range[2] = {200, 450};
   double a0Range[2] = {-0.7, 4.0};

   double distThreshold   = 0.01;
   bool isXFXN = false; // only use event for both XF and XN valid
   
   int nTrial = 1000;
   
   double coinTimeGate = 10.; // TMath::Abs(coinTime) < coinTimeGate

/**///======================================================== 
   printf("==========================================================\n");
   printf("=================== Cali_compareF.C ======================\n");
   printf("============ calibrate e using kinematics ================\n");
   printf("==========================================================\n");
   TBenchmark gClock;  
   gClock.Reset(); gClock.Start("gTimer");
   
   gStyle->SetPalette(50);
   
   printf("======================== Cali_compareF\n");
   printf("      e Threshold : %f \n", eThreshold);
   printf("distant Threshold : %f \n", distThreshold);

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
   
   TFile * caliResult = NULL;
   if( option == -1 ) caliResult = new TFile("caliResult.root", "recreate");

/**///======================================================== display tree
   int totnumEntry = expTree->GetEntries();
   printf("============== Total #Entry: %10lld \n", expTree->GetEntries());
   
/**///======================================================== Plot

   Int_t Div[2] = {3,1};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }
   if( cScript->GetShowEditor()) cScript->ToggleEditor();
   if( cScript->GetShowToolBar()) cScript->ToggleToolBar();
   
   gStyle->SetOptStat(0);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
   gStyle->SetPalette(1,0);
/**///========================================================= load correction

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
   
   /*//========================================= xf = xn correction
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      return;
   }
   file.close();
   
   //========================================= e = xf + xn correction

   printf("----- loading xf/xn-e correction.");
   file.open("correction_xfxn_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         xfxneCorr[i][0] = a;
         xfxneCorr[i][1] = b;
         i = i + 1;
      }
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      return;
   }
   file.close(); */

/**///======================================================== setup tree

   double  eTemp;
   double  zTemp;
   int detIDTemp; 
   int hitIDTemp;
   float coinTimeTemp = 0.0;
   
   bool isCoinTimeBranchExist = false;
   TBranch * br = (TBranch*) expTree->GetListOfBranches()->FindObject("coinTime");
   if( br == NULL ) {
      printf("-------- Branch : coinTime NOT exist.\n");
      isCoinTimeBranchExist = false;
   }else{
      printf("-------- Branch : coinTime exist.\n");
      isCoinTimeBranchExist = true;
   }

   expTree->SetBranchStatus("*",0);
   expTree->SetBranchStatus("e",1);
   expTree->SetBranchStatus("z",1);
   expTree->SetBranchStatus("detID",1);
   expTree->SetBranchStatus("hitID",1);
   if(isCoinTimeBranchExist) expTree->SetBranchStatus("coinTime",1);
   
   expTree->SetBranchAddress("e",     &eTemp);
   expTree->SetBranchAddress("z",     &zTemp);
   expTree->SetBranchAddress("detID", &detIDTemp);
   expTree->SetBranchAddress("hitID", &hitIDTemp);
   if(isCoinTimeBranchExist) expTree->SetBranchAddress("coinTime", &coinTimeTemp);
   
   TObjArray * fxList = (TObjArray*) refFile->FindObjectAny("fxList");
   int numFx = fxList->GetEntries();
   TGraph ** fx = new TGraph *[numFx];
   for( int i = 0; i < numFx ; i++){
      fx[i] = (TGraph*) fxList->At(i);
   }
   
/**///======================================================== Extract tree entry, create new smaller trees, use that tree to speed up
   double B1 [numDet]; // best a1 of rDet
   double B0 [numDet]; // best a0 of rDet

   TBenchmark clock;  
   TGraph2D * gDist = new TGraph2D(); 

   int startDet = 0;
   int endDet = numDet;
   if(option >= 0 ) {
      startDet = option;
      endDet = option + 1;
   }
   
   TH2F ** exPlot  = new TH2F*[numDet];
   TH2F ** exPlotC = new TH2F*[numDet];
   TH2F ** dummy   = new TH2F*[numDet]; // for fx draw
   
   for( int idet = startDet; idet < endDet; idet ++){

      bool shown = false; clock.Reset(); clock.Start("timer");
      TString title; title.Form("detID-%d[%d]", idet,  idet%rDet);      
      
      /**///======================================================== histogram
      double iDet = idet%rDet;
      double zRange[2];
      if( firstPos > 0 ){
         zRange[0] = pos[iDet] - 10;
         zRange[1] = pos[iDet] + length + 10;
      }else{
         zRange[0] = pos[iDet] - length - 10;
         zRange[1] = pos[iDet] + 10;
      }
      
      printf("=============================== detID = %d (%6.2f mm, %6.2f mm) \n", idet, zRange[0], zRange[1]);
      
      TString name;
      name.Form("exPlot%d[%d]", idet, idet%rDet);
      exPlot[idet]  = new TH2F(name , "exPlot" , 200, zRange[0], zRange[1], 200, 0, 3000);
      exPlot[idet]->Reset();
      exPlot[idet]->SetTitle(title + "(exp)");

      name.Form("exPlotC%d", idet);
      exPlotC[idet] = new TH2F(name, "exPlotC", 200, zRange[0], zRange[1], 200, 0, 13);
      exPlotC[idet]->Reset();
      exPlotC[idet]->SetTitle(title + "(corr)");
      
      name.Form("dummy%d", idet);
      dummy[idet] = new TH2F(name, "exPlotC", 200, zRange[0], zRange[1], 200, 0, 13);
      dummy[idet]->Reset();
      dummy[idet]->SetTitle(title + "(sim)");
      
      /**///========================================================  plot when single detector calibration
      
      if( option >= 0 ){
         
         cScript->cd(1);
         for( int i = 0; i < expTree->GetEntries() ; i++){
            expTree->GetEntry(i);
            if( detIDTemp != idet ) continue;
            if( eTemp < eThreshold) continue;
            if( cut != NULL &&  !cut->IsInside(zTemp, eTemp) ) continue; 
            if( isXFXN && hitIDTemp != 0 ) continue;
            if( isCoinTimeBranchExist && TMath::Abs(coinTimeTemp) > coinTimeGate ) continue;
            
            //printf("%d | %f, %f, %f \n", i, zTemp, eTemp, coinTimeTemp);
            
            exPlot[idet]->Fill(zTemp, eTemp);
         }
         exPlot[idet]->Draw("box");
         
         cScript->cd(2);
         dummy[idet]->Draw();
         for( int i = 0; i < numFx; i++){
            fx[i]->Draw("same");
         }         
         cScript->Modified();
         cScript->Update();
         
         gSystem->ProcessEvents();
         
      }
      /**///======================================================== Calculate minDist

      clock.Reset(); clock.Start("timer");
      
      double A0 = 0.;
      double A1 = 260.;
      
      double minTotalMinDist = 99.;
      
      TString gDistName; 
      gDistName.Form("gDist%d", idet);
      gDist->SetName(gDistName);
      gDist->SetTitle("Total min-dist; a1; a0; min-dist");
      gDist->Clear();
      
      TRandom * ranGen = new TRandom();
      TDatime time;
      ranGen->SetSeed(time.GetTime());
      
      //calculate number of event will be used.
      int countEvent = 0;
      for( int eventE = 0 ; eventE < expTree->GetEntries(); eventE ++ ){
        expTree->GetEntry(eventE);
        if( detIDTemp != idet ) continue;
        if( eTemp < eThreshold) continue;
        if( isXFXN && hitIDTemp != 0 ) continue;
        if( cut != NULL &&  !cut->IsInside(zTemp, eTemp) ) continue; 
        if( isCoinTimeBranchExist && TMath::Abs(coinTimeTemp) > coinTimeGate ) continue;
        countEvent++;
      }
      
      int countMax = 0;
      printf("========== find fit by Monle Carlo. #Point: %d, #event: %d\n", nTrial, countEvent);
      for( int iTrial = 0; iTrial < nTrial; iTrial ++){ 
         
         double a1, a0;
         a1 = a1Range[0] + (a1Range[1] - a1Range[0])*ranGen->Rndm();
         a0 = a0Range[0] + (a0Range[1] - a0Range[0])*ranGen->Rndm();
         
         double totalMinDist    = 0.;
         int count = 0; 
         for( int eventE = 0 ; eventE < expTree->GetEntries(); eventE ++ ){
            expTree->GetEntry(eventE);
            if( detIDTemp != idet ) continue;
            if( eTemp < eThreshold) continue;
            if( isXFXN && hitIDTemp != 0 ) continue;
            if( cut != NULL &&  !cut->IsInside(zTemp, eTemp) ) continue; 
            if( isCoinTimeBranchExist && TMath::Abs(coinTimeTemp) > coinTimeGate ) continue;
            double minDist = 99;
            
            for( int i = 0; i < numFx; i++){

               double eR = fx[i]->Eval(zTemp);               
               
               //calculate dist
               double tempDist = TMath::Power( (eTemp/a1 + a0) - eR,2);
               if( tempDist < minDist ) {
                  minDist = tempDist;
               }
               //if( eventE%100 == 0) printf("%d, %d, %8.4f, %8.4f| %8.4f < %8.4f \n", eventE, i, eR, eTemp/a1+a0, tempDist, minDist);               
                       
            }
            if( minDist < distThreshold ) {
               count ++;  
               totalMinDist += minDist; 
            }else{
               totalMinDist += distThreshold;
            }
         }
         
         if( count == 0 ) totalMinDist = minTotalMinDist + 0.2; // to avoid no fill
         
         gDist->SetPoint(iTrial, a1, a0, totalMinDist);
         
         
         if( iTrial%(nTrial/5) == 0) {
            clock.Stop("timer");
            Double_t time = clock.GetRealTime("timer");
            clock.Start("timer");
            printf("%4d | %7.3f < %6.3f [%4d, %4d(%2.0f%%)] ", iTrial, totalMinDist, minTotalMinDist, count, countMax, countMax*100./countEvent);   
            printf( "|%5.0f sec| \n", time);
         }
                  
         if( totalMinDist < minTotalMinDist && count >= countMax ) {
            countMax = count; // next best fit must have more data points
            minTotalMinDist = totalMinDist;
            A0 = a0;
            A1 = a1;
            
            clock.Stop("timer");
            Double_t time = clock.GetRealTime("timer");
            clock.Start("timer");
            //display
            printf("%4d | %7.3f < %6.3f [%4d, %4d(%2.0f%%)] ", iTrial, totalMinDist, minTotalMinDist, count, countMax, countMax*100./countEvent);   
            printf( "|%5.0f sec| ", time);
            printf("%5.1f, %6.3f \n", A1, A0);
            
            if( option >= 0 ){
               exPlotC[idet]->Reset();
               for( int eventE = 0 ; eventE < expTree->GetEntries(); eventE ++ ){
                  expTree->GetEntry(eventE);
                  if( detIDTemp != idet ) continue;
                  if( isXFXN && hitIDTemp != 0 ) continue;
                  if( cut != NULL &&  !cut->IsInside(zTemp, eTemp) ) continue; 
                  if( isCoinTimeBranchExist && TMath::Abs(coinTimeTemp) > coinTimeGate ) continue;
                  exPlotC[idet]->Fill(zTemp, eTemp/A1 + A0);
               }
               cScript->cd(2);
               exPlotC[idet]->Draw("colz same");
               cScript->Update();
               
               gSystem->ProcessEvents();
            }            

         }
            
      }// end of loop
      
      //After founded the best fit, plot the result
      //======== time
      clock.Stop("timer");
      printf("==========> %7.0f sec (A1, A0) : (%f\t%f) \n", clock.GetRealTime("timer"), A1, A0);
      B1[idet] = A1;
      B0[idet] = A0;

      if( option == -1 ){
         cScript->cd(1);
         exPlot[idet]->Reset();
         exPlot[idet]->SetTitle(title + "(exp)");
         exPlot[idet]->GetXaxis()->SetRange(zRange[0], zRange[1]);
         for( int i = 0; i < expTree->GetEntries() ; i++){
            expTree->GetEntry(i);         
            if( detIDTemp != idet ) continue;
            if( eTemp < eThreshold) continue;
            if( isXFXN && hitIDTemp != 0 ) continue;
            if( cut != NULL &&  !cut->IsInside(zTemp, eTemp) ) continue; 
            if( isCoinTimeBranchExist && TMath::Abs(coinTimeTemp) > coinTimeGate ) continue;
            exPlot[idet]->Fill(zTemp, eTemp);
         }
         exPlot[idet]->Draw("box");
         caliResult->cd(); exPlot[idet]->Write(); caliResult->Write("exPlot", TObject::kSingleKey);
         
         cScript->cd(2);
         dummy[idet]->Draw();
         //caliResult->cd(); dummy[idet]->Write(); caliResult->Write("dummy", TObject::kSingleKey);
         
         for( int i = 0; i < numFx; i++){
            fx[i]->Draw("same");
            caliResult->cd(); fx[i]->Write(); caliResult->Write("fx", TObject::kSingleKey);
         }  
         
         cScript->Update();
         gSystem->ProcessEvents();
         
      }

      cScript->cd(2);      
      exPlotC[idet]->Reset();
      for( int eventE = 0 ; eventE < expTree->GetEntries(); eventE ++ ){
         expTree->GetEntry(eventE);
         if( detIDTemp != idet ) continue;
         if( isXFXN && hitIDTemp != 0 ) continue;
         if( cut != NULL &&  !cut->IsInside(zTemp, eTemp) ) continue; 
         if( isCoinTimeBranchExist && TMath::Abs(coinTimeTemp) > coinTimeGate ) continue;
         exPlotC[idet]->Fill(zTemp, eTemp/A1 + A0);
      } 
      exPlotC[idet]->Draw("colz same");
      if( option == -1 ) {
         caliResult->cd(); 
         exPlotC[idet]->Write(); 
         caliResult->Write("exPlotC", TObject::kSingleKey);
      }   

      cScript->cd(3);
      double zMax = gDist->GetZmax();
      double zMin = gDist->GetZmin();
      gDist->SetMaximum(zMax);
      gDist->SetMinimum(zMin);
      gDist->Draw("tri1");
      if( option == -1 ){
         caliResult->cd(); 
         gDist->Write(); 
         caliResult->Write("gDist", TObject::kSingleKey);
      }
         
      cScript->cd(3)->SetTheta(90);
      cScript->cd(3)->SetPhi(0);
      cScript->Update();
      cScript->Draw();
      
      gSystem->ProcessEvents();
   
   } // end of loop idet  
/**///======================================================== save result
   
   if( option < 0 ){
      FILE * paraOut;
      TString filename;
      filename.Form("correction_e.dat");
      paraOut = fopen (filename.Data(), "w+");

      printf("=========== save parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
      fprintf(paraOut, "%9.6f  %9.6f\n", B1[i], B0[i]);
      }

      fflush(paraOut);
      fclose(paraOut);
   }
   
   gClock.Stop("gTimer");
   double gTime =  gClock.GetRealTime("gTimer");
   printf("=========== finsihed, total runTime : %7.0f sec \n", gTime);
   
}
