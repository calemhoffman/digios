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


//TODO , need a dis-similarity function

double f(double x, double a0, double a1, double a2){
  return a0 + a1*x + a2*x*x;
}

double distant(double x1, double y1, double x2, double y2 ){
  return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) ;
}

void PrintD(double *D, int size ){

  for ( int i = 0; i < size ; i ++ ){
    if( i == 0 ){
       for( int j = -1 ; j < size  ; j++) printf("%6d| ", j );
       printf("\n");
       
       for( int j = 0 ; j < size + 1 ; j++) printf("%6s+ ", "------" );
       printf("\n");
       
    }
    for ( int j = 0; j < size ; j ++ ){
      if (j == 0 ) printf("%6d| ", i);
      printf("%6.4f| ", D[i*size+j]);
    }
    printf("\n");
  }
}

struct MinPos{
  double min;
  vector<int> pos;
};

MinPos FindMin(double *D, int size){

  MinPos mp;

  mp.min = 1e9;
  for( int i = 0 ; i < size; i++){
    for( int j = i ; j < size; j++){
      if( D[i*size+j] < mp.min ) {
        mp.min = D[i*size+j];
        mp.pos = {i, j};
      }
    }
  }

  if( mp.min == 1e9 ) mp.min = TMath::QuietNaN();

  return mp;
}

double * ReduceDMatrix(double *D, int size, MinPos mp){

  double * Dnew = new double [size*size];
  for( int i = 0 ; i < size; i++){
    for( int j = 0 ; j < size; j++){
      if( i == mp.pos[0] || i == mp.pos[1]) {
        Dnew[i*size+j]= TMath::Min( D[mp.pos[0]*size+j] , D[mp.pos[1]*size+j] );
      }else if( j == mp.pos[0] || j == mp.pos[1]){
        Dnew[i*size+j]= TMath::Min( D[i*size+mp.pos[0]] , D[i*size+mp.pos[1]] );
      }else{
        Dnew[i*size+j] = D[i*size+j];
      }
    }
  }
  for( int i = 0 ; i < 2 ; i++){  
    for( int j = 0 ; j < 2 ; j++){
      Dnew[mp.pos[i]*size+mp.pos[j]] = TMath::QuietNaN();
    }
  }
  for( int j = 0; j < size; j++) Dnew[mp.pos[1] * size + j ] = TMath::QuietNaN();
  for( int i = 0; i < size; i++) Dnew[i * size + mp.pos[1] ] = TMath::QuietNaN();

  ///for( int i = 0; i < size; i ++){
  ///  for( int j = 0; j < i; j++){
  ///    Dnew[i*size+j] = TMath::QuietNaN();
  ///  }
  ///}


  return Dnew;

}

//#####################################################
void FitCoinTime(){

  ///============== General data
  const int n = 300;
  double x[n], y[n], ey[n], oy[n];

  double rangey[2] = {-1, 10};
  TH2F * h1 = new TH2F("h1", "h1", 500, -0.1, 1.1, 500, rangey[0], rangey[1]);

  for( int i = 0; i < n; i++){
    x[i] = gRandom->Rndm();
    ey[i] = gRandom->Gaus(0,0.1);
    oy[i] = gRandom->Integer(2);
    y[i] = -10*x[i]*(x[i]-1) + 5* ey[i] + 6*oy[i];
    //y[i] = x[i] + ey[i] + 1* oy[i];    

    h1->Fill(x[i], y[i]);

  }

  bool isDebug = false;

  if( isDebug ){
    for( int i = 0; i < n ; i++) printf("%d, {%6.4f, %10.4f}, \n",i,  x[i], y[i]);
  }
  

  ///================ Clustering (agglomerative, single-linkage clustering 

  double * D1 = new double [n*n];

  for( int i = 0 ; i < n; i++){
    for( int j = 0 ; j < n; j++){
      if( i ==j ) {
        D1[i*n+j] = TMath::QuietNaN();
        continue;
      }
      D1[i*n+j] = distant(x[i], y[i], x[j], y[j]);
    }
  }
  
  if( isDebug ){
    printf("====================D1\n");
    PrintD(D1, n);
  }

  vector<vector<int>> dendrogram;
  vector<double> branchLength;

  MinPos mp = FindMin(D1, n);
  if( isDebug ) printf("=================== min : %f\n", mp.min);
  if( isDebug ) printf("===================== k : %d, %d\n", mp.pos[0], mp.pos[1]);
  dendrogram.push_back(mp.pos);
  branchLength.push_back(mp.min/2.);

  double * D2 = D1;
  int count = 2;
  do{
    D2 = ReduceDMatrix(D2, n, mp);
    if( isDebug ) printf("====================  D%d\n", count);
    if( isDebug ) PrintD(D2, n);
    mp = FindMin(D2, n);

    if( TMath::IsNaN(mp.min) ){
      break;
    }else{
    if( isDebug )   printf("=================== min : %f\n", mp.min);
    if( isDebug )   printf("===================== k : %d, %d\n", mp.pos[0], mp.pos[1]);

      dendrogram.push_back(mp.pos);
      branchLength.push_back(mp.min/2.);
    }
    count ++;
  }while( !TMath::IsNaN(mp.min) );

  if( isDebug ) {
    printf("======================================\n");
    for( int i = 0; i < (int) dendrogram.size() ; i++){
      printf(" %2d | (%d, %d) , %f \n", i, dendrogram[i][0],dendrogram[i][1], branchLength[i]);
    }
  }

  int nTree = (int) dendrogram.size() ;
  vector<int> group1;
  for( int i = nTree -1 ; i >= 0 ; i--){
    if( i == nTree-1 ){
      group1.push_back(dendrogram[i][0]);
    }else{
      int nG = (int) group1.size();
      for( int p = 0; p < nG; p++){
        if( group1[p] == dendrogram[i][0] ) {
              group1.push_back(dendrogram[i][1]);
        }
      }
    }
  }

  if( isDebug ) {
    printf("======================================\n");
    for( int i = 0; i < (int) group1.size() ; i++ ) printf("%d, ", group1[i]);
    printf("\n");
  }
  
  TH2F * h2 = new TH2F("h2", "h2", 500, -0.1, 1.1, 500, rangey[0], rangey[1]);

  for( int i = 0; i < (int) group1.size() ; i++ ) {
    h2->Fill(x[group1[i]], y[group1[i]]);
  }

  gStyle->SetOptStat("");
  h1->Draw("box");
  h2->SetLineColor(2);
  h2->Draw("box same");
  

  ///================ fit, least square
  double cutoff = 0.5; // this is the max rms, if rms > cutoff, discard


  //double rms = 0;  
  //for( int i = 0; i < n ; i++){
  //  double diff = 
  //
  //
  //}
  





}
