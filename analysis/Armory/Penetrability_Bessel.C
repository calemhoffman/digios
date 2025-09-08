#include <iostream>
#include "TF1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TLegend.h"
#include "TStyle.h"
#include <gsl/gsl_sf_bessel.h>

// Penetrability function using GSL Bessel functions
Double_t penetrability(Double_t E, Double_t l, Double_t R) {
    Double_t k = TMath::Sqrt(2 * E); // Assuming mass = 1 for simplicity
    Double_t rho = k * R;

    double j_l = gsl_sf_bessel_jl(l, rho); // Regular spherical Bessel function
    double n_l = gsl_sf_bessel_yl(l, rho); // Regular spherical Neumann function

    return rho / (j_l * j_l + n_l * n_l);
}

Double_t breitWignerWithPenetrability(Double_t *x, Double_t *par) {
    Double_t E = x[0]; 
    Double_t E0 = par[0];  
    Double_t Gamma0 = par[1];  
    Double_t A = par[2];  
    Double_t l = par[3];  
    Double_t R = par[4];  

    Double_t P = penetrability(E, l, R);
    Double_t P0 = penetrability(E0, l, R);
    Double_t Gamma = Gamma0 * P / P0;

    return A * (Gamma / 2) / (TMath::Power(E - E0, 2) + TMath::Power(Gamma / 2, 2));
}

Double_t breitWignerWithPenetrabilityAndResolution(Double_t *x, Double_t *par) {
    Double_t E = x[0];
    Double_t E0 = par[0];
    Double_t Gamma0 = par[1];
    Double_t A = par[2];
    Double_t l = par[3];
    Double_t R = par[4];
    Double_t sigma = par[5];

    TF1 *bw = new TF1("bw", breitWignerWithPenetrability, E - 5 * sigma, E + 5 * sigma, 5);
    bw->SetParameters(E0, Gamma0, A, l, R);

    Double_t result = 0;
    Double_t step = sigma / 20;
    for (Double_t e = E - 5 * sigma; e <= E + 5 * sigma; e += step) {
        result += bw->Eval(e) * TMath::Gaus(E - e, 0, sigma) * step;
    }

    delete bw;
    return result;
}

void drawResonanceWithPenetrabilityAndFit() {
    gStyle->SetOptStat(0);
    TCanvas *c1 = new TCanvas("c1", "Breit-Wigner with Penetrability, Resolution, Data, and Fit", 800, 600);
    
    // Parameters
    Double_t E0 = 5.0;    
    Double_t Gamma0 = 0.5; 
    Double_t A = 1000.0;  
    Double_t l = 0;       
    Double_t R = 5.0;     
    Double_t sigma = 0.1; 

    TF1 *f1 = new TF1("f1", breitWignerWithPenetrabilityAndResolution, 0, 10, 6);
    f1->SetParameters(E0, Gamma0, A, l, R, sigma);
    
    f1->SetParNames("E0", "Gamma0", "A", "l", "R", "sigma");
    f1->SetNpx(1000);

    TH1F *hData = new TH1F("hData", "Synthetic Data", 100, 0, 10);

    TRandom3 *rand = new TRandom3();
    Int_t nEvents = 100000;
    
    for (Int_t i = 0; i < nEvents; i++) {
        Double_t E = rand->Uniform(0, 10);
        if (rand->Uniform(0, 1) < f1->Eval(E) / f1->GetMaximum()) {
            Double_t E_smeared = rand->Gaus(E, sigma);
            hData->Fill(E_smeared);
        }
    }

    TF1 *fFit = new TF1("fFit", breitWignerWithPenetrabilityAndResolution, 0, 10, 6);
    
    fFit->SetParameters(5.0, 0.5, hData->GetMaximum(), l, R, sigma);  
   
   // Fix parameters if necessary
   fFit->FixParameter(3, l);  
   fFit->FixParameter(4, R);  

   TFitResultPtr fitResult = hData->Fit(fFit,"S");

   hData->Draw("E");
   hData->SetMarkerStyle(20);
   hData->SetMarkerSize(0.7);
   hData->SetTitle("Breit-Wigner with Penetrability and Resolution");
   hData->GetXaxis()->SetTitle("Energy (MeV)");
   hData->GetYaxis()->SetTitle("Counts");
   
   f1->SetLineColor(kBlue);
   f1->SetLineWidth(2);
   f1->DrawCopy("SAME");

   fFit->SetLineColor(kRed);
   fFit->SetLineWidth(2);
   fFit->Draw("SAME");

   TLegend *legend = new TLegend(0.65, 0.7, 0.85, 0.85);
   legend->AddEntry(hData,"Data","p");
   legend->AddEntry(f1,"True","l");
   legend->AddEntry(fFit,"Fit","l");
   legend->SetBorderSize(0);
   legend->Draw();

   c1->Draw();

   std::cout << "Fit Results:" << std::endl;
   std::cout << "E0     = " << fFit->GetParameter(0) << " +/- " << fFit->GetParError(0) << std::endl;
   std::cout << "Gamma0 = " << fFit->GetParameter(1) << " +/- " << fFit->GetParError(1) << std::endl;
   std::cout << "A      = " << fFit->GetParameter(2) << " +/- " << fFit->GetParError(2) << std::endl;
   std::cout << "sigma  = " << fFit->GetParameter(5) << " +/- " << fFit->GetParError(5) << std::endl;
}
