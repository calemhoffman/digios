#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TCutG.h>
#include <TProfile.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TMath.h>
#include <TRandom.h>

#include "../Woods-Saxon/matrix.cpp"

double f(double x, double a0, double a1, double a2){
  return a0 + a1*x + a2*x*x;
}

/*
class LsqPt{

  private:
    double chiSq;
    int count;
    double ms; /// mean square

    int dim; /// size of pt
    vector<double> pt;

  public:

    void SetChiSq(double a){ this->chiSq = a;}
    

}*/

struct Lsq {
  double chiSq;
  int count;
  double ms; /// mean square

  int dim; /// size of pt
  vector<double> pt;
};

Lsq CalLsq(double * x, double * y, int size, Lsq haha, double threshold){

  int count = 0;
  double chiSq = 0;

  int ptSize = haha.dim;

  for( int i = 0; i < size; i++){
    bool isCounted = false;
    for( int j = 0; j < ptSize-1; j++){

      if (isCounted ) continue;
      
      double ny = haha.pt[0]*x[i]+haha.pt[1+j];
      double diff = TMath::Power(y[i] - ny, 2);

      if( diff < threshold ) {
        count ++;
        chiSq += diff;
        isCounted = true;
      }
    }
  }

  haha.chiSq = chiSq;
  haha.count = count;
  haha.ms = chiSq/count;

  return haha;

}

void PrintLsq(Lsq haha){
  printf("%4d, %8.4f | %8.4f | %d | ", haha.count, haha.chiSq, haha.ms, haha.dim);
  for( int j = 0; j < haha.dim; j++){
    printf("%6.4f, ", haha.pt[j]);
  }
  printf("\n");
}

void PrintLsqList(Lsq haha[], int n){
  for( int i = 0; i < n; i++){
    PrintLsq(haha[i]);
  }
  printf("------------------\n");
}

void BubbleSort(Lsq haha[], int n, int aCount){
  
  for( int i = 0 ; i < n-1; i++){
    for( int j = 0 ; j < n-1-i; j++){
      if( haha[j].count < aCount  ){
        if ( haha[j].count < haha[j+1].count){
          Lsq kaka = haha[j];
          haha[j] = haha[j+1];
          haha[j+1] = kaka;
        }
      }else{
        if ( haha[j].ms > haha[j+1].ms){
          Lsq kaka = haha[j];
          haha[j] = haha[j+1];
          haha[j+1] = kaka;
        }
      }
    }
  }

}

//#####################################################
void FitCoinTime2(){

  double threshold = 0.02; /// when distance is larger than theshold, discard
  int maxIteration = 1000;

  ///============== General data
  const int n = 500;
  double x[n], y[n], ey[n], oy[n];

  double rangey[2] = {-2, 4};
  TH2F * h1 = new TH2F("h1", "h1", 500, -0.1, 1.1, 500, rangey[0], rangey[1]);

  for( int i = 0; i < n; i++){
    x[i] = gRandom->Rndm();
    ey[i] = gRandom->Gaus(0,0.1);
    oy[i] = gRandom->Integer(2);
    //y[i] = -10*x[i]*(x[i]-1) + 5* ey[i] + 6*oy[i];
    y[i] = x[i] + ey[i] + 1.5* oy[i]-1.;    

    h1->Fill(x[i], y[i]);

  }

  bool isDebug = false;

  //if( isDebug ){
  //  for( int i = 0; i < n ; i++) printf("%d, {%6.4f, %10.4f}, \n",i,  x[i], y[i]);
  //}

  ///================= Calculate least-square distance

  int nLine = 2; /// number of parallel line
  const int nPar = 3; /// number of parameter
  const int nPt = nPar+1;

  Lsq haha[nPt];
  
  ///generate initial points
  for( int i = 0; i < nPt ; i++){
    (haha[i].pt).clear();
    haha[i].dim = nPar;
    for( int j = 0; j < nPar ; j++){
      (haha[i].pt).push_back((gRandom->Rndm()-1.)*3);
    }
  }

  ///calculate mean square for all points
  for( int i = 0; i < nPt; i++){
    haha[i] = CalLsq(x, y, n, haha[i], threshold);
  }

  if( isDebug ){
    printf("----------- init \n");
    PrintLsqList(haha, nPt);
  }

  int count = 0;
  bool isGo = true;

  Lsq hahaOld;

  double dis = 0;
  
  do{

    if( isDebug ) printf("######################### %d \n", count);

    ///============ sorting, bubble-sort
    if( isDebug ) printf("----------- sort \n");
    BubbleSort(haha, nPt, n);
    if( isDebug ) PrintLsqList(haha, nPt);  

    ///============ Calculate mid-point
    Lsq x0;
    x0.pt.clear();
    x0.dim = nPar;
    for( int j = 0; j < nPar ; j++){
      double temp = 0;
      for( int i = 0; i < nPt ; i++){
        temp += (haha[i].pt)[j]/nPt;
      }
      x0.pt.push_back(temp);
    }
    ///x0 = CalLsq(x, y, n, x0, threshold);
    ///printf("----------- x0 \n");
    ///PrintLsq(x0);

    ///============ Calculate reflection point
    Lsq xr;
    xr.pt.clear();
    xr.dim = nPar;
    for( int j = 0; j < nPar ; j++){
      xr.pt.push_back(x0.pt[j] + x0.pt[j] - haha[nPt-1].pt[j]);
    }
    xr = CalLsq(x,y, n, xr, threshold);

    if( isDebug ) {
      printf("----------- xr \n");
      PrintLsq(xr);
    }
    
    ///============== Compare
    int compareID = -1;
    
    if( xr.ms < haha[0].ms ){
      if( isDebug ) printf("Reflection\n");
      haha[nPt-1] = xr;
      compareID = 0;
    }else if( haha[0].ms <= xr.ms && xr.ms < haha[nPt-2].ms ){

      Lsq xe;
      xe.pt.clear();
      xe.dim = nPar;
      for( int j = 0; j < nPar ; j++){
        xe.pt.push_back(x0.pt[j] + 2*(xr.pt[j] - x0.pt[j]));
      }
      xe = CalLsq(x,y, n, xe, threshold);
      if( isDebug ) {
        printf("----------- xe \n");
        PrintLsq(xe);
      }
      
      if( xe.ms < xr.ms ){
        if( isDebug ) printf("Extension - E\n");
        haha[nPt-1] = xe ;
        compareID = 1;
      }else{
        if( isDebug ) printf("Extension - R\n");
        haha[nPt-1] = xr ;
        compareID = 2;
      }

    }else if( haha[nPt-2].ms <= xr.ms && xr.ms < haha[nPt-1].ms ){

      Lsq xc;
      xc.pt.clear();
      xc.dim = nPar;
      for( int j = 0; j < nPar ; j++){
        xc.pt.push_back(x0.pt[j] + 0.5*(haha[nPt-1].pt[j] - x0.pt[j]));
      }
      xc = CalLsq(x,y, n, xc, threshold);

      if( isDebug ){
        printf("----------- xc \n");
        PrintLsq(xc);
      }

      if( isDebug ) printf("Contraction\n");
      haha[nPt-1] = xc ;
      compareID = 3;
      
    }else {

      if( isDebug ) printf("Shrink\n");

      for( int i = 1; i < nPt; i++){
        for( int j = 0 ; j < nPar ; j++){
          haha[i].pt[j] = haha[0].pt[j] + 0.5*(haha[i].pt[j] - haha[0].pt[j]);
        }
        haha[i] = CalLsq(x,y, n, haha[i], threshold);
      }
      compareID = 4;
    }

    if( isDebug ) PrintLsqList(haha, nPt);

    ///============= Check if CoPlaner by R-squared 
    Matrix F(nPt, nPar);
    Matrix Y(nPt, 1);

    for( int i = 0; i < nPt; i++){
      for( int j = 0; j < nPar; j++){
        if( j == 0 ) {
          F(i+1,j+1) = 1.0;
        }else{
          F(i+1,j+1) = haha[i].pt[j-1];
        }
      }
    }
    for( int i = 0; i < nPt; i++){
      Y(i+1,1) = haha[i].pt[nPar-1];
    }
    
    Matrix Ft = F.Transpose();
    Matrix FtF = Ft*F;

    double detFtF = FtF.Det();
    double Rsq = 0.;
    if( detFtF != 0 ){
      Matrix iFtF = (Ft*F).Inverse();
      Matrix FtY = Ft*Y;
      Matrix beta = iFtF* FtY;
      Matrix YY = F*beta ;
      Matrix ee = YY - Y;
      
      double SSres = 0;
      for( int i = 1; i <= nPt; i++){
        SSres += ee(i,1)*ee(i,1); 
      }

      double Ymean = x0.pt[nPar-1];
      double SStot = 0;
      for( int i = 1; i <= nPt; i++){
        SStot += (Y(i,1) - Ymean)*(Y(i,1) - Ymean); 
      }

      Rsq = 1.- SSres/SStot;
      if( isDebug ) printf("================ R-sq : %f \n", Rsq);
    }
    
    if( Rsq > 0.95 || Rsq < 0 || detFtF == 0){

      if( isDebug ) printf("Re-Randomized\n");
      
      for( int i = 1; i < nPt; i++){
        for( int j = 0; j < nPar; j++){
          haha[i].pt[j] = (gRandom->Rndm()-1.)*3;
        }

        haha[i] = CalLsq(x,y, n, haha[i], threshold);
        
      }

      if( isDebug ) PrintLsqList(haha, nPt);
    }

    hahaOld = haha[0];

    if( count > 0 ){
      dis = 0;
      for( int j = 0; j < nPar ; j++){
        dis += TMath::Power(haha[0].pt[j] - x0.pt[j],2);
      }

      if( isDebug )  printf("========= dis : %f \n", dis);

      if( abs(hahaOld.ms - haha[0].ms) < 1e-5 && dis < 1e-15 ) {
        isGo = false;
      }
    }
      

    count ++;

  }while(count < maxIteration && isGo );
  //}while(count < 200  );

  printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ %d\n", count);
  printf(" dis = %.9e\n", dis);
  PrintLsqList(haha, nPt);

  ///=============== Plot result;

  h1->Draw("box");

  TF1 * f1 = new TF1("f1", "[0]*x + [1]", rangey[0], rangey[1]);
  f1->SetParameter(0, haha[0].pt[0]);
  f1->SetParameter(1, haha[0].pt[1]);

  TF1 * f2 = new TF1("f2", "[0]*x + [1]", rangey[0], rangey[1]);
  f2->SetParameter(0, haha[0].pt[0]);
  f2->SetParameter(1, haha[0].pt[2]);

  f1->Draw("same");
  f2->Draw("same");


}
