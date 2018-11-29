#define Cali_e_trace_Proof_cxx

//##########################################################
//      Cali e, xf, xn, coinTime with gate
//      the gate should be modified as needed. 
//##########################################################

#include "Cali_e_trace_Proof.h"
#include <TH2.h>
#include <TStyle.h>

TString saveFileName = "A_sortedTrace.root";

void Cali_e_trace_Proof::Begin(TTree * /*tree*/)
{
   TString option = GetOption();  
   printf( "=========================================================================== \n");
   printf( "========================== Cali_e_trace_Proof.h ================================= \n");
   printf( "=================== Make a new tree with all calibrations ================= \n");
   printf( "=========================================================================== \n");

}

void Cali_e_trace_Proof::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();
   
   //create tree in slave
   proofFile = new TProofOutputFile(saveFileName);
   saveFile = proofFile->OpenFile("RECREATE");
   
   newTree = new TTree("tree","PSD Tree w/ trace");
   newTree->SetDirectory(saveFile);
   newTree->AutoSave();
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   newTree->Branch("run",&run,"run/I"); 
   
   newTree->Branch("e" ,  eC, "eC[24]/F");
   newTree->Branch("x" ,   x, "x[24]/F");
   newTree->Branch("z" ,   z, "z[24]/F");
   newTree->Branch("detID", &det, "det/I");
   newTree->Branch("hitID", &hitID, "hitID/I");
   newTree->Branch("multiHit", &multiHit, "multiHit/I");
   
   newTree->Branch("Ex", &Ex, "Ex/F");
   newTree->Branch("thetaCM", &thetaCM, "thetaCM/F");
   newTree->Branch("thetaLab", &thetaLab, "thetaLab/F");
   
   newTree->Branch("e_t", eC_t, "e_t[24]/l");
   
   newTree->Branch("rdt", rdtC, "rdtC[8]/F");
   newTree->Branch("rdt_t", rdtC_t, "rdtC_t[8]/l");
   
   newTree->Branch("coin_t", &coin_t, "coin_t/I");
   
   newTree->Branch("tcoin_t", &tcoin_t, "tcoin_t/F");
   newTree->Branch("coinTimeUC", &coinTimeUC, "coinTimeUC/F");
   newTree->Branch("coinTime", &coinTime, "coinTime/F");
    
   newTree->Branch("te",     &teS,     "teS/F");
   newTree->Branch("te_t",   &te_tS,   "te_tS/F");
   newTree->Branch("te_r",   &te_rS,   "te_rS/F");
   newTree->Branch("trdt",   &trdtS,   "trdtS/F");
   newTree->Branch("trdt_t", &trdt_tS, "trdt_tS/F");
   newTree->Branch("trdt_r", &trdt_rS, "trdt_rS/F");
   
   
   //===================================================== loading parameter
   //========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) Bfield   = atof(x.c_str());
         if( i == 2 ) a        = atof(x.c_str());         
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) jDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < iDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       firstPos =  0;
       a = 9;
       length = 50.5;
       jDet = 4;
       iDet = 6;
       pos[0] = 0;
       pos[1] = 58.6;
       pos[2] = 117.9;
       pos[3] = 176.8;
       pos[4] = 235.8;
       pos[5] = 294.8;
   }
   
   numDet = iDet * jDet;
   
   //========================================= xf = xn correction
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         //xnCorr[i] = 1;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      
      for(int i = 0; i < numDet; i++){
	xnCorr[i] = 1;
      }
   }
   file.close();
   
   //========================================= e = xf + xn correction
   
   printf("----- loading xf/xn-e correction.");
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
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      for(int i = 0; i < numDet; i++){
	      xfxneCorr[i][0] = 0;
	      xfxneCorr[i][1] = 1;
      }
   }
   file.close();

   //========================================= e correction
   
   printf("----- loading e correction.");
   file.open("correction_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr[i][0] = a;
         eCorr[i][1] = b;
         //printf("\n%2d, e0: %f, e1: %f ", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf("... done.\n");
      
   }else{
      printf("... fail.\n");
      for( int i = 0; i < 24 ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
   }
   file.close();
   
   //========================================= coinTime correction
   if( isTraceDataExist ){
      printf("----- loading coin-Time correction parameters.");
      file.open("correction_coinTime.dat");
      
      f7 = new TF1*[numDet];
      fList = new TObjArray();
      
      if( file.is_open() ){
         double d, a0, a1, a2, a3, a4, a5, a6, a7, a8;
         int i = 0;
         while( file >> d >> a0 >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 >> a7 >> a8){
            if( i >= numDet) break;
            cTCorr[i][0] = a0;
            cTCorr[i][1] = a1;
            cTCorr[i][2] = a2;
            cTCorr[i][3] = a3;
            cTCorr[i][4] = a4;
            cTCorr[i][5] = a5;
            cTCorr[i][6] = a6;
            cTCorr[i][7] = a7;
            cTCorr[i][8] = a8;
            printf("\n%2d, a0: %f, a1: %f .... a7: %f", i, cTCorr[i][0], cTCorr[i][1], cTCorr[i][7]);
            i = i + 1;
         }
         printf("... done.\n");
         
      }else{
         printf("... fail.\n");
         for( int i = 0; i < numDet; i++){
            for( int j = 0 ; j < 9; j++){
               cTCorr[i][j] = 0.;
            } 
         }
      }

      TString name;
      for(int i = 0; i < numDet; i++){
         name.Form("f%d", i);
         f7[i] = new TF1(name, "pol7", -2, 2);
            
         for(int j = 0 ; j < 8; j++){
            f7[i]->SetParameter(j, cTCorr[i][j]);
         }

	      if(file.is_open()) fList->Add(f7[i]);
      }

      if( file.is_open()){
	      fList->Write("fList", TObject::kSingleKey);
      }

      
      file.close();
   }
   
   //========================================= reaction parameters
   printf("----- loading reaction parameter.");
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
      printf("... done.\n");

      isReaction = true;
      alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.;
      gamma = 1./TMath::Sqrt(1-beta*beta);
      G = alpha * gamma * beta * a ;
      printf("============\n");
      printf("mass-b  : %f MeV/c2 \n", mass);
      printf("charge-b: %f \n", q);
      printf("E-total : %f MeV \n", Et);
      printf("mass-B  : %f MeV/c2 \n", massB);      
      printf("beta    : %f \n", beta);
      printf("B-field : %f T \n", Bfield);
      printf("alpha   : %f MeV/mm \n", alpha);
      printf("a       : %f mm \n", a);
      printf("G       : %f MeV \n", G);


   }else{
      printf("... fail.\n");
      isReaction = false;
   }
   file.close();

   printf("================================== numDet : %d \n", numDet);
   
   
}

Bool_t Cali_e_trace_Proof::Process(Long64_t entry)
{

   //#################################################################### initialization
   for(int i = 0; i < numDet; i++){
      eC[i]    = TMath::QuietNaN();
      x[i]     = TMath::QuietNaN();
      z[i]     = TMath::QuietNaN();
      eC_t[i]  = 0;
   }
   
   det = -4;
   hitID = -4;
   multiHit = 0;
   
   Ex  = TMath::QuietNaN();
   thetaCM  = TMath::QuietNaN();
   thetaLab  = TMath::QuietNaN();
   
   for(int i = 0; i < 8 ; i++){
      rdtC[i] = TMath::QuietNaN();
      rdtC_t[i] = 0;
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
   eventID = entry;
   
   b_Energy->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   b_RDT->GetEntry(entry,0);
   b_EnergyTimestamp->GetEntry(entry,0);
   b_RDTTimestamp->GetEntry(entry,0);
   
   b_TAC->GetEntry(entry,0);
   
   if ( isTraceDataExist ){
      b_Trace_Energy->GetEntry(entry,0);
      b_Trace_Energy_Time->GetEntry(entry,0);
      b_Trace_Energy_RiseTime->GetEntry(entry,0);

      b_Trace_RDT->GetEntry(entry,0);
      b_Trace_RDT_Time->GetEntry(entry,0);
      b_Trace_RDT_RiseTime->GetEntry(entry,0);
   }
   
   //#################################################################### gate
   bool rdt_energy = false;
   for( int rID = 0; rID < 8; rID ++){
      if( rdt[rID] > 5000 ) rdt_energy = true; 
   }
   if( !rdt_energy ) return kTRUE;
   
   
   for(int i = 0 ; i < 8 ; i++){
      rdtC[i]   = rdt[i];
      rdtC_t[i] = rdt_t[i]; 
   }

   //#################################################################### processing
   ULong64_t eTime = -2000; //this will be the time for Ex valid
   Float_t teTime = 0.; //time from trace
   for(int idet = 0 ; idet < numDet; idet++){
      
      if( e[idet] > 0 ){
         eC[idet]   = e[idet]/eCorr[idet][0] + eCorr[idet][1];  
         eC_t[idet] = e_t[idet]; // ch
      }
            
      double xfC = 0, xnC = 0;
      if( xf[idet] > 0) xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
      if( xn[idet] > 0) xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
   
      //========= calculate x
      if(xf[idet] > 0  && xn[idet] > 0 ) {
         x[idet] = (xfC-xnC)/(xfC+xnC);
         hitID = 0;
      }else if(xf[idet] == 0 && xn[idet] > 0 ){
         x[idet] = (1-2*xnC/e[idet]);
         hitID = 1;
      }else if(xf[idet] > 0 && xn[idet] == 0 ){
         x[idet] = (2*xfC/e[idet]-1);
         hitID = 2;
      }else{
         x[idet] = TMath::QuietNaN();
      }
      
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
         det = idet;
      
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
   if( multiHit >= 1 ) {
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
         double f7corr = f7[det]->Eval(x[det]) + cTCorr[det][8];
         coinTime = (coinTimeUC - f7corr)*10.;
      }
   }
   
   if( multiHit == 0 ) return kTRUE;
   
   //Fill
   /************************************************************************/
   newTree->Fill();  

   return kTRUE;
}

void Cali_e_trace_Proof::SlaveTerminate()
{

   printf("========================= Slave Terminate.\n");
   
   // i don't know why it works...
   saveFile->cd();
   newTree->Write();
   fOutput->Add(proofFile);
   saveFile->Close();

}

void Cali_e_trace_Proof::Terminate()
{
   
   printf("========================= Terminate.\n");
   
   // i don't know why it works...
   proofFile = dynamic_cast<TProofOutputFile*>(fOutput->FindObject(saveFileName));
   saveFile = TFile::Open(saveFileName);
   
   //get entries
   TTree * tree = (TTree*) saveFile->FindObjectAny("tree");
   int validCount = tree->GetEntries();
   
   printf("=======================================================\n");
   printf("----- saved as %s. valid event: %d\n", saveFileName.Data() , validCount); 

}
