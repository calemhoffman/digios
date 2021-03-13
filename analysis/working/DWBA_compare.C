#include "DWBARatio.C"

void DWBA_compare(){
   
   TGraph * w[12];
   for( int i = 0 ; i < 12; i++){
      
      w[i] = DWBARatio(i, i+12, "DWBA.root", false);
      w[i]->SetLineColor(i+1);
      
      i == 0 ? w[i]->Draw("Al") : w[i]->Draw("same");
      
   }
   
   
}
