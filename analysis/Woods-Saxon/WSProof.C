#define WSProof_cxx
// The class definition in WSProof.h has been generated automatically
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
// root> T->Process("WSProof.C")
// root> T->Process("WSProof.C","some options")
// root> T->Process("WSProof.C+")
//

#include "WSProof.h"
#include <TH2.h>
#include <TStyle.h>
#include <TMath.h>

vector<string> WSProof::SplitStr(string tempLine, string splitter, int shift){

  vector<string> output;

  size_t pos;
  do{
    pos = tempLine.find(splitter); // fine splitter
    if( pos == 0 ){ //check if it is splitter again
      tempLine = tempLine.substr(pos+1);
      continue;
    }

    string secStr;
    if( pos == string::npos ){
      secStr = tempLine;
    }else{
      secStr = tempLine.substr(0, pos+shift);
      tempLine = tempLine.substr(pos+shift);
    }

    //check if secStr is begin with space
    if( secStr.substr(0, 1) == " "){
      secStr = secStr.substr(1);
    }

    output.push_back(secStr);
    //printf(" |%s---\n", secStr.c_str());

  }while(pos != string::npos );

  return output;
}

void WSProof::ReadEnergyFile(TString expFile){
	
	//========= reading expFile
  ifstream file_in;
  file_in.open(expFile.Data(), ios::in);
  if( !file_in ){
    printf(" cannot read file: %s. \n", expFile.Data());
    isFileLoaded = false;
    return;
  }
  
  isFileLoaded = true;
  
  //expEnergy.ReadFile(expFile.Data());
  
  NLJ.clear();
  BE.clear();

  while( file_in.good() ) {
    string tempLine;
    getline(file_in, tempLine );

    if( tempLine.substr(0, 1) == "#" ) continue;
    if( tempLine.size() < 1 ) continue;
    
    vector<string> str0 = SplitStr(tempLine, " ");
    if( str0.size() == 0 ) continue;
    
    NLJ.push_back(str0[0]);
    BE.push_back(atof(str0[1].c_str()));
    
  }

  file_in.close();
  printf("============ Input from %s.\n", expFile.Data());
  for( int i = 0; i < (int) NLJ.size() ; i++){
    printf("NLJ: %6s | %f \n", NLJ[i].c_str(), BE[i]);    
  }
  printf("==============================\n");
}

void WSProof::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption(); 
   
   vector<string> optionList = SplitStr(option.Data(), ",");
   //for(int i = 0; i < (int) optionList.size(); i++){
   //  Info("Begin", "%s", optionList[i].c_str());
   //}

   ReadEnergyFile(optionList[1]); // just diaplay, not feed to Slave
   saveFileName = optionList[2]; // this will be use at terminate
   
   //use this macro to pass the expEnergy to Slave via fInput
   expEnergy = new TMacro();
   expEnergy->ReadFile(optionList[1].c_str());
   fInput->Add(expEnergy);

   printf("========================== Begin\n");

}

void WSProof::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).
   
   Info("begin", "========================== Slave Begin");

   TString option = GetOption();
	vector<string> optionList = SplitStr(option.Data(), ",");
   
	wsA = atoi(optionList[0].c_str());
   saveFileName = optionList[2];
	wsNStep = atoi(optionList[3].c_str());
	wsdr = atof(optionList[4].c_str());
   wsZ = atoi(optionList[5].c_str());
	printf(" ----- save File Name  : %s \n", saveFileName.Data());
   
   proofFile = new TProofOutputFile(saveFileName, "M"); // M for merge
   proofFile->SetOutputFileName(saveFileName);  
   
   saveFile = proofFile->OpenFile("RECREATE");
   
   newTree = new TTree("tree","PSD Tree w/ trace");
   newTree->SetDirectory(saveFile);
   newTree->AutoSave();

	newTree->Branch("VO", &V0, "V0/D");
   newTree->Branch("RO", &R0, "R0/D");
   newTree->Branch("rO", &r0, "r0/D");
   newTree->Branch("aO", &a0, "a0/D");
   newTree->Branch("VSO", &VSO, "VSO/D");
   newTree->Branch("RSO", &RSO, "RSO/D");
   newTree->Branch("rSO", &rSO, "rSO/D");
   newTree->Branch("aSO", &aSO, "aSO/D");
   newTree->Branch("rms", &rms, "rms/D");
   newTree->Branch("lesq", &lesq, "lesq/D");
   
   Printf("================== Slave Start\n");

}

Bool_t WSProof::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either WSProof::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
   
   if( isFileLoaded == false ) return kFALSE;

   //Printf("================== entry %llu \n", entry);

	b_V0->GetEntry(entry);
	b_R0->GetEntry(entry);
	b_r0->GetEntry(entry);
	b_a0->GetEntry(entry);
	
	b_VSO->GetEntry(entry);
	b_RSO->GetEntry(entry);
	b_rSO->GetEntry(entry);
	b_aSO->GetEntry(entry);
   
	ws.V0 = V0;
	ws.R0 = R0;
	ws.a0 = a0;
	ws.VSO = VSO;
	ws.RSO = RSO;
	ws.aSO = aSO;
   ws.r0 = r0;
   ws.rSO = rSO;
   ws.A = wsA;
   ws.Z = wsZ;
   ws.Rc = R0;
   ws.rc = r0;
   ws.dr = wsdr;
   ws.nStep = wsNStep;

	//ws.PrintWSParas();
	ws.CalWSEnergies();
   bool showStat = false;
   if( entry % 500 == 0 ) showStat = true;
	//ws.PrintEnergyLevels();
	rms = 0;
	lesq = 0;
	int nDiff = 0;

   if( showStat) {
     printf("========================================\n");
     printf("    Experiment  |  Woods-Saxon   |\n");
   }
   for( int i = 0; i < (int) NLJ.size(); i++){
     if(showStat) printf(" %d %6s (%9.6f) | ",i,  NLJ[i].c_str(), BE[i] );	  
     bool isMatched = false; 
     for( int j = 0; j < (int) ws.orbString.size(); j++){
		if( NLJ[i] == ws.orbString[j] ){
		  double diff = BE[i] -  ws.energy[j];
		  rms += pow(diff,2);
		  nDiff ++;
		  isMatched = true;
        if(showStat) printf("%d %6s (%9.6f) | diff : %f \n", j, ws.orbString[j].c_str(), ws.energy[j], diff);
		  continue;
		}
	 }
	 if(showStat && !isMatched) printf(" ------ \n");
	}
	if( showStat) printf("========================================\n");
   
	if( rms == 0 || nDiff != (int) NLJ.size() ){
	 rms = TMath::QuietNaN();
	 lesq = TMath::QuietNaN();
	}else{
	 rms = sqrt(rms/NLJ.size());
	 lesq = sqrt(rms);
	}
	
	newTree->Fill();

   return kTRUE;
}

void WSProof::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.
   
   printf("=================== Slave Terminate \n");
   
   saveFile->cd();
   newTree->Write("tree", TObject::kOverwrite);
   fOutput->Add(proofFile);
   saveFile->Close();

}

void WSProof::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
  
   //proofFile = dynamic_cast<TProofOutputFile*>(fOutput->FindObject(saveFileName));
   //
   saveFile = TFile::Open(saveFileName, "UPDATE");
   
   expEnergy->Write("expEnergy");
   
   //get entries
   TTree * tree = (TTree*) saveFile->FindObjectAny("tree");
   int validCount = tree->GetEntries();
   
   saveFile->Close();
   
   printf("=======================================================\n");
   printf("----- saved as %s. valid event: %d\n", saveFileName.Data() , validCount); 
}
