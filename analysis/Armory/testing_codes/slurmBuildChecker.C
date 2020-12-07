#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void slurmBuildChecker(TString fileName){
  
  string line;
  ifstream file (fileName.Data());

  if( file.is_open()){

    int entriesFound = 0;
    vector<int> data1;
    vector<ULong64_t> data2;
    int lastRunNum = 0;

    printf("%3s, %8s, %16s, %16s, %6s,  %5s \n", "RUN", "Entries", "TS start", "TS stop", "min", "hour");
    printf("------------------------------------------------------------------------------\n");    
    while( getline(file, line)){
      
      //========= Get run number
      int runNum = -1;
      size_t found   = line.find("==== RUN ");
      size_t nfound  = line.find("GEBSort");
      size_t nfound2 = line.find("to");
      if( found != string::npos && nfound == string::npos && nfound2 == string::npos ) {
        //printf("%s\n", line.c_str());
        found = line.find("RUN");
        string run=line.substr(found + 3, 4);
        runNum= atoi(run.c_str());
        //printf("%d\n", runNum); 
        data1.clear();
        data2.clear();
        data1.push_back(runNum);
      }


      //========== Get Entries
      int entries = -1;
      found = line.find("Entries");
      if( found != string::npos ){
        if ( entriesFound%2 == 0 ) {
          //printf("%s\n", line.c_str());
          found = line.find_last_of(':');
          entries=atoi(line.substr(found - 10, 9).c_str());
          //printf("%d\n",entries);
          data1.push_back(entries);
        }
        entriesFound ++;
      }  

      //========== Get timeStamp
      ULong64_t TS1 = 0;
      ULong64_t TS2 = 0;
      found = line.find("-->");
      if( found != string::npos ){
        //printf("%s\n", line.c_str());
        size_t found1 = line.find("range");
        TString ts1 = line.substr(found1+6, found - found1-6 );
        size_t found2 = line.find_last_of(':');
        TString ts2 = line.substr(found + 3, found2-found-3);
        //printf("%s, %s \n", ts1.Data(), ts2.Data());
        TS1=ts1.Atoll();
        TS2=ts2.Atoll();
        //printf("%llu, %llu \n", TS1, TS2);
        data2.push_back(TS1);
        data2.push_back(TS2);
      }

      if( data1.size() == 2 &&  data2.size() == 2 ) {
        
        if( data1[0] - lastRunNum != 1 ) {
          printf("\033[1;31m");
        }
        double minute = (data2[1] - data2[0])*1e-8 / 60;
        double hour = minute / 60;
        printf("%03d, %8d, %16llu, %16llu, %6.2f,  %5.2f\n", data1[0], data1[1], data2[0], data2[1], minute, hour);
        data1.clear();
        data2.clear();
        printf("\033[0m");
        lastRunNum = data1[0];
      }

    }

    file.close();

  }else{
    printf("cannot open file : %s \n", fileName.Data());

  }
  

  gROOT->ProcessLine(".q");

}
