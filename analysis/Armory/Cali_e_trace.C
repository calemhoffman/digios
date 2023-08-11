#define Cali_e_trace_cxx

//##########################################################
//      Cali e, xf, xn, coinTime with gate
//      the gate should be modified as needed. 
//##########################################################

#include "Cali_e_trace.h"
#include <TH2.h>
#include <TStyle.h>

//#################################################################### User setting

bool rejZeroHit = true; /// when no hit on array, don't save that event

bool useTraceToReplaceArrayEnergy = false;


//####################################################################


void Cali_e_trace::Begin(TTree * /*tree*/){
   TString option = GetOption();  
   
   if( rejZeroHit ) printf(" ================= reject when zero hit on array.\n");
   if( useTraceToReplaceArrayEnergy && isTraceDataExist) printf(" ================= use Trace energy replace array energy.\n");
   
}

void Cali_e_trace::SlaveBegin(TTree * /*tree*/){
   TString optcion = GetOption();
}

Bool_t Cali_e_trace::Process(Long64_t entry){

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
   rdtdEMultiHit = 0;
   
   Ex  = TMath::QuietNaN();
   thetaCM  = TMath::QuietNaN();
   thetaLab  = TMath::QuietNaN();
   
   for(int i = 0; i < NRDT ; i++){
      rdtC[i] = TMath::QuietNaN();
      rdtC_t[i] = 0;
   }

   rdtID = -4;
   
   coin_t = -2000;

   if( isTraceDataExist ){
      tcoin_t = TMath::QuietNaN();
      coinTimeUC = TMath::QuietNaN(); //uncorrected
      coinTime = TMath::QuietNaN();
      
      for(int i = 0; i < numDet; i++){
         te[i] = TMath::QuietNaN();
         te_t[i] = TMath::QuietNaN();
         te_r[i] = TMath::QuietNaN();
      }
      for(int i = 0; i < NRDT; i++){
         trdt[i] = TMath::QuietNaN();
         trdt_t[i] = TMath::QuietNaN();
         trdt_r[i] = TMath::QuietNaN();
      }
   }
   
   //#################################################################### get event
   eventID += 1;
   
   if( isRunIDExist ) {
      b_runID->GetEntry(entry,0);  
      run = runID;
   }

   b_Energy->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   b_RING->GetEntry(entry,0);
   b_RDT->GetEntry(entry,0);
   b_EnergyTimestamp->GetEntry(entry,0);
   b_RDTTimestamp->GetEntry(entry,0);
   
   if( isEBISExist ) b_EBISTimestamp->GetEntry(entry,0);
   if( isELUMExist ) {
     b_ELUM->GetEntry(entry,0);
     b_ELUMTimestamp->GetEntry(entry,0);
   }
   if( isEZEROExist) {
      b_EZERO->GetEntry(entry,0);
      b_EZEROTimestamp->GetEntry(entry,0);
   }
   if( isTACExist ) {
      b_TAC->GetEntry(entry,0);
      b_TACTimestamp->GetEntry(entry,0);
   }
   if ( isTraceDataExist ){
      b_Trace_Energy->GetEntry(entry,0);
      b_Trace_Energy_Time->GetEntry(entry,0);
      b_Trace_Energy_RiseTime->GetEntry(entry,0);

      b_Trace_RDT->GetEntry(entry,0);
      b_Trace_RDT_Time->GetEntry(entry,0);
      b_Trace_RDT_RiseTime->GetEntry(entry,0);
   }

   //#################################################################### gate
    
   ///============ Simple RDT gate base on energy
///   bool rdt_energy = false;
///   for( int rID = 0; rID < 8; rID ++){
///      if( rdt[rID] > 5000 ) rdt_energy = true; 
///   }
///   if( !rdt_energy ) return kTRUE;

   ///============================= RDT TCutG gate
   bool rejRDT1 = true; 
   bool rejRDT2 = true; 
   bool rejRDT3 = true; 
   bool rejRDT4 = true; 

   if( isRDTCutExist ){
      if ( isTraceDataExist ){
         if(cut[0]->IsInside( rdt[0], rdt[1] )) rejRDT1 = false;
         if(cut[1]->IsInside( rdt[2], rdt[3] )) rejRDT2 = false;
         if(cut[2]->IsInside( rdt[4], rdt[5] )) rejRDT3 = false;
         if(cut[3]->IsInside( rdt[6], rdt[7] )) rejRDT4 = false;
      }else{
         if(cut[0]->IsInside( trdt[0], trdt[1] )) rejRDT1 = false;
         if(cut[1]->IsInside( trdt[2], trdt[3] )) rejRDT2 = false;
         if(cut[2]->IsInside( trdt[4], trdt[5] )) rejRDT3 = false;
         if(cut[3]->IsInside( trdt[6], trdt[7] )) rejRDT4 = false;
      }
   }else{
      rejRDT1 = false;
      rejRDT2 = false;
      rejRDT3 = false;
      rejRDT4 = false;
   }
   
   // uncomment for ???
//   if( rejRDT1 && rejRDT2 && rejRDT3 && rejRDT4) return kTRUE; ///######### rdt gate
   
   ///============================= timestamp Coincident gate
   bool coinFlag = false;

   ///if no recoil. i.e. all rdt are NAN, coinFlag == true, i.e disable
   int countInvalidRDT = 0;
   for( int j = 0; j < NRDT; j++){
      if( TMath::IsNaN(rdt[j]) ) countInvalidRDT ++;
   }
   if( countInvalidRDT == NRDT ) {
      coinFlag = true;
   }else{
      for( int i = 0; i < numDet ; i++){
         for( int j = 0; j < NRDT ; j++){
            if( TMath::IsNaN(rdt[j]) ) continue; 
            int tdiff = rdt_t[j] - e_t[i];
            if( -200 < tdiff && tdiff < 200 )   
            coinFlag = true;
         }
      }
   }
// uncomment for recoil coincidence
  //if( coinFlag == false ) return kTRUE;
   
   //#################################################################### processing
   
   //========================== Array
   int uniqeDetID = -1;
   for(int idet = 0 ; idet < numDet; idet++){
     //crh need to be careful and check these here ...
     hitID[idet] = 0; /// hitID = 1 for only xf, hitID = 2 for only xn, hitID = 3 for both xf and xn
     ///======= Basic array gate
     if( TMath::IsNaN(e[idet])) continue;
     //if( ring[idet] < -100 || ring[idet] > 100 ) continue;
     if( !TMath::IsNaN(xf[idet]) ) hitID[idet] += 1;
     if( !TMath::IsNaN(xn[idet]) ) hitID[idet] += 2;
     if( hitID[idet] == 0 ) continue;
     //if( isTraceDataExist  && te_r[idet] > 200 ) continue; /// when rise time > 50, skip 
     
     ///====== Calibrations go here
     if( isTraceDataExist  && useTraceToReplaceArrayEnergy ){
       eC[idet] = te[idet]/eCorr[idet][0] + eCorr[idet][1];
       eC[idet] = (te[idet]/eCorr[idet][0] + eCorr[idet][1])*eCorr2[idet][0]+eCorr2[idet][1];
       eC_t[idet] = e_t[idet] - 100 + te_t[idet];
       e[idet] = te[idet];
     }else{
       eC[idet]   = e[idet]/eCorr[idet][0] + eCorr[idet][1];  
       eC[idet] = (e[idet]/eCorr[idet][0] + eCorr[idet][1])*eCorr2[idet][0]+eCorr2[idet][1];
       eC_t[idet] = e_t[idet]; 
     }
     
     if (eC[idet] < 0.6) continue; //

     xfC[idet] = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
     xnC[idet] = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
      
     ///========= calculate x, range (-1,1)    //crh otherwise check...
     if( hitID[idet] == 3 ) {
      // x[idet] = (xfC[idet]-xnC[idet])/(xfC[idet]+xnC[idet]);
      if (xfC[idet] >= e[idet]/2.) {
         hitID[idet] = 1;
      } 
      if (xnC[idet] >= e[idet]/2.) {
         hitID[idet] = 2;
      }
     }
     if( hitID[idet] == 1 ) x[idet] = 2.0*xfC[idet]/e[idet] - 1.0;
     if( hitID[idet] == 2 ) x[idet] = 1.0 - 2.0 * xnC[idet]/e[idet];
      
     x[idet] = x[idet] / xCorr[idet];

     ///========= Calculate z
     int detID = idet%detGeo.nDet;
     if( pos[detID] < 0 ){
       z[idet] = pos[detID] - (-x[idet] + 1.)*length/2 ; 
     }else{
       z[idet] = pos[detID] + (-x[idet] + 1.)*length/2 ; 
     }

     ///========= multiplicity the 2 array detectors are hit.
     if( !TMath::IsNaN(z[idet]) ) multiHit++;
     det = idet;
   }
   
   //================ Calculate Ex and theta when single array hit.
   if( multiHit == 1 ) {
      validEventCount ++;
      
      if( isReaction == true ){

         int idet = det;

         double y = eC[idet] + mass;
         Z = alpha * gamma * beta * z[idet];
         H = TMath::Sqrt(TMath::Power(gamma * beta,2) * (y*y - mass * mass) ) ;

         if( TMath::Abs(Z) < H ) {            
            ///using Newton's method to solve 0 ==  H * sin(phi) - G * tan(phi) - Z = f(phi) 
            double tolerrence = 0.001;
            double phi = 0; ///initial phi = 0 -> ensure the solution has f'(phi) > 0
            double nPhi = 0; /// new phi

            int iter = 0;
            do{
               phi = nPhi;
               nPhi = phi - (H * TMath::Sin(phi) - G * TMath::Tan(phi) - Z) / (H * TMath::Cos(phi) - G /TMath::Power( TMath::Cos(phi), 2));               
               iter ++;
               if( iter > 10 || TMath::Abs(nPhi) > TMath::PiOver2()) break;
            }while( TMath::Abs(phi - nPhi ) > tolerrence);
            phi = nPhi;

            /// check f'(phi) > 0
            double Df = H * TMath::Cos(phi) - G / TMath::Power( TMath::Cos(phi),2);
            if( Df > 0 && TMath::Abs(phi) < TMath::PiOver2()  ){
               double K = H * TMath::Sin(phi);
               double x = TMath::ACos( mass / ( y * gamma - K));
               double k = mass * TMath::Tan(x); /// momentum of particel b or B in CM frame
               double EB = TMath::Sqrt(mass*mass + Et*Et - 2*Et*TMath::Sqrt(k*k + mass * mass));
               Ex = EB - massB;

               double hahaha1 = gamma* TMath::Sqrt(mass * mass + k * k) - y;
               double hahaha2 = gamma* beta * k;
               thetaCM = TMath::ACos(hahaha1/hahaha2) * TMath::RadToDeg();

               double pt = k * TMath::Sin(thetaCM * TMath::DegToRad());
               double pp = gamma*beta*TMath::Sqrt(mass*mass + k*k) - gamma * k * TMath::Cos(thetaCM * TMath::DegToRad());

               thetaLab = TMath::ATan(pt/pp) * TMath::RadToDeg();
            }
         }
      }
   }
   
   //=============================== Recoil
   Float_t trdtCorr[8][2] = {{0.4,1.0},{0.0,1.5},{-1.0,1.0},{0.0,0.75},{0.45,1.0},{0.0,1.3},{0.1,1.0},{0.0,1.3}};
   for(int i = 0 ; i < NRDT ; i++){
      if( TMath::IsNaN(rdt[i]) ) continue;
      if( isTraceDataExist ){
         //New RDT CAL
         trdt[i]   = (trdt[i] - trdt_r[i]*trdtCorr[i][0]) * trdtCorr[i][1];
         trdt_t[i] = trdt_t[i]; 
      }
      //New RDT CAL
      rdtC[i]   = rdtCorr[i][0] * rdt[i] + rdtCorr[i][1];
      rdtC_t[i] = rdt_t[i]; 

      //printf("%2d, %f| %f %f %f \n", i, rdt[i], rdtCorr[i][0], rdtCorr[i][1], rdtC[i]);
   }
   for (int i = 0 ; i < 4 ; i++) {
      if( isTraceDataExist ){
         trdte[i] = (trdt[2*i+1]+trdt[2*i]);
      }
      rdte[i] = (rdtC[2*i+1]+rdtC[2*i]);
   }
   //double check below - ok, except maybe could be > E not just NaN ??
   for( int i = 0; i< NRDT/2 ; i++){
      if( /* !TMath::IsNaN(rdt[2*i]) &&*/ !TMath::IsNaN(rdt[2*i+1]) ) {
         rdtdEMultiHit ++;
         rdtID = 2*i+1; //dE
      }
   }
   
   //================================= for coincident time bewteen array and rdt
   //if( multiHit == 1 && rdtdEMultiHit == 1) {
   if( multiHit ==1 && rdtdEMultiHit >= 1 ){ 
      ///===== no Trace data
      ULong64_t eTime = e_t[det];

      /// for dE detector only
      ULong64_t rdtTime = rdt_t[rdtID];

      coin_t = (int) eTime - rdtTime; // digitizer timeStamp
     
      ///======== with trace data
      if( isTraceDataExist ) {
         
         Float_t teTime = te_t[det];
         Float_t trdtTime =  trdt_t[rdtID];
         
         tcoin_t = teTime - trdtTime; // trace trigger time 
         
         //Ad-hoc solution 
         double rdtCorrAux = 0;
         switch(rdtID){
           case 1: rdtCorrAux = 0.5 ; break;
           case 3: rdtCorrAux = -7.0 ; break;
           case 5: rdtCorrAux = 2.0 ; break;
           case 7: rdtCorrAux = 10.0 ; break;
         }
         
         coinTimeUC = 10.0*((float)coin_t + tcoin_t);// // in nano-sec
         
         double f7corr = f7[det]->Eval(x[det]);// - cTCorr[det][8];
         
         coinTime = (coinTimeUC - f7corr) + rdtCorrAux;; //+ corrofftime
      }
      
   }
   
   //if( rejZeroHit && multiHit == 0 ) return kTRUE;
   //if (multiHit < 1) return kTRUE; //commented CRH
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

void Cali_e_trace::SlaveTerminate(){
}

void Cali_e_trace::Terminate(){
   
   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();

   printf("-------------- done, saved in %s, z-valid count: %d\n", saveFileName.Data(), validEventCount);
   
   gROOT->ProcessLine(".q");

}
