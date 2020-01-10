#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
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

vector<vector<double>> combination(const vector<double> arr, int r){
  
  vector<vector<double>> output;
  
  int n = arr.size();
  std::vector<int> v(n);
  std::fill(v.begin(), v.begin()+r, 1);
  do {
    ///printf("%d |", r);
    ///for( int i = 0; i < n; i++) { printf("%d ", v[i]); }; printf("\n");
    
    vector<double> temp;
    for (int i = 0; i < n; ++i) { 
      if (v[i]) {
        ///printf("%.1f, ", arr[i]); 
        temp.push_back(arr[i]);
      }
    }
    ///printf("\n");
    
    output.push_back(temp);
    
  } while (std::prev_permutation(v.begin(), v.end()));
  
  return output;
}

double* sumMeanVar(vector<double> data){
  
  
  int n = data.size();
  //for( int k = 0; k < n ; k++){ printf("%.1f, ", data[k]); }; printf("\n");
  
  double sum = 0;
  for( int k = 0; k < n; k++) sum += data[k];
  double mean = sum/n;
  double var = 0;
  for( int k = 0; k < n; k++) var += pow(data[k] - mean,2);
  
  static double output[3];
  output[0] = sum;
  output[1] = mean;
  output[2] = var;
  
  return output;
}

void Cali_xf_xn(TTree * tree){
/**///======================================================== initial input
   
   const int rowDet = 6;
   const int colDet = 5;
   
   const int numDet = rowDet * colDet;
   
   int energyRange[3] = {200, 3100, 7000}; // bin, min, max
   
/**///========================================================  load tree

   printf("============================================================= \n");
   printf("====================== Cali_xf_xn.C ========================= \n");
   printf("============================================================= \n");
   printf("==   calibration for PSD detectors using alpha souce. \n");
   printf("------------------------------------------------------------- \n");
   printf("1, calibration energy using charateristic energy of alpha souce. \n");
   printf("2, calibration xf-xn with energy-gate. \n");
   printf("------------------------------------------------------------- \n");
   printf("=========== Total #Entry: %10lld \n", tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {colDet,rowDet};  //x,y
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
      q[i] = new TH1F(name, name, energyRange[0], energyRange[1], energyRange[2]);
      q[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d] >> q%d" ,i, i);
      //gate[i].Form("ring[%d]==0 && !TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i, i);
      //gate[i].Form("!TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i);
      //gate[i].Form("e[%d] > 0", i);
      
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
   
   if( method == 9 ) {
      gROOT->ProcessLine(".q");
      return;
   }
   
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
         p[i] = new TH1F(name, name,  energyRange[0], energyRange[1], energyRange[2]);
         p[i]->SetXTitle(name);
         
         TString expression;
         expression.Form("e[%d]  * %f >> p%d", i,  xHalf[refID]/xHalf[i], i);
         //gate[i].Form("e[%d] > 0", i);
         cAlpha->cd(i+1);
         tree->Draw(expression, gate[i] , "");
         gSystem->ProcessEvents();
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
         nPeaks = spec->Search(q[i], 1, "", 0.30);
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
      printf("-2 =  use 228Th, first 5 strongest peaks \n");
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
         for( int k = 0; k < n; k++) refEnergy.push_back(energy[refID][k]);
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
      
      if( refID == -2 ){
         refEnergy.clear();
         refEnergy.push_back(5.423);
         refEnergy.push_back(5.685);
         refEnergy.push_back(6.288);
         refEnergy.push_back(6.778);
         refEnergy.push_back(8.785);
      }
      
      printf("----- adjusting the energy to det-%d......\n", refID);
      int n = refEnergy.size();
      for( int k = 0; k < n; k++) printf("%2d-th peak : %f \n", k,  refEnergy[k]);
      
      const vector<double> refEnergy0 = refEnergy; 
      
      for( int i = 0; i < numDet; i ++){
        
        nPeaks = energy[i].size();
        printf("------- refID - %d, nPeaks: %d \n", i, nPeaks);
        
        refEnergy = refEnergy0;
        
        if( refID >= 0 && refID == i ){
          a0[i] = 0;
          a1[i] = 1;
          printf("skipped - itself\n");
          continue;
        }
        
        if( energy[i].size() == 0) {
          a0[i] = 0;
          a1[i] = 1;
          printf("skipped\n");
          continue;
        }
        
        vector<double> fitEnergy;
        
        //===== when nPeaks != refEnergy.size(), need to matching the two vector size by checking the r-squared.
        if( nPeaks > n ){
          
          vector<vector<double>> output = combination(energy[i], n);
        
          double * smvY = sumMeanVar(refEnergy0);
          double sumY = smvY[0];
          double meanY = smvY[1];
          double varY = smvY[2];
          
          double optRSquared = 0;
          double absRSqMinusOne = 1;
          int maxID = 0;
          
          for( int k = 0; k < output.size(); k++){
            
            ///for( int t = 0 ; t < nPeaks; t++) printf("%f\t", output[k][t]);
            
            double * smvX = sumMeanVar(output[k]);
            double sumX = smvX[0];
            double meanX = smvX[1];
            double varX = smvX[2];
            
            double sumXY = 0;
            for( int j = 0; j < n; j++) sumXY += output[k][j] * refEnergy0[j];
            
            double rSq = (sumXY - sumX*sumY/n)/sqrt(varX*varY);
            
            //for( int j = 0; j < n ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
            
            if( abs(rSq-1) < absRSqMinusOne ) {
              absRSqMinusOne = abs(rSq-1);
              optRSquared = rSq;
              maxID = k;
            }
          }
          
          fitEnergy = output[maxID];
          
          printf(" R^2 : %.20f\n", optRSquared);      
          
          //calculation fitting coefficient
          //double * si = fitSlopeIntercept(fitEnergy, refEnergy);
          //printf( " y = %.4f x + %.4f\n", si[0], si[1]);
          
        }else if( nPeaks < n ){
          
          vector<vector<double>> output = combination(refEnergy0, energy[i].size());
          
          fitEnergy = energy[i];
          
          double * smvX = sumMeanVar(fitEnergy);
          double sumX = smvX[0];
          double meanX = smvX[1];
          double varX = smvX[2];
          
          double optRSquared = 0;
          double absRSqMinusOne = 1;
          int maxID = 0;
          
          for( int k = 0; k < output.size(); k++){
            
            double * smvY = sumMeanVar(output[k]);
            double sumY = smvY[0];
            double meanY = smvY[1];
            double varY = smvY[2];
            
            double sumXY = 0;
            for( int j = 0; j < nPeaks; j++) sumXY += output[k][j] * fitEnergy[j];
            
            double rSq = (sumXY - sumX*sumY/nPeaks)/sqrt(varX*varY);
            
            //for( int j = 0; j < n ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
            
            if( abs(rSq-1) < absRSqMinusOne ) {
              absRSqMinusOne = abs(rSq-1);
              optRSquared = rSq;
              maxID = k;
            }
          }
          
          refEnergy = output[maxID];
          printf(" R^2 : %.20f\n", optRSquared);   
        
        }else{
          fitEnergy = energy[i];
        }
        
        printf("   Energy : ");
        for( int k = 0; k < nPeaks; k++){ printf("%.1f, ", energy[i][k]);};printf("\n");
        printf("fitEnergy : ");
        for( int k = 0; k < min(n,nPeaks) ; k++){ printf("%.1f, ", fitEnergy[k]); }; printf("\n");
        printf("refEnergy : ");
        for( int k = 0; k < min(n,nPeaks) ; k++){ printf("%.1f, ", refEnergy[k]); }; printf("\n");
        
        TGraph * graph = new TGraph(min(n, nPeaks), &fitEnergy[0], &refEnergy[0] );
        cAlpha->cd(i+1);
        graph->Draw("A*");

        TF1 * fit = new TF1("fit", "pol1" );
        graph->Fit("fit", "q");

        a0[i] = fit->GetParameter(0);
        a1[i] = fit->GetParameter(1);

        printf("%2d | a0: %6.3f, a1: %6.3f (%14.8f) \n", i, a0[i], a1[i], 1./a1[i]);
        
      }
        
   }
   
  //====== Plot adjusted spectrum
  TCanvas * cAux = new TCanvas ("cAux", "cAux", 600, 400);
  TH1F ** p = new TH1F*[numDet];
  double yMax = 0;

  for ( int i = 0; i < numDet; i ++){
    TString name;
    name.Form("p%d", i);
    
    p[i] = new TH1F(name, name,  energyRange[0], refEnergy[0] * 0.9, refEnergy.back() * 1.1);
    p[i]->SetXTitle(name);
    p[i]->SetLineColor(i+1);

    TString expression;
    expression.Form("e[%d] * %.8f + %.8f >> p%d", i, a1[i], a0[i], i);
    gate[i].Form("");
    tree->Draw(expression, gate[i] , "");
    
    if( p[i]->GetMaximum() > yMax ) yMax = p[i]->GetMaximum();
    cAux->Update();
    gSystem->ProcessEvents();
  }   

  cAux->cd(1);
  yMax = yMax * 1.1;
  p[0]->SetMaximum(yMax);
  p[0]->Draw();
  gSystem->ProcessEvents();
  for( int  i = 0; i < numDet; i++){
    p[i]->SetMaximum(yMax);
    p[i]->Draw("same");
    gSystem->ProcessEvents();
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
   
   gSystem->ProcessEvents();
   
   //############################################################  for xf-xn correction
   printf("############## xf - xn correction \n");
   TLine line(0,0,0,0);
   line.SetLineColor(4);
   
   double eGate = 0;
   if( method == 2) {
      gSystem->ProcessEvents();
      int peakID = 0;
      printf("------ pick the i-th peak (0, 1, ... , %d, -1 to stop): ", (int) refEnergy.size() - 1);
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
      h[i] = new TH2F(name, name,  energyRange[0], 0, energyRange[2], energyRange[0], 0, energyRange[2]);
      name.Form("xf[%d]", i); h[i]->SetYTitle(name);
      name.Form("xn[%d]", i); h[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]>> h%d" , i, i, i);
      if(method == 1) gate[i].Form("xf[%d]>0 && xn[%d]>0 && TMath::Abs(e[%d]-%f)<50", i, i, i, xHalf[i]-25);
      if(method == 2) gate[i].Form("xf[%d]>0 && xn[%d]>0 && TMath::Abs(e[%d] * %f + %f - %f)< %f", i, i, i, a1[i], a0[i], eGate, eGate * 0.05);
      
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "colz");
      
      line.SetX2(xHalf[i]-100);
      line.SetY1(xHalf[i]-100);
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
      
      if( energy[i].size() == 0) {
        xnScale[i] = 0.000;
        printf("skipped - detID-%d\n", i);
        continue;
      }
      
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
      k[i] = new TH2F(name, name,  energyRange[0], 0, energyRange[2],  energyRange[0], 0, energyRange[2]);
      name.Form("xf[%d]", i); k[i]->SetYTitle(name);
      name.Form("xn[%d]", i); k[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]*%f>> k%d" ,i ,i, xnScale[i], i);
      //gate[i].Form("xf[%d]!=0 && xn[%d]!=0", i, i);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "colz");
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
   
   gROOT->ProcessLine(".q");
   return;
   
}

