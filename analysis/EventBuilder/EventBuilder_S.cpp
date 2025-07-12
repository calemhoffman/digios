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
#include <thread>
#include <mutex>

#define MAX_TRACE_LEN 1250 
#define MAX_TRACE_MULTI 200
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
  
#include "../working/GeneralSortMapping.h" // Include the mapping header file

class Data {
public:
  int runiD = 0;

  float Energy[NARRAY];
  float XF[NARRAY];
  float XN[NARRAY];
  float Ring[NARRAY];
  uint64_t EnergyTimestamp[NARRAY];
  uint64_t XFTimestamp[NARRAY];
  uint64_t XNTimestamp[NARRAY];
  uint64_t RingTimestamp[NARRAY];
  
  // trace
  unsigned short                            traceCount = 0; 
  unsigned short           traceDetID[MAX_TRACE_MULTI] = {0};
  unsigned short          traceKindID[MAX_TRACE_MULTI] = {0};
  unsigned short             traceLen[MAX_TRACE_MULTI] = {0};
  unsigned short trace[MAX_TRACE_MULTI][MAX_TRACE_LEN] = {0};

  // trace analysis
  float tracePara[MAX_TRACE_MULTI][4]; // trace parameters, 0 = Amplitude, 1 = Rise time, 2 = Timestamp, 3 = Baseline
  TF1 * fitFunc = nullptr; 
  TGraph * graph = nullptr;

#if NRDT > 0
  float RDT[NRDT];
  uint64_t RDTTimestamp[NRDT];
  float trdt[NRDT][5]; // Recoil trace
#endif
#if NTAC > 0
  float TAC[NTAC];
  uint64_t TACTimestamp[NTAC];
#endif
#if NELUM > 0
  float ELUM[NELUM];
  uint64_t ELUMTimestamp[NELUM];
#endif
#if NEZERO > 0
  float EZERO[NEZERO];
  uint64_t EZEROTimestamp[NEZERO];
#endif
#if NCRDT > 0
  float CRDT[NCRDT];
  uint64_t CRDTTimestamp[NCRDT];
#endif
#if NAPOLLO > 0
  float APOLLO[NAPOLLO];
  uint64_t APOLLOTimestamp[NAPOLLO];
#endif

  Data(){}
  ~Data(){}

  void Reset() {
    runiD = 0;

    for (int i = 0; i < NARRAY; i++) {
      Energy[i] = TMath::QuietNaN();
      XF[i] = TMath::QuietNaN();
      XN[i] = TMath::QuietNaN();
      Ring[i] = TMath::QuietNaN();
      EnergyTimestamp[i] = 0;
      XFTimestamp[i] = 0;
      XNTimestamp[i] = 0;
      RingTimestamp[i] = 0;
    }

    traceCount = 0; 
    for (int i = 0; i < MAX_TRACE_MULTI; i++) {
      traceDetID[i] = 0;
      traceKindID[i] = 0;
      traceLen[i] = 0;
      for (int j = 0; j < MAX_TRACE_LEN; j++) trace[i][j] = 0;
      for (int j = 0; j < 4; j++) tracePara[i][j] = TMath::QuietNaN();
    }

#if NRDT > 0
    for (int i = 0; i < NRDT; i++) {
      RDT[i] = TMath::QuietNaN();
      RDTTimestamp[i] = 0;
      for (int j = 0; j < 5; j++) trdt[i][j] = TMath::QuietNaN();
    }
#endif
#if NTAC > 0
    for (int i = 0; i < NTAC; i++) {
      TAC[i] = TMath::QuietNaN();
      TACTimestamp[i] = 0;
    }
#endif
#if NELUM > 0
    for (int i = 0; i < NELUM; i++) {
      ELUM[i] = TMath::QuietNaN();
      ELUMTimestamp[i] = 0;
    }
#endif
#if NEZERO > 0    
    for (int i = 0; i < NEZERO; i++) {
      EZERO[i] = TMath::QuietNaN();
      EZEROTimestamp[i] = 0;
    }
#endif
#if NCRDT > 0
    for (int i = 0; i < NCRDT; i++) {
      CRDT[i] = TMath::QuietNaN();
      CRDTTimestamp[i] = 0;
    }
#endif
#if NAPOLLO > 0
    for (int i = 0; i < NAPOLLO; i++) {
      APOLLO[i] = TMath::QuietNaN();
      APOLLOTimestamp[i] = 0;
    }
#endif
  }

  void FillData(std::vector<Event> events, bool saveTrace){

    for( int i = 0; i <  events.size(); i++) {
        int id                    = events[i].board * 10 + events[i].channel - 1010;
        uint32_t pre_rise_energy  = events[i].pre_rise_energy; // Pre-rise energy
        uint32_t post_rise_energy = events[i].post_rise_energy; // Post-rise energy
        uint64_t timestamp        = events[i].timestamp; // Timestamp

        int idDet = idDetMap[id]; 
        int idKind = idKindMap[id]; 

        if( idDet < 0 ) continue;

        float eee = ((float)post_rise_energy - (float)pre_rise_energy) / MWIN; 

        // printf(" post_rise_energy : %u, pre_rise_energy : %u, eee : %.3f \n", post_rise_energy, pre_rise_energy, eee);

        if( 0 <= idDet && idDet < NARRAY ) { // Check if the ID is within the range of NARRAY
          switch (idKind ){
            case 0: // Energy signal
              Energy[idDet] = eee; // Calculate energy
              EnergyTimestamp[idDet] = timestamp; // Set the timestamp for energy
              break;
            case 1: // XF
              XF[idDet] = eee * POLARITY_XFXN; // Calculate XF
              XFTimestamp[idDet] = timestamp; // Set the timestamp for XF
              break;
            case 2: // XN
              XN[idDet] = eee * POLARITY_XFXN; // Calculate XN
              XNTimestamp[idDet] = timestamp; // Set the timestamp for XN
              break;
            case 3: // Ring
              Ring[idDet] = eee; // Calculate Ring
              RingTimestamp[idDet] = timestamp; // Set the timestamp for Ring
                break;
              }
            }

    #if NRDT > 0
        if( 100 <= idDet && idDet < 100 + NRDT ) { // Recoil
          int rdtID = idDet - 100; // Recoil ID
          RDT[rdtID] = eee * POLARITY_RDT; // Calculate RDT
          RDTTimestamp[rdtID] = timestamp; // Set the timestamp for RDT
        }
    #endif

    #if NELUM > 0 
        if( 200 <= idDet && idDet < 200 + NELUM ) { // ELUM
          int elumID = idDet - 200; // ELUM ID
          ELUM[elumID] = eee;
          ELUMTimestamp[elumID] = timestamp; // Set the timestamp for ELUM
        }
    #endif

    #if NEZERO > 0
        if( 300 <= idDet && idDet < 300 + NEZERO ) { // EZERO
          int ezeroID = idDet - 300; // EZERO ID
          EZERO[ezeroID] = eee;
          EZEROTimestamp[ezeroID] = timestamp; // Set the timestamp for EZERO
        }
    #endif

    #if NTAC > 0
        if( 400 <= idDet && idDet < 400 + NTAC ) { // TAC
          int tacID = idDet - 400; // TAC ID
          TAC[tacID] = eee;
          TACTimestamp[tacID] = timestamp; // Set the timestamp for TAC
        }
    #endif

    #if NCRDT > 0
        if( 500 <= idDet && idDet < 500 + NCRDT ) { // Circular Recoil
          int crdtID = idDet - 500; // Circular Recoil ID
          CRDT[crdtID] = eee;
          CRDTTimestamp[crdtID] = timestamp; // Set the timestamp for CRDT
        }
    #endif

    #if NAPOLLO > 0
        if( 600 <= idDet && idDet < 600 + NAPOLLO ) { // APOLLO
          int apolloID = idDet - 600; // APOLLO ID
          APOLLO[apolloID] = eee;
          APOLLOTimestamp[apolloID] = timestamp; // Set the timestamp for APOLLO
        }
    #endif

        if( saveTrace ){

          bool isArray = (0 <= idDet && idDet < NARRAY); 
          bool isRDT = (NRDT > 0 && 100 <= idDet && idDet < 100 + NRDT); 
          if( isArray || isRDT ) { 

            traceLen[traceCount] = events[i].traceLength;
            traceDetID[traceCount] = idDet;
            traceKindID[traceCount] = idKind;
            uint16_t regulatedValue = 0;
            for( int j = 0; j < traceLen[traceCount]; j++){
              if( events[i].trace[j] < 16000 ) regulatedValue = events[i].trace[j]; 
              trace[traceCount][j] = regulatedValue; 
            }

            traceCount++;
          }
        }

      }

  }

  void SetTraceFunction(){
    if( fitFunc ) return;
    // fit trace[i] with  A/(1+exp(-(x - x0)/tau)) + B
    // where A is the amplitude, x0 is the timestamp, tau is the rise time, and B is the baseline
    fitFunc = new TF1("fitFunc", "[0] / (1 + exp(-(x - [1]) / [2])) + [3]"); 
    graph = new TGraph(); 
  }

  void TraceAnalysis(){
    if( traceCount <= 0 ) return; // No traces to analyze

    for( int i = 0; i < traceCount; i++ ){
      if( traceLen[i] <= 0 ) continue; // Skip if trace length is zero

      // Prepare the fitting function
      fitFunc->SetRange(0, traceLen[i] - 1); // Set the range for the fit function
      fitFunc->SetParameters(100.0, 100, 1.0, 8000.0); // Initial parameters: A, x0, tau, B

      // Create a TGraph for the trace data
      graph->Clear(); 
      for( int j = 0; j < traceLen[i]; j++ ) graph->SetPoint(j, j, trace[i][j]); // Set the x and y values for the graph
      
      graph->Fit(fitFunc, "QR"); 

      // Store the results in the te array
      tracePara[i][0] = fitFunc->GetParameter(0); // Amplitude
      tracePara[i][1] = fitFunc->GetParameter(1); // Rise time
      tracePara[i][2] = fitFunc->GetParameter(2); // Timestamp
      tracePara[i][3] = fitFunc->GetParameter(3); // Baseline

    }

  }

};

//^============================================
int main(int argc, char* argv[]) {

  printf("=======================================================\n");
  printf("===          Event Builder  raw data --> root       ===\n");
  printf("=======================================================\n");  

  if( argc <= 4){
    printf("%s [outfile] [timeWindow] [save Trace] [trace Analysis] [file-1] [file-2] ... \n", argv[0]);
    printf("        outfile : output root file name\n");
    printf("     timeWindow : nano-sec; if < 0, no event build\n"); 
    printf("     Save Trace : 0 : no, 1 : yes\n");
    printf(" trace Analysis : 0 : no, 1 : yes (single core), >1 : multi-core\n");
    printf("         file-X : the input file(s)\n");
    return -1;
  }

  unsigned int runStartTime = getTime_us();

  TString outFileName = argv[1];
  int timeWindow = atoi(argv[2]);
  const bool saveTrace = atoi(argv[3]);
  const short nWorkers = atoi(argv[4]);

  Data data;

  const int nFile = argc - 5;
  TString inFileName[nFile];
  for( int i = 0 ; i < nFile ; i++){
    inFileName[i] = argv[i+5];
  }

  printf(" out file : \033[1;33m%s\033[m\n", outFileName.Data());
  if ( timeWindow < 0 ){
    printf(" Event building time window : no event build\n");
  }else{
    printf(" Event building time window : %d nsec \n", timeWindow);
  }
  printf(" Save Trace ? %s\n", saveTrace ? "Yes" : "No");
  printf(" Trace Analysis ? %s %s\n", nWorkers ? "Yes" : "No", nWorkers > 0 ? "" : Form("(%d-core)", nWorkers));
  printf(" Number of input file : %d \n", nFile);
  
  //*=============== setup reader
  printf(" Scaning input files...\n");
  uint64_t totalNumHits = 0;
  uint64_t totFileSize = 0; // Total file size in bytes
  BinaryReader ** reader = new BinaryReader *[nFile];

  std::vector<std::thread> threads;

  for( int i = 0 ; i < nFile ; i++){
    reader[i] = new BinaryReader((MAX_READ_HITS)); 
    reader[i]->Open(inFileName[i].Data());
    threads.emplace_back([](BinaryReader* reader) { 
      reader->Scan(true); 
      printf("%s | %6.1f MB | # hit : %10d (%d)\n", reader->GetFileName().c_str(), reader->GetFileSize()/1024./1024., reader->GetNumData(), reader->GetMaxHitSize());
    }, reader[i]);
  }

  uint64_t globalEarliestTime = UINT64_MAX; // Global earliest timestamp
  uint64_t globalLastTime = 0; // Global last timestamp

  // Wait for all threads to finish
  for (int i = 0; i < threads.size(); ++i) {
    threads[i].join();
    totalNumHits += reader[i]->GetNumData();
    totFileSize += reader[i]->GetFileSize();

    if (reader[i]->GetGlobalEarliestTime() < globalEarliestTime) globalEarliestTime = reader[i]->GetGlobalEarliestTime();
    if (reader[i]->GetGlobalLastTime() > globalLastTime) globalLastTime = reader[i]->GetGlobalLastTime();
  }

  threads.clear();
  
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
  printf("                        Total Run Time: %.3f s = %.3f min\n", (globalLastTime - globalEarliestTime) / 1e8, (globalLastTime - globalEarliestTime) / 1e8 / 60.0);


  //*=============== create output file and setup TTree
  TFile * outFile = TFile::Open(outFileName.Data(), "RECREATE");
  
  TTree * outTree = new TTree("tree", outFileName.Data());
  outTree->SetDirectory(outFile);

  outTree->Branch("runID",   &data.runiD, "runID/I");
  
  outTree->Branch("e",             data.Energy, Form("e[%d]/F", NARRAY));
  outTree->Branch("e_t",  data.EnergyTimestamp, Form("e_t[%d]/l", NARRAY));
  outTree->Branch("xf",                data.XF, Form("xf[%d]/F", NARRAY));
  outTree->Branch("xf_t",     data.XFTimestamp, Form("xf_t[%d]/l", NARRAY));
  outTree->Branch("xn",                data.XN, Form("xn[%d]/F", NARRAY));
  outTree->Branch("xn_t",     data.XNTimestamp, Form("xn_t[%d]/l", NARRAY));
  outTree->Branch("ring",            data.Ring, Form("ring[%d]/F", NARRAY));
  outTree->Branch("ring_t", data.RingTimestamp, Form("ring_t[%d]/l", NARRAY));
  
  printf(" -----  %d array det.\n", NARRAY);

#if NRDT > 0
    outTree->Branch("rdt", data.RDT, Form("rdt[%d]/F", NRDT));
    outTree->Branch("rdt_t", data.RDTTimestamp, Form("rdt_t[%d]/l", NRDT));
#else
    printf(" -----  no recoil.\n");
#endif

#if NTAC > 0
  outTree->Branch("tac", data.TAC, Form("tac[%d]/F", NTAC));
  outTree->Branch("tac_t", data.TACTimestamp, Form("tac_t[%d]/l", NTAC));
#else
  printf(" -----  no TAC.\n");
#endif

#if NELUM > 0
  outTree->Branch("elum", data.ELUM, Form("elum[%d]/F", NELUM));
  outTree->Branch("elum_t", data.ELUMTimestamp, Form("elum_t[%d]/l", NELUM));
#else
  printf(" -----  no ELUM.\n");
#endif

#if NEZERO > 0
  outTree->Branch("ezero", data.EZERO, Form("ezero[%d]/F", NEZERO));
  outTree->Branch("ezero_t", data.EZEROTimestamp, Form("ezero_t[%d]/l", NEZERO));
#else
  printf(" -----  no EZERO.\n");
#endif

#if NCRDT > 0
  outTree->Branch("crdt", data.CRDT, Form("crdt[%d]/F", NCRDT));
  outTree->Branch("crdt_t", data.CRDTTimestamp, Form("crdt_t[%d]/l", NCRDT));
#else
  printf(" -----  no CRDT.\n");
#endif

#if NAPOLLO > 0
  outTree->Branch("apollo", data.APOLLO, Form("apollo[%d]/F", NAPOLLO));
  outTree->Branch("apollo_t", data.APOLLOTimestamp, Form("apollo_t[%d]/l", NAPOLLO));
#else
  printf(" -----  no APOLLO.\n");
#endif

  if( saveTrace ){

    outTree->Branch("traceCount", &data.traceCount, "traceCount/s");
    outTree->Branch("traceDetID",   data.traceDetID, "traceDetID[traceCount]/s");
    outTree->Branch("traceKindID",  data.traceKindID, "traceKindID[traceCount]/s");
    outTree->Branch("traceLength",   data.traceLen, "traceLength[traceCount]/s");
    outTree->Branch(       "trace",     data.trace, Form("trace[traceCount][%d]/s", MAX_TRACE_LEN));

  }
  if( nWorkers > 0 ){ 
    outTree->Branch("tracePara", data.tracePara, Form("tracePara[%d][4]/F", NARRAY));
  }

  //*=============== print ID map
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
  std::map<unsigned short, unsigned int> hitID; // store the hit ID for the current reader for each DigID
  std::map<unsigned short, short> fileID; // store the file ID for each DigID
  
  std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue; // Priority queue to store events for each thread

  for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
    unsigned short digID = group.first; // DigID
    const auto& readers = group.second; // Vector of BinaryReader pointers

    hitID[digID] = 0; // Initialize hitID for this DigID
    fileID[digID] = 0; 

    BinaryReader* reader = readers[0];
    reader->ReadNextNHitsFromFile(); // Read 10,000 hits at a time

    for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(nWorkers)); // Decode the hit payload and push it to the event queue

  }

  //*=============== event building
  std::vector<Event> events; // Vector to store events
  unsigned int eventID = 0;
  
  unsigned int hitProcessed = 0; // Number of hits processed
  double last_precentage = 0.0; // Last percentage printed


  std::mutex mtx; // Mutex for thread safety fro outTree->Fill()
  if( nWorkers > 0 ) {
    threads.reserve(nWorkers);
  }
  int busyThreadCount = 0; // Count of busy threads
  bool workerBusy[nWorkers];
  for (int i = 0; i < nWorkers; i++) {
    workerBusy[i] = false; // Initialize all workers as not busy
  }

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
            for( int i = 0; i < reader->GetHitSize(); i++)  eventQueue.push(reader->GetHit(i).DecodePayload(nWorkers)); 
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
            for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(nWorkers)); 
            
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
      
      // data.Reset(); 
      // data.FillData(events, saveTrace);

      // if (nWorkers) {
      //   data.TraceAnalysis(); // Perform trace analysis if enabled
      //   outTree->Fill(); 
      // }

      if( nWorkers > 0 ) {
        // Multi-threaded processing

        Data threadData; // Create a local copy for thread processing
        threadData.Reset(); 
        threadData.FillData(events, saveTrace); 
        // threadData.SetTraceFunction(); 

        int threadID = eventID % nWorkers; // Get the thread ID based on the busy count

        if( workerBusy[threadID] ) { // If the worker is busy, wait for it to finish
          threads[threadID].join(); // Wait for the thread to finish
        } 

        threads[threadID] = std::thread([&data, threadData, &mtx, threadID, outTree, &workerBusy]() {
          workerBusy[threadID] = true; // Mark this worker as busy
          // workerData.TraceAnalysis(); // Perform trace analysis
          printf("\033[32mThread %d is processing data...\033[0m\n", threadID);
          for( int k = 0; k < 1e6 ; k++){};
          // mtx.lock(); // Lock the mutex to ensure thread safety
          // data = threadData;
          // outTree->Fill(); // Fill the tree in a thread-safe manner
          // mtx.unlock();
          workerBusy[threadID] = false; // Mark this worker as not busy
          printf("\033[33mThread %d finished processing data.\033[0m\n", threadID);
        });
        

      } else {
        // Single-threaded processing, o trace analysis
        data.Reset();
        data.FillData(events, saveTrace);
        outTree->Fill(); // Fill the tree
      }

      
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


  //*=============== save some marco
  //Save the global earliest and last timestamps as a TMacro
  TMacro macro("info", "Earliest and Last Timestamps");
  macro.AddLine(Form("globalEarliestTime = %lu", globalEarliestTime));
  macro.AddLine(Form("globalLastTime = %lu", globalLastTime));
  macro.AddLine(Form("totalNumHits = %lu", totalNumHits));
  macro.AddLine(Form("totFileSizeMB = %.1f", totFileSize / (1024.0 * 1024.0))); // Convert to MB
  macro.Write("info");

  TMacro macro2("trace_info", "Maximum Trace Length"); //this macro is for read Raw trace
  macro2.AddLine(Form("%d", MAX_TRACE_LEN));
  macro2.Write("trace_info");

  //*=============== summary
  printf("=======================================================\n");
  printf("===          Event Builder finished                 ===\n");
  printf("=======================================================\n");
  unsigned int runEndTime = getTime_us();
  printf("              Total time taken: %.3f s = %.3f min\n", (runEndTime - runStartTime) / 1000000.0, (runEndTime - runStartTime) / 1000000.0 / 60.0);
  printf("Total number of hits processed: %10u (%lu)\n", hitProcessed, totalNumHits);
  printf("  Total number of events built: %10u\n", eventID);
  printf("     Number of entries in tree: %10lld\n", outTree->GetEntries());
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