void readCaliResult(int detID){

   TFile * file = new TFile("caliResult.root");
   
   
   int colID = detID%5;
   
   TH2F* exp = (TH2F*) file->FindObjectAny(Form("exPlot%d[%d]", detID, colID));
   
   TH2F* corr = (TH2F*) file->FindObjectAny(Form("exPlotC%d", detID));
   
   TGraph2D* dist = (TGraph2D*) file->FindObjectAny(Form("gDist%d", detID));
   
   TGraph * fx = (TGraph *) file->FindObjectAny("fx0");
   
   TCanvas * cCanvas = new TCanvas("cCanvas", "read Cali-Result", 900, 300);
   cCanvas->Divide(3,1);
   
   
   cCanvas->cd(1);
   exp->Draw("box");

   cCanvas->cd(2);
   corr->Draw("box"); 
   fx->Draw("same");
   
   
   cCanvas->cd(3);
   dist->Draw("tri1");   
   cCanvas->cd(3)->SetTheta(90);
   cCanvas->cd(3)->SetPhi(0);
   dist->Draw("tri1");   
   

}
