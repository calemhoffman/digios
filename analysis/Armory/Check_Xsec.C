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

TGraph * g1, * g2;
Double_t func1(Double_t *x, Double_t *) {return g1->Eval(x[0]);}
Double_t func2(Double_t *x, Double_t *) {return g2->Eval(x[0]);}
Double_t func(Double_t *x, Double_t *para) {
   double f1 = g1->Eval(x[0]);
   double f2 = g2->Eval(x[0]);
   double val = para[0] * f1 + para[1] * f2;
   
   return val;
}

void Check_Xsec(){  
/**///======================================================== User input
   const char* rootfile="checkResult.root"; 

	int ExID = 0;
	double scale = 80;
	int fitCol[2] = {ExID+1,ExID+1};

/**///======================================================== read tree   
   printf("################### Check_Xsec.C ######################\n");
   
   TFile *file0 = new TFile (rootfile, "read"); 
   
   if( !file0->IsOpen() ){
		printf("cannot found checkResult.root, please run AutoCali/Check_e_z.C\n");
		return;
	}
   
   gStyle->SetOptStat(0);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.4);
   gStyle->SetStatH(0.2);
   gStyle->SetLabelSize(0.05, "XY");
   gStyle->SetTitleFontSize(0.1);

   TCanvas * cXsec = new TCanvas("cXsec", "cXsec", 0, 0, 600, 400);
   if(cXsec->GetShowEditor() )cXsec->ToggleEditor();
   if(cXsec->GetShowToolBar() )cXsec->ToggleToolBar();
	cXsec->SetLogy();
	cXsec->SetGrid();

/**///======================================================== Get the thetaCM
	TObjArray * hThetaList = (TObjArray *) file0->Get("hThetaList");
	//TLegend * legend = (TLegend*) file0->Get("legend"); 
	TF1 * fit = (TF1*) file0->Get("fit");
	
	int numPlot = hThetaList->GetEntries();
	printf("========== found %d hTheta plots\n", numPlot);
	//legend->Print();
	const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double * ExPos   = new double[numPlot];
   double * ExSigma = new double[numPlot];   
   for(int i = 0; i < numPlot ; i++){
      ExPos[i] = paraA[3*i+1];
      ExSigma[i] = paraA[3*i+2];
      printf("%2d | mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
              i, 
              paraA[3*i+1], paraE[3*i+1],
              paraA[3*i+2], paraE[3*i+2]);
   }
	
	TGraph * gData = (TGraph *)hThetaList->At(ExID);
	gData->Draw();
	
/**///========================================================== if Xsec exist, plot and fit
   string xsecFileName = "208Pb_dp.out.Xsec.txt";
   ifstream fileXsec(xsecFileName.c_str(), std::ifstream::in);
   printf("==========  using theoretical Xsec : %s \n", xsecFileName.c_str());
   string line;
   TString g1Name, g2Name;
   double range[2] = {1,1};
   
	g1 = new TGraph(); g1->SetLineColor(4); g1->SetTitle(g1Name);
   g2 = new TGraph(); g2->SetLineColor(2); g2->SetTitle(g2Name);

   if( fileXsec.is_open() ){
		int n = 0;
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
			double angle = atof(line.substr(0, 18).c_str());
         double f1 = atof(line.substr(18*fitCol[0], 18).c_str());
         double f2 = atof(line.substr(18*fitCol[1], 18).c_str());
         
         if( f1 > range[0] ) range[0] = f1;
         if( f1 < range[1] ) range[1] = f1;
         
         if( fitCol[1] > 0 ){
            if( f2 > range[0] ) range[0] = f2;
            if( f2 < range[1] ) range[1] = f2;
         }
         
         g1->SetPoint(n, angle, scale*f1);
			g2->SetPoint(n, angle, scale*f2);   
         
         n++;
         //printf("%d| %f, %f, %f \n", lineNum,  angle.back(), f1.back(), f2.back());
      }
      fileXsec.close();
   }else{
      printf("... fail\n");
      return;
   }
   
   cXsec->cd(1);
   g1->Draw("same");
   
   TLegend* legend = new TLegend(0.6, 0.1, 0.9, 0.3);
   legend->AddEntry(g1, g1Name , "l");
   if( fitCol[1] > 0 && fitCol[1] != fitCol[0] ) legend->AddEntry(g2, g2Name , "l");
   TString gDataTitle;
   gDataTitle.Form("Ex: %5.2f MeV %5.3f keV", ExPos[ExID], ExSigma[ExID]);
   legend->AddEntry(gData, gDataTitle, "l");
   legend->Draw();
   
   
	TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 500, 600, 400);
   if(cAux->GetShowEditor() )cAux->ToggleEditor();
   if(cAux->GetShowToolBar() )cAux->ToggleToolBar();
	cAux->SetGrid();
   
   TH2F * hExTheta = (TH2F*) file0->Get("hExTheta");
   hExTheta->Draw("colz");
   
/**///=================  end of program
}
