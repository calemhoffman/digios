#ifndef Analysis_Library_h
#define Analysis_Library_h

#include <TF1.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> SplitStr(std::string tempLine, std::string splitter, int shift = 0){

  std::vector<std::string> output;

  size_t pos;
  do{
    pos = tempLine.find(splitter); // fine splitter
    if( pos == 0 ){ //check if it is splitter again
      tempLine = tempLine.substr(pos+1);
      continue;
    }

    std::string secStr;
    if( pos == std::string::npos ){
      secStr = tempLine;
    }else{
      secStr = tempLine.substr(0, pos+shift);
      tempLine = tempLine.substr(pos+shift);
    }

    //check if secStr is begin with space
    while( secStr.substr(0, 1) == " "){
      secStr = secStr.substr(1);
    };
    
    //check if secStr is end with space
    while( secStr.back() == ' '){
      secStr = secStr.substr(0, secStr.size()-1);
    }

    output.push_back(secStr);
    //printf(" |%s---\n", secStr.c_str());
    
  }while(pos != std::string::npos );

  return output;
}

std::vector<std::vector<double>> combination(std::vector<double> arr, int r){
  
  std::vector<std::vector<double>> output;
  
  int n = arr.size();
  std::vector<int> v(n);
  std::fill(v.begin(), v.begin()+r, 1);
  do {
    //for( int i = 0; i < n; i++) { printf("%d ", v[i]); }; printf("\n");
    
    std::vector<double> temp;
    for (int i = 0; i < n; ++i) { 
      if (v[i]) {
        //printf("%.1f, ", arr[i]); 
        temp.push_back(arr[i]);
      }
    }
    //printf("\n");
    
    output.push_back(temp);
    
  } while (std::prev_permutation(v.begin(), v.end()));
  
  return output;
}

double* sumMeanVar(std::vector<double> data){
  
  int n = data.size();
  double sum = 0;
  for( int k = 0; k < n; k++) sum += data[k];
  double mean = sum/n;
  double var = 0;
  for( int k = 0; k < n; k++) var += pow(data[k] - mean,2);
  
  static double output[3];
  output[0] = sum;
  output[1] = mean;
  output[2] = var;
  
  return output;
}

double*  fitSlopeIntercept(std::vector<double> dataX, std::vector<double> dataY){
  
  double * smvY = sumMeanVar(dataY);
  double sumY = smvY[0];
  double meanY = smvY[1];

  double * smvX = sumMeanVar(dataX);
  double sumX = smvX[0];
  double meanX = smvX[1];
  double varX = smvX[2];
  
  int n = dataX.size();
  double sumXY = 0;
  for( int j = 0; j < n; j++) sumXY += dataX[j] * dataY[j];

  double slope = ( sumXY - sumX * sumY/n ) / varX;
  double intercept = meanY - slope * meanX;
  
  static double output[2];
  output[0] = slope;
  output[1] = intercept;
  
  return output;
  
}



std::vector<std::vector<double>> FindMatchingPair(std::vector<double> enX, std::vector<double> enY){

   //output[0] = fitEnergy;
   //output[1] = refEnergy;

   int nX = enX.size();
   int nY = enY.size();
   
   std::vector<double> fitEnergy;
   std::vector<double> refEnergy;
   
   if( nX > nY ){
      
      std::vector<std::vector<double>> output = combination(enX, nY);
      
      double * smvY = sumMeanVar(enY);
      double sumY = smvY[0];
      double meanY = smvY[1];
      double varY = smvY[2];
      
      double optRSquared = 0;
      double absRSqMinusOne = 1;
      int maxID = 0;
      
      for( int k = 0; k < (int) output.size(); k++){
        
        double * smvX = sumMeanVar(output[k]);
        double sumX = smvX[0];
        double meanX = smvX[1];
        double varX = smvX[2];
        
        double sumXY = 0;
        for( int j = 0; j < nY; j++) sumXY += output[k][j] * enY[j];
        
        double rSq = abs(sumXY - sumX*sumY/nY)/sqrt(varX*varY);
        
        //for( int j = 0; j < nY ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
        
        if( abs(rSq-1) < absRSqMinusOne ) {
          absRSqMinusOne = abs(rSq-1);
          optRSquared = rSq;
          maxID = k;
        }
      }
      
      fitEnergy = output[maxID];
      refEnergy = enY;
      
      printf(" R^2 : %.20f\n", optRSquared);      
      
      //calculation fitting coefficient
      //double * si = fitSlopeIntercept(fitEnergy, refEnergy);
      //printf( " y = %.4f x + %.4f\n", si[0], si[1]);
      
    }else if( nX < nY ){
    
      std::vector<std::vector<double>> output = combination(enY, nX);
      
      
      double * smvX = sumMeanVar(enX);
      double sumX = smvX[0];
      double meanX = smvX[1];
      double varX = smvX[2];
      
      double optRSquared = 0;
      double absRSqMinusOne = 1;
      int maxID = 0;
      
      for( int k = 0; k < (int) output.size(); k++){
        
        double * smvY = sumMeanVar(output[k]);
        double sumY = smvY[0];
        double meanY = smvY[1];
        double varY = smvY[2];
        
        double sumXY = 0;
        for( int j = 0; j < nX; j++) sumXY += output[k][j] * enX[j];
        
        double rSq = abs(sumXY - sumX*sumY/nX)/sqrt(varX*varY);
        
        //for( int j = 0; j < nX ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
        
        if( abs(rSq-1) < absRSqMinusOne ) {
          absRSqMinusOne = abs(rSq-1);
          optRSquared = rSq;
          maxID = k;
        }
      }
      
      fitEnergy = enX;
      refEnergy = output[maxID];
      printf(" R^2 : %.20f\n", optRSquared);   
    
    }else{
      fitEnergy = enX;
      refEnergy = enY;
      
      //if nX == nY, ther could be cases that only partial enX and enY are matched. 
      
    }
    
    
    printf("fitEnergy = ");for( int k = 0; k < (int) fitEnergy.size() ; k++){ printf("%7.2f, ", fitEnergy[k]); }; printf("\n");
    printf("refEnergy = ");for( int k = 0; k < (int) refEnergy.size() ; k++){ printf("%7.2f, ", refEnergy[k]); }; printf("\n");
    
    std::vector<std::vector<double>> haha;
    haha.push_back(fitEnergy);
    haha.push_back(refEnergy);
    
    return haha;
   
}

#endif 

