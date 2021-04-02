

TGraph * DWBARatio(int id1, int id2, TString rootFile="DWBA.root", bool isPlot = true){
   
   TGraph * gR = NULL;
   
   TFile * file = new TFile(rootFile, "READ");
   if( file != NULL ){
       printf("----- Opening %s\n", rootFile.Data());
   }else{
       printf("----- Opening %s .... Fail.\n", rootFile.Data());
      return gR;
   }
   
   ///get the  TGraph of the distribution.
   TObjArray * gList = (TObjArray *) file->Get("qList");
   int size = gList->GetLast()+1;
   printf("----- found %d d.s.c\n", size);
   
   if( id1 > size || id2 > size ) {
      printf(" id1 > %d || id2 > %d \n", size, size);
      return gR;
   }
   
   TGraph * g1 = (TGraph *) gList->At(id1);
   TGraph * g2 = (TGraph *) gList->At(id2);
   
   double g1MaxY = g1->GetHistogram()->GetMaximum();
   double g2MaxY = g2->GetHistogram()->GetMaximum();

   g2->SetLineColor(2);


   TCanvas * cDWBA = NULL ;

   if( isPlot ){
      cDWBA= new TCanvas("cDWBA", "DWBA Ratio", 1000, 500);
      cDWBA->Divide(2,1);
      
      cDWBA->cd(1);
      cDWBA->cd(1)->SetLogy();

      if( g1MaxY > g2MaxY ) {
         g1->Draw();
         g2->Draw("same");
      }else{
         g2->Draw();
         g1->Draw("same");
      }
      
      TLegend * legend = new TLegend( 0.1, 0.9, 0.9, 1.0); 
      legend->AddEntry(g1, g1->GetName());
      legend->AddEntry(g2, g2->GetName());
      
      legend->Draw();
      
      cDWBA->cd(2);
      
   }
   gR = new TGraph();
   double x, y1, y2;
   for( int i = 0 ; i < g1->GetN(); i++){
      g1->GetPoint(i, x, y1);
      g2->GetPoint(i, x, y2);
      gR->SetPoint(i, x, y1/y2);
   }
   
   if( isPlot) gR->Draw();
   
   return gR;
   
}

