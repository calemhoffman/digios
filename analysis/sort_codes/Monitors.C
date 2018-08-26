#define Monitors_cxx
//test
#include "Monitors.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCutG.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TString.h>
#include <TSystem.h>
#include <TObjArray.h>


#define NUMPRINT 20 //>0
ULong64_t NUMSORT=100000000;
ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; //Progress bar
TStopwatch StpWatch;

TCanvas *cCanvas;
TCanvas *cEX;
TGraph *graphRate;// = new TGraph(n,x,y);
Int_t n=1;

TString cutName("cut1");
TCutG* cutG; //!
TObjArray * cutList;
TString cutTag;
Bool_t isCutFileOpen;
int numCut;
vector<int> countFromCut;
TGraph ** graphRateCut; //!
TMultiGraph * rateGraph; //!


TH2F* hxfxn[24];
TH2F* heVx[24]; 
TH2F* hecalVxcal[24];
TH2F* hecalVz;
TH2F* hecalVzR;
TH2F* hrdt[4]; 
TH2F* helum[2];
TH2F* he0dee;//ezero
TH2F* he0det;
TH2F* he0et;
TH1F* h0detet;
TH1F* h0dettact;
TH1F* h0ettact;
TH1F* h0de;
TH1F* h0e;
TH1F* h0tac;

//INFL001
TH2F *hExAt;
TH2F *hExDeE,*hAtDeE;
TH2F *hExDeT,*hAtDeT;
TH2F *hExET, *hAtET;
TH2F *hAtEtotT;

//applied cut
TH2F *hExDeEg,*hAtDeEg;
TH2F *hExDeTg,*hAtDeTg;
TH2F *hExETg, *hAtETg;
TH2F *hAtEtotTg;

//calibrated, no cut
TH2F *hExDeEc,*hAtDeEc;
TH2F *hExDeTc;
TH2F *hExETc;

//calibrated, and cut!!
TH2F *hExDeEgc,*hAtDeEgc;
TH2F *hExDeTgc;
TH2F *hExETgc;

//time in sec
Float_t timeZero=0;
Float_t timeCurrent=0;
Float_t timeRef=0;

TH1F* htacE;
//TH1F* hex;
TH1F* hexR;
TH1I* htacArray[24];
TH1F* htac[4];//0 array-rdt, 1 elum-rdt

Float_t x[24],z[24];
Float_t xcal[24],ecal[24],xfcal[24],xncal[24],ecrr[24],ezero[10];
Int_t tacA[24];
Float_t z_array_pos[6] = {35.868,29.987,24.111,18.248,12.412,6.676};//in cm
Float_t z_off=60.0;//to the physical end of array from the target (-20 to si start)
Float_t xcoef[24] = {0.977877, 0.988848, 0.977536, 0.940756, 1.07479, 0.948377,
		     0.938322, 0.987471, 0.98292, 0.991469, 0.996435, 0.0,
		     1.04777, 1.01165, 0.969064, 1.01138, 0.944809, 1.05395,
		     0.972853, 0.969341, 0.961707, 0.989349, 1.01909, 1.12121};
Float_t ecoef[24] = {0.87125, 0.9734, 1.05417, 1.11849, 0.87099, 0.967404,
		     1.00563, 0.976017, 0.927414, 0.959165, 0.916457, 0.0,
		     0.855793, 0.905978, 0.928997, 0.960064, 1.07153, 0.915901,
		     0.881094, 0.956482, 1.01326, 0.887234, 1.09654, 0.869338};
Float_t kcoef[24] = {0.003444976, 0.003457902, 0.003132563, 0.003341067, 0.003563254, 0.003654822, 
		     0.003366944, 0.003534284, 0.003526171, 0.003309869, 0.003735409, 0.0,
		     0.003791968, 0.003526171, 0.00364557, 0.003384254, 0.003353712, 0.003434705,
		     0.003505249, 0.003378299, 0.003289549, 0.003448586, 0.002994541, 0.003576529};
Float_t bcoef[24] = {0.453923445, 0.417753264, 0.501465942, 0.538881671, 0.449696876, 0.546873096,
		     0.436930294, 0.433740451, 0.448512397, 0.595999138, 0.389035019, 0.0,
		     0.369876234, 0.470022039, 0.533858228, 0.589323149, 0.387687336, 0.52972093,
		     0.494175262, 0.41855132, 0.473385494, 0.584180512, 0.49499818, 0.584009314};

Float_t ezero_cal[5][2]={{0.0367,4.6086},{0.0367,4.6086},{0.01,0},{0.01,0},{0.01,0}};
Float_t tac_slope=0.07048;//ns/ch
Float_t tac_offset=100.0;//122.0;

Float_t tempTime=-1000;
Long64_t tempTimeLong=10001;

void Monitors::Begin(TTree *tree)
{
  TString option = GetOption();
  NumEntries = tree->GetEntries();

  //Generate all of the histograms needed for drawing later on

  for (Int_t i=0;i<24;i++) {//array loop
    hxfxn[i] = new TH2F(Form("hxfxn%d",i),
			Form("Raw PSD XF vs. XN (ch=%d);XF (channel);XN (channel)",i),
			500,0,4000,500,0,4000);
    heVx[i] = new TH2F(Form("heVx%d",i),
		       Form("Raw PSD E vs. X (ch=%d);X (channel);E (channel)",i),
		       500,-0.1,1.1,500,0,4000);
  }//array loop
  hecalVz = new TH2F("hecalVz","E vs. Z;Z (cm);E (MeV)",450,60,105,500,0,30);
  hecalVzR = new TH2F("hecalVzR","E vs. Z gated;Z (cm);E (MeV)",450,60,105,500,0,30);


  //Recoils
  for (Int_t i=0;i<4;i++) {
    hrdt[i] = new TH2F(Form("hrdt%d",i),
		       Form("Raw Recoil DE vs Eres (ch=%d); Eres (channel); DE (channel)",i),
		       500,0,8000,500,0,8000);
  }

  //ELUM
  helum[0] = new TH2F("helum0","Elum Ring Energies; E (channels); Ring Number",
		      500,100,4000,16,0,16);
  helum[1] = new TH2F("helum1","Elum Wedge Energies; E (channels); Ring Number",
		      500,100,4000,16,0,16);

  //TAC
  htac[0] = new TH1F("htac0","Array-RDT0 TAC; DT [clock ticks]; Counts",6,0,6);
  htac[1] = new TH1F("htac1","Array-RDT1 TAC; DT [clock ticks]; Counts",6,0,6);
  htac[2] = new TH1F("htac2","Array-RDT2 TAC; DT [clock ticks]; Counts",6,0,6);
  htac[3] = new TH1F("htac3","Array-RDT3 TAC; DT [clock ticks]; Counts",6,0,6);

  htacE = new TH1F("htacE","Elum-RDT TAC; DT [clock ticks]; Counts",4,0,4);

  // hex = new TH1F("hex","excitation spectrum",200,-2,10);
  hexR = new TH1F("hexR","excitation spectrum with Recoil",500,-2,10);

  for (Int_t i=0;i<24;i++) {
    htacArray[i] = new TH1I(Form("htacArray%d",i), Form("Array-RDT TAC for ch%d",i), 200, -100,100);
  }

  //EZERO
  //INFL001
  hExAt = new TH2F("hExAt","Raw RF (0), Exit (1,2) and ATSCAT (3,4,5) Data; Detector ; Energy [ch]",2000,-8000,8000,6,0,6);//Raw Data
  hExDeE = new TH2F("hExDeE","Exit DE-E; E [ch]; DE [ch]",2000,0,8000,2000,0,2000);//EXIT
  hExDeT = new TH2F("hExDeT","Exit DE-RF; RF [ch]; DE [ch]",500,2000,3500,2000,0,2000);//
  hExET = new TH2F("hExET","Exit E-RF; RF [ch]; E [ch]",500,2000,3500,2000,0,8000);//
  hAtDeE = new TH2F("hAtDeE","ATSCAT DE-E; E [ch]; DE [ch]",1000,0,12000,1000,0,12000);//ATSCAT
  hAtDeT = new TH2F("hAtDeT","ATSCAT DE-RF; RF [ch]; DE [ch]",1000,2000,3500,1000,0,12000);//
  hAtET = new TH2F("hAtET","ATSCAT E-RF; RF [ch]; E [ch]",500,2000,3500,500,0,8000);//
  hAtEtotT = new TH2F("hAtEtotT","ATSCAT ETOT-RF; RF [ch]; ETOT [ch]",500,2000,3500,1000,0,12000);//

  //gated
  hExDeEg = new TH2F("hExDeEg","Exit DE-E (cut); E [ch]; DE [ch]",500,0,8000,500,0,8000);//EXIT
  hExDeTg = new TH2F("hExDeTg","Exit DE-RF (cut); RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
  hExETg = new TH2F("hExETg","Exit E-RF (cut); RF [ch]; E [ch]",500,2000,3500,500,0,8000);//
  hAtDeEg = new TH2F("hAtDeEg","ATSCAT DE-E (cut); E [ch]; DE [ch]",500,0,8000,500,0,8000);//ATSCAT
  hAtDeTg = new TH2F("hAtDeTg","ATSCAT DE-RF (cut); RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
  hAtETg = new TH2F("hAtETg","ATSCAT E-RF (cut); RF [ch]; E [ch]",500,2000,3500,500,0,8000);//
  hAtEtotTg = new TH2F("hAtEtotTg","ATSCAT ETOT-RF (cut); RF [ch]; ETOT [ch]",500,2000,3500,500,0,8000);//

  //calibrated
  hExDeEc = new TH2F("hExDeEc","Exit DE-E (cal); E [MeV]; DE [MeV]",2000,0,200,2000,0,80);//EXIT
  hExDeTc = new TH2F("hExDeTc","Exit DE-RF (cal); TAC [ns]; DE [MeV]",4000,0,1000,2000,0,80);//EXIT
  hExETc = new TH2F("hExETc","Exit E-RF (cal); TAC [ns]; E [MeV]",150,80,200,1000,0,200);//EXIT
  hAtDeEc = new TH2F("hAtDeEc","ATSCAT DE-E (cal); E [MeV]; DE [MeV]",500,0,200,500,0,200);//ATSCAT
  //calibrated & cut
  hExDeEgc = new TH2F("hExDeEgc","Exit DE-E (cal & cut); E [MeV]; DE [MeV]",2000,0,200,2000,0,80);//EXIT
  hExDeTgc = new TH2F("hExDeTgc","Exit DE-RF (cal & cut); TAC [ns]; DE [MeV]",4000,100,180,2000,0,80);//EXIT
  hExETgc = new TH2F("hExETgc","Exit E-RF (cal & cut); TAC [ns]; E [MeV]",150,80,200,1000,0,200);//EXIT
  hAtDeEgc = new TH2F("hAtDeEgc","ATSCAT DE-E (cal & cut); E [MeV]; DE [MeV]",500,0,200,500,0,200);//ATSCAT

  he0dee = new TH2F("he0dee","EZERO DE-E; E [ch]; DE [ch]",500,0,8000,500,0,8000);//ezero
  he0det = new TH2F("he0det","EZERO DE-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
  he0et = new TH2F("he0et","EZERO E-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
  h0detet = new TH1F("h0detet","EZERO DE Time - E Time; DET-ET [ch]",500,-250,250);//
  h0dettact = new TH1F("h0dettact","EZERO DE Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
  h0ettact = new TH1F("h0ettact","EZERO E Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
  h0de = new TH1F("h0de","EZERO DE ; DE [ch]",500,50,4050);//
  h0e = new TH1F("h0e","EZERO - E; E [ch]",500,50,4050);//
  h0tac = new TH1F("h0tac","EZERO RF; RF [ch]",500,50,4050);//

  rateGraph = new TMultiGraph();
  graphRate = new TGraph();
  graphRate->SetTitle("Total Rate [pps]");
  graphRate->SetMarkerColor(4);
  graphRate->SetMarkerStyle(20);
  graphRate->SetMarkerSize(1);

  //rateGraph->Add(graphRate);
  rateGraph->SetTitle("Instantaneous Beam rate [pps]; Delta Time [sec]; Rate [pps]");
	
  //Get any cuts;
  //TFile *fi=new TFile("cut.root");		   // open file
  TFile * fCut = new TFile("cutsFile.root");		   // open file
  isCutFileOpen = fCut->IsOpen(); 
  numCut = 0 ;
  if( isCutFileOpen ){	  
	cutList = (TObjArray *) fCut->FindObjectAny("cutList");
	numCut = cutList->GetEntries();
	printf("=========== found %d cutG \n", numCut);
	
	cutG = new TCutG();
	graphRateCut = new TGraph * [numCut];
	for(int i = 0; i < numCut ; i++){
		printf(" cut name : %s \n", cutList->At(i)->GetName());
		countFromCut.push_back(0);	
			
		graphRateCut[i] = new TGraph();
		graphRateCut[i]->SetMarkerColor(i+1);	 
		graphRateCut[i]->SetMarkerStyle(20+i);
		graphRateCut[i]->SetMarkerSize(1);
		rateGraph->Add(graphRateCut[i]);
	}
  }
  
  printf("======== number of cuts found : %d \n", numCut);


  cCanvas  = new TCanvas("cCanvas","Running Plots",1250,1000);
  cCanvas->Divide(1,3);cCanvas->cd(2);gPad->Divide(2,1);
  cCanvas->cd(3);gPad->Divide(2,1);
  cCanvas->Modified(); cCanvas->Update();
  StpWatch.Start();
}

void Monitors::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();

}

Bool_t Monitors::Process(Long64_t entry)
{
  ProcessedEntries++;
  if (ProcessedEntries<NUMSORT) {
    
    if (ProcessedEntries>NumEntries*Frac-1) {
      printf(" %3.0f%% (%llu/%llu k) processed in %6.1f seconds\n",
	     Frac*100,ProcessedEntries/1000,NumEntries/1000,StpWatch.RealTime());
      StpWatch.Start(kFALSE);
      Frac+=0.1;
    }

    b_Energy->GetEntry(entry);
    b_XF->GetEntry(entry);
    b_XN->GetEntry(entry);
    b_RDT->GetEntry(entry);
    b_TAC->GetEntry(entry);
    b_ELUM->GetEntry(entry);
    b_EZERO->GetEntry(entry);
    b_EnergyTimestamp->GetEntry(entry);
    b_RDTTimestamp->GetEntry(entry);
    b_TACTimestamp->GetEntry(entry);
    b_ELUMTimestamp->GetEntry(entry);
    b_EZEROTimestamp->GetEntry(entry);

    //Do calculations and fill histograms
    //Array calcs first
    for (Int_t i=0;i<24;i++) {
      //Calibrations go here
      xfcal[i] = xf[i];
      xncal[i] = xn[i]*xcoef[i];
      ecal[i] = e[i]/ecoef[i];
      ecrr[i] = e[i]*kcoef[i]+bcoef[i];

      if (xf[i]>0 || xn[i]>0) {
	x[i] = 00.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;//for downstream?
	//x[i] = 0.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;

	if (xfcal[i]>xncal[i]) {
	  xcal[i] = xfcal[i]/ecal[i];
	} else if (xncal[i]>=xfcal[i]) {
	  xcal[i] = 1.0 - xncal[i]/ecal[i];
	}
	z[i] = 5.0*(xcal[i]-0.5) + z_off + z_array_pos[i%6];//for downstream?
	//z[i] = 5.0*(xcal[i]-0.5) - z_off - z_array_pos[i%6];
      }

      //Array fill next
      hxfxn[i]->Fill(xf[i],xn[i]);
      if (x[i]>-1.1&&x[i]<1.1&&e[i]>100&&(xn[i]>0||xf[i]>0)) {
	heVx[i]->Fill(x[i],e[i]);
	hecalVz->Fill(z[i],ecal[i]);
      }

    }//array loop

    /* RECOILS */
    for (Int_t i=0;i<4;i++) hrdt[i]->Fill(rdt[i+4],rdt[i]);

    /* ELUM */
    for (Int_t i=0;i<16;i++) {
      helum[0]->Fill(elum[i],i);
      helum[1]->Fill(elum[i+16],i);
      for(Int_t j=0;j<4;j++){
	if(rdt[j]-elum_t[i]>-10&&rdt[j]-elum[i]<10)htacE->Fill(0.5+j);
      }
    }

    //EZERO
    hExAt->Fill(tac[0],0);
    for (Int_t i=0;i<5;i++) hExAt->Fill(ezero[i],i+1);
    
    hExDeE->Fill(TMath::Abs(ezero[1]),TMath::Abs(ezero[0]));
    hExDeT->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[0]));
    hExET->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[1]));
    hAtDeE->Fill(TMath::Abs(ezero[3]),TMath::Abs(ezero[2]));
    hAtDeT->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[2]));
    hAtET->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[3]));
    hAtEtotT->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[4]));

    //calibrated
    Float_t temp[5]={0,0,0,0,0};
    for (Int_t i=0;i<5;i++) temp[i] = (Float_t)(TMath::Abs(ezero[i]))*ezero_cal[i][0]+ezero_cal[i][1];
    Float_t tempT = (2119.1 - TMath::Abs(tac[0]))*tac_slope+tac_offset+82.5;

    hExDeEc->Fill(temp[1],temp[0]);
    hExDeTc->Fill(tempT,temp[0]);
    hExETc->Fill(tempT,temp[1]);
    hAtDeEc->Fill(temp[3],temp[2]);
    
	if( isCutFileOpen){
		for( int i = 0 ; i < numCut; i++ ){
			cutG = (TCutG *)cutList->At(i) ;
			if( cutG->IsInside(temp[1], temp[0]) || cutG->IsInside(TMath::Abs(ezero[3]),TMath::Abs(ezero[2]) ) ) { //CRH
				countFromCut[i] += 1;
				//printf("count %d : %d \n", i, countFromCut[i]);
				hExDeEgc->Fill(temp[1],temp[0]);
				hExDeTgc->Fill(tempT,temp[0]);
				hExETgc->Fill(tempT,temp[1]);
				hAtDeEgc->Fill(temp[3],temp[2]);
 
				hExDeEg->Fill(TMath::Abs(ezero[1]),TMath::Abs(ezero[0]));
				hExDeTg->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[0]));
				hExETg->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[1]));
				hAtDeEg->Fill(TMath::Abs(ezero[3]),TMath::Abs(ezero[2]));
				hAtDeTg->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[2]));
				hAtETg->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[3]));
				hAtEtotTg->Fill(TMath::Abs(tac[0]),TMath::Abs(ezero[4]));
			}
		}
	}
	
	//Time
	if (TMath::Abs(ezero[0])>50 ){//CRH
	  timeCurrent = (Float_t) ezero_t[0]/1e8;
	} else if (TMath::Abs(ezero[2])>50 ){//CRH
	  timeCurrent = (Float_t) ezero_t[2]/1e8;
	}
	//initial timeZero and timeRef
	if (timeZero==0) {
	  timeZero = timeCurrent;
	  timeRef  = timeCurrent;
	}
	
	timeCurrent = timeCurrent - timeZero;
	Float_t timeDelta = timeCurrent - timeRef;
	
	if (ProcessedEntries>1 && ProcessedEntries%1000==0) {      
	  printf("Total Run Time: %10.1f sec, dT : %5.3f sec, Total Evt Rate: %10.0f pps \n",
		 timeCurrent, timeDelta,
		 ProcessedEntries/timeCurrent);
	  
	  if( timeCurrent > 0 ){
	    graphRate->SetPoint(n-1, timeCurrent, ProcessedEntries/timeDelta);
	    
	    if( isCutFileOpen){
	      for( int i = 0 ; i < numCut; i++ ){
		printf("cut%d : %5d, %6.3f pps \n", i+1, countFromCut[i], countFromCut[i]/timeDelta);			 
		graphRateCut[i]->SetPoint(n-1, timeCurrent, countFromCut[i]/timeDelta);
		
		countFromCut[i] = 0; // reset count for semi-Instantaneous beam rate;
		timeRef = timeCurrent;
	      }
	    }
	    
	    cCanvas->cd(1);
	    rateGraph->Draw("AP");
	    rateGraph->GetXaxis()->SetRangeUser(0, timeCurrent);
	    gPad->Modified(); cCanvas->Modified(); cCanvas->Update();
	    
	    n++;
	  }
	  
	}
	
	
	if (ProcessedEntries>1 && ProcessedEntries%5000==0) {
	  cCanvas->cd(2); gPad->cd(1); hAtDeE->Draw("colz");
	  cCanvas->cd(3); gPad->cd(1); hExDeEc->Draw("colz");//CRH
	  
	  if( isCutFileOpen){
	    for( int i = 0 ; i < numCut; i++ ){
	      cCanvas->cd(2); gPad->cd(1);	 cutList->At(i) ->Draw("same");
	      cCanvas->cd(3); gPad->cd(1);	 cutList->At(i) ->Draw("same");
	    }
	  }
	  
	  cCanvas->cd(2); gPad->cd(2); hAtDeT->Draw("colz");
	  cCanvas->cd(3); gPad->cd(2); hExDeTgc->Draw("colz");//CRH
	  gPad->Modified(); cCanvas->Modified(); cCanvas->Update();
	  gSystem->ProcessEvents();
	}
	
    
    he0dee->Fill(ezero[1],ezero[0]);
    he0det->Fill(TMath::Abs(tac[0]),ezero[0]);
    he0et->Fill(TMath::Abs(tac[0]),ezero[1]);
    h0detet->Fill((int)(ezero_t[0]-ezero_t[1]));
    h0dettact->Fill((int)(ezero_t[0]-tac_t[0]));
    h0ettact->Fill((int)(ezero_t[1]-tac_t[0]));
    h0de->Fill(ezero[0]);
    h0e->Fill(ezero[1]);
    h0tac->Fill(TMath::Abs(tac[0]));
    
    //TACs
    for (Int_t i=0;i<4;i++){
      for(Int_t j=0;j<6;j++){
	if(i==0&&e[i*6+j]>100){
	  tacA[i*6+j]= (int)(rdt_t[0]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[0]>5000){hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.14028+13.638-ecrr[i*6+j])*1.1068-0.3137);
	    htac[0]->Fill(j+0.5);}
	}
	  if(i==1&&e[i*6+j]>100){
	    	  tacA[i*6+j]= (int)(rdt_t[3]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[3]>5300){hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.14028+13.638-ecrr[i*6+j])*1.1068-0.3137);
	    htac[3]->Fill(j+0.5);}
	  }
	  if(i==2&&e[i*6+j]>100){
	    	  tacA[i*6+j]= (int)(rdt_t[2]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[2]>5000){hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.14028+13.638-ecrr[i*6+j])*1.1068-0.3137);
	  htac[2]->Fill(j+0.5);}
	  }
	  if(i==3&&e[i*6+j]>100){
	    	  tacA[i*6+j]= (int)(rdt_t[1]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[1]>5300){hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.14028+13.638-ecrr[i*6+j])*1.1068-0.3137);
	    htac[1]->Fill(j+0.5);}
	  }
      }
    }

   
  }
   return kTRUE;
}

void Monitors::SlaveTerminate()
{

}

void Monitors::Terminate()
{
  cCanvas->cd(1);
  if( isCutFileOpen) {
	for (int i = 0 ; i < numCut ; i++){
		graphRateCut[i]->Fit("pol0");
	}
  }
 

  if (ProcessedEntries>=NUMSORT)
    printf("Sorted only %llu\n",NUMSORT);
  // printf("Total time for sort: %3.1f\n",StpWatch.RealTime());
  //printf("Which is a rate of: %3.1f k/s\n",(Float_t)ProcessedEntries/StpWatch.RealTime()/1000.);

  StpWatch.Start(kFALSE);
}
