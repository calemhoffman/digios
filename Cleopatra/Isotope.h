/***********************************************************************
 * 
 *  This is Isotope.h, To extract the isotope mass from massXX.txt
 * 
 *-------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/


#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include "constant.h" // amu
#include <stdlib.h>  //atoi
using namespace std;

string dataPath="mass16.txt";


// about the mass**.txt
// Mass Excess = (ATOMIC MASS - A)*amu | e.g. n : (1.088664.91585E-6-1)*amu
// mass excess uncertaintly
// BEA = (Z*M(1H) + N*M(1n) - Me(A,Z))/A , Me is the mass with electrons
// BEA = (Z*mp + N*mn - M(A,Z))/A , M is the mass without electrons

class Isotope {
public:
   int A, Z;
   double Mass, MassError, BEA;
   string Name, Symbol;
   string dataSource;
   
   Isotope(int a, int z);
   Isotope(string name);
   double CalSp(int na, int nz);
   
private:
   void FindMassByAZ(int a, int z); // give mass, massError, BEA, Name, Symbol;
   void FindMassByName(string name); // give Z, mass, massError, BEA;
   
};

Isotope::Isotope(int a, int z){
    this->A = a;
    this->Z = z;
    FindMassByAZ(a,z); 
    this->dataSource = dataPath;
}

Isotope::Isotope(string name){
    
    FindMassByName(name); 
    this->dataSource = dataPath;
}

void Isotope::FindMassByAZ(int A, int Z){
  string line;
  int    lineNum=0;
  int    list_A, list_Z;

  ifstream myfile;
  int    flag=0;

  int numLineStart = 40;
  int numLineEnd  = 3392;

  if ( A >= 50 && A < 100) numLineStart = 447; //545;
  if ( A >=100 && A < 150) numLineStart = 1072;//1100;
  if ( A >=150 && A < 200) numLineStart = 1833;//1899;
  if ( A >=200 ) numLineStart = 2534;//2622;

  myfile.open(dataPath.c_str());

  if (myfile.is_open()) {
    while (/*! myfile.eof() &&*/ flag == 0 && lineNum <numLineEnd){
      lineNum ++ ;
      //printf("%3d  ",lineNum);
      getline (myfile,line);

      if (lineNum >= numLineStart ){
        list_Z     = atoi((line.substr(10,5)).c_str());
      	list_A     = atoi((line.substr(15,5)).c_str());

      	if ( A == list_A && Z == list_Z) {
            this->BEA       = atof((line.substr(54,11)).c_str());
      		this->Mass      = list_Z*mp + (list_A-list_Z)*mn - this->BEA/1000*list_A;
            this->MassError = atof((line.substr(65,7)).c_str());
            this->Symbol    = line.substr(20,2);
            ostringstream ss;
            ss << A << this->Symbol;
            this->Name      = ss.str();
     		flag = 1;
      	}else if ( list_A > A) {
          this->BEA  = -404;
          this->Mass = -404;
          this->MassError = -404;
          this->Symbol = "non";
          this->Name   = "non";
          break;
        }

      }
    }
    myfile.close();
  }else {
    printf("Unable to open %s\n", dataPath.c_str());
  }
}

void Isotope::FindMassByName(string name){

    // done seperate the Mass number and the name 
    string temp = name;
    int lastDigit = 0;

    for(int i=0; temp[i]; i++){
      if(temp[i] == '0') lastDigit =  i; 
      if(temp[i] == '1') lastDigit =  i; 
      if(temp[i] == '2') lastDigit =  i; 
      if(temp[i] == '3') lastDigit =  i; 
      if(temp[i] == '4') lastDigit =  i; 
      if(temp[i] == '5') lastDigit =  i; 
      if(temp[i] == '6') lastDigit =  i; 
      if(temp[i] == '7') lastDigit =  i; 
      if(temp[i] == '8') lastDigit =  i; 
      if(temp[i] == '9') lastDigit =  i; 
    }

    this->Symbol = temp.erase(0,  lastDigit +1);

    temp = name;
    int len = temp.length();
    temp = temp.erase(lastDigit+1, len);
    
    this->A = atoi(temp.c_str());    
    //printf(" Symbol = %s , Mass = %d\n", this->Symbol.c_str(), this->A);

    // find the nucleus in the data
    string line;
    int    lineNum=0;
    int    list_A;
    string list_symbol;

    ifstream myfile;
    int    flag=0;

    int numLineStart = 40;
    int numLineEnd  = 3392;

    if ( this->A >= 50 && this->A < 100) numLineStart = 447; //545;
    if ( this->A >=100 && this->A < 150) numLineStart = 1072;//1100;
    if ( this->A >=150 && this->A < 200) numLineStart = 1833;//1899;
    if ( this->A >=200 ) numLineStart = 2534;//2622;

    myfile.open(dataPath.c_str());

    if (myfile.is_open()) {
      while (/*! myfile.eof() &&*/ flag == 0 && lineNum <numLineEnd){
        lineNum ++ ;
        //printf("%3d  ",lineNum);
        getline (myfile,line);

        if (lineNum >= numLineStart ){
          list_symbol  = line.substr(20,2);
          list_A       = atoi((line.substr(15,5)).c_str());

          //printf(" A = %d, Sym = %s \n", list_A, list_symbol.c_str());

          if ( this->A == list_A &&  this->Symbol == list_symbol) {
            this->Z         = atoi((line.substr(10,5)).c_str());
            this->BEA       = atof((line.substr(54,11)).c_str());
      		this->Mass      = this->Z*mp + (list_A-this->Z)*mn - this->BEA/1000*list_A;
            this->MassError = atof((line.substr(65,7)).c_str());
            ostringstream ss;
            ss << this->A << this->Symbol;
            this->Name      = ss.str();
     		flag = 1;
          }else if ( list_A > this->A) {
            this->BEA  = -404;
            this->Mass = -404;
            this->MassError = -404;
            this->Symbol = "non";
            this->Name   = "non";
            break;
          }

        }
      }
      myfile.close();
    }else {
      printf("Unable to open %s\n", dataPath.c_str());
    }
}

double Isotope::CalSp(int Np, int Nn){
  Isotope nucleusD(A - Np - Nn, Z - Np);  

  if( nucleusD.Mass != -404){
	return nucleusD.Mass + Nn*mn + Np*mp - this->Mass;
  }else{
	return -404;
  }
}

#endif
