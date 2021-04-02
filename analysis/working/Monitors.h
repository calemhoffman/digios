//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Apr  4 14:39:10 2017 by ROOT version 5.32/03
// from TTree psd_tree/PSD Tree
// found on file: psd.root
//////////////////////////////////////////////////////////

#ifndef Monitors_h
#define Monitors_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCutG.h>

#include "GeneralSortMapping.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Monitors : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Int_t           runID;
   Float_t         e[NARRAY];
   ULong64_t       e_t[NARRAY];
   Float_t         xf[NARRAY];
   ULong64_t       xf_t[NARRAY];
   Float_t         xn[NARRAY];
   ULong64_t       xn_t[NARRAY];
   Float_t         ring[NARRAY];
   ULong64_t       ring_t[NARRAY];
   Float_t         rdt[NRDT];
   ULong64_t       rdt_t[NRDT];
   Float_t         tac[NTAC];
   ULong64_t       tac_t[NTAC];
   Float_t         elum[NELUM];
   ULong64_t       elum_t[NELUM];
   Float_t         ezero[NEZERO];
   ULong64_t       ezero_t[NEZERO];
   
   // List of branches
   TBranch        *b_runID;   //!
   TBranch        *b_Energy;   //!
   TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XFTimestamp;   //!
   TBranch        *b_XN;   //!
   TBranch        *b_XNTimestamp;   //!
   TBranch        *b_Ring;   //!
   TBranch        *b_RingTimestamp;   //!
   TBranch        *b_RDT;   //!
   TBranch        *b_RDTTimestamp;   //!
   TBranch        *b_TAC;   //!
   TBranch        *b_TACTimestamp;   //!
   TBranch        *b_ELUM;   //!
   TBranch        *b_ELUMTimestamp;   //!
   TBranch        *b_EZERO;   //!
   TBranch        *b_EZEROTimestamp;   //!
   

   // trace analysis data
   Float_t         te[NARRAY];
   Float_t         te_r[NARRAY];
   Float_t         te_t[NARRAY];
   Float_t         trdt[NRDT];
   Float_t         trdt_t[NRDT];
   Float_t         trdt_r[NRDT];
   
   TBranch        *b_Trace_Energy;   //!
   TBranch        *b_Trace_Energy_RiseTime;   //!
   TBranch        *b_Trace_Energy_Time;   //!
   TBranch        *b_Trace_RDT;   //!
   TBranch        *b_Trace_RDT_Time;   //!
   TBranch        *b_Trace_RDT_RiseTime;   //!
   
   bool isArrayTraceExist;
   bool isRDTTraceExist;
   
   bool isTACExist;
   bool isELUMExist;
   bool isEZEROExist;
   
   
   int printControlID;
   void printControl(int n){this->printControlID = n;}
   
   double z[100];
   
   ULong64_t startTime ;
   ULong64_t endTime ;

   Monitors(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Monitors() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   
   
   TString FindStartEndTime(TTree * tree,TString BranchName);
   void Draw2DHist(TH2F * hist);
   
   void PlotEZ(bool isRaw);
   void PlotTDiff(bool isGated);
   void PlotRDT(int id, bool isRaw);

   ClassDef(Monitors,0);
};

#endif


#ifdef Monitors_cxx
void Monitors::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runID", &runID, &b_runID);
   fChain->SetBranchAddress("e", e, &b_Energy);
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);
   fChain->SetBranchAddress("ring", ring, &b_Ring);
   fChain->SetBranchAddress("ring_t", ring_t, &b_RingTimestamp);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);
   
   TBranch * br = (TBranch *) fChain->GetListOfBranches()->FindObject("tac");
   if( br == NULL ){
      printf(" ++++++++ no TAC.\n");
      isTACExist = false;
   }else{
      isTACExist = true;
      fChain->SetBranchAddress("tac", tac, &b_TAC);
      fChain->SetBranchAddress("tac_t", tac_t, &b_TACTimestamp);
   }
   
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("elum");
   if( br == NULL ){
      printf(" ++++++++ no ELUM.\n");
      isELUMExist = false;
   }else{
      isELUMExist = true;
      fChain->SetBranchAddress("elum", elum, &b_ELUM);
      fChain->SetBranchAddress("elum_t", elum_t, &b_ELUMTimestamp);
   }

   br = (TBranch *) fChain->GetListOfBranches()->FindObject("ezero");
   if( br == NULL ){
      printf(" ++++++++ no EZERO.\n");
      isEZEROExist = false;
   }else{
      isEZEROExist = true;
      fChain->SetBranchAddress("ezero", ezero, &b_EZERO);
      fChain->SetBranchAddress("ezero_t", ezero_t, &b_EZEROTimestamp);
   }
   
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("te");
   if( br == NULL ){
      printf(" ++++++++ no Array trace.\n");
      isArrayTraceExist = false;
   }else{
      isArrayTraceExist = true;
      if( isUseArrayTrace ){
         fChain->SetBranchAddress("te", e, &b_Energy);// replace  e with te
      }else{
         fChain->SetBranchAddress("te", te, &b_Trace_Energy);
      }
      fChain->SetBranchAddress("te_r", te_r, &b_Trace_Energy_RiseTime);
      fChain->SetBranchAddress("te_t", te_t, &b_Trace_Energy_Time);
   }
   
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("trdt");
   if( br == NULL ){
      printf(" ++++++++ no Recoil trace.\n");
      isRDTTraceExist = false;
   }else{
      isRDTTraceExist = true;
      if( isUseRDTTrace ) {
         fChain->SetBranchAddress("trdt", rdt, &b_RDT); // replace  rdt with trdt
         printf("************ using Trace in recoil \n");
      }else{
         fChain->SetBranchAddress("trdt", trdt, &b_Trace_RDT);
      }
      fChain->SetBranchAddress("trdt_t", trdt_t, &b_Trace_RDT_Time);
      fChain->SetBranchAddress("trdt_r", trdt_r, &b_Trace_RDT_RiseTime);
   }
   
   startTime = 0;
   endTime = 0;
   
   //printControlID = 1; // don't quite, cannot pass to Termination() ?? why??
   
   printf("=================================== End of Branch Pointer Inititization. \n");
}

Bool_t Monitors::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

TString Monitors::FindStartEndTime(TTree * tree, TString BranchName){
   
   ULong64_t timeStamp[100];
   
   int totEnetries = tree->GetEntries();
   
   printf(">>>>>>> Number of Entries : %d, Check Branch : %s\n", totEnetries, BranchName.Data());
   
   tree->SetBranchAddress(BranchName, timeStamp);
   for( int ev = 0; ev < totEnetries; ev ++ ){
      tree->GetEntry(ev);
      bool breakFlag = false;
      for( int id = 0; id < numDet; id ++){
         if( timeStamp[id] > 0 ) {
            startTime = timeStamp[id];
            breakFlag = true;
            break;
         }
      }
      if( breakFlag ) break;
   }
   
   for( int ev =  totEnetries - 1 ; ev > 0 ;  ev -- ){
      tree->GetEntry(ev);
      bool breakFlag = false;
      for( int id = 0; id < numDet; id ++){
         if( timeStamp[id] > 0 ) {
            endTime = timeStamp[id];
            breakFlag = true;
            break;
         }
      }
      if( breakFlag ) break;
   }
   
   return BranchName;
}


void DrawLine(TH1 * hist, double pos){
   
   double yMax = hist->GetMaximum();
   TLine * line = new TLine(pos, 0, pos, yMax);
   line->SetLineColor(2);
   line->Draw("");
   
}

void DrawBox(TH1* hist, double x1, double x2, Color_t color, float alpha){

   double yMax = hist->GetMaximum();
   TBox * box = new TBox (x1, 0, x2, yMax);
   box->SetFillColorAlpha(color, alpha);
   box->Draw();

}

void Monitors::Draw2DHist(TH2F * hist){
   
   if( hist->Integral() < 2000 ){
      hist->SetMarkerStyle(20);
      hist->SetMarkerSize(0.3);
      hist->Draw("");
   }else{
      hist->Draw("colz"); 
   }
}

void Monitors::SlaveBegin(TTree * /*tree*/){
   /// not use, if use, place in Monitors.C
   TString option = GetOption();
}


void Monitors::SlaveTerminate(){
  /// not use, if use, place in Monitors.C
}


double solid_angle( double th ) {
   
   double ang = 2. * TMath::Pi() * TMath::Sin( th*TMath::DegToRad() );
   
   return 1.0 / ( ang * TMath::RadToDeg() );
   
}

void LoadDetectorGeo(){
   //========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   printf("=======================\n");
   printf(" loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( x.substr(0,1) == "#" )  break;
         if( i == 0 ) Bfield  = abs(atof(x.c_str()));
         if( i == 3 )     a   = atof(x.c_str());
         if( i == 5 ) length   = atof(x.c_str());
         if( i == 14 ) firstPos = atof(x.c_str());
         if( i >= 18 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");

      vector<double> posTemp = pos;
      for(int id = 0; id < numCol; id++){
        if( firstPos > 0 ) pos[id] = firstPos + posTemp[numCol-1-id];
        if( firstPos < 0 ) pos[id] = firstPos - posTemp[numCol-1-id];
      }
      
      for(int i = 0; i < numCol ; i++){
         if( firstPos > 0 ){
            printf("\t%d, %8.2f mm - %8.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("\t%d, %8.2f mm - %8.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      
      if( firstPos > 0 ){
         zRange[1] = pos[0] + length + 30;
         zRange[0] = pos[numCol-1]  - 30;
      }else{
         zRange[0] = pos[0] -length - 30;
         zRange[1] = pos[numCol-1] + 30;
      }
      
      printf(" ------- z Range : %.2f - %.2f mm\n", zRange[0], zRange[1]);
      
   }else{
      printf("... fail\n");
   }
   
}

void LoadXFXNCorr(){
   //========================================= xf = xn correction
   printf(" loading xf-xn correction.");
   ifstream file;
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf(".......... done.\n");
   }else{
      printf(".......... fail.\n");
      
      for(int i = 0; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
}

void LoadXFXN2ECorr(){
   //========================================= e = xf + xn correction
   printf(" loading xf/xn-e correction.");
   ifstream file;
   file.open("correction_xfxn_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         xfxneCorr[i][0] = a;
         xfxneCorr[i][1] = b;
         i = i + 1;
      }
      printf("........ done.\n");
   }else{
      printf("........ fail.\n");
      for(int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0;
         xfxneCorr[i][1] = 1;
      }
   }
   file.close();
} 

void LoadXScaleCorr(){
   //========================================= X-Scale correction
   printf(" loading x-Scale correction.");
   ifstream file;
   file.open("correction_scaleX.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xScale[i] = a;  
         i = i + 1;
      }
      printf("........ done.\n");
      
   }else{
      printf("........ fail.\n");
      for( int i = 0; i < numDet ; i++){
         xScale[i] = 1.;
      }
   }
   file.close();
   
}


void LoadECorr(){
   //========================================= e correction
   printf(" loading e correction.");
   ifstream file;
   file.open("correction_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr[i][0] = a;  // 1/a1
         eCorr[i][1] = b;  //  a0 , e' = e * a1 + a0
         //printf("\n%2d, e0: %9.4f, e1: %9.4f", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf(".............. done.\n");
      
   }else{
      printf(".............. fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
   }
   file.close();
   
   printf(" loading e correction second.");
   file.open("correction_e2.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr2[i][0] = a;  // 1/a1
         eCorr2[i][1] = b;  //  a0 , e' = e * a1 + a0
         //printf("\n%2d, e0: %9.4f, e1: %9.4f", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf(".............. done.\n");
      
   }else{
      printf(".............. fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr2[i][0] = 1.;
         eCorr2[i][1] = 0.;
      }
   }
   file.close();
   
}

void LoadReactionPars(){
   
   //========================================= reaction parameters
   //check is the transfer.root is using the latest reactionConfig.txt   
   //sicne reaction.dat is generated as a by-product of transfer.root
   TFile * transfer = new TFile("transfer.root");
   TString aaa1 = "";
   TString aaa2 = "";
   if( transfer->IsOpen() ){
      TMacro * reactionConfig = (TMacro *) transfer->FindObjectAny("reactionConfig");
      TMacro presentReactionConfig ("reactionConfig.txt");
      aaa1 = ((TMD5*) reactionConfig->Checksum())->AsString();
      aaa2 = ((TMD5*) presentReactionConfig.Checksum())->AsString();
   }
   //printf("%s\n", aaa1.Data());
   //printf("%s\n", aaa2.Data());

   if( aaa1 != aaa2 ) {
     printf("########################## recalculate transfer.root \n");
     system("../Cleopatra/Transfer");
     printf("########################## transfer.root updated\n");
   }
   printf(" loading reaction parameters");
   ifstream file;
   file.open("reaction.dat");
   isReaction = false;
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x ){
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) mass = atof(x.c_str());
         if( i == 1 ) q    = atof(x.c_str());
         if( i == 2 ) beta = atof(x.c_str()); 
         if( i == 3 ) Et   = atof(x.c_str()); 
         if( i == 4 ) massB = atof(x.c_str()); 
         i = i + 1;
      }
      printf("........ done.\n");

      isReaction = true;
      alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.; //MeV/mm
      gamm = 1./TMath::Sqrt(1-beta*beta);
      G = alpha * gamm * beta * a ;
      printf("\tmass-b    : %f MeV/c2 \n", mass);
      printf("\tcharge-b  : %f \n", q);
      printf("\tE-total   : %f MeV \n", Et);
      printf("\tmass-B    : %f MeV/c2 \n", massB);		 
      printf("\tbeta      : %f \n", beta);
      printf("\tB-field   : %f T \n", Bfield);
      printf("\tslope     : %f MeV/mm \n", alpha * beta);
      printf("\tdet radius: %f mm \n", a);
      printf("\tG-coeff   : %f MeV \n", G);
      printf("=================================\n");

   }else{
      printf("........ fail.\n");
      isReaction = false;
   }
   file.close();
   
}


/*###########################################################
 * Plotting Function
###########################################################*/

void Monitors::PlotEZ(bool isRaw){
   padID++; cCanvas->cd(padID);

   if( isRaw ) {
      Draw2DHist(heCalVz);
      heCalVz->SetTitle("E vs Z | " + canvasTitle + " | " + rdtCutFile1);
      if( skipDetID.size() > 0 ){
         text.DrawLatex(0.15, 0.3, "skipped Detector:");
         for( int i = 0; i < (int) skipDetID.size(); i++){
            text.DrawLatex(0.15 + 0.1*i, 0.25, Form("%d", skipDetID[i]));
         }
      }

      text.DrawLatex(0.15, 0.8, Form("eCal > %.1f MeV", eCalCut));
      if( xGate < 1 ) text.DrawLatex(0.15, 0.75, Form("with |x-0.5|<%.4f", xGate/2.));

   }else{
      Draw2DHist(heCalVzGC);
   
      if( isCutFileOpen1 ) text.DrawLatex(0.15, 0.8, "with Recoil gate");
      if( xGate < 1 ) text.DrawLatex(0.15, 0.75, Form("with |x-0.5|<%.4f", xGate/2.));
      if( isTACGate ) text.DrawLatex(0.15, 0.7, Form("%d < TAC < %d", tacGate[0], tacGate[1]));
      if(isTimeGateOn)text.DrawLatex(0.15, 0.7, Form("%d < coinTime < %d", timeGate[0], timeGate[1]));

   }
   
   TFile * transfer = new TFile("transfer.root");
   TObjArray * gList  = NULL ;
   TObjArray * fxList = NULL ;
   int nGList = 0;
   int nFxList = 0;
   if( transfer->IsOpen() ) {
     gList  = (TObjArray *) transfer->FindObjectAny("gList");
     nGList = gList->GetLast() + 1;
     fxList = (TObjArray *) transfer->FindObjectAny("fxList");
     nFxList = fxList->GetLast() +1 ;
   }
   
   ///the constant thetaCM line
   if( transfer->IsOpen() ) gList->At(0)->Draw("same");
   ///the e-z line for excitation 
   if( transfer->IsOpen() ){
      for( int i = 0 ; i < nFxList ; i++){
         ((TF1*)fxList->At(i))->SetLineColor(6);
         fxList->At(i)->Draw("same");
      }
   }

}

void Monitors::PlotTDiff(bool isGated){
   padID++; cCanvas->cd(padID); 

   double yMax = 0;
   if( isGated ){
      yMax = htdiff->GetMaximum()*1.2;
      htdiff->GetYaxis()->SetRangeUser(0, yMax);
   }
   htdiff->Draw();
   if( isGated ){
      htdiffg->SetLineColor(2);
      htdiffg->Draw("same");
   }
   
   if( isCutFileOpen1 ) text.DrawLatex(0.15, 0.8, "with Recoil gate");
   if(isTimeGateOn)text.DrawLatex(0.15, 0.7, Form("%d < coinTime < %d", timeGate[0], timeGate[1])); 
   DrawBox(htdiff, timeGate[0], timeGate[1], kGreen, 0.2);
}

void Monitors::PlotRDT(int id, bool isRaw){
   padID++; cCanvas->cd(padID);

   if( isRaw ){
      Draw2DHist(hrdt2D[id]);
   }else{
      Draw2DHist(hrdt2Dg[id]);
   }
   if(isTimeGateOn)text.DrawLatex(0.15, 0.8, Form("%d < coinTime < %d", timeGate[0], timeGate[1])); 
   if( isTACGate ) text.DrawLatex(0.15, 0.7, Form("%d < TAC < %d", tacGate[0], tacGate[1]));
   if( isCutFileOpen1 && numCut1 > id ) {cutG = (TCutG *)cutList1->At(id) ; cutG->Draw("same");}
   if( isCutFileOpen2 && numCut2 > id ) {cutG = (TCutG *)cutList2->At(id) ; cutG->Draw("same");}

}

#endif // #ifdef Monitors_cxx
