#include "../Armory/AutoFit.C"

void generateHistogram(){


  vector<double> peak = {   1,    4,    10,     2,     2,     3,    20,    10,     5,     6,     7,    15 };
  vector<double> mean = { 0.0, 0.35, 2.796, 3.735, 4.525, 4.728, 4.867, 5.336, 5.550, 5.693, 5.822, 6.609};

  double sigma = 0.05;

  nPeaks = peak.size();
  nPols = 0;
  int nPars = 3*nPeaks + nPols + 1 ;
  TF1 * func = new TF1("func", nGaussPol, -2, 10, nPars);

  for( int i = 0 ; i < nPeaks; i++){
    func->SetParameter(3*i + 0, peak[i]);
    func->SetParameter(3*i + 1, mean[i]);
    func->SetParameter(3*i + 2, sigma);
  }

  if( nPols >= 0 ){
    func->SetParameter(3*nPeaks + 0, 1);
//    func->SetParameter(3*nPeaks + 1, 0.0);
//    func->SetParameter(3*nPeaks + 1, -0.2);
  }
  
  


   TH1F * h1 = new TH1F("h1", "h1", 1000, -1, 8);
   h1->FillRandom("func", 100000);

   h1->Draw();

   ShowFitMethod();

}
