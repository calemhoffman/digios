#define inflightSort_cxx

#include "inflightSort.h"
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>

#define NUMPRINT 20 //>0
ULong64_t NUMSORT=100000000;

#define MAXNUMHITS 200 //Highest multiplicity
#define M 100 //M value for energy filter from digi setting

ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; //Progress bar
Int_t CrapPrint=0;

//Arrays for mapping things...
//With new mapping...1.15..starts with 1010 now...
Int_t idDetMap[160] = {400,300,301,-1,-1,-1,-1,-1,-1,-1,//RF TIMING STUFF, DE Exit, E Exit
		       -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,//Empty
		       -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,//Empty
		       -1,302,-1,303,-1,304,-1,-1,-1,-1};//DE ATSCAT, E ATSCAT, ETOT ATSCAT
		       
Int_t idKindMap[160] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

TStopwatch StpWatch;

TFile *oFile;
TTree *infl_tree;

TH1F *hEvents;

TCanvas *cc0;

//PSD struct
typedef struct {
  Float_t Energy[100];
  Float_t XF[100];
  Float_t XN[100];
  Float_t Ring[100];
  Float_t RDT[100];
  Float_t TAC[100];
  Float_t ELUM[32];
  Float_t EZERO[10];//0,1 - DE-E exit, 2,3 - DE-E atscat, 4 - ETOT
// 2,3 - DEX,EX

  ULong64_t EnergyTimestamp[100];
  ULong64_t XFTimestamp[100];
  ULong64_t XNTimestamp[100];
  ULong64_t RingTimestamp[100];
  ULong64_t RDTTimestamp[100];
  ULong64_t TACTimestamp[100];
  ULong64_t ELUMTimestamp[32];
  ULong64_t EZEROTimestamp[10];

} PSD;

PSD psd; 

void inflightSort::Begin(TTree * tree)
{
   
  TString option = GetOption();
  NumEntries = tree->GetEntries();

  hEvents = new TH1F("hEvents","Number of events; Events;",NumEntries*1.2,0,NumEntries*1.2);

  oFile = new TFile("infl.root","RECREATE");

  infl_tree = new TTree("infl_tree","PSD Tree");
  infl_tree->Branch("e",psd.Energy,"Energy[100]/F");
  infl_tree->Branch("e_t",psd.EnergyTimestamp,"EnergyTimestamp[100]/l");
  
  infl_tree->Branch("xf",psd.XF,"XF[100]/F");
  infl_tree->Branch("xf_t",psd.XFTimestamp,"XFTimestamp[100]/l");
 
  infl_tree->Branch("xn",psd.XN,"XN[100]/F");
  infl_tree->Branch("xn_t",psd.XNTimestamp,"XNTimestamp[100]/l"); 

  infl_tree->Branch("rdt",psd.RDT,"RDT[100]/F");
  infl_tree->Branch("rdt_t",psd.RDTTimestamp,"RDTTimestamp[100]/l"); 

  infl_tree->Branch("tac",psd.TAC,"TAC[100]/F");
  infl_tree->Branch("tac_t",psd.TACTimestamp,"TACTimestamp[100]/l"); 
  
  infl_tree->Branch("elum",psd.ELUM,"ELUM[32]/F");
  infl_tree->Branch("elum_t",psd.ELUMTimestamp,"ELUMTimestamp[32]/l"); 

  infl_tree->Branch("ezero",psd.EZERO,"EZERO[10]/F");
  infl_tree->Branch("ezero_t",psd.EZEROTimestamp,"EZEROTimestamp[10]/l"); 
 
  StpWatch.Start();
}

void inflightSort::SlaveBegin(TTree * /*tree*/)
{
  
  TString option = GetOption();
  
}

Bool_t inflightSort::Process(Long64_t entry)
{ 
  ProcessedEntries++;
  if (ProcessedEntries<NUMSORT) {
    hEvents->Fill(ProcessedEntries);

    if (ProcessedEntries>NumEntries*Frac-1) {
      printf(" %3.0f%% (%llu/%llu Mil) processed in %6.1f seconds\n",Frac*100,ProcessedEntries/1000000,NumEntries/1000000,StpWatch.RealTime());
      StpWatch.Start(kFALSE);
      Frac+=0.1;
    }

    //Zero struct
    for (Int_t i=0;i<100;i++) {//num dets
      psd.Energy[i]=TMath::QuietNaN();
      psd.XF[i]=TMath::QuietNaN();
      psd.XN[i]=TMath::QuietNaN();
      psd.Ring[i]=TMath::QuietNaN();
      psd.RDT[i]=TMath::QuietNaN();
      psd.TAC[i]=TMath::QuietNaN();
      if (i<32) psd.ELUM[i]=TMath::QuietNaN();
      if (i<10) psd.EZERO[i]=TMath::QuietNaN();
    
      psd.EnergyTimestamp[i]=TMath::QuietNaN();
      psd.XFTimestamp[i]=TMath::QuietNaN();
      psd.XNTimestamp[i]=TMath::QuietNaN();
      psd.RingTimestamp[i]=TMath::QuietNaN();
      psd.RDTTimestamp[i]=TMath::QuietNaN();
      psd.TACTimestamp[i]=TMath::QuietNaN();
      if (i<32) psd.ELUMTimestamp[i]=TMath::QuietNaN();
      if (i<10) psd.EZEROTimestamp[i]=TMath::QuietNaN();	    
    }

    
    //Pull needed entries
    b_NumHits->GetEntry(entry);
    b_id->GetEntry(entry);
    b_pre_rise_energy->GetEntry(entry);
    b_post_rise_energy->GetEntry(entry);
    //   b_base_sample->GetEntry(entry);
    //    b_baseline->GetEntry(entry);
    b_event_timestamp->GetEntry(entry);

    //ID PSD Channels
 
    Int_t idKind = -1;
    Int_t idDet=-1; // Detector number
    Int_t idConst=1010; //Temp value to get idDet
    
    /* -- Loop over NumHits -- */
    for (Int_t i=0;i<NumHits;i++) {
      Int_t psd8Chan = id[i]%10;     
      Int_t idTemp = id[i] - idConst;
      idDet = idDetMap[idTemp];
      idKind = idKindMap[idTemp];
 
      //TAC & RF TIMING
      /***********************************************************************/
      if ((id[i]>1000&&id[i]<2000)&&(idDet>=400&&idDet<=450)) { //RF TIMING SWITCH
	if (ProcessedEntries<NUMPRINT)
	  printf("RF id %i, idDet %i\n",id[i],idDet);
	
	Int_t tacTemp = idDet-400;
	psd.TAC[tacTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
	psd.TACTimestamp[tacTemp] = event_timestamp[i];
      }
       

      //EZERO
      /************************************************************************/
      if ((id[i]>1000&&id[i]<2000)&&(idDet>=300&&idDet<310)) {
	Int_t ezeroTemp = idDet - 300;
	if (ezeroTemp<10) {
	  psd.EZERO[ezeroTemp] = ((float)(post_rise_energy[i])
				-(float)(pre_rise_energy[i]))/M;
	  psd.EZEROTimestamp[ezeroTemp] = event_timestamp[i];
	}
      }//end EZERO
    } // End NumHits Loop
    
    infl_tree->Fill();
  }  
  return kTRUE;
}

void inflightSort::SlaveTerminate()
{

}

void inflightSort::Terminate()
{
  if (ProcessedEntries>=NUMSORT)
    printf("Sorted only %llu\n",NUMSORT);
  infl_tree->Write();
  oFile->Close();
  
  //  cc0 = new TCanvas("cc0","cc0",800,600);
  // cc0->Clear(); hEvents->Draw();  
  
  printf("Total processed entries : %3.1f k\n",ProcessedEntries/1000.0);
  printf("Total time for sort: %3.1f\n",StpWatch.RealTime());
  printf("Rate for sort: %3.1f k/s\n",(Float_t)ProcessedEntries/StpWatch.RealTime()/1000.0);
  StpWatch.Start(kFALSE);
}
