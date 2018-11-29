#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <fstream>
#include <TObjArray.h>

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


TGraph * g1, *g2, *gData;

Double_t func1(Double_t *x, Double_t *) {return g1->Eval(x[0]);}
Double_t func2(Double_t *x, Double_t *) {return g2->Eval(x[0]);}
Double_t func(Double_t *x, Double_t *para) {
   double f1 = g1->Eval(x[0]);
   double f2 = g2->Eval(x[0]);
   double val = para[0] * f1 + para[1] * f2;
   
   return val;
}

TString expression, gate_sim, gate_exp, gate_det, gate_aux;

void AngularDist(int ExID, int nSplit = 2, double dEx = 0.2, int bin = 80) {   
   // number of partition = nSplit + 1
   
/**///======================================================== initial input
   
   const char* rootfile0="A_gen_run30.root"; const char* treeName0="tree";
   const char* rootfile1="transfer.root"; const char* treeName1="tree";
   //the transfer.root contain tx# for thetaCM vs z
   
   double zRange[3] = {400, -500, -100};
   
   //setting for simulation
   double thetaCMLow = 5; //deg
   double dEx_sim = 0.05; // MeV
   int detID[2] = {1, 4}; // start, end detID
   
   double ExRange[3] = {100, -1, 4};
   double threshold = 0.1;
   
   bool isIgnoreEdge = true; // at least nSplit >= 2;
   
   double factor = 1./400; // scaling factor to d.s.c.
   
   TString gate_x;
   
   gate_aux = "&& e > 1 ";
   
   gate_exp.Form("hitID >= 0 && 0 <= detID && detID < 24");
   
    
/**///========================================================  load tree
   
   TFile *file0 = new TFile (rootfile0, "read"); 
   TTree *tree0 = (TTree*)file0->Get(treeName0);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10lld \n", rootfile0,  tree0->GetEntries());
   
   TFile *file1 = new TFile (rootfile1, "read"); 
   TTree *tree1 = (TTree*)file1->Get(treeName1);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10lld \n", rootfile1,  tree1->GetEntries());
   
/**///======================================================== Browser or Canvas
   
   int numDetID = detID[1]-detID[0]+1;
   Int_t Div[2] = {numDetID,3};  //x,y
   Int_t size[2] = {300,300}; //x,y // for each division
   
   TCanvas * cAngularDist = new TCanvas("cAngularDist", "cAngularDist", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAngularDist->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAngularDist->cd(i)->SetGrid();
   }
   cAngularDist->cd(1);
   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(0.8);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files
   vector<double> pos;
   double length = 50.5;
   double firstPos = 0;
   int iDet = 6;
   int jDet = 4;

   string detGeoFileName = "detectorGeo_upstream.txt";
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file(detGeoFileName.c_str(), std::ifstream::in);
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) jDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < iDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       return ;
   }
/**///========================================================= Analysis
   
   TH1F * spec  = new TH1F("spec" , "spec"  , ExRange[0], ExRange[1], ExRange[2]);
   spec ->SetXTitle("Ex [MeV]");   
   
   vector<double> ExAdapted;
   vector<double> countAdapted;
   vector<double> countErrAdapted;
   vector<int> detIDAdapted;
   vector<double> meanAngle;
   ExAdapted.clear();
   countAdapted.clear();
   countErrAdapted.clear();
   detIDAdapted.clear();
   meanAngle.clear();
   vector<double> dCos; 
   dCos.clear(); 
   
   //TH1F* h = new TH1F("h", "thetaCM", 200, 0, 50);
   TH2F* h2 = new TH2F("h2", "thetaCMvZ", zRange[0], zRange[1], zRange[2], 500, 0, 50); 

   gate_sim.Form("loop == 1 && ExID == %d && thetaCM > %f && hit == 1", ExID, thetaCMLow);      

   printf("============================== gates:\n");
   printf("gate_sim    : %s\n", gate_sim.Data());
   printf("gate_exp    : %s\n", gate_exp.Data());
   printf("gate_aux    : %s\n", gate_aux.Data());
   printf("threshold   : %f\n", threshold);
   if( isIgnoreEdge ){
      printf("isIgnoreEdge: true \n");
   }else{
      printf("isIgnoreEdge: false \n");
   }
   
   TGraph * g0 = (TGraph *) ((TObjArray *)file1->Get("txList"))->At(ExID);
   
   for( int idet = detID[0]; idet <= detID[1]; idet++){
      
      printf("============================== detID : %d\n", idet);
      gate_det.Form("&& detID%6 == %d", idet);  
      
      double pos0, pos1;
      if( firstPos > 0 ){
         pos0 = pos[idet];
         pos1 = pos[idet]+length;
      }else{
         pos0 = pos[idet]-length;
         pos1 = pos[idet];
      }
      
      for(int iSplit = 0; iSplit < nSplit+1; iSplit ++){
            
         //========define gate_x
         double split[2] = {-1.5, 1.5};
         split[0] = -1. + 2./ (nSplit+1) * (iSplit);
         split[1] = -1. + 2./ (nSplit+1) * (iSplit+1);
 
         if( split[0] < -1.0 + 0.01 ) split[0] = -1.5;
         if( split[1] > +1.0 - 0.01 ) split[1] = +1.5;

         if( isIgnoreEdge && nSplit >=2 && (split[0] < -1 || split[1] > 1)) continue;

         gate_x.Form("&& (%f < x && x < %f)", split[0], split[1]);
         
         printf("gate_x      : %s\n", gate_x.Data());
               
         detIDAdapted.push_back(idet);
         
         //########################################################## for simulation data
         cAngularDist->cd(idet+1 - detID[0]);
         //tree1->Draw("thetaCM >> h", gate_sim + gate_det + gate_x);
         tree1->Draw("thetaCM : z >> h2", gate_sim + gate_det+gate_x);
         g0->Draw("same");
         cAngularDist->Update();

         //if( h->GetEntries() == 0){
         //   printf(" no data for detID == %d \n", idet);
         //   continue;
         //}
         //============== find the acceptance, the angle the count drop
         // using the calculated TGraph in transfer.root
         vector<double> angle;
         angle.clear();
         
         double posSplit[2];
         if( split[0] < -1 ){ 
            posSplit[0] = pos0;
         }else{
            posSplit[0] = pos0 + (split[0] + 1) * (pos1 - pos0)/2;
         }
         
         if( split[1] > +1 ){ 
            posSplit[1] = pos1;
         }else{
            posSplit[1] = pos0 + (split[1] + 1) * (pos1 - pos0)/2;
         }
         
         if( g0->Eval(posSplit[0]) < thetaCMLow ) {
            angle.push_back(thetaCMLow);
         }else{
            angle.push_back(g0->Eval(posSplit[0]));
         }
         angle.push_back(g0->Eval(posSplit[1]));
             
         //============ print angle
         for( int j = 0; j < (int) angle.size()/2; j++){
            double delta = angle[2*j+1] - angle[2*j];
            meanAngle.push_back((angle[2*j+1] + angle[2*j])/2);
            
            dCos.push_back(TMath::Sin(meanAngle.back()*TMath::DegToRad())*(delta*TMath::DegToRad()));
            
            printf("%10.5f - %10.5f = %10.5f | %10.5f, %10.5f \n", 
                       angle[2*j], 
                       angle[2*j+1], 
                       delta,
                       meanAngle.back(),
                       1./TMath::Sin(meanAngle.back()*TMath::DegToRad())*(delta*TMath::DegToRad()));
         }
         
         //########################################################## for experimental data
         //================= plot Ex
         //printf("============= plot Ex with gates\n");    
			cAngularDist->cd(idet+1 - detID[0] + numDetID);
         tree0->Draw("Ex>>spec ", gate_exp + gate_x + gate_det + gate_aux, "");
         
         spec ->Draw();
         //gPad->WaitPrimitive();
         
         TH1F * specS = (TH1F*) spec->Clone();
         
         //=================== find peak and fit
         //printf("============= estimate background and find peak\n");
         TSpectrum * peak = new TSpectrum(10);
         peak->Search(spec, 1, "", threshold);
         TH1 * h1 = peak->Background(spec,10);
         h1->Draw("same");
         
         TString title;
         specS->SetName("specS");
         specS->Add(h1, -1.);
         specS->Sumw2();
         cAngularDist->cd(idet+1 - detID[0] + numDetID*2);
         specS->Draw();
         
         cAngularDist->Update();
         
         //========== Fitting 
         //printf("============= Fit.....");
         nPeaks  = peak->Search(specS, 1, "", threshold);
         printf("========== found %d peaks \n", nPeaks);
         //float * xpos = peak->GetPositionX();
         //float * ypos = peak->GetPositionY();
         
         Double_t * xpos = peak->GetPositionX();
         Double_t * ypos = peak->GetPositionY();
         
         int * inX = new int[nPeaks];
         TMath::Sort(nPeaks, xpos, inX, 0 );
         vector<double> energy, height;
         for( int j = 0; j < nPeaks; j++){
            energy.push_back(xpos[inX[j]]);
            height.push_back(ypos[inX[j]]);
         }
         
         const int  n = 3 * nPeaks;
         double * para = new double[n]; 
         for(int j = 0; j < nPeaks ; j++){
            para[3*j+0] = height[j] * 0.05 * TMath::Sqrt(TMath::TwoPi());
            para[3*j+1] = energy[j];
            para[3*j+2] = 0.05;
         }

         TF1 * fit2 = new TF1("fit2", fpeaks, ExRange[1], ExRange[2], 3* nPeaks );
         fit2->SetNpx(1000);
         fit2->SetParameters(para);
         specS->Fit("fit2", "q");
          
         const Double_t* paraE = fit2->GetParErrors();
         const Double_t* paraA = fit2->GetParameters();
         
         double bw = specS->GetBinWidth(1);
         
         double * ExPos = new double[nPeaks];
         double * count = new double[nPeaks];
         double * countErr = new double[nPeaks];
         for(int j = 0; j < nPeaks ; j++){
            ExPos[j] = paraA[3*j+1];
            count[j] = paraA[3*j] / bw;
            countErr[j] = paraE[3*j] / bw; 
            printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
                    idet, 
                    paraA[3*j] / bw,   paraE[3*j] /bw, 
                    paraA[3*j+1], paraE[3*j+1],
                    paraA[3*j+2], paraE[3*j+2]);
         }
         
         //pick sutable count, using ExID
         //TODO, check Ex is correct, or change to Ex, not ExID
         ExAdapted.push_back(ExPos[ExID]);
         countAdapted.push_back(count[ExID]);
         countErrAdapted.push_back(countErr[ExID]);
      
      }
      
   }
   
   printf("================ summary \n");
   int nDet = (int) detIDAdapted.size();
   vector<double> dXsec;
   double countMax = 0;
   double countMin = 10000;
   for( int idet = 0; idet < nDet; idet++){
      dXsec.push_back( countAdapted[idet] * factor );
      if( dXsec[idet] > countMax) countMax = dXsec[idet];
      if( dXsec[idet] < countMin) countMin = dXsec[idet];
      printf("detID : %d, Mean-Angle: %6.2f, Count: %6.0f(%3.0f), Ex-Fit:%6.2f, Xsec:%6.4e mb/sr \n", 
               detIDAdapted[idet], 
               meanAngle[idet], countAdapted[idet], countErrAdapted[idet], ExAdapted[idet], dXsec[idet]); 
      
   }
   
   double yMax = TMath::Power(10,TMath::Ceil(TMath::Log10(countMax)));
   double yMin = TMath::Power(10,TMath::Floor(TMath::Log10(countMin)));
   
   if( ExID >= 1) yMax = 30;
      
   TCanvas * cG = new TCanvas("cG", "cG", 200, 50, 500, 900);
   cG->Divide(1,2);
   cG->cd(1);
   cG->cd(1)->SetGrid();
   cG->cd(1)->SetLogy();

   gData = new TGraph(nDet, &meanAngle[0], &dXsec[0]);
   gData->GetYaxis()->SetRangeUser(yMin, yMax);
   gData->GetXaxis()->SetLimits(10,40);
   TString gTitle;
   gTitle.Form("dXsec/dOmega, ExID = %d", ExID);
   gData->SetTitle(gTitle);
   gData->GetXaxis()->SetTitle("thetaCM [deg]");
   gData->GetYaxis()->SetTitle("d.s.c [mb/sr]");
   gData->Draw("A*");
   
   cG->cd(2);
   cG->cd(2)->SetGrid();
   tree1->Draw("z : thetaCM >> h3(300, 10, 40, 300, -500, -100)", gate_sim +gate_x);
  
   
   //################################################################## if Xsec exist, plot and fit
    //======== load Xsec data
   string xsecFileName = "208Pb_dp.out.Xsec.txt";
   ifstream fileXsec(xsecFileName.c_str(), std::ifstream::in);
   printf("==========  using theoretical Xsec : %s \n", xsecFileName.c_str());
   string line;
   vector<double> angle;
   vector<double> f1;
   vector<double> f2;
   TString g1Name, g2Name;
   double range[2] = {1,1};
   
   int fitCol[2] = {ExID+1,ExID+1};
   
   if( fileXsec.is_open() ){
      int lineNum = 0;
      while( ! fileXsec.eof() ){
         getline(fileXsec, line);
         lineNum += 1;
         if( lineNum == 1 ) {
            g1Name = line.substr(18*fitCol[0], 18);
            g2Name = line.substr(18*fitCol[1], 18);
            continue;
         }
         // get angle
         int len = line.length();
         if( (18 + 18*fitCol[0] > len) || (18 + 18*fitCol[1] > len)) {
            printf("accessing location longer then line.length() @ lineNum : %d, line-Length: %d \n", lineNum, len);
            break;
         }
         angle.push_back(atof(line.substr(0, 18).c_str()));
         f1.push_back(atof(line.substr(18*fitCol[0], 18).c_str()));
         f2.push_back(atof(line.substr(18*fitCol[1], 18).c_str()));
         
         if( f1.back() > range[0] ) range[0] = f1.back();
         if( f1.back() < range[1] ) range[1] = f1.back();
         
         if( fitCol[1] > 0 ){
            if( f2.back() > range[0] ) range[0] = f2.back();
            if( f2.back() < range[1] ) range[1] = f2.back();
         }
         
         //printf("%d| %f, %f, %f \n", lineNum,  angle.back(), f1.back(), f2.back());
      }
      fileXsec.close();
   }else{
      printf("... fail\n");
      return;
   }
   
   g1 = new TGraph(); g1->SetLineColor(4); g1->SetTitle(g1Name);
   g2 = new TGraph(); g2->SetLineColor(2); g2->SetTitle(g2Name);
   
   for( int i =0 ; i < (int) angle.size() ; i++){
      g1->SetPoint(i, angle[i], f1[i]);
      g2->SetPoint(i, angle[i], f2[i]);
   }
   
   cG->cd(1);
   g1->Draw();
   
   TLegend* legend = new TLegend(0.6, 0.7, 0.9, 0.9);
   legend->AddEntry(g1, g1Name , "l");
   if( fitCol[1] > 0 && fitCol[1] != fitCol[0] ) legend->AddEntry(g2, g2Name , "l");
   TString gDataName;
   legend->AddEntry(gData, gTitle, "l");
   legend->Draw();
  
}

