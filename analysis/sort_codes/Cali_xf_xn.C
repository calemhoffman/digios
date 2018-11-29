#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraph.h>
#include <TLine.h>
#include <TSpectrum.h>

int nPeaks = 16;
Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1);
   }
   return result;
}


void Cali_xf_xn(TTree * tree){
/**///======================================================== initial input
   
   const int numDet = 24;
   
/**///========================================================  load tree

   printf("============================================================= \n");
   printf("====================== Cali_xf_xn.C ========================= \n");
   printf("============================================================= \n");
   printf("============ calibration for PSD detectors using alpha souce. \n");
   printf("1, calibration energy using charateristic energy of alpha souce. \n");
   printf("2, calibration xf-xn with energy-gate. \n");
   printf("------------------------------------------------- \n");
   printf("=========== Total #Entry: %10lld \n", tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cAlpha = new TCanvas("cAlpha", "cAlpha", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAlpha->Divide(Div[0],Div[1]);
   
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAlpha->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(0);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
   if(cAlpha->GetShowEditor()  )cAlpha->ToggleEditor();
   if(cAlpha->GetShowToolBar() )cAlpha->ToggleToolBar();
   
/**///========================================================= Analysis

   //############################################################ energy correction
   printf("############## e correction \n");
   TH1F ** q = new TH1F*[numDet];
   TString * gate = new TString[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("q%d", i);
      q[i] = new TH1F(name, name, 200, 500, 1800);
      q[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d] >> q%d" ,i, i);
      gate[i].Form("e[%d] > 0", i);
      
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "");
      cAlpha->Update();
      gSystem->ProcessEvents();
   }
   
   //----------- 1, pause for save Canvas
   int dummy = 0;
   int temp = 0;
   cAlpha->Update();
   gSystem->ProcessEvents();

   //----------- 2, find the edge of the energy    
   printf("============== method to find edge:\n");
   printf("1) maximum peak edge.\n");
   printf("2) TSpectrum fit.\n");
   printf("9) Exit.\n");
   printf("====== choice : ");
   int method = 0;
   temp = scanf("%d", &method);
   
   if( method == 9 ) return;
   
   double xHalf[numDet];
   if( method == 1 ){
      printf("----- finding the edge of maximum peak...\n");
      
      double yMax[numDet];      
      for( int i = 0; i < numDet; i++){
         yMax[i] = q[i]->GetMaximum();
         int iMax = q[i]->GetMaximumBin();
         int iHalf = q[i]->FindLastBinAbove(yMax[i]/2);
         xHalf[i] = q[i]->GetBinCenter(iHalf);
      
         printf("%2d | iHalf : %3d, xHalf : %6f \n", i, iHalf, xHalf[i]);
      }
      
      int refID = 0;
      printf("========== which detector to be the reference? ");
      temp = scanf("%d", &refID);
      printf("----- adjusting the energy to det-%d......\n", refID);
      //------------ 3, correction
      TH1F ** p = new TH1F*[numDet];
      for( int i = 0; i < numDet; i ++){
         TString name;
         name.Form("p%d", i);
         p[i] = new TH1F(name, name, 300, 100, 1800);
         p[i]->SetXTitle(name);
         
         TString expression;
         expression.Form("e[%d]  * %f >> p%d", i,  xHalf[refID]/xHalf[i], i);
         gate[i].Form("e[%d] > 0", i);
         cAlpha->cd(i+1);
         tree->Draw(expression, gate[i] , "");
      }
      
   }
   
   int nPeaks = 10;
   vector<double> * energy = new vector<double> [numDet]; 
   double a0[numDet];
   double a1[numDet];
   vector<double> refEnergy;
   if( method == 2 ){
      printf("---- finding edge using TSepctrum Class...\n");
      for( int i = 0; i < numDet; i++){
         
         TSpectrum * spec = new TSpectrum(10);
         nPeaks = spec->Search(q[i], 1, "", 0.05);
         printf("%2d | found %d peaks | ", i,  nPeaks);

         double * xpos = spec->GetPositionX();
         double * ypos = spec->GetPositionY();
         
         vector<double> height;
         
         int * inX = new int[nPeaks];
         TMath::Sort(nPeaks, xpos, inX, 0 );
         for( int j = 0; j < nPeaks; j++){
            energy[i].push_back(xpos[inX[j]]);
            height.push_back(ypos[inX[j]]);
         }
         
         for( int j = 0; j < nPeaks; j++){
            printf("%7.2f, ", energy[i][j]);
         }
         printf("\n");
         
         
        /*//========== Fitting 
        printf("============= Fitting.....\n");

        
        double xMin = q[i]->GetXaxis()->GetXmin();
        double xMax = q[i]->GetXaxis()->GetXmax();

        const int  n = 3 * nPeaks;
        double * para = new double[n]; 
        for(int j = 0; j < nPeaks ; j++){
          para[3*j+0] = height[j] * 0.05 * TMath::Sqrt(TMath::TwoPi());
          para[3*j+1] = energy[i][j];
          para[3*j+2] = 50;
        }

        TF1 * fit = new TF1("fit", fpeaks, xMin, xMax, 3* nPeaks );
        fit->SetLineWidth(1);
        fit->SetLineColor(2);
        fit->SetNpx(1000);
        fit->SetParameters(para);
        q[i]->Fit("fit", "q");
        
        const Double_t* paraA = fit->GetParameters();
        
        energy[i].clear();
        
        for( int j = 0; j < nPeaks; j++){
            energy[i].push_back(paraA[3*j+1]);
            printf("%7.2f, ", energy[i][j]);
        }
        printf("\n");
        */ 
      }
      
      for( int i = 0; i < numDet; i++){
         cAlpha->cd(i+1);
         q[i]->Draw();
         cAlpha->Update();
         gSystem->ProcessEvents();
      }
      
      //------------ 3, correction
      int refID = 0;
      printf("========== which detector to be the reference?\n");
      printf(" X =  det-X reference\n");
      printf("-1 =  manual reference\n");
      printf("-9 =  stop \n");
      printf("your choice = ");
      temp = scanf("%d", &refID);
      
      if( refID == -9 ) {
         printf("------ stopped by user.\n");
         return;
      }
      
      //======== fill reference energy
      if( refID >= 0 ){
         int n = energy[refID].size();
         for( int k = 0; k < n; k++){
            refEnergy.push_back(energy[refID][k]);
            
         }
      }
      
      if(refID == -1){
         int n = 0;
         float eng = -1;
         do{
            printf("%2d-th peak energy (< 0 to stop):", n);
            temp = scanf("%f", &eng);
            printf("             input: %f \n", eng);
            if( eng >= 0 ) refEnergy.push_back(eng);
            n ++ ;
         }while(eng >= 0);
      }
      
      printf("----- adjusting the energy to det-%d......\n", refID);
      int n = refEnergy.size();
      for( int k = 0; k < n; k++){
         printf("%2d-th peak : %f \n", k,  refEnergy[k]);
      }
      printf("----------------------------------\n");
      TH1F ** p = new TH1F*[numDet];
      for( int i = 0; i < numDet; i ++){
         
         TGraph * graph = new TGraph(nPeaks, &energy[i][0], &refEnergy[0] );
         
         TF1 * fit = new TF1("fit", "pol1" );
         graph->Fit("fit", "q");
         
         a0[i] = fit->GetParameter(0);
         a1[i] = fit->GetParameter(1);
         
         //det-11 is broken 
         if( i == 11) {
            a0[11] = 0.0;
            a1[11] = 1.0;
         }
         
         printf("%2d | a0: %6.3f, a1: %6.3f (%14.8f) \n", i, a0[i], a1[i], 1./a1[i]);
         
         TString name;
         name.Form("p%d", i);
         p[i] = new TH1F(name, name, 1000, 1., refEnergy.back() * 1.3);
         p[i]->SetXTitle(name);
         
         TString expression;
         expression.Form("e[%d] * %f + %f >> p%d", i, a1[i], a0[i], i);
         gate[i].Form("e[%d] > 0", i);
         cAlpha->cd(i+1);
         tree->Draw(expression, gate[i] , "");
         cAlpha->Update();
         gSystem->ProcessEvents();
      }
      
      TCanvas * cAux = new TCanvas ("cAux", "cAux", 600, 800);
      cAux->cd(1);
      p[0]->Draw();
      gSystem->ProcessEvents();
      for( int  i = 1; i < numDet; i++){
         p[i]->Draw("same");
         gSystem->ProcessEvents();
      }      
   }
   

    
   
   //----------- 4, pause for saving correction parameters
   cAlpha->Update();
   gSystem->ProcessEvents();
   //printf("0 for stop, 1 for save e-correction & Canvas, 2 for xf - xn correction: ");
   //temp = scanf("%d", &dummy);
   dummy = 1;
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("correction_e_alpha.dat");
      paraOut = fopen (filename.Data(), "w+");
      printf("=========== save e-correction parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         if( method == 1) fprintf(paraOut, "%14.8f\t%9.6f\n", xHalf[i], 0.);
         if( method == 2) fprintf(paraOut, "%14.8f\t%9.6f\n", 1./a1[i], a0[i]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      //cAlpha->SaveAs("alpha_e_corrected.pdf");
   }  
   
   //############################################################  for xf-xn correction
   printf("############## xf - xn correction \n");
   TLine line(0,0,0,0);
   line.SetLineColor(4);
   
   double eGate = 0;
   if( method == 2) {
      int peakID = 0;
      printf("------ pick the i-th peak (0..%d, -1 to stop): ", nPeaks-1);
      temp = scanf("%d", &peakID);
      if( peakID < 0 ) {
         return;
      }
      eGate = refEnergy[peakID];
      printf("------ using the peak at : %f \n", eGate);
   }
   
   printf("----- plotting xf vs xn with energy gate near the peak...\n");
   TH2F ** h = new TH2F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH2F(name, name, 100, -100, 1800, 100, -100, 1800);
      name.Form("xf[%d]", i); h[i]->SetYTitle(name);
      name.Form("xn[%d]", i); h[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]>> h%d" , i, i, i);
      if(method == 1) gate[i].Form("xf[%d]>0 && xn[%d]>0 && TMath::Abs(e[%d]-%f)<50", i, i, i, xHalf[i]-25);
      if(method == 2) gate[i].Form("xf[%d]>0 && xn[%d]>0 && TMath::Abs(e[%d] * %f + %f - %f)< %f", i, i, i, a1[i], a0[i], eGate, eGate * 0.05);
      
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "");
      
      line.SetX2(1600);
      line.SetY1(1600);
      line.Draw("same");
      
      cAlpha->Update();
      gSystem->ProcessEvents();
      
   }
   
   //-------- 1, pause for saving Canvas
   cAlpha->Update();
   gSystem->ProcessEvents();
   //printf("0 for stop, 1 for save Canvas, 2 for continuous : ");
   //temp = scanf("%d", &dummy);
   //if( dummy == 0 ) {
   //   return;
   //}else if(dummy == 1){
   //   cAlpha->SaveAs("alpha_xf_xn.pdf");
   //}
   
   //-------- 2, profileX and get the fit function
   printf("----- profile and obtain the fit function...\n");
   double para[numDet][2];
   double xnScale[numDet];
   TF1 * fit = new TF1("fit", "pol1");
   for( int i = 0; i < numDet; i++){
      cAlpha->cd(i+1);
      h[i]->ProfileX()->Fit("fit", "Q");
      fit->GetParameters(para[i]);
      xnScale[i] = -para[i][1]; 
   }

   //--------- 3, correction
   printf("----- correcting...\n");
   TH2F ** k = new TH2F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH2F(name, name, 100, -100, 1800, 100, -100, 1800);
      name.Form("xf[%d]", i); k[i]->SetYTitle(name);
      name.Form("xn[%d]", i); k[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]*%f>> k%d" ,i ,i, xnScale[i], i);
      gate[i].Form("xf[%d]!=0 && xn[%d]!=0", i, i);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "");
      line.SetX2(para[i][0]);
      line.SetY1(para[i][0]);
      line.Draw("same");
      
      cAlpha->Update();
      gSystem->ProcessEvents();
   }
   
   //--------- 4, pause for saving correction parameter
   cAlpha->Update();
   gSystem->ProcessEvents();
   printf("0 for stop, 1 for save xf-xn-correction : ");
   temp = scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){   
      FILE * paraOut;
      TString filename;
      filename.Form("correction_xf_xn.dat");
      paraOut = fopen (filename.Data(), "w+");
      printf("=========== save xf_xn-correction parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f\n", -para[i][1]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      //cAlpha->SaveAs("alpha_xf_xn_corrected.pdf");
   }
   
   return;
   
}

