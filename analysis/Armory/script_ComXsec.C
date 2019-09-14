void script_ComXsec(int id){

  TString fileName1 = "Xsec209Pb_NPA.root";
  TString fileName2 = "Xsec209Pb_PR.root";

  TFile * f1 = new TFile(fileName1);
  TFile * f2 = new TFile(fileName2);

  TObjArray * a1 = (TObjArray*) f1->FindObjectAny("xList");
  TObjArray * a2 = (TObjArray*) f2->FindObjectAny("xList");

  if( id > a1->GetLast()) {
    printf("input id(%d) > number of TGraphErrors (%d) at %s", id, a1->GetLast(), fileName1.Data());
    return;
  }
  if( id > a2->GetLast()) {
    printf("input id(%d) > number of TGraphErrors (%d) at %s", id, a2->GetLast(), fileName2.Data());
    return;
  }

  TGraphErrors *x1 = (TGraphErrors*) a1->At(id);
  x1->SetLineColor(2);  
  x1->SetMarkerColor(2);
  x1->SetMarkerSize(1.5);
  x1->SetMarkerStyle(4);
  TGraphErrors *x2 = (TGraphErrors*) a2->At(id);
  x2->SetLineColor(4);
  x2->SetMarkerColor(4);
  x2->SetMarkerSize(1.5);
  x2->SetMarkerStyle(4);

  TCanvas * cXsec = new TCanvas("cXsec", "Compare two Xsecs", 0, 0, 800, 600);
  cXsec->SetLogy();

  TLegend * legend = new TLegend(0.7, 0.7, 0.9, 0.9);
  legend->AddEntry(x1, fileName1 + Form("(%d)", id));
  legend->AddEntry(x2, fileName2 + Form("(%d)", id));

  x1->Draw("APC");

  double yRange[2], xRange[2];
  yRange[0] = 1000.0;
  yRange[1] = 0.0001;
  xRange[0] = 0;
  xRange[1] = 0;
  for( int i = 0 ; i < x1->GetN(); i++){
    double x, y;
    x1->GetPoint(i, x, y);
    if( x > xRange[1] ) xRange[1] = x;
    if( x < xRange[0] ) xRange[0] = x;
    if( y > yRange[1] ) yRange[1] = y;
    if( y < yRange[0] ) yRange[0] = y;
  }
  for( int i = 0 ; i < x2->GetN(); i++){
    double x, y;
    x2->GetPoint(i, x, y);
    if( x > xRange[1] ) xRange[1] = x;
    if( x < xRange[0] ) xRange[0] = x;
    if( y > yRange[1] ) yRange[1] = y;
    if( y < yRange[0] ) yRange[0] = y;
  }

  printf("yRange : %f, %f \n", yRange[0], yRange[1]);
  double yMin, yMax, yMean, dy;
  yMean = (yRange[1] + yRange[0])/2.;
  dy = abs(yRange[1] - yRange[0])/2.;
  yMin = TMath::Power(10, TMath::Floor(TMath::Log10(yRange[0])));
  yMax = TMath::Power(10, TMath::Ceil(TMath::Log10(yRange[1])));

  x1->GetYaxis()->SetRangeUser(yMin, yMax);
  x1->GetXaxis()->SetLimits(0, xRange[1] * 1.1);
  
  x1->GetXaxis()->SetTitle("#theta_{CM} [deg]");
  x1->GetYaxis()->SetTitle("d#sigma/d#Omega [mb/sr]");

  x2->Draw("PCsame");
  legend->Draw();
}
