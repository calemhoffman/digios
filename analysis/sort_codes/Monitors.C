#define Monitors_cxx

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
TH2F* hrdtg[4];
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

//time in sec
Float_t timeZero=0;
Float_t timeCurrent=0;
Float_t timeRef=0;

TH1F* htacE;
TH1F* hexC;
TH1F* hexR;
TH1I* htacArray[24];
TH1F* htac[4];//0 array-rdt, 1 elum-rdt

Float_t x[24],z[24];
Float_t xcal[24],ecal[24],xfcal[24],xncal[24],ecrr[24],ezero[10];
Int_t tacA[24];
Float_t z_array_pos[6] = {35.868,29.987,24.111,18.248,12.412,6.676};//in cm
Float_t z_off=9.64;//to the physical end of array from the target (-20 to si start)
Float_t xnCorr[24] = {0.907342,0.907342,0.976727,0.914866,1.021736,
		      0.887032,0.923250,0.953968,1.020180,0.918340,
		      0.983084,0.983084,0.997550,0.985319,0.959048,
		      1.008677,0.959601,1.066846,0.927771,0.985274,
		      0.921273,0.976498,1.062241,1.079507};
Float_t xfxneCorr[24][2] = {{29.091896,0.919262},{-0.744352,0.989133},{5.332432,1.046711},
			    {4.770114,1.073863},{-4.352881,0.901518},{-8.543459,0.995114},
			    {4.678705,1.015215},{3.955090,0.972769},{5.163730,0.998306},
			    {3.863314,  0.989275},{2.298429,  0.916884},{-17.435897,  0.897436},
			    {8.143049,  0.571533},{5.428828,  0.927071},{4.554876,  0.960028},
			    {4.423083,  0.967342},{1.436683,  1.026855},
			    {0.747782,  0.912706},
			    {6.048360, 0.914865},
			    {2.104460,  0.962689},
			    {1.011006,  1.034467},
			    {15.249334,  0.887257},
			    {14.071915,  1.095258},
			    {-2.256993,  0.896878}};
Float_t eCorr[24][2] = {{256.060637	,0.021569},
			{253.083810	,0.010404},
			{275.757609	,-0.012115},
			{266.830570	,0.028129},
			{247.134021	,0.013641},
			{244.161153	,0.002046},
			{263.857355	,0.042191},
			{250.108256	,-0.001003},
			{262.017938	,0.018393},
			{256.060637	,0.021569},
			{238.219726	,0.005357},
			{ 1.000000	,0.000000},
			{0	,0},
			{248.283604	,-0.026163},
			{242.321161	,-0.024002},
			{250.108256	,-0.001003},
			{262.017938	,-0.006414},
			{257.914882	,0.020954},
			{250.108256	,0.024985},
			{259.038694	,0.007406},
			{266.830570	,0.028129},
			{250.108256	,0.024985},
			{292.477670	,0.015062},
			{239.341772	-0.009266}};

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
    hecalVxcal[i] = new TH2F(Form("hecalVxcal%d",i),
			     Form("Cal PSD E vs. X (ch=%d);X (cm);E (MeV)",i),
			     500,-0.25,5.25,500,0,20);
  }//array loop
  hecalVz = new TH2F("hecalVz","E vs. Z;Z (cm);E (MeV)",350,-70,0,500,0,15);
  hecalVzR = new TH2F("hecalVzR","E vs. Z gated;Z (cm);E (MeV)",350,-70,0,500,0,15);


  //Recoils
  for (Int_t i=0;i<4;i++) {
    hrdt[i] = new TH2F(Form("hrdt%d",i),
		       Form("Raw Recoil DE vs Eres (ch=%d); Eres (channel); DE (channel)",i),
		       1000,0,10000,1000,0,4000);
    hrdtg[i] = new TH2F(Form("hrdtg%d",i),
			Form("Gated Recoil DE vs Eres (ch=%d); Eres (channel); DE (channel)",i),
			1000,0,10000,1000,0,4000);
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

  hexC = new TH1F("hexC","excitation spectrum",1500,-5,10);
  hexR = new TH1F("hexR","excitation spectrum with Recoil",1500,-5,10);

  for (Int_t i=0;i<24;i++) {
    htacArray[i] = new TH1I(Form("htacArray%d",i), Form("Array-RDT TAC for ch%d",i), 200, -100,100);
  }

  //EZERO
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
  graphRate->SetTitle("Raw Total Rate");
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


  //cCanvas  = new TCanvas("cCanvas","Running Plots",1250,1000);
  //cCanvas->Divide(1,3);cCanvas->cd(2);gPad->Divide(2,1);
  //cCanvas->cd(3);gPad->Divide(2,1);
  //cCanvas->Modified(); cCanvas->Update();
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
      xfcal[i] = xf[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      xncal[i] = xn[i]*xnCorr[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      ecal[i] = e[i]/eCorr[i][0]+eCorr[i][1];
      ecrr[i] = e[i]/eCorr[i][0]+eCorr[i][1];
      
      if (xf[i]>0 || xn[i]>0) {
	x[i] = 0.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;
      }
      
      if (xfcal[i]>0.5*e[i]) {
	xcal[i] = xfcal[i]/e[i];
      } else if (xncal[i]>=0.5*e[i]) {
	xcal[i] = 1.0 - xncal[i]/e[i];
      }
      
      //	z[i] = 5.0*(xcal[i]-0.5) + z_off + z_array_pos[i%6];//for downstream?
      z[i] = 5.0*(xcal[i]-0.5) - z_off - z_array_pos[i%6];
      
      //Array fill next
      hxfxn[i]->Fill(xf[i],xn[i]);
      if (x[i]>-1.1&&x[i]<1.1&&e[i]>100&&(xn[i]>0||xf[i]>0)) {
	heVx[i]->Fill(x[i],e[i]);
	hecalVxcal[i]->Fill(xcal[i]*5.0,ecrr[i]);
	hecalVz->Fill(z[i],ecrr[i]);
	for (Int_t ii=0;ii<4;ii++) hrdtg[ii]->Fill(rdt[ii+4],rdt[ii]);
      }
      
    }//array loop
    
    /* RECOILS */
    for (Int_t ii=0;ii<4;ii++) hrdt[ii]->Fill(rdt[ii+4],rdt[ii]);

    /* ELUM */
    for (Int_t i=0;i<16;i++) {
      helum[0]->Fill(elum[i],i);
      helum[1]->Fill(elum[i+16],i);
      for(Int_t j=0;j<4;j++){
	if(rdt[j]-elum_t[i]>-10&&rdt[j]-elum[i]<10)htacE->Fill(0.5+j);
      }
    }

    //EZERO
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
	//ungated excitation energy
	hexC->Fill((z[i*6+j]*0.14028+13.638-ecrr[i*6+j])*1.11-2.45);
	//CUTS
	if( isCutFileOpen){
	  for( int k = 0 ; k < numCut; k++ ){
	    cutG = (TCutG *)cutList->At(k) ;
	    if( cutG->IsInside(rdt[k+4], rdt[k]) ) { //CRH
	      for (Int_t kk=0;kk<4;kk++) { /****/
		tacA[i*6+j]= (int)(rdt_t[kk]-e_t[i*6+j]); /****/
		if(tacA[i*6+j]>-10&&tacA[i*6+j]<10) {
		  hexR->Fill((z[i*6+j]*0.14028+13.638-ecrr[i*6+j])*1.11-2.45);
		  hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
		} /***/
	      } /***/
	    }
	  }
	}
	if(e[i*6+j]>100){
	  for (Int_t k=0;k<4;k++) {
	    tacA[i*6+j]= (int)(rdt_t[k]-e_t[i*6+j]);
	    htacArray[i*6+j]->Fill(tacA[i*6+j]);
	    if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[k]>50){
	      htac[k]->Fill(j+0.5);
	    }
	  }
	}
	/*
	if(i==1&&e[i*6+j]>100){
	  tacA[i*6+j]= (int)(rdt_t[0]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[0]>50){
	    htac[0]->Fill(j+0.5);}
	}
	if(i==2&&e[i*6+j]>100){
	  tacA[i*6+j]= (int)(rdt_t[3]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[3]>50){
	    htac[3]->Fill(j+0.5);}
	}
	if(i==3&&e[i*6+j]>100){
	  tacA[i*6+j]= (int)(rdt_t[2]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[2]>50){
	    htac[2]->Fill(j+0.5);}
	}
	if(i==0&&e[i*6+j]>100){
	  tacA[i*6+j]= (int)(rdt_t[1]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-5&&tacA[i*6+j]<5&&rdt[1]>50){
	    htac[1]->Fill(j+0.5);}
	}
	*/
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
  //when recoils are available...
  cCanvas  = new TCanvas("cCanvas","Plots",1250,1000);
  cCanvas->Modified(); cCanvas->Update();
  cCanvas->cd(); cCanvas->Divide(1,2);
  cCanvas->cd(1); gPad->Divide(4,1);
  for (Int_t i=0;i<4;i++) {
    cCanvas->cd(1);gPad->cd(i+1); hrdtg[i]->Draw("");
    if( isCutFileOpen ) {
      cutG = (TCutG *)cutList->At(i);
      cutG->Draw("same");
    }
  }
  cCanvas->cd(2); gPad->Divide(2,1);
  cCanvas->cd(2);gPad->cd(1); hecalVz->Draw("colz");
  cCanvas->cd(2);gPad->cd(2); hecalVzR->Draw("colz");//hexC->Draw();
  cCanvas->cd();
  
  if (ProcessedEntries>=NUMSORT)
    printf("Sorted only %llu\n",NUMSORT);
  // printf("Total time for sort: %3.1f\n",StpWatch.RealTime());
  //printf("Which is a rate of: %3.1f k/s\n",(Float_t)ProcessedEntries/StpWatch.RealTime()/1000.);
  StpWatch.Start(kFALSE);
}
