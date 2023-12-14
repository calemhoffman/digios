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
#include <TCutG.h>

#include "../Armory/AnalysisLibrary.h"

void cleo_to_csv(TString rootfile){  
const char* treeName="tree";
TFile * refFile = new TFile(rootfile, "read");

TObjArray * fxList = (TObjArray*) refFile->FindObjectAny("fxList");
int numFx = fxList->GetEntries();
TGraph ** fx = new TGraph *[numFx];
for( int i = 0; i < numFx ; i++){
    fx[i] = (TGraph*) fxList->At(i);
    fx[i]->SetLineColor(2);
}  

for (int i = 0; i<400; i++){
    float z = (float)i + -500.;
    printf("%f",z);
    for (int j = 0;j< numFx; j ++) {
        if (fx[j]->Eval(z) > 0.5) {
            printf(",%f",fx[j]->Eval(z));
        } else {
            printf(",None");
        }
    }
    printf("\n");
}


}