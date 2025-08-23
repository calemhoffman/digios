#include "TF1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TLegend.h"

// Penetrability function
Double_t penetrability(Double_t E, Double_t l, Double_t R) {
    Double_t k = TMath::Sqrt(2 * E); // Assuming mass = 1 for simplicity
    Double_t rho = k * R;
    if (l == 0) return rho;
    if (l == 1) return TMath::Power(rho, 3) / (1 + TMath::Power(rho, 2));
    if (l == 2) return TMath::Power(rho, 5) / (9 + 3*TMath::Power(rho, 2) + TMath::Power(rho, 4));
    return 0; // For higher l, implement more complex calculations
}

Double_t breitWignerWithPenetrability(Double_t *x, Double_t *par) {
    Double_t E = x[0];
    Double_t E0 = par[0];  // Resonance energy
    Double_t Gamma0 = par[1];  // Reduced width
    Double_t A = par[2];  // Amplitude
    Double_t l = par[3];  // Angular momentum
    Double_t R = par[4];  // Channel radius

    Double_t P = penetrability(E, l, R);
    Double_t P0 = penetrability(E0, l, R);
    Double_t Gamma = Gamma0 * P / P0;

    if( E <= 0 ) return 0 ;

    return A * (Gamma/2) / (TMath::Power(E - E0, 2) + TMath::Power(Gamma/2, 2));
}

Double_t breitWignerWithPenetrabilityAndResolution(Double_t *x, Double_t *par) {
    Double_t E = x[0];
    Double_t sigma = par[5];  // Resolution (standard deviation of Gaussian)

    if( sigma <= 0 ) return breitWignerWithPenetrability(x, par);

    Double_t result = 0;
    Double_t nSigma = 5;
    Double_t nStep = 100.;
    Double_t stepSize = 2 * nSigma * sigma / nStep; // intergaret from (-nSigma, nSigma) * sigma


    for( int i = 0; i < nStep + 1 ; i++ ){
        double e = E - 5 * sigma  +  stepSize * i ;
        if( e <= 0 ) continue; // reduce calculation
        result +=  breitWignerWithPenetrability(&e, par) * TMath::Gaus(E-e, 0, sigma, true) * stepSize;
    }

    return result;
}


void drawResonanceWithPenetrabilityAndFit() {
    TCanvas *c1 = new TCanvas("c1", "Breit-Wigner with Penetrability, Resolution, Data, and Fit", 800, 600);

    // Parameters
    Double_t E0 = 1;    // Resonance energy
    Double_t Gamma0 = 1; // Reduced width
    Double_t A = 1000.0;  // Amplitude
    Double_t l = 0;       // Angular momentum (s-wave)
    Double_t R = 5.0;     // Channel radius
    Double_t sigma = 1; // Resolution

    TF1 *f1 = new TF1("f1", breitWignerWithPenetrabilityAndResolution, 0, 10, 6);
    f1->SetParameters(E0, Gamma0, A, l, R, sigma);
    f1->SetParNames("E0", "Gamma0", "A", "l", "R", "sigma");
    f1->SetNpx(1000);
    
    // Create histogram for synthetic data
    TH1F *hData = new TH1F("hData", "Synthetic Data", 100, 0, 10);
    
    // Generate synthetic data
    Int_t nEvents = 50000;
    hData->FillRandom("f1", nEvents);

    // Fit the data
    TF1 *fFit = new TF1("fFit", breitWignerWithPenetrabilityAndResolution, 0, 10, 6);
    fFit->SetParameters(E0, Gamma0, hData->GetMaximum(), l, R, sigma);  // Initial guess
    fFit->SetParNames("E0", "Gamma0", "A", "l", "R", "sigma");
    fFit->FixParameter(3, 0);  // Fix l
    fFit->FixParameter(4, 5.0);  // Fix R
    fFit->FixParameter(5, sigma);
    fFit->SetNpx(1000);
    
    TFitResultPtr fitResult = hData->Fit(fFit);
    
    // Draw
    hData->SetMarkerStyle(20);
    hData->SetMarkerSize(0.7);
    hData->SetTitle("Breit-Wigner with Penetrability, Resolution, Data, and Fit; Energy (MeV); Counts");
    
    f1->SetLineColor(kBlue);
    f1->SetLineWidth(2);
    f1->DrawCopy("SAME");

    fFit->SetLineColor(kRed);
    fFit->SetLineWidth(2);
    fFit->Draw("SAME");
    
    // Add legend
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hData, "Data", "p");
    legend->AddEntry(f1, "True", "l");
    legend->AddEntry(fFit, "Fit", "l");
    legend->Draw();
    
    c1->Draw();
    
    // Print fit results
    std::cout << "Fit Results:" << std::endl;
    std::cout << "E0     = " << fFit->GetParameter(0) << " +/- " << fFit->GetParError(0) << std::endl;
    std::cout << "Gamma0 = " << fFit->GetParameter(1) << " +/- " << fFit->GetParError(1) << std::endl;
    std::cout << "A      = " << fFit->GetParameter(2) << " +/- " << fFit->GetParError(2) << std::endl;
    std::cout << "sigma  = " << fFit->GetParameter(5) << " +/- " << fFit->GetParError(5) << std::endl;
}
