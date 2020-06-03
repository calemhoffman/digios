#define Tree4NN_cxx
// The class definition in Tree4NN.h has been generated automatically
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
// root> T->Process("Tree4NN.C")
// root> T->Process("Tree4NN.C","some options")
// root> T->Process("Tree4NN.C+")
//

#include "Tree4NN.h"
#include <TH2.h>
#include <TStyle.h>


void Tree4NN::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   
}

void Tree4NN::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t Tree4NN::Process(Long64_t entry)
{

   //=============== initialization
   e_n = TMath::QuietNaN();
   z_n = TMath::QuietNaN();
   coinTime_n = TMath::QuietNaN();
   rdtdE = TMath::QuietNaN();
   rdtE = TMath::QuietNaN();
   detID = -4;
   
   //=============== get Branch
   b_e->GetEntry(entry);
   b_z->GetEntry(entry);
   b_det->GetEntry(entry);
   b_coinTime_ns->GetEntry(entry);
   b_Trace_RDT->GetEntry(entry);
   
   b_multiHit->GetEntry(entry);
   b_rdtdEMultiHit->GetEntry(entry);
   b_Ex->GetEntry(entry);
   
   
   //=============== gate
   if ( multiHit != 1 ) return kTRUE;
   if ( rdtdEMultiHit != 1 ) return kTRUE;
   if( TMath::Abs(coinTime-16) > 17 ) return kTRUE;
  
   
   // need to change manually depends on rdt or trdt
   bool rejRDT1 = true; if( isRDTCutExist && cut[0]->IsInside( trdt[0], trdt[1] )) rejRDT1 = false;
   bool rejRDT2 = true; if( isRDTCutExist && cut[1]->IsInside( trdt[2], trdt[3] )) rejRDT2 = false;
   bool rejRDT3 = true; if( isRDTCutExist && cut[2]->IsInside( trdt[4], trdt[5] )) rejRDT3 = false;
   bool rejRDT4 = true; if( isRDTCutExist && cut[3]->IsInside( trdt[6], trdt[7] )) rejRDT4 = false;
   
   if( !isRDTCutExist ){
      rejRDT1 = false;
      rejRDT2 = false;
      rejRDT3 = false;
      rejRDT4 = false;
   }
   
   if( rejRDT1 && rejRDT2 && rejRDT3 && rejRDT4) return kTRUE; //######### rdt gate
   
   //=============== fill tree
   for( int idet = 0 ; idet < 30 ; idet ++){
      if( TMath::IsNaN(e[idet]) ) continue;
      
      e_n = e[idet];
      z_n = z[idet];
      
      detID = idet;
   }
   
   coinTime_n = coinTime;
   Ex_n = Ex;
   
   for( int idet = 0 ; idet < 8 ; idet += 2 ){
     
     if( TMath::IsNaN( trdt[idet] ) ) continue;
     
     //printf("%i, %f, %f\n", idet, rdtScale[idet], rdtScale[idet+1]); 
     //printf("%f, %f\n", trdt[idet], trdt[idet+1]);
     
     rdtE  = rdtScale[idet]*trdt[idet];
     rdtdE = rdtScale[idet+1]*trdt[idet+1];
     
     //printf("%f, %f\n", rdtE, rdtdE);
     
     //rdtE  = trdt[idet];
     //rdtdE = trdt[idet+1];
     rdtID_n = idet;
     
   }
   
   if( TMath::IsNaN(coinTime_n) ) return kTRUE;
   if( TMath::IsNaN(Ex_n) )       return kTRUE;
   if( TMath::IsNaN(e_n) )        return kTRUE;
   if( TMath::IsNaN(z_n) )        return kTRUE;
   if( TMath::IsNaN(rdtE) )       return kTRUE;
   if( TMath::IsNaN(rdtdE) )      return kTRUE;
   
   isGood = 1;
   if( !cut[1]->IsInside( rdtE, rdtdE ) || TMath::Abs(coinTime-16) > 8 || (Ex_n > 2 || Ex_n < -0.5) || rdtE < 2000 ) isGood = 0;

   
   validEventCount++;
   
   //#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   clock.Stop("timer");
   Double_t time = clock.GetRealTime("timer");
   clock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%llu[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               entry, 
               TMath::Nint((entry+1)*100./totnumEntry),
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
               totnumEntry*time/(entry+1.)/60.);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }

   return kTRUE;
}

void Tree4NN::SlaveTerminate()
{

}

void Tree4NN::Terminate()
{
   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();

   printf("-------------- done, saved in %s. valid count = %d\n", saveFileName.Data(), validEventCount);
   
   gROOT->ProcessLine(".q");

}
