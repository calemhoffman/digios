#define Cali_e_trace_cxx

//##########################################################
//      Cali e, xf, xn, coinTime with gate
//      the gate should be modified as needed. 
//##########################################################

#include "Cali_e_trace.h"
#include <TH2.h>
#include <TStyle.h>

bool rejZeroHit = true;

void Cali_e_trace::Begin(TTree * /*tree*/)
{
   TString option = GetOption();  
   
   if( rejZeroHit ) printf(" ================= reject when zero hit on array \n");
   
}

void Cali_e_trace::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();
}

double xCorr[24] = {
   1.011, //  0
   0.981, //  1
   0.987, //  2
   0.993, //  3
   0.999, //  4
   0.975, //  5
   0.807, //  6
   0.837, //  7
   0.813, //  8
   0.987, //  9
   0.981, // 10
   1.000, // 11
   0.771, // 12
   0.789, // 13
   0.789, // 14
   0.789, // 15
   1.005, // 16
   0.981, // 17
   0.963, // 18
   0.969, // 19
   0.987, // 20
   0.957, // 21
   0.981, // 22
   1.023, // 23
};

Bool_t Cali_e_trace::Process(Long64_t entry)
{

   //#################################################################### initialization
   for(int i = 0; i < numDet; i++){
      eC[i]    = TMath::QuietNaN();
      xfC[i]   = TMath::QuietNaN();
      xnC[i]   = TMath::QuietNaN();
      x[i]     = TMath::QuietNaN();
      z[i]     = TMath::QuietNaN();
      hitID[i] = -4;
      eC_t[i]  = 0;
   }
   
   det   = -4;
   
   multiHit = 0;
   rdtMultiHit = 0;
   
   arrayRDT = -4;
   
   Ex  = TMath::QuietNaN();
   thetaCM  = TMath::QuietNaN();
   thetaLab  = TMath::QuietNaN();
   
   for(int i = 0; i < 8 ; i++){
      rdtC[i] = TMath::QuietNaN();
      rdtC_t[i] = 0;
      rdtID[i] = -4;
   }
   
   coin_t = -2000;

   if( isTraceDataExist ){
      tcoin_t = TMath::QuietNaN();
      coinTimeUC = TMath::QuietNaN(); //uncorrected
      coinTime = TMath::QuietNaN();
      
      teS = TMath::QuietNaN();
      te_tS = TMath::QuietNaN();
      te_rS = TMath::QuietNaN();
      trdtS = TMath::QuietNaN();
      trdt_tS = TMath::QuietNaN();
      trdt_rS = TMath::QuietNaN();
   }
   
   //#################################################################### get event
   eventID += 1;
   //if( entry == 1 ) run += 1; //TODO need to modified the GeneralSort.C for run_number
   
   b_runID->GetEntry(entry,0);  run = runID;
   b_Energy->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   b_RDT->GetEntry(entry,0);
   b_EnergyTimestamp->GetEntry(entry,0);
   b_RDTTimestamp->GetEntry(entry,0);
   
   if( isEBISExist ) b_EBISTimestamp->GetEntry(entry,0);
   if( isELUMExist ) b_ELUM->GetEntry(entry,0);
   if( isEZEROExist) b_EZERO->GetEntry(entry,0);
   if( isTACExist ) b_TAC->GetEntry(entry,0);
   
   if ( isTraceDataExist ){
      b_Trace_Energy->GetEntry(entry,0);
      b_Trace_Energy_Time->GetEntry(entry,0);
      b_Trace_Energy_RiseTime->GetEntry(entry,0);

      b_Trace_RDT->GetEntry(entry,0);
      b_Trace_RDT_Time->GetEntry(entry,0);
      b_Trace_RDT_RiseTime->GetEntry(entry,0);
   }
   
   //#################################################################### gate
//   bool rdt_energy = false;
//   for( int rID = 0; rID < 8; rID ++){
//      if( rdt[rID] > 5000 ) rdt_energy = true; 
//   }
//   if( !rdt_energy ) return kTRUE;
   
   //#################################################################### processing
   for(int i = 0 ; i < 8 ; i++){
      rdtC[i]   = rdtCorr[i] * rdt[i];
      rdtC_t[i] = rdt_t[i]; 
      if( !TMath::IsNaN(rdt[i]) ) {
         rdtID[i] = i;
         rdtMultiHit ++;
      }
   }
   
   ULong64_t eTime = -2000; //this will be the time for Ex valid
   Float_t teTime = 0.; //time from trace
   int detTime = -1; // for coinTime
   for(int idet = 0 ; idet < numDet; idet++){
      
      if( !TMath::IsNaN(e[idet]) || e[idet] > 0 ){
         eC[idet]   = e[idet]/eCorr[idet][0] + eCorr[idet][1];  
         eC_t[idet] = e_t[idet]; // ch
      }else{
         continue; // when e is invalid, nothing need to do
      }
       
      if( !TMath::IsNaN(xf[idet]) || xf[idet] > 0) xfC[idet] = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
      if( !TMath::IsNaN(xn[idet]) || xn[idet] > 0) xnC[idet] = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
      
      //========= calculate x
      if(xf[idet] > 0  && xn[idet] > 0 ) {
         x[idet] = (xfC[idet]-xnC[idet])/(xfC[idet]+xnC[idet]);
         hitID[idet] = 0;
      }else if((xf[idet] == 0 || TMath::IsNaN(xf[idet])) && ( xn[idet] > 0 || !TMath::IsNaN(xn[idet])) ){
         x[idet] = (1-2*xnC[idet]/e[idet]);
         hitID[idet] = 1;
      }else if(( xf[idet] > 0 || !TMath::IsNaN(xf[idet])) && (xn[idet] == 0 || TMath::IsNaN(xn[idet])) ){
         x[idet] = (2*xfC[idet]/e[idet]-1);
         hitID[idet] = 2;
      }else{
         x[idet] = TMath::QuietNaN();
      }
   
      x[idet] = x[idet] / xCorr[idet];
   
      //if( idet == 1 && e[idet] > 0 ){
      //   printf(" e: %9.3f , %9.3f | xf : %9.3f, %9.3f | xn : %9.3f, %9.3f | x : %9.3f , hitID: %d\n", e[idet], eC[idet], xf[idet], xfC[idet], xn[idet], xnC[idet], x[idet], hitID[idet]);
      //} 
      
      //if( idet >= 17 && e[idet] > 0) printf("%d, %d , %f, %f \n", eventID, idet, eC[idet], e[idet]);
      
      //========= calculate z, det
      if( TMath::IsNaN(x[idet]) ) {
         z[idet] = TMath::QuietNaN();
      }else{ 
         int detID = idet%iDet;
         if( pos[detID] < 0 ){
            z[idet] = pos[detID] - (-x[idet] + 1.)*length/2 ; 
         }else{
            z[idet] = pos[detID] + (x[idet] + 1.)*length/2 ; 
         }
         multiHit ++;
         count ++;
         det = idet;
         
         //========== coincident between array and RDT
         if( 0 <= det && det < iDet ){
            if( !TMath::IsNaN(rdt[0]) && !TMath::IsNaN(rdt[4]) ) arrayRDT = 1;
            if( !TMath::IsNaN(rdt[1]) && !TMath::IsNaN(rdt[5]) ) arrayRDT = 2;
            if( !TMath::IsNaN(rdt[2]) && !TMath::IsNaN(rdt[6]) ) arrayRDT = 3;
            if( !TMath::IsNaN(rdt[3]) && !TMath::IsNaN(rdt[7]) ) arrayRDT = 0;
         }else if( iDet <= det && det < 2*iDet ){
            if( !TMath::IsNaN(rdt[0]) && !TMath::IsNaN(rdt[4]) ) arrayRDT = 0;
            if( !TMath::IsNaN(rdt[1]) && !TMath::IsNaN(rdt[5]) ) arrayRDT = 1;
            if( !TMath::IsNaN(rdt[2]) && !TMath::IsNaN(rdt[6]) ) arrayRDT = 2;
            if( !TMath::IsNaN(rdt[3]) && !TMath::IsNaN(rdt[7]) ) arrayRDT = 3;
         }else if( 2*iDet <= det && det < 3*iDet ){
            if( !TMath::IsNaN(rdt[0]) && !TMath::IsNaN(rdt[4]) ) arrayRDT = 3;
            if( !TMath::IsNaN(rdt[1]) && !TMath::IsNaN(rdt[5]) ) arrayRDT = 0;
            if( !TMath::IsNaN(rdt[2]) && !TMath::IsNaN(rdt[6]) ) arrayRDT = 1;
            if( !TMath::IsNaN(rdt[3]) && !TMath::IsNaN(rdt[7]) ) arrayRDT = 2;
         }else if( 3*iDet <= det && det < 4*iDet ){
            if( !TMath::IsNaN(rdt[0]) && !TMath::IsNaN(rdt[4]) ) arrayRDT = 2;
            if( !TMath::IsNaN(rdt[1]) && !TMath::IsNaN(rdt[5]) ) arrayRDT = 3;
            if( !TMath::IsNaN(rdt[2]) && !TMath::IsNaN(rdt[6]) ) arrayRDT = 0;
            if( !TMath::IsNaN(rdt[3]) && !TMath::IsNaN(rdt[7]) ) arrayRDT = 1;
         }
            
         //========== coincident time
         detTime = idet;
         eTime  = eC_t[idet];
         if ( isTraceDataExist ){
            teTime = te_t[idet];
            teS    = te[idet];
            te_tS  = te_t[idet];
            te_rS  = te_r[idet];
         }
         //printf(" det: %d, detID: %d, x: %f, pos:%f, z: %f \n", det, detID, x[idet], pos[detID], z[idet]);
      
         //========== Calculate Ex and thetaCM
         if( TMath::IsNaN(eC[idet]) || isReaction == false ){
            Ex = TMath::QuietNaN();
            thetaCM = TMath::QuietNaN(); 
            thetaLab = TMath::QuietNaN();
            
         }else{
         
            double y = eC[idet] + mass;
            Z = alpha * gamma * beta * z[idet];
            H = TMath::Sqrt(TMath::Power(gamma * beta,2) * (y*y - mass * mass) ) ;
            
            if( TMath::Abs(Z) < H ) {            
              //using Newton's method to solve 0 ==  H * sin(phi) - G * tan(phi) - Z = f(phi) 
              double tolerrence = 0.001;
              double phi = 0; //initial phi = 0 -> ensure the solution has f'(phi) > 0
              double nPhi = 0; // new phi

              int iter = 0;
              do{
                phi = nPhi;
                nPhi = phi - (H * TMath::Sin(phi) - G * TMath::Tan(phi) - Z) / (H * TMath::Cos(phi) - G /TMath::Power( TMath::Cos(phi), 2));               
                iter ++;
                if( iter > 10 || TMath::Abs(nPhi) > TMath::PiOver2()) break;
              }while( TMath::Abs(phi - nPhi ) > tolerrence);
              phi = nPhi;

              // check f'(phi) > 0
              double Df = H * TMath::Cos(phi) - G / TMath::Power( TMath::Cos(phi),2);
              if( Df > 0 && TMath::Abs(phi) < TMath::PiOver2()  ){
                double K = H * TMath::Sin(phi);
                double x = TMath::ACos( mass / ( y * gamma - K));
                double k = mass * TMath::Tan(x); // momentum of particel b or B in CM frame
                double EB = TMath::Sqrt(mass*mass + Et*Et - 2*Et*TMath::Sqrt(k*k + mass * mass));
                Ex = EB - massB;
            
                double hahaha1 = gamma* TMath::Sqrt(mass * mass + k * k) - y;
                double hahaha2 = gamma* beta * k;
                thetaCM = TMath::ACos(hahaha1/hahaha2) * TMath::RadToDeg();
                
                double pt = k * TMath::Sin(thetaCM * TMath::DegToRad());
                double pp = gamma*beta*TMath::Sqrt(mass*mass + k*k) - gamma * k * TMath::Cos(thetaCM * TMath::DegToRad());
                
                thetaLab = TMath::ATan(pt/pp) * TMath::RadToDeg();
                
              }else{
                Ex = TMath::QuietNaN();
                thetaCM = TMath::QuietNaN();
                thetaLab = TMath::QuietNaN();
              }

            }else{
              Ex = TMath::QuietNaN();
              thetaCM = TMath::QuietNaN();  
              thetaLab = TMath::QuietNaN();
            } // end of if |Z| > H
         } //end of e is valid
      }//end of x is valid
   }//end of idet-loop
   
   //================================= for coincident time bewteen array and rdt
   if( multiHit == 1 ) {
      ULong64_t rdtTime = 0;
      Float_t rdtQ = 0;
      Float_t trdtTime = 0.;
      for(int i = 0; i < 8 ; i++){
         if( rdt[i] > rdtQ ) {
            rdtQ    = rdt[i];
            rdtTime = rdt_t[i];
            
            if ( isTraceDataExist ){
               trdtTime = trdt_t[i];
               trdtS    = trdt[i];
               trdt_tS  = trdt_t[i];
               trdt_rS  = trdt_r[i];
            }
         }
      }
     
      coin_t = (int)eTime - rdtTime;
      
      if ( isTraceDataExist ){
         tcoin_t = teTime - trdtTime;
         coinTimeUC = coin_t + tcoin_t;
         double f7corr = f7[detTime]->Eval(x[detTime]) + cTCorr[detTime][8];
         coinTime = (coinTimeUC - f7corr)*10.;
      }
   }
   
   if( rejZeroHit && multiHit == 0 ) return kTRUE;
   
   //#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   clock.Stop("timer");
   Double_t time = clock.GetRealTime("timer");
   clock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10d[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               eventID, 
               TMath::Nint((eventID+1)*100./totnumEntry),
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
               totnumEntry*time/(eventID+1.)/60.);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }

   return kTRUE;
}

void Cali_e_trace::SlaveTerminate()
{
}

void Cali_e_trace::Terminate()
{
   
   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();

   printf("-------------- done. %s, z-valid count: %d\n", saveFileName.Data(), count);
   
   gROOT->ProcessLine(".q");

}
