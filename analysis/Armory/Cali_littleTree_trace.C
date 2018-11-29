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

double rdtGate = 5000;

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
   
   //#################################################################### Get Tree
   eventID += 1;
   
   b_Energy->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   b_RDT->GetEntry(entry,0);
   b_EnergyTimestamp->GetEntry(entry,0);
   b_RDTTimestamp->GetEntry(entry,0);
   
   if( isTraceDataExist ){
      b_Trace_Energy_Time->GetEntry(entry,0);
      b_Trace_RDT_Time->GetEntry(entry,0);
   }
   
   //=========== gate
   //bool rdt_energy = false;
   //for( int rID = 0; rID < 8; rID ++){
   //   if( rdt[rID] > rdtGate ) rdt_energy = true; 
   //}
   //if( !rdt_energy ) return kTRUE;
   
   //int rdtMultiHit = 0;
   //for( int i = 0; i < 8; i++){
   //   if( !TMath::IsNaN(rdt[i]) ) rdtMultiHit ++;
   //}
   //
   //if( rdtMultiHit != 2 ) return kTRUE; //######### multiHit gate   
   
   //bool rejRDT1 = true; if( isRDTCutExist && cut[0]->IsInside( rdt[4], rdt[0] )) rejRDT1 = false;
   //bool rejRDT2 = true; if( isRDTCutExist && cut[1]->IsInside( rdt[5], rdt[1] )) rejRDT2 = false;
   //bool rejRDT3 = true; if( isRDTCutExist && cut[2]->IsInside( rdt[6], rdt[2] )) rejRDT3 = false;
   //bool rejRDT4 = true; if( isRDTCutExist && cut[3]->IsInside( rdt[7], rdt[3] )) rejRDT4 = false;
   //
   //if( rejRDT1 && rejRDT2 && rejRDT3 && rejRDT4) return kTRUE; //######### rdt gate

   //#################################################################### processing
   ULong64_t eTime = -2000; //this will be the time for Ex valid
   Float_t teTime = TMath::QuietNaN(); //time from trace
   
   for(int idet = 0 ; idet < numDet; idet++){
      
      if( TMath::IsNaN(e[idet]) ) continue;
      if( TMath::IsNaN(xf[idet]) && TMath::IsNaN(xn[idet])  ) continue;
      
      if( e[idet] == 0 ) continue;
      if( xf[idet] == 0 && xn[idet] == 0 ) continue;
      
      detIDTemp = idet;
      eTemp = e[idet];
            
      double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
      double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
      
      //========= calculate x
      if(xf[idet] > 0  && xn[idet] > 0 ) {
         xTemp = (xfC-xnC)/(xfC+xnC);
         multiHit++;
         hitID = 0;
      }else if(xf[idet] == 0 && xn[idet] > 0 ){
         xTemp = (1-2*xnC/e[idet]);
         multiHit++;
         hitID = 1;
      }else if(xf[idet] > 0 && xn[idet] == 0 ){
         xTemp = (2*xfC/e[idet]-1);
         multiHit++;
         hitID = 2;
      }else{
         xTemp = TMath::QuietNaN();
      }
      
      //if( idet >= 17 && e[idet] > 0) printf("%d, %d , %f, %f \n", eventID, idet, eC[idet], e[idet]);
      //printf("%d, %d , %f \n", eventID, idet, e[idet]);
      
      
      //========= calculate z
      
      int detID = idet%iDet;
      if( pos[detID] < 0 ){
         zTemp = pos[detID] - (-xTemp + 1.)*length/2 ; 
      }else{
         zTemp = pos[detID] + (xTemp + 1.)*length/2 ; 
      }
   
      if( isTraceDataExist ) {
         eTime  = e_t[idet];
         teTime = te_t[idet];         
      }
   }//end of idet-loop
   
   if( multiHit == 0 ) return kTRUE;
   
   //for coincident time bewteen array and rdt
   if( multiHit == 1 && isTraceDataExist ) {
      ULong64_t rdtTime = 0;
      Float_t rdtQ = 0;
      Float_t trdtTime = 0.;
      for(int i = 0; i < 8 ; i++){
         if( rdt[i] > rdtQ ) {
            rdtQ    = rdt[i];
            
            rdtTime = rdt_t[i];
            trdtTime = trdt_t[i];
         }
      }
     
      int coin_t = (int) eTime - rdtTime;
      float tcoin_t = teTime - trdtTime;
      
      coinTimeUC = coin_t + tcoin_t;
      
      double f7corr = f7[detIDTemp]->Eval(xTemp) + cTCorr[detIDTemp][8];
      coinTime = (coinTimeUC - f7corr)*10.;
   }
   
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
