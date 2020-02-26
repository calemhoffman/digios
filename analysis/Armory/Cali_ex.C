#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TCutG.h>
#include <TProfile.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TMath.h>
#include "../Armory/AutoFit.C"


//================ prototype
int nPeakFpeaks = 16;
Double_t fpeaks(Double_t *x, Double_t *par);
vector<vector<double>> combination(vector<double> arr, int r);
double* sumMeanVar(vector<double> data);
double*  fitSlopeIntercept(vector<double> dataX, vector<double> dataY);
vector<vector<double>> FindMatchingPair(vector<double> en1, vector<double> en2);

void Cali_ex(int detID = -1){

   int numRow = 4; // number of side of the array
   int numCol = 6; // 
   
   double eRange[2] = {500, 3000}; 
   
   int numSec = 10; // number of section in x
   
   
   vector<double> refEnergy;
   refEnergy.push_back(0.00);
   refEnergy.push_back(0.798);
   refEnergy.push_back(1.242);
   //refEnergy.push_back(1.771);
   
   int nRefEnergy = refEnergy.size();
   
   //====================================================== read file
   
   TChain * tree = new TChain("tree");
   tree->Add("temp.root"); 
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
   
   printf("\033[0;32m");
   tree->GetListOfFiles()->Print();
   printf("\033[0m");
   printf("=====================================================\n");
   
   //====================================================== output files
   FILE * paraOut1;
   TString filename1;
   filename1.Form("correction_e_flat.dat");
   paraOut1 = fopen (filename1.Data(), "a+");
   
   FILE * paraOut2;
   TString filename2;
   filename2.Form("correction_eflat_Ex.dat");
   paraOut2 = fopen (filename2.Data(), "a+");

   //====================================================== Canvas
   gStyle->SetOptStat(0);
   
   TCanvas *cAnaT  = new TCanvas("cAnaT", "cAnaT", 800, 800);
   if( !cAnaT->GetShowToolBar() ) cAnaT->ToggleToolBar();
   
   
   int Div[2] = {numSec/2, 2};
   int size[2] = {200,200}; //x,y
   TCanvas * cAna = new TCanvas("cAna", "cAna", 900, 0, size[0]*Div[0], size[1]*Div[1]);
   if( cAna->GetShowEditor() )  cAna->ToggleEditor();
   if( cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   cAna->Divide(Div[0],Div[1]);
   
   TCanvas * cAna2 = new TCanvas("cAna2", "cAna2", 0, 500, 1200, 600);
   cAna2->Divide(1,2);
   cAna2->cd(1)->Divide(4,1);
   
   const int numDet = numRow * numCol;
   
   //====================================================== looping detector
   TString expression;
   
   TH2F * hex[numDet];
   TH2F * hexG[numDet];
   TH1F * he[numDet][numSec];
   TH2F * hexS[numDet];
   TH1F * heS[numDet];
   TH1F * heC[numDet];
   

   
   double * paraFlat[numDet];
   double * parae2Ex[numDet];
   
   int iStart = 0;
   int iEnd = numDet;
   if( detID >= 0 ){
      iStart = detID;
      iEnd = detID+1;
   } 
   
   for( int iDet = iStart; iDet < iEnd ; iDet ++ ){
      
      cAna->SetTitle(Form("detID = %d", iDet));
      cAna2->SetTitle(Form("detID = %d", iDet));
      
      for( int i = 0; i < 4; i++) cAna2->cd(1)->cd(i+1)->Clear();
      cAna2->cd(2)->Clear();
      
      //===================== A TCutG
      cAnaT->Show();
      cAnaT->cd();
      hex[iDet] = new TH2F(Form("hex%d", iDet), Form("e vs x detID = %d", iDet), 200, -1.5, 1.5, 200, eRange[0], eRange[1]);
      
      expression.Form("e:x>>hex%d", iDet);
      tree->Draw(expression, Form("detID==%d", iDet), "colz");
      cAnaT->Modified();
      cAnaT->Update();
      gSystem->ProcessEvents();
      
      printf("======== wait for TCutG (detID =%d)\n", iDet);
      gPad->WaitPrimitive();
      TCutG * cut = (TCutG*) gROOT->FindObject("CUTG");
         
      bool isCutDefined = false;
      if( cut == NULL ){
         printf(" no cut, stopped by user. \n");
         return ;
      }else{
         cut->SetName(Form("cutex%d", iDet));
         cut->SetVarX("x");
         cut->SetVarY("e");
         cut->SetLineColor(2);
         cut->SetLineWidth(2);
         
         isCutDefined = true;
         
         cAnaT->Modified();
         cAnaT->Update();
         gSystem->ProcessEvents();
      }
      
      
      //===================== find the straigthen function
      if( isCutDefined ) {
         
         cAna->Clear();
         cAna->Divide(2,1);
         cAna->cd(1);
         hexG[iDet] = new TH2F(Form("hexG%d", iDet), Form("e vs x detID = %d", iDet), 200, -1.5, 1.5, 200, eRange[0], eRange[1]);
         tree->Draw(Form("e:x>>hexG%d", iDet), Form("cutex%d && detID == %d", iDet, iDet), "colz");
         
         TProfile * haha = hexG[iDet]->ProfileX();
         cAna->cd(2);
         haha->Draw("");
         
         TF1 * fitexG = new TF1("fitexG", "pol4");
         haha->Fit("fitexG", "q");
         paraFlat[iDet] = fitexG->GetParameters();
         
         cAna->Modified();
         cAna->Update();
         gSystem->ProcessEvents();
         
         
         cAna2->cd(1)->cd(1);
         hex[iDet]->Draw("colz");
      
      }else{
         //====================== trace the strongest peak 
      
         TString gate;
         TSpectrum * spec = new TSpectrum();
      
         vector<double> y;
         vector<double> x;
      
         cAna->Clear();
         cAna->Divide(Div[0],Div[1]);
         for( int i = 0; i < numSec; i++) cAna->cd(i+1)->Clear();
      
         for( int i = 0; i < numSec; i++){
            cAna->cd(1+i);
            
            double xPos = -1 + 2. / (numSec - 1) * i;
            
            gate.Form("detID == %d && abs(x-%f)< 0.1", iDet, xPos);
            
            he[i][iDet] = new TH1F(Form("he%d_%d", i, iDet), Form("e at |x-%f|<0.1",xPos), 200, eRange[0], eRange[1]);
            
            tree->Draw(Form("e>>he%d_%d", i, iDet), gate, "");
            gSystem->ProcessEvents();
            
            int event = he[i][iDet]->GetEntries();
            
            int nPeak = spec->Search(he[i][iDet]);
            
            cAna->Modified();
            cAna->Update();
            gSystem->ProcessEvents();
            
            
            if( nPeak > 0  && event > 100) {
               double * ePos = spec->GetPositionX();
               double * eAmp = spec->GetPositionY();
               
               if( eAmp[0] > 3){
                  double yTemp = ePos[0];
                  printf("%d | %f, %f \n", i, xPos, yTemp);
                  y.push_back(yTemp);
                  x.push_back(xPos);
               }
            }
         }
      
         //========================== Plot   
         cAna2->cd(1)->cd(1);
         hex[iDet]->Draw("colz");
         
         TGraph * gex = new TGraph(y.size(), &x[0], &y[0]);
         gex->SetLineColor(2);
         gex->SetLineWidth(2);
         gex->Draw("same");
         
         TF1 * fit = new TF1("fit", "pol4", -2, 2);
         gex->Fit("fit", "q");

         paraFlat[iDet] = fit->GetParameters();
         
         
      }
      
      cAna2->Show();
      cAna2->cd(1)->cd(2);
      hexS[iDet] = new TH2F(Form("hexS%d",iDet), "e vs x straighten", 200, -1.5, 1.5, 200, eRange[0], eRange[1]);
      tree->Draw(Form("e - (%f *x + %f * x*x + %f * x*x*x + %f *x*x*x*x) : x >>hexS%d", paraFlat[iDet][1], paraFlat[iDet][2], paraFlat[iDet][3], paraFlat[iDet][4], iDet), Form("detID==%d", iDet), "colz");
      cAna2->Modified();
      cAna2->Update();
      gSystem->ProcessEvents();
      
      cAna2->cd(1)->cd(3);
      heS[iDet] = new TH1F(Form("heS%d", iDet), "e straighten", 200, eRange[0], eRange[1]);
      tree->Draw(Form("e - (%f *x + %f * x*x + %f * x*x*x + %f *x*x*x*x) >>heS%d", paraFlat[iDet][1], paraFlat[iDet][2], paraFlat[iDet][3], paraFlat[iDet][4], iDet), Form("detID==%d", iDet), "colz");

         
      TSpectrum * peak = new TSpectrum();
      int nPeak = peak->Search(heS[iDet], 2, "", 0.1);
      
      cAna2->Modified();
      cAna2->Update();
      gSystem->ProcessEvents();
      
      double * xpos = peak->GetPositionX();
      double * ypos = peak->GetPositionY();

      int * inX = new int[nPeak];
      TMath::Sort(nPeak, xpos, inX, 0 );
      vector<double> energy, height;
      for( int j = nPeak-1; j >= 0; j--){
         energy.push_back(xpos[inX[j]]);
         height.push_back(ypos[inX[j]]);
         printf("%d | %f, %f \n", j, xpos[inX[j]], ypos[inX[j]]);
      }
      
      int option = 0;
      printf("========== which detector to be the reference?\n");
      printf("-1 =  manual reference\n");
      printf("-2 =  Auto Match \n");
      printf("-9 =  Exit \n");
      printf("your choice = ");
      int temp = scanf("%d", &option);
      
      if( option == -9 ) return;
   
      vector<double> k1, k2;
      k1.clear();
      k2.clear();
      
      if( option == -1 ){
         int n = 0;
         float eng = -1;
         for( int i = 0; i < energy.size(); i++){
            printf("%2d-th peak | %.0f ch to energy (-1 to skip, -2 to stop ):",i, energy[i]);
            temp = scanf("%f", &eng);
            if( eng == -1 ){
               printf("             input: skipped \n");
            }else if (eng == -2){
               break;
            }else{
               printf("             input: %f \n", eng);
               k2.push_back(eng);
               k1.push_back(energy[i]);
            }
         }
         
      }
   
   
      if( option == -2 ){
         //==== Method for finding best matching between energy and refEnergy
         vector<vector<double>> haha = FindMatchingPair(energy, refEnergy);
         
         vector<double> k1 = haha[0];
         vector<double> k2 = haha[1];
      }
      
      TGraph * gX = new TGraph(k1.size(), &k1[0], &k2[0]);
      
      cAna2->cd(1)->cd(4);
      gX->Draw("A*");
      
      
      TF1* fitE = new TF1("fitE", "pol1");
      gX->Fit("fitE", "q");
      cAna2->Modified();
      cAna2->Update();
      gSystem->ProcessEvents();
      
      parae2Ex[iDet] = fitE->GetParameters();
      
      cAna2->cd(2);
      heC[iDet] = new TH1F(Form("heC%d",iDet), "e Corrected", 200, -1, 4);
      tree->Draw(Form("%f + %f*(e - (%f *x + %f * x*x + %f * x*x*x + %f *x*x*x*x)) >>heC%d", parae2Ex[iDet][0], parae2Ex[iDet][1], paraFlat[iDet][1], paraFlat[iDet][2], paraFlat[iDet][3], paraFlat[iDet][4], iDet), Form("detID==%d", iDet), "colz");
      cAna2->Modified();
      cAna2->Update();
      gSystem->ProcessEvents();
      
      //====================================================== save parameter
      fprintf(paraOut1, "%d\t", iDet);
      for( int i = 1; i < 5; i++){
         fprintf(paraOut1, "%11.6f\t", paraFlat[iDet][i]);
      }
      fprintf(paraOut1, "\n");
      
      fprintf(paraOut2, "%d\t", iDet);
      fprintf(paraOut2, "%11.6f\t%11.6f", parae2Ex[iDet][0], 1./parae2Ex[iDet][1]);
      fprintf(paraOut2, "\n");
      
   }
  
   //fitAuto(heC);
   
  
}

/*#####################################################################*/
/*#####################################################################*/
/*#####################################################################*/

Double_t fpeaks(Double_t *x, Double_t *par) {
  Double_t result = 0;
  for (Int_t p=0;p<nPeakFpeaks;p++) {
    Double_t norm  = par[3*p+0];
    Double_t mean  = par[3*p+1];
    Double_t sigma = par[3*p+2];
    result += norm * TMath::Gaus(x[0],mean,sigma, 1);
  }
  return result;
}

vector<vector<double>> combination(vector<double> arr, int r){
  
  vector<vector<double>> output;
  
  int n = arr.size();
  std::vector<int> v(n);
  std::fill(v.begin(), v.begin()+r, 1);
  do {
    //for( int i = 0; i < n; i++) { printf("%d ", v[i]); }; printf("\n");
    
    vector<double> temp;
    for (int i = 0; i < n; ++i) { 
      if (v[i]) {
        //printf("%.1f, ", arr[i]); 
        temp.push_back(arr[i]);
      }
    }
    //printf("\n");
    
    output.push_back(temp);
    
  } while (std::prev_permutation(v.begin(), v.end()));
  
  return output;
}

double* sumMeanVar(vector<double> data){
  
  int n = data.size();
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


double*  fitSlopeIntercept(vector<double> dataX, vector<double> dataY){
  
  double * smvY = sumMeanVar(dataY);
  double sumY = smvY[0];
  double meanY = smvY[1];

  double * smvX = sumMeanVar(dataX);
  double sumX = smvX[0];
  double meanX = smvX[1];
  double varX = smvX[2];
  
  int n = dataX.size();
  double sumXY = 0;
  for( int j = 0; j < n; j++) sumXY += dataX[j] * dataY[j];

  double slope = ( sumXY - sumX * sumY/n ) / varX;
  double intercept = meanY - slope * meanX;
  
  static double output[2];
  output[0] = slope;
  output[1] = intercept;
  
  return output;
  
}


vector<vector<double>> FindMatchingPair(vector<double> enX, vector<double> enY){
   
   int nX = enX.size();
   int nY = enY.size();
   
   vector<double> fitEnergy;
   vector<double> refEnergy;
   
   if( nX > nY ){
      
      vector<vector<double>> output = combination(enX, nY);
      
      double * smvY = sumMeanVar(enY);
      double sumY = smvY[0];
      double meanY = smvY[1];
      double varY = smvY[2];
      
      double optRSquared = 0;
      double absRSqMinusOne = 1;
      int maxID = 0;
      
      for( int k = 0; k < output.size(); k++){
        
        double * smvX = sumMeanVar(output[k]);
        double sumX = smvX[0];
        double meanX = smvX[1];
        double varX = smvX[2];
        
        double sumXY = 0;
        for( int j = 0; j < nY; j++) sumXY += output[k][j] * enY[j];
        
        double rSq = abs(sumXY - sumX*sumY/nY)/sqrt(varX*varY);
        
        //for( int j = 0; j < nY ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
        
        if( abs(rSq-1) < absRSqMinusOne ) {
          absRSqMinusOne = abs(rSq-1);
          optRSquared = rSq;
          maxID = k;
        }
      }
      
      fitEnergy = output[maxID];
      refEnergy = enY;
      
      printf(" R^2 : %.20f\n", optRSquared);      
      
      //calculation fitting coefficient
      //double * si = fitSlopeIntercept(fitEnergy, refEnergy);
      //printf( " y = %.4f x + %.4f\n", si[0], si[1]);
      
    }else if( nX < nY ){
    
      vector<vector<double>> output = combination(enY, nX);
      
      
      double * smvX = sumMeanVar(enX);
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
        for( int j = 0; j < nX; j++) sumXY += output[k][j] * enX[j];
        
        double rSq = abs(sumXY - sumX*sumY/nX)/sqrt(varX*varY);
        
        //for( int j = 0; j < nX ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
        
        if( abs(rSq-1) < absRSqMinusOne ) {
          absRSqMinusOne = abs(rSq-1);
          optRSquared = rSq;
          maxID = k;
        }
      }
      
      fitEnergy = enX;
      refEnergy = output[maxID];
      printf(" R^2 : %.20f\n", optRSquared);   
    
    }else{
      fitEnergy = enX;
    }
    
    
    printf("fitEnergy = ");for( int k = 0; k < fitEnergy.size() ; k++){ printf("%7.2f, ", fitEnergy[k]); }; printf("\n");
    printf("refEnergy = ");for( int k = 0; k < refEnergy.size() ; k++){ printf("%7.2f, ", refEnergy[k]); }; printf("\n");
    
    vector<vector<double>> haha;
    haha.push_back(fitEnergy);
    haha.push_back(refEnergy);
    
    return haha;
   
}
