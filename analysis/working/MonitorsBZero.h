/***********************************************************************
 *
 *  MonitorsBZero.h  --  TSelector for B = 0 T setup (h098 20Ne(a,p)23Na)
 *
 *  detectors:
 *    det 21     : stub PSD (e, xf, xn), long axis along X (-25 ~ +25 mm), at Y = -150 mm, Z = +25 mm
 *    elum[16]   : S1 annular, 16 rings, r = 24 - 48 mm, z = -50 mm
 *    rdt[8]     : recoil telescopes at z = +1000 mm (even = E, odd = dE)
 *
 *  trace mode (trace_runXXX.root, Process_RUN -BZ 2 ...) : as Monitors.h, the trace branch is bound
 *  to the SAME array and TBranch pointer (te -> e via b_Energy, txf -> xf, txn -> xn, trdt -> rdt,
 *  telum -> elum), so Process() is unchanged. the firmware values go to fw_* for the trace scale table.
 *
 *  all user settings / calibrations are in MonitorsBZero.C
 *
***********************************************************************/

#ifndef MonitorsBZero_h
#define MonitorsBZero_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCutG.h>

#include "mapping.h"

class MonitorsBZero : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   UShort_t        runID;
   Float_t         e[NARRAY];
   ULong64_t       e_t[NARRAY];
   Float_t         xf[NARRAY];
   ULong64_t       xf_t[NARRAY];
   Float_t         xn[NARRAY];
   ULong64_t       xn_t[NARRAY];
   Float_t         rdt[NRDT];
   ULong64_t       rdt_t[NRDT];
   Float_t         elum[NELUM];
   ULong64_t       elum_t[NELUM];

   // List of branches
   TBranch        *b_runID;   //!
   TBranch        *b_Energy;   //!
   TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XFTimestamp;   //!
   TBranch        *b_XN;   //!
   TBranch        *b_XNTimestamp;   //!
   TBranch        *b_RDT;   //!
   TBranch        *b_RDTTimestamp;   //!
   TBranch        *b_ELUM;   //!
   TBranch        *b_ELUMTimestamp;   //!

   bool isArrayExist;
   bool isRDTExist;
   bool isELUMExist;

   //---- trace analysis (EventBuilder fit : amplitude, T0 [sample = 10 ns] from trace start, rise time [sample])
   Float_t         te_t[NARRAY];
   Float_t         te_r[NARRAY];
   Float_t         txf_r[NARRAY];
   Float_t         txn_r[NARRAY];
   Float_t         trdt_t[NRDT];
   Float_t         trdt_r[NRDT];
   Float_t         telum_t[NELUM];
   Float_t         telum_r[NELUM];
   //---- firmware values in trace mode (e/xf/xn/rdt/elum hold the trace values)
   Float_t         fw_e[NARRAY];
   Float_t         fw_xf[NARRAY];
   Float_t         fw_xn[NARRAY];
   Float_t         fw_rdt[NRDT];
   Float_t         fw_elum[NELUM];

   TBranch        *b_TE_T;   //!
   TBranch        *b_TE_R;   //!
   TBranch        *b_TXF_R;   //!
   TBranch        *b_TXN_R;   //!
   TBranch        *b_TRDT_T;   //!
   TBranch        *b_TRDT_R;   //!
   TBranch        *b_TELUM_T;   //!
   TBranch        *b_TELUM_R;   //!
   TBranch        *b_FW_E;   //!
   TBranch        *b_FW_XF;   //!
   TBranch        *b_FW_XN;   //!
   TBranch        *b_FW_RDT;   //!
   TBranch        *b_FW_ELUM;   //!

   bool isArrayTrace;  /// e, xf, xn  <- te, txf, txn
   bool isRDTTrace;    /// rdt        <- trdt
   bool isELUMTrace;   /// elum       <- telum (EventBuilder trace analysis, NELUM > 0)

   ULong64_t startTime;
   ULong64_t endTime;

   MonitorsBZero(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~MonitorsBZero() { }
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

   bool FindStartEndTime(TTree * tree, TString branchName, int nCh);
   void DetectTrace(TTree * tree);

   ClassDef(MonitorsBZero,0);
};

#endif


#ifdef MonitorsBZero_cxx

void MonitorsBZero::Init(TTree *tree){

   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runID", &runID, &b_runID);

   DetectTrace(fChain);

   TBranch * br = (TBranch *) fChain->GetListOfBranches()->FindObject("e");
   if( br == NULL ){
      printf(" ++++++++ no Array (e/xf/xn).\n");
      isArrayExist = false;
   }else{
      isArrayExist = true;
      if( isArrayTrace ){
         fChain->SetBranchAddress("te",   e,     &b_Energy); // replace e  with te
         fChain->SetBranchAddress("txf",  xf,    &b_XF);     // replace xf with txf
         fChain->SetBranchAddress("txn",  xn,    &b_XN);     // replace xn with txn
         fChain->SetBranchAddress("te_t", te_t,  &b_TE_T);
         fChain->SetBranchAddress("te_r", te_r,  &b_TE_R);
         fChain->SetBranchAddress("txf_r", txf_r, &b_TXF_R);
         fChain->SetBranchAddress("txn_r", txn_r, &b_TXN_R);
         fChain->SetBranchAddress("e",    fw_e,  &b_FW_E);
         fChain->SetBranchAddress("xf",   fw_xf, &b_FW_XF);
         fChain->SetBranchAddress("xn",   fw_xn, &b_FW_XN);
         printf("************ using Trace in array (te, txf, txn) \n");
      }else{
         fChain->SetBranchAddress("e",    e,    &b_Energy);
         fChain->SetBranchAddress("xf",   xf,   &b_XF);
         fChain->SetBranchAddress("xn",   xn,   &b_XN);
      }
      fChain->SetBranchAddress("e_t",  e_t,  &b_EnergyTimestamp);
      fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
      fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);
   }

   br = (TBranch *) fChain->GetListOfBranches()->FindObject("rdt");
   if( br == NULL ){
      printf(" ++++++++ no Recoil.\n");
      isRDTExist = false;
   }else{
      isRDTExist = true;
      if( isRDTTrace ){
         fChain->SetBranchAddress("trdt",   rdt,    &b_RDT);   // replace rdt with trdt
         fChain->SetBranchAddress("trdt_t", trdt_t, &b_TRDT_T);
         fChain->SetBranchAddress("trdt_r", trdt_r, &b_TRDT_R);
         fChain->SetBranchAddress("rdt",    fw_rdt, &b_FW_RDT);
         printf("************ using Trace in recoil (trdt) \n");
      }else{
         fChain->SetBranchAddress("rdt"  , rdt,   &b_RDT);
      }
      fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);
   }

   br = (TBranch *) fChain->GetListOfBranches()->FindObject("elum");
   if( br == NULL ){
      printf(" ++++++++ no ELUM.\n");
      isELUMExist = false;
   }else{
      isELUMExist = true;
      if( isELUMTrace ){
         fChain->SetBranchAddress("telum",   elum,    &b_ELUM);   // replace elum with telum
         fChain->SetBranchAddress("telum_t", telum_t, &b_TELUM_T);
         fChain->SetBranchAddress("telum_r", telum_r, &b_TELUM_R);
         fChain->SetBranchAddress("elum",    fw_elum, &b_FW_ELUM);
         printf("************ using Trace in ELUM (telum) \n");
      }else{
         fChain->SetBranchAddress("elum",   elum,   &b_ELUM);
         if( isUseELUMTrace && fChain->GetListOfBranches()->FindObject("trdt") != NULL )
            printf(" ++++++++ trace file without ELUM trace (telum), sorted by an older EventBuilder, re-sort the run \n");
      }
      fChain->SetBranchAddress("elum_t", elum_t, &b_ELUMTimestamp);
   }

   //---- branches not present -> NaN, so nothing gets filled
   if( !isArrayExist ) for( int i = 0; i < NARRAY; i++) { e[i] = xf[i] = xn[i] = TMath::QuietNaN(); e_t[i] = xf_t[i] = xn_t[i] = 0; }
   if( !isRDTExist   ) for( int i = 0; i < NRDT;   i++) { rdt[i]  = TMath::QuietNaN(); rdt_t[i]  = 0; }
   if( !isELUMExist  ) for( int i = 0; i < NELUM;  i++) { elum[i] = TMath::QuietNaN(); elum_t[i] = 0; }

   startTime = 0;
   endTime = 0;

   printf("=================================== End of Branch Pointer Inititization. \n");
}

/// which trace branches are used ; called in Begin (before Init) and Init
void MonitorsBZero::DetectTrace(TTree * tree){
   TObjArray * list = tree->GetListOfBranches();
   isArrayTrace = isUseArrayTrace && list->FindObject("e")    && list->FindObject("te")
                                  && list->FindObject("txf")  && list->FindObject("txn");
   isRDTTrace   = isUseRDTTrace   && list->FindObject("rdt")  && list->FindObject("trdt");
   isELUMTrace  = isUseELUMTrace  && list->FindObject("elum") && list->FindObject("telum");
}

Bool_t MonitorsBZero::Notify(){
   return kTRUE;
}

/// scan first / last entries for the first non-zero timestamp in branchName[nCh]
bool MonitorsBZero::FindStartEndTime(TTree * tree, TString branchName, int nCh){

   if( nCh <= 0 ) return false;
   if( tree->GetListOfBranches()->FindObject(branchName) == NULL ) return false;

   ULong64_t timeStamp[200];
   Long64_t totEntries = tree->GetEntries();

   tree->SetBranchStatus("*", 0);
   tree->SetBranchStatus(branchName, 1);
   tree->SetBranchAddress(branchName, timeStamp);

   startTime = 0; endTime = 0;
   for( Long64_t ev = 0; ev < totEntries && startTime == 0; ev ++ ){
      tree->GetEntry(ev);
      for( int id = 0; id < nCh; id ++) if( timeStamp[id] > 0 ) { startTime = timeStamp[id]; break; }
   }
   for( Long64_t ev = totEntries - 1 ; ev >= 0 && endTime == 0;  ev -- ){
      tree->GetEntry(ev);
      for( int id = 0; id < nCh; id ++) if( timeStamp[id] > 0 ) { endTime = timeStamp[id]; break; }
   }
   tree->ResetBranchAddresses();
   tree->SetBranchStatus("*", 1);

   printf(">>>>>>> time range from %s : %llu - %llu \n", branchName.Data(), startTime, endTime);
   return ( startTime > 0 && endTime > startTime );
}

void MonitorsBZero::SlaveBegin(TTree * /*tree*/){
   TString option = GetOption();
}

void MonitorsBZero::SlaveTerminate(){
}

#endif // #ifdef MonitorsBZero_cxx
