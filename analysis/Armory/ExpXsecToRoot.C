#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TObjArray.h"
#include "TLegend.h"
#include <string>
#include <fstream>
#include <vector>

void ExpXsecToRoot(TString expXsecFile){

  //read data
  printf("----- loading experimental Xsec from : %s", expXsecFile.Data());
  ifstream file;
  file.open(expXsecFile.Data());
  
  vector<vector<double>> angle, xsec; 
  vector<vector<double>> dangle, dxsec; 
  
  if( file.is_open() ){
    string x;
    int i = 0;
    int id = -1;
    while( file >> x){
      //printf("%d, %s \n", i,  x.c_str());
      
      if( x.substr(0,2) == "//" )  continue;
      
      //when seeing #, start a new TGraph
      if( x.substr(0,1) == "#" ) { 
        vector<double> temp;
        angle.push_back(temp);
        xsec.push_back(temp);
        dangle.push_back(temp);
        dxsec.push_back(temp);
        ++id;
        continue;
      }
      
      if( i%4 == 0 ) angle[id].push_back(atof(x.c_str()));
      //if( i%4 == 1 ) dangle[id].push_back(atof(x.c_str()));
      if( i%4 == 1 ) dangle[id].push_back(0);
      if( i%4 == 2 ) xsec[id].push_back(atof(x.c_str()));
      if( i%4 == 3 ) dxsec[id].push_back(atof(x.c_str()));
      
      i = i + 1;
      
    }
    
    printf("... done.\n");

  }else{
    printf("... fail.\n");
    return;
  }  
  
  //convert to TGraph with TObjArray
  TString saveFileName = expXsecFile;
  int len = saveFileName.Length();
  saveFileName.Remove(len-3);
  saveFileName += "root";
  
  
  TFile * fout = new TFile(saveFileName, "RECREATE");
  
  TObjArray * xList = new TObjArray();
  
  const int n = angle.size();
  
  TGraphErrors * gx[n];
    
  for(int i = 0; i < n; i++){
    int m = angle[i].size();
    printf("============= %d/%d, size = %d \n", i, n,  m);
    gx[i] = new TGraphErrors();
    for( int j = 0; j < m; j++){
      printf("%2d | %f , %f\n", j, angle[i][j], xsec[i][j]);
      gx[i]->SetPoint(j, angle[i][j], xsec[i][j]);
      gx[i]->SetPointError(j, dangle[i][j], dxsec[i][j]);
    }
    xList->Add(gx[i]);
  }
  
  xList->Write("xList", TObjArray::kSingleKey);
  
  //fout->Write();
  
  fout->Close();
  
  printf("==== saved root as : %s \n", saveFileName.Data());
  
}

