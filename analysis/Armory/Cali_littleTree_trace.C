#define Cali_littleTree_trace_cxx
// The class definition in Cali_littleTree_trace.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("Cali_littleTree_trace.C")
// root> T->Process("Cali_littleTree_trace.C","some options")
// root> T->Process("Cali_littleTree_trace.C+")
//

//#######################################################//
//   this processor is for gating before AutoCalibration
//#######################################################//

#include "Cali_littleTree_trace.h"
#include <TH2.h>
#include <TStyle.h>

void Cali_littleTree_trace::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   
}

void Cali_littleTree_trace::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t Cali_littleTree_trace::Process(Long64_t entry)
{
   
   //#################################################################### initialization
   eTemp    = TMath::QuietNaN();
   xTemp    = TMath::QuietNaN();
   zTemp    = TMath::QuietNaN();
   
   detIDTemp = -4;
   hitID = -4;
   multiHit = 0;
   
   coinTimeUC = TMath::QuietNaN(); //uncorrected
   coinTime = TMath::QuietNaN();
   bool rdtgate1 = false;
   
   //#################################################################### Get Tree
   eventID += 1;
   
   b_Energy->GetEntry(entry,0);
   b_Ring->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   b_RDT->GetEntry(entry,0);
   b_EnergyTimestamp->GetEntry(entry,0);
   b_RDTTimestamp->GetEntry(entry,0);
   
   if( isTraceDataExist ){
      b_Trace_Energy_Time->GetEntry(entry,0);
      b_Trace_RDT_Time->GetEntry(entry,0);
      b_Trace_RDT->GetEntry(entry,0);
   }
   
   //=========== gate
  
   //=================== Recoil Gate
      if( isRDTCutExist){
        for(int i = 0 ; i < numCut; i++ ){
            if(cut[i]->IsInside(rdt[2*i],rdt[2*i+1])) {

            rdtgate1= true;
            break; /// only one is enough
          }
        }
      }
   
   if( !isRDTCutExist ){
      rdtgate1 = false;
   }
   
   //printf("%d, %d, %d, %d \n", rejRDT1, rejRDT2, rejRDT3, rejRDT4 ); 

   if(rdtgate1) return kTRUE; //######### rdt gate
   
   //for( int i = 0; i < 4; i ++) printf("%d | %f, %f \n", i, trdt[2*i], trdt[2*i+1]);
    //printf("============HERE==============\n");
   
   //#################################################################### processing
   ULong64_t eTime = -2000; //this will be the time for Ex valid
   Float_t teTime = TMath::QuietNaN(); //time from trace
   
   for(int idet = 0 ; idet < numDet; idet++){
      
      if( e[idet] < 100 ) continue;
      //if( ring[idet] > 100 ) continue;
      if( TMath::IsNaN(e[idet]) ) continue;
      if( TMath::IsNaN(xf[idet]) && TMath::IsNaN(xn[idet])  ) continue;
      
      if( e[idet] == 0 ) continue;
      if( xf[idet] == 0 && xn[idet] == 0 ) continue;
   
      double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
      double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
      
      eTemp = e[idet];

      if (idet%6 == 5 && eTemp > 1150) continue;
      if (idet%6 == 4 && eTemp > 1600) continue;
      if (idet%6 == 3 && eTemp > 2300) continue;

//===================== calculate X
      /// range of x is (0, 1)
      if  ( !TMath::IsNaN(xfC) && !TMath::IsNaN(xnC) ) xTemp = 0.5 + 0.5 * (xfC - xnC ) / e[idet];
      if  ( !TMath::IsNaN(xfC) &&  TMath::IsNaN(xnC) ) xTemp = xfC/ e[idet];
      if  (  TMath::IsNaN(xfC) && !TMath::IsNaN(xnC) ) xTemp = 1.0 - xnC/ e[idet];
      
      xTemp = (xTemp-0.5)/xScale[idet]+0.5;
      
      /*
      if(xfC > eTemp/2.){
         xTemp = 2*xfC/eTemp - 1. ;
         hitID = 1;
      }else if(xnC > eTemp/2.){
         xTemp = 1. - 2* xnC/eTemp;
         hitID = 2;
      }else{
         xTemp = TMath::QuietNaN();
      }*/
      
      if( abs(xTemp-0.5) > 0.8/2. ) continue;
         
      //if( idet >= 17 && e[idet] > 0) printf("%d, %d , %f, %f \n", eventID, idet, eC[idet], e[idet]);
      //printf("%d, %d , %f , %f\n", eventID, idet, e[idet], xTemp);
      
      
   //==================== calculate Z
      numCol = numDet/numRow;
      if( detGeo.firstPos > 0 ) {
        zTemp = detGeo.detLength*(1.0-xTemp) + detGeo.detPos[5-idet%numCol];
      }else{
        zTemp = detGeo.detLength*(xTemp-1.0) + detGeo.detPos[5-idet%numCol];
      }

   bool coinFlag = false;

    //================ coincident with Recoil when z is calculated.
      if( !TMath::IsNaN(zTemp) ) { 
        for( int j = 0; j < 8 ; j++){
          if( TMath::IsNaN(rdt[j]) ) continue; 

          int tdiff = rdt_t[j] - e_t[idet];

         if( -20 < tdiff && tdiff < 40 )  {
            coinFlag = true;
         }
      }
   }
   if( coinFlag == false ) return kTRUE;
      
      if( !TMath::IsNaN(zTemp) ) {
         multiHit++;
         detIDTemp = idet;
         eTime  = e_t[idet];
         if( isTraceDataExist ) teTime = te_t[idet];         
      }
   

   }//end of idet-loop
   
   
   //printf(" ----%d \n", multiHit);
   
   if( multiHit != 1 ) return kTRUE;
   
   
   //========= for recoil
   ULong64_t rdtTime = 0;
   Float_t trdtTime = 0.;
   int rdtIDtemp = -1;
   rdtdEMultiHit = 0;

    for( int i = 0; i< 8/2 ; i++){
      if( !TMath::IsNaN(rdt[2*i]) && !TMath::IsNaN(rdt[2*i+1]) ) {
         rdtdEMultiHit ++;
         rdtID = 2*i; //dE
      }
   }
   
   //printf("===== %d \n", rdtdEMultiHit);
   
   if ( rdtdEMultiHit < 1) return kTRUE;
   
   //for coincident time bewteen array and rdt
   if( multiHit == 1 && rdtdEMultiHit >= 1 ){

      ULong64_t rdtTime = rdt_t[rdtID];
      Float_t trdtTime =  trdt_t[rdtID];

      int coin_t = (int) rdtTime - eTime;
      float tcoin_t = trdtTime - teTime;
      
      coinTimeUC = 10.0 * (coin_t + tcoin_t) ; // in nano-sec
      
      if( isTraceDataExist ){
         double f7corr = f7[detIDTemp]->Eval(xTemp) + cTCorr[detIDTemp][8];
         
         //Ad-hoc solution 
         double rdtCorr = 0;
         switch(rdtIDtemp){
            case 0: rdtCorr = 23.0 ; break;
            case 1: rdtCorr = 0.0 ; break;
            case 2: rdtCorr = 4.3 ; break;
            case 3: rdtCorr = 15.3 ; break;
         }
         
         coinTime = (coinTimeUC - f7corr) - rdtCorr;
      }
   }
   //printf("%f \n", coinTime);
   
   //if ( abs(coinTime) > 50) return kTRUE;
   
   //printf("============HERE==============\n");
   
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

void Cali_littleTree_trace::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void Cali_littleTree_trace::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   
   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();

   printf("-------------- done. %s, z-valid count: %d\n", saveFileName.Data(), count);

}
