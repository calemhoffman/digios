#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

void SaveTH1IntoText(TH1F * hist, TString fileName){

   int nBin = hist->GetNbinsX();
   
   
   FILE * paraOut;
   paraOut = fopen (fileName.Data(), "w+");
   printf("=========== save histogram to %s \n", fileName.Data());
   
   for( int i = 1; i <= nBin ; i++){
      
      double x = hist->GetBinCenter(i);
      double y = hist->GetBinContent(i);
      
      fprintf(paraOut, "%14.8f\t%9.6f\n", x, y);
   }

   fflush(paraOut);
   fclose(paraOut);
}
