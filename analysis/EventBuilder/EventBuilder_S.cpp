/*************************************** 
 * 
 * created by Ryan Tang 2025, July 10
 * 
 * Base on the EventBuilder_Q.cpp,
 * This event builder apply the mapping 
 * S for super
 * 
****************************************/

#include "BinaryReader.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TString.h"
#include "TGraph.h"
#include "TMacro.h"
#include "TF1.h"
#include "TClonesArray.h"
#include <queue>

#define MAX_MULTI 200
#define MAX_READ_HITS 100000 // Maximum hits to read at a time

#include <sys/time.h> /** struct timeval, select() */
inline unsigned int getTime_us(){
  unsigned int time_us;
  struct timeval t1;
  struct timezone tz;
  gettimeofday(&t1, &tz);
  time_us = (t1.tv_sec) * 1000 * 1000 + t1.tv_usec;
  return time_us;
}

// Comparator for min-heap (smallest timestamp on top)
struct CompareEvent {
  bool operator()(const Event& a, const Event& b) {
    return a.timestamp > b.timestamp;
  }
};
  
//unsigned long long                        evID = 0;
// unsigned int                            numHit = 0;
// unsigned short                   id[MAX_MULTI] = {0}; 
// unsigned int        pre_rise_energy[MAX_MULTI] = {0};  
// unsigned int       post_rise_energy[MAX_MULTI] = {0};  
// unsigned long long        timestamp[MAX_MULTI] = {0};
// unsigned short             traceLen[MAX_MULTI] = {0};
// unsigned short trace[MAX_MULTI][MAX_TRACE_LEN] = {0};


#include "../working/GeneralSortMapping.h" // Include the mapping header file

int runiD = 0;

float Energy[NARRAY];
float XF[NARRAY];
float XN[NARRAY];
float Ring[NARRAY];
float RDT[NRDT];
float TAC[NTAC];
float ELUM[NELUM];
float EZERO[NEZERO];
float CRDT[NCRDT];
float APOLLO[NAPOLLO];

uint64_t EnergyTimestamp[NARRAY];
uint64_t XFTimestamp[NARRAY];
uint64_t XNTimestamp[NARRAY];
uint64_t RingTimestamp[NARRAY];
uint64_t RDTTimestamp[NRDT];
uint64_t TACTimestamp[NTAC];
uint64_t ELUMTimestamp[NELUM];
uint64_t EZEROTimestamp[NEZERO];
uint64_t CRDTTimestamp[NCRDT];
uint64_t APOLLOTimestamp[NAPOLLO];

float te[NARRAY]; // Energy trace
float te_r[NARRAY]; // Energy trace (rise time)
float te_t[NARRAY]; // Energy trace timestamp

float trdt[NRDT]; // Recoil trace
float trdt_r[NRDT]; // Recoil trace (rise time)
float trdt_t[NRDT]; // Recoil trace timestamp

//^============================================
int main(int argc, char* argv[]) {

  printf("=======================================================\n");
  printf("===          Event Builder  raw data --> root       ===\n");
  printf("=======================================================\n");  

  if( argc <= 3){
    printf("%s [outfile] [timeWindow] [trace Analysis] [file-1] [file-2] ... \n", argv[0]);
    printf("        outfile : output root file name\n");
    printf("     timeWindow : nano-sec; if < 0, no event build\n"); 
    printf(" trace Analysis : \n");
    printf("         file-X : the input file(s)\n");
    return -1;
  }

  unsigned int runStartTime = getTime_us();

  TString outFileName = argv[1];
  int timeWindow = atoi(argv[2]);
  const bool traceAna = atoi(argv[3]);

  TClonesArray * arr = nullptr;
  TGraph * gTrace = nullptr;
  TF1 * fitFunction = nullptr;

  const int nFile = argc - 4;
  TString inFileName[nFile];
  for( int i = 0 ; i < nFile ; i++){
    inFileName[i] = argv[i+4];
  }

  printf(" out file : \033[1;33m%s\033[m\n", outFileName.Data());
  if ( timeWindow < 0 ){
    printf(" Event building time window : no event build\n");
  }else{
    printf(" Event building time window : %d nsec \n", timeWindow);
  }
  printf(" Trace Analysis ? %s \n", traceAna ? "Yes" : "No");
  printf(" Number of input file : %d \n", nFile);
  
  //*=============== setup reader
  printf(" Scaning input files...\n");
  uint64_t totalNumHits = 0;
  uint64_t totFileSize = 0; // Total file size in bytes
  BinaryReader ** reader = new BinaryReader *[nFile];
  for( int i = 0 ; i < nFile ; i++){
    reader[i] = new BinaryReader((MAX_READ_HITS)); 
    reader[i]->Open(inFileName[i].Data());
    reader[i]->Scan(true);
    totalNumHits += reader[i]->GetNumData();
    totFileSize += reader[i]->GetFileSize();
    printf("%3d: %s | %6.1f MB | # hit : %10d (%d)\n", i, reader[i]->GetFileName().c_str(), reader[i]->GetFileSize()/1024./1024., reader[i]->GetNumData(), reader[i]->GetMaxHitSize());
  }
  
  //*=============== group files by DigID and sort the fileIndex
  std::map<unsigned short, std::vector<BinaryReader*>> fileGroups;
  for( int i = 0 ; i < nFile ; i++){
    unsigned short digID = reader[i]->GetDigID();
    if (!fileGroups.count(digID) ) {
      fileGroups[digID] = std::vector<BinaryReader*>();
    }
    fileGroups[digID].push_back(reader[i]);
  }
  // printf("Found %zu DigIDs\n", fileGroups.size());
  
  // printf out the file groups
  // for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
  //   unsigned short digID = group.first; // DigID
  //   const auto& readers = group.second; // Vector of BinaryReader pointers
  //   printf("----- DigID %03d has %zu files\n", digID, readers.size());
  //   for (size_t j = 0; j < readers.size(); j++) {
  //     printf("  File %zu: %s | %6.1f MB | num. of hit : %10d\n", j, readers[j]->GetFileName().c_str(), readers[j]->GetFileSize()/1024./1024., readers[j]->GetNumData());
  //   }
  // }
  
  printf("================= Total number of hits: %lu\n", totalNumHits);
  printf("                       Total file size: %.1f MB\n", totFileSize / (1024.0 * 1024.0));

  //*=============== create output file and setup TTree
  TFile * outFile = TFile::Open(outFileName.Data(), "RECREATE");
  
  TTree * outTree = new TTree("tree", outFileName.Data());
  outTree->SetDirectory(outFile);

  outTree->Branch("runID",   &runiD, "runID/I");
  
  outTree->Branch("e",             Energy, Form("e[%d]/F", NARRAY));
  outTree->Branch("e_t",  EnergyTimestamp, Form("e_t[%d]/l", NARRAY));
  outTree->Branch("xf",                XF, Form("xf[%d]/F", NARRAY));
  outTree->Branch("xf_t",     XFTimestamp, Form("xf_t[%d]/l", NARRAY));
  outTree->Branch("xn",                XN, Form("xn[%d]/F", NARRAY));
  outTree->Branch("xn_t",     XNTimestamp, Form("xn_t[%d]/l", NARRAY));
  outTree->Branch("ring",            Ring, Form("ring[%d]/F", NARRAY));
  outTree->Branch("ring_t", RingTimestamp, Form("ring_t[%d]/l", NARRAY));
  
  printf(" -----  %d array det.\n", NARRAY);

  if( NRDT > 0 ) {
    outTree->Branch("rdt", RDT, Form("rdt[%d]/F", NRDT));
    outTree->Branch("rdt_t", RDTTimestamp, Form("rdt_t[%d]/l", NRDT));
  } else {
    printf(" -----  no recoil.\n");
  }

  if( NTAC > 0 ) {
    outTree->Branch("tac", TAC, Form("tac[%d]/F", NTAC));
    outTree->Branch("tac_t", TACTimestamp, Form("tac_t[%d]/l", NTAC));
  } else {
    printf(" -----  no TAC.\n");
  }

  if( NELUM > 0 ) {
    outTree->Branch("elum", ELUM, Form("elum[%d]/F", NELUM));
    outTree->Branch("elum_t", ELUMTimestamp, Form("elum_t[%d]/l", NELUM));
  } else {
    printf(" -----  no ELUM.\n");
  }

  if( NEZERO > 0 ) {
    outTree->Branch("ezero", EZERO, Form("ezero[%d]/F", NEZERO));
    outTree->Branch("ezero_t", EZEROTimestamp, Form("ezero_t[%d]/l", NEZERO));
  } else {
    printf(" -----  no EZERO.\n");
  }

  if( NCRDT > 0 ) {
    outTree->Branch("crdt", CRDT, Form("crdt[%d]/F", NCRDT));
    outTree->Branch("crdt_t", CRDTTimestamp, Form("crdt_t[%d]/l", NCRDT));
  } else {
    printf(" -----  no CRDT.\n");
  }

  if( NAPOLLO > 0 ) {
    outTree->Branch("apollo", APOLLO, Form("apollo[%d]/F", NAPOLLO));
    outTree->Branch("apollo_t", APOLLOTimestamp, Form("apollo_t[%d]/l", NAPOLLO));
  } else {
    printf(" -----  no APOLLO.\n");
  }

  if( traceAna ){
    arr = new TClonesArray("TGraph");

    fitFunction = new TF1("fitFunction", "[0] / (1 + exp(-(x-[1])/[2])) + [3]");

    outTree->Branch("trace", &arr, 16000, 0); // 16kB for trace
    outTree->Branch("te", te, Form("te[%d]/F", NARRAY));
    outTree->Branch("te_r", te_r, Form("te_r[%d]/F", NARRAY));
    outTree->Branch("te_t", te_t, Form("te_t[%d]/l", NARRAY));
    
    if( NRDT > 0 ){
      outTree->Branch("trdt", trdt, Form("trdt[%d]/F", NRDT));
      outTree->Branch("trdt_r", trdt_r, Form("trdt_r[%d]/F", NRDT));
      outTree->Branch("trdt_t", trdt_t, Form("trdt_t[%d]/l", NRDT));
    }
  }

  printf("======= ID-MAP: \n");
  printf("%11s|", ""); 
  for(int i = 0 ; i < 10; i++ ) printf("%7d|", i);
  printf("\n");
  for(int i = 0 ; i < 96; i++ ) printf("-");
  for(int i = 0 ; i < 160; i ++){
    if( (i) % 10 == 0 ) {
      printf("\n");
      if(((i+1)/10)/4+1 < 5) printf("%11s|", Form("VME%d-Dig%d", ((i+1)/10)/4+1, ((i+1)/10)%4+1)); 
    }
    if( 100 + NRDT >= idDetMap[i] && idDetMap[i] >= 100){
      printf("\033[36m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Recoil, Cyan
    }else if( 200+NELUM >= idDetMap[i] && idDetMap[i] >= 200){
      printf("\033[91m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Elum, 
    }else if( 300+NEZERO >= idDetMap[i] && idDetMap[i] >= 300){
      printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// EZERO, 
    }else if( 400+NTAC >= idDetMap[i] && idDetMap[i] >= 400){
      printf("\033[93m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// TAC, 
    }else if( 500+NCRDT >= idDetMap[i] && idDetMap[i] >= 500){
      printf("\033[100m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Circular Recoil
    }else if( 600+NAPOLLO >= idDetMap[i] && idDetMap[i] >= 600){
      printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// APOLLO
    }else if(  NARRAY > idDetMap[i] && idDetMap[i] >= 0){    
      switch (idKindMap[i]) {
        case -1: printf("%7s|", ""); break;
        case  0: printf("\033[31m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// RED
        case  1: printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Green
        case  2: printf("\033[33m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Yellow
        case  3: printf("\033[34m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Blue
        case  4: printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Magenta
        default: printf("%3d(%2d)|", idDetMap[i], idKindMap[i]); break; /// no color
      }
    }else{
      printf("%7s|", "");
    }
  }
  printf("\n");
  for(int i = 0 ; i < 96; i++ ) printf("=");
  printf("\n");


  //*=============== read n data from each file

  uint64_t globalEarliestTime = UINT64_MAX; // Global earliest timestamp
  uint64_t globalLastTime = 0; // Global last timestamp

  std::map<unsigned short, unsigned int> hitID; // store the hit ID for the current reader for each DigID
  std::map<unsigned short, short> fileID; // store the file ID for each DigID
  
  std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue; // Priority queue to store events

  for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
    unsigned short digID = group.first; // DigID
    const auto& readers = group.second; // Vector of BinaryReader pointers

    hitID[digID] = 0; // Initialize hitID for this DigID
    fileID[digID] = 0; 

    BinaryReader* reader = readers[0];
    reader->ReadNextNHitsFromFile(); // Read 10,000 hits at a time

    for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(traceAna)); // Decode the hit payload and push it to the event queue

  }

  globalEarliestTime = eventQueue.top().timestamp; // Set the global earliest time from the first event in the queue

  //*=============== event building
  std::vector<Event> events; // Vector to store events
  unsigned int eventID = 0;
  
  unsigned int hitProcessed = 0; // Number of hits processed
  double last_precentage = 0.0; // Last percentage printed

  do{

    do{

      events.push_back(eventQueue.top()); 
      int digID = events.back().board; 
      hitID[digID]++;
      hitProcessed ++;

      if( fileID[digID] >= 0  ){ // check shoudl read the next N data or next file for this DigID

        BinaryReader* reader = fileGroups[digID][fileID[digID]];
        if( hitID[digID] >= reader->GetHitSize() ) { // If the hitID exceeds the number of hits in the current file
          reader->ReadNextNHitsFromFile(); // Read more hits from the current file
          if( reader->GetHitSize() > 0 ) {
            hitID[digID] = 0; // Reset hitID for this DigID
            for( int i = 0; i < reader->GetHitSize(); i++)  eventQueue.push(reader->GetHit(i).DecodePayload(traceAna)); 
          }
        }

        if( reader->GetHitSize() == 0 ) { // load next file if no hits
          if( fileID[digID] < fileGroups[digID].size() - 1 ) {
            reader->DeleteHits(); // Delete the hits from the current reader, free memory
            fileID[digID]++;
            // printf("\033[34m====== No hits in current file for DigID %03d, loading next file...%s\033[0m\n", digID, readers[fileID[digID]]->GetFileName().c_str());
            reader = fileGroups[digID][fileID[digID]];
            reader->ReadNextNHitsFromFile(); // Read more hits from the next file

            hitID[digID] = 0; // Reset hitID for this DigID
            for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(traceAna)); 
            
          } else {
            fileID[digID] = -1; // Mark that there are no more files for this DigID
          }
        }
      }      

      if( timeWindow >= 0 && events.size() > 0 && events.back().timestamp - events.front().timestamp > timeWindow) {
        events.pop_back(); // Remove the last event if it exceeds the time window
        hitID[digID]--; // Decrement the hitID for this DigID
        hitProcessed--; // Decrement the hitProcessed count
        break;
      }

      eventQueue.pop();
      if( timeWindow < 0 ) break;

    }while(!eventQueue.empty()); // Loop until the event queue is empty

    if( events.size() > 0 ) {
      std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return a.timestamp < b.timestamp; // Sort events by timestamp
      });
      
      for( int i = 0; i < NARRAY; i++) {
        Energy[i] = 0.0f; // Reset energy array
        XF[i] = 0.0f; // Reset XF array
        XN[i] = 0.0f; // Reset XN array
        Ring[i] = 0.0f; // Reset Ring array
        EnergyTimestamp[i] = 0; // Reset energy timestamp
        XFTimestamp[i] = 0; // Reset XF timestamp
        XNTimestamp[i] = 0; // Reset XN timestamp
        RingTimestamp[i] = 0; // Reset Ring timestamp
      }

      if( NRDT > 0 ) {
        for( int i = 0; i < NRDT; i++) {
          RDT[i] = 0.0f; // Reset RDT array
          RDTTimestamp[i] = 0; // Reset RDT timestamp
        }
      }

      if( traceAna ) arr->Clear(); // Clear the TClonesArray for the new event
      
      for( int i = 0; i <  events.size(); i++) {
        int id               = events[i].board * 10 + events[i].channel - 1010;
        uint32_t pre_rise_energy  = events[i].pre_rise_energy; // Pre-rise energy
        uint32_t post_rise_energy = events[i].post_rise_energy; // Post-rise energy
        uint64_t timestamp        = events[i].timestamp; // Timestamp

        int idDet = idDetMap[id]; 
        int idKind = idKindMap[id]; 

        int traceCount = 0;

        if( 0 <= idDet && idDet < NARRAY ) { // Check if the ID is within the range of NARRAY
          switch (idKind ){
            case 0: // Energy signal
              Energy[id] = (float)(post_rise_energy - pre_rise_energy) / MWIN; // Calculate energy
              EnergyTimestamp[id] = timestamp; // Set the timestamp for energy
              break;
            case 1: // XF
              XF[id] = (float)(post_rise_energy - pre_rise_energy) / MWIN * POLARITY_XFXN; // Calculate XF
              XFTimestamp[id] = timestamp; // Set the timestamp for XF
              break;
            case 2: // XN
              XN[id] = (float)(post_rise_energy - pre_rise_energy) / MWIN * POLARITY_XFXN; // Calculate XN
              XNTimestamp[id] = timestamp; // Set the timestamp for XN
              break;
            case 3: // Ring
              Ring[id] = (float)(post_rise_energy - pre_rise_energy) / MWIN; // Calculate Ring
              RingTimestamp[id] = timestamp; // Set the timestamp for Ring
              break;
          }
        }

        if( NRDT > 0 && 100 <= idDet && idDet < 100 + NRDT ) { // Recoil
          int rdtID = idDet - 100; // Recoil ID
          RDT[rdtID] = (float)(post_rise_energy - pre_rise_energy) / MWIN * POLARITY_RDT; // Calculate RDT
          RDTTimestamp[rdtID] = timestamp; // Set the timestamp for RDT
        }

        if( NELUM > 0 && 200 <= idDet && idDet < 200 + NELUM ) { // ELUM
          int elumID = idDet - 200; // ELUM ID
          ELUM[elumID] = (float)(post_rise_energy - pre_rise_energy) / MWIN; // Calculate ELUM
          ELUMTimestamp[elumID] = timestamp; // Set the timestamp for ELUM
        }

        if( NEZERO > 0 && 300 <= idDet && idDet < 300 + NEZERO ) { // EZERO
          int ezeroID = idDet - 300; // EZERO ID
          EZERO[ezeroID] = (float)(post_rise_energy - pre_rise_energy) / MWIN; // Calculate EZERO
          EZEROTimestamp[ezeroID] = timestamp; // Set the timestamp for EZERO
        }

        if( NTAC > 0 &&  400 <= idDet && idDet < 400 + NTAC ) { // TAC
          int tacID = idDet - 400; // TAC ID
          TAC[tacID] = (float)(post_rise_energy - pre_rise_energy) / MWIN; // Calculate TAC
          TACTimestamp[tacID] = timestamp; // Set the timestamp for TAC
        }

        if( NCRDT > 0 && 500 <= idDet && idDet < 500 + NCRDT ) { // Circular Recoil
          int crdtID = idDet - 500; // Circular Recoil ID
          CRDT[crdtID] = (float)(post_rise_energy - pre_rise_energy) / MWIN; // Calculate CRDT
          CRDTTimestamp[crdtID] = timestamp; // Set the timestamp for CRDT
        }

        if( NAPOLLO > 0 && 600 <= idDet && idDet < 600 + NAPOLLO ) { // APOLLO
          int apolloID = idDet - 600; // APOLLO ID
          APOLLO[apolloID] = (float)(post_rise_energy - pre_rise_energy) / MWIN; // Calculate APOLLO
          APOLLOTimestamp[apolloID] = timestamp; // Set the timestamp for APOLLO
        }

        if( traceAna ){

          bool isArray = (0 <= idDet && idDet < NARRAY); 
          bool isRDT = (NRDT > 0 && 100 <= idDet && idDet < 100 + NRDT); 
          if( isArray || isRDT ) { 
            gTrace = (TGraph*)arr->ConstructedAt(traceCount); // Construct a new TGraph at index i
            traceCount++;

            unsigned short traceLen = events[i].traceLength;
            gTrace->SetName(Form("det_%d", idDet));
            gTrace->SetTitle(Form("ev=%llu, id=%d, nHit=%d, length=%d", eventID, idDet, i, traceLen ));
            gTrace->Set(traceLen); // Set the number of points in the TGraph
            uint16_t normalValue = 0;
            for( int j = 0; j < traceLen; j++){
               if( events[i].trace[j] < 16000){
                  normalValue = events[i].trace[j];
                  gTrace->SetPoint(j, j, normalValue);
               }else{
                  gTrace->SetPoint(j, j, normalValue);
               }
            }

            //fitting the trace with function A/ (1 + Exp(-(x-time0)/riseTime)))

            fitFunction->SetRange(0, traceLen - 1); // Set the range for the fit function
            fitFunction->SetParameters(1000, 100.0, 1.0, 8000);  

            gTrace->Fit(fitFunction, "QR");

            // Optionally, you can retrieve the fit parameters
            double A = fitFunction->GetParameter(0);
            double time0 = fitFunction->GetParameter(1);
            double riseTime = fitFunction->GetParameter(2);
            double B = fitFunction->GetParameter(3);

            if( isArray ) {
              te[id] = A;
              te_r[id] = riseTime;
              te_t[id] = timestamp; 
            }

            if( isRDT ) {
              trdt[idDet - 100] = A;
              trdt_r[idDet - 100] = riseTime;
              trdt_t[idDet - 100] = timestamp; 
            }

          }
        }

      }


      outTree->Fill(); // Fill the TTree with the current event
      globalLastTime = events.back().timestamp; // Update the global last time
    }
    
    double percentage = hitProcessed * 100/ totalNumHits;
    if( percentage >= last_precentage ) {
      size_t memoryUsage = sizeof(Event) * eventQueue.size();
      printf("Processed : %u, %.0f%% | %u/%lu | %.3f Mb", eventID, percentage, hitProcessed, totalNumHits, memoryUsage/ (1024. * 1024.));
      printf(" \n\033[A\r");
      last_precentage = percentage + 1.0;
    }
    
    // prepare for the next event
    eventID ++;
    events.clear(); // Clear the events vector for the next event

  }while(!eventQueue.empty()); 

  //Save the global earliest and last timestamps as a TMacro
  TMacro macro("info", "Earliest and Last Timestamps");
  macro.AddLine(Form("globalEarliestTime = %lu", globalEarliestTime));
  macro.AddLine(Form("globalLastTime = %lu", globalLastTime));
  macro.AddLine(Form("totalNumHits = %lu", totalNumHits));
  macro.AddLine(Form("totFileSizeMB = %.1f", totFileSize / (1024.0 * 1024.0))); // Convert to MB
  macro.Write("info");

  //summary
  printf("=======================================================\n");
  printf("===          Event Builder finished                 ===\n");
  printf("=======================================================\n");
  unsigned int runEndTime = getTime_us();
  printf("              Total time taken: %.3f s = %.3f min\n", (runEndTime - runStartTime) / 1000000.0, (runEndTime - runStartTime) / 1000000.0 / 60.0);
  printf("Total number of hits processed: %10u (%lu)\n", hitProcessed, totalNumHits);
  printf("  Total number of events built: %10u\n", eventID);
  printf("     Number of entries in tree: %10lld\n", outTree->GetEntries());
  printf("                Total Run Time: %.3f s = %.3f min\n", (globalLastTime - globalEarliestTime) / 1e8, (globalLastTime - globalEarliestTime) / 1e8 / 60.0);
  //clean up
  outFile->Write();
  outFile->Close();
  for( int i = 0 ; i < nFile ; i++){
    delete reader[i]; // Delete each BinaryReader
  }
  delete[] reader; // Delete the array of BinaryReader pointers
  printf("Output file \033[31m%s\033[0m created successfully.\n", outFileName.Data());

  return 0;
}