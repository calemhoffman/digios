#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <gsl/gsl_sf_coulomb.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TLegend.h>

const double hbar = 6.582119569e-22; // Reduced Planck's constant in MeV·s
const double c = 3.0e23; // Speed of light in fm/s
const double mp = 1.0078250 ; // Proton mass in umas
const double mBe = 10.0135338 ; // 10Beryllium mass in umas
const double Z1 = 1; // Charge of proton
const double Z2 = 4; // Charge of beryllium nucleus
const double e2 = 1.4399764; // Coulomb constant in MeV·fm
const double Sp = 11.228;
const double r = 4.1; // Interaction radius in fm

// Function to calculate penetrability and shift factor using Coulomb wave functions
double calculatePenetrabilityAndShift(double E, double L, double &shiftFactor) {
    // Calculate parameters, rho and eta in umas if I use this formulas
    double k = sqrt(2 * mp*931.4936*mBe*E/(mp+mBe)) / (hbar*c); // Wave number (1/fm)
    double eta = 0.1574854 * Z1 * Z2 * sqrt(mp*mBe/(E*(mp+mBe))) ;   // Formula in iliadis 2015
    double rho = 0.218735 * r * sqrt(mp * mBe * E / (mp + mBe)); // Formula in iliadis 2015

    // Calculate the Coulomb wave function F_l(eta, rho)
    gsl_sf_result F_result, Fp_result, G_result, Gp_result;
    double exp_F_value, exp_G_value;
    int status = gsl_sf_coulomb_wave_FG_e(eta, rho, L, 0, &F_result, &Fp_result, &G_result, &Gp_result, &exp_F_value, &exp_G_value);

    double F_l = F_result.val; // Regular Coulomb wave function F_l(eta, rho)
    double G_l = G_result.val; // Irregular Coulomb wave function G_l(eta, rho)
    double dF = Fp_result.val * k; // Derivative of F_l
    double dG = Gp_result.val * k; // Derivative of G_l

    // Calculate penetrability T
    double H_plus_squared = F_l * F_l + G_l * G_l;
    double T = k*r/ H_plus_squared; // Formula in iliadis 2015

    // Calculate the shift factor
    shiftFactor = r * ((F_l*dF + G_l*dG)/(H_plus_squared));  // Formula in iliadis 2015

    return T;
}

void Penetrability_proton() {

    std::vector<int> L_values = {0, 1, 2, 3};
    double E_start = 11.23;
    double E_end = 14.40;
    double E_step = 0.01;

    // Canvas for Penetrability
    TCanvas *c1 = new TCanvas("c1", "Penetrability vs Proton Energy", 800, 600);
    c1->SetLogy(); // Set logarithmic scale on the y-axis
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);

    double y_min = 1e-45; // Adjust this to set the lower limit of the y-axis
    double y_max = 1.5; // Upper limit of the y-axis

    // Canvas for Shift Factor
    TCanvas *c2 = new TCanvas("c2", "Shift Factor vs Proton Energy", 800, 600);
    TLegend *legendShift = new TLegend(0.7, 0.7, 0.9, 0.9);

    // Set the y-axis range for the shift factor plot
    double shift_y_min = -4.0;
    double shift_y_max = 0.0;

    for (size_t i = 0; i < L_values.size(); ++i) {
        double L = L_values[i];
        std::vector<double> proton_energies;
        std::vector<double> penetrabilities;
        std::vector<double> shiftFactors;

        for (double E = E_start; E <= E_end; E += E_step) {
            double proton_energy_MeV = E - Sp;
            double proton_energy_keV = proton_energy_MeV * 1000.0; // Convert to keV
            double shiftFactor = 0.0;
            double T = calculatePenetrabilityAndShift(proton_energy_MeV, L, shiftFactor);

            if (T >= 0) {
                proton_energies.push_back(proton_energy_keV);
                penetrabilities.push_back(T);
                shiftFactors.push_back(shiftFactor);

                std::cout << "Penetrability for L = " << L << ", E = " << E << ": " << T << "\n";
                std::cout << "Shift factor for L = " << L << ", E = " << E << ": " << shiftFactor << "\n";
            }
        }

        // Penetrability Graph
        TGraph *gr = new TGraph(proton_energies.size(), &proton_energies[0], &penetrabilities[0]);
        gr->SetLineColor(i + 1);
        gr->SetLineWidth(2);
        gr->SetTitle("Penetrability vs. Energy");

        // Shift Factor Graph
        TGraph *grShift = new TGraph(proton_energies.size(), &proton_energies[0], &shiftFactors[0]);
        grShift->SetLineColor(i + 1);
        grShift->SetLineWidth(2);
        grShift->SetTitle("Shift Factor vs. Energy");

        if (proton_energies.size() > 0) { // Only draw if there is data
            if (i == 0) {
                c1->cd();
                gr->Draw("AL");
                gr->GetXaxis()->SetTitle("E_{p} in CM (keV)"); // x-axis in keV
                gr->GetYaxis()->SetTitle("Penetrability");
                gr->GetYaxis()->SetRangeUser(y_min, y_max); // Set y-axis limits

                c2->cd();
                grShift->Draw("AL");
                grShift->GetXaxis()->SetTitle("E_{p} in CM (keV)");
                grShift->GetYaxis()->SetTitle("Shift Factor");
                grShift->GetYaxis()->SetRangeUser(shift_y_min, shift_y_max); // Set y-axis limits for shift factor
            } else {
                c1->cd();
                gr->Draw("L same");

                c2->cd();
                grShift->Draw("L same");
            }

            legend->AddEntry(gr, Form("L = %d", (int)L), "l");
            legendShift->AddEntry(grShift, Form("L = %d", (int)L), "l");
        }

    }

    c1->cd();
    legend->Draw();
    c1->Update();

    c2->cd();
    legendShift->Draw();
    c2->Update();

}

