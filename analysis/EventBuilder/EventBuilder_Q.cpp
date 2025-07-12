/*************************************** 
 * 
 * created by Ryan Tang 2025, July 10
 * 
 * Based on the EventBuidler.cpp,
 * This event builder use priority queue to build events from multiple files.
 * 
****************************************/

#include "BinaryReader.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TString.h"
#include "TMacro.h"
#include "TClonesArray.h"
#include "TGraph.h"

#include <queue>
#include <thread>
#include <vector>

#define MAX_TRACE_LEN 1250 
#define FULL_OUTPUT false
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
unsigned int                            numHit = 0;
unsigned short                   id[MAX_MULTI] = {0}; 
unsigned int        pre_rise_energy[MAX_MULTI] = {0};  
unsigned int       post_rise_energy[MAX_MULTI] = {0};  
unsigned long long        timestamp[MAX_MULTI] = {0};
unsigned short             traceLen[MAX_MULTI] = {0};
unsigned short trace[MAX_MULTI][MAX_TRACE_LEN] = {0};


#if FULL_OUTPUT
  uint32_t                      baseline[MAX_MULTI] = {0};
  unsigned short                geo_addr[MAX_MULTI] = {0};
  unsigned short                   flags[MAX_MULTI] = {0}; // bit 0 : external disc, bit 1 : peak valid, bit 2 : offset, bit 3 : sync error, bit 4 : general error, bit 5 : pileup only, bit 6 : pileup
  unsigned long long last_disc_timestamp[MAX_MULTI] = {0}; // Last discriminator timestamp
  unsigned int            peak_timestamp[MAX_MULTI] = {0}; // Peak timestamp
  unsigned int           m1_begin_sample[MAX_MULTI] = {0}; // M1 begin sample
  unsigned int             m1_end_sample[MAX_MULTI] = {0}; // M1 end sample
  unsigned int           m2_begin_sample[MAX_MULTI] = {0}; // M2 begin sample
  unsigned int             m2_end_sample[MAX_MULTI] = {0}; // M2 end sample
  unsigned int               peak_sample[MAX_MULTI] = {0}; // Peak sample
  unsigned int               base_sample[MAX_MULTI] = {0}; // Base sample
#endif

//^============================================
int main(int argc, char* argv[]) {

  printf("=======================================================\n");
  printf("===          Event Builder  raw data --> root       ===\n");
  printf("=======================================================\n");  

  if( argc <= 3){
    printf("%s [outfile] [timeWindow] [saveTrace] [sol-1] [sol-2] ... \n", argv[0]);
    printf("      outfile : output root file name\n");
    printf("   timeWindow : nano-sec; if < 0, no event build\n"); 
    printf("    saveTrace : 1 = save trace, 0 = no trace\n");
    printf("        sol-X : the sol file(s)\n");
    return -1;
  }

  unsigned int runStartTime = getTime_us();

  TString outFileName = argv[1];
  int timeWindow = atoi(argv[2]);
  const bool saveTrace = atoi(argv[3]);

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
  printf(" Save Trace ? %s \n", saveTrace ? "Yes" : "No");
  printf(" Number of input file : %d \n", nFile);
  
  //*=============== setup reader
  printf(" Scaning input files...\n");
  uint64_t totalNumHits = 0;
  uint64_t totFileSize = 0; // Total file size in bytes
  BinaryReader ** reader = new BinaryReader *[nFile];

  std::vector<std::thread> threads; // Create a vector of threads for parallel scanning

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

  //TODO there is potential time not sort for different files from the same DigID....
  
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
  // outTree->SetAutoSave(10000000); // autosave every 10 million bytes
  // outTree->SetAutoFlush(1000000); // auto-flush every 1 million bytes 
  outTree->SetDirectory(outFile);

  outTree->Branch(         "NumHits",          &numHit, "NumHits/i");
  outTree->Branch(              "id",               id, "id[NumHits]/s");
  outTree->Branch( "pre_rise_energy",  pre_rise_energy, "pre_rise_energy[NumHits]/i");
  outTree->Branch("post_rise_energy", post_rise_energy, "post_rise_energy[NumHits]/i");
  outTree->Branch( "event_timestamp",        timestamp, "event_timestamp[NumHits]/l");
#if FULL_OUTPUT
  outTree->Branch(           "baseline",            baseline, "baseline[NumHits]/i");
  outTree->Branch(           "geo_addr",            geo_addr, "geo_addr[NumHits]/s");
  outTree->Branch(              "flags",               flags, "flags[NumHits]/s"); 
  outTree->Branch("last_disc_timestamp", last_disc_timestamp, "last_disc_timestamp[NumHits]/l"); 
  outTree->Branch(     "peak_timestamp",      peak_timestamp, "peak_timestamp[NumHits]/i"); 
  outTree->Branch(    "m1_begin_sample",     m1_begin_sample, "m1_begin_sample[NumHits]/i"); 
  outTree->Branch(      "m1_end_sample",       m1_end_sample, "m1_end_sample[NumHits]/i"); 
  outTree->Branch(    "m2_begin_sample",     m2_begin_sample, "m2_begin_sample[NumHits]/i");
  outTree->Branch(      "m2_end_sample",       m2_end_sample, "m2_end_sample[NumHits]/i");
  outTree->Branch(        "peak_sample",         peak_sample, "peak_sample[NumHits]/i");
  outTree->Branch(        "base_sample",         base_sample, "base_sample[NumHits]/i");
#endif
  if( saveTrace ){
    outTree->Branch("trace_length", traceLen, "trace_length[NumHits]/s");
    outTree->Branch(       "trace",    trace, Form("trace[NumHits][%d]/s", MAX_TRACE_LEN));
  } 

  //*=============== read n data from each file

  std::map<unsigned short, unsigned int> hitID; // store the hit ID for the current reader for each DigID
  std::map<unsigned short, short> fileID; // store the file ID for each DigID
  
  std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue; // Priority queue to store events

  for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
    unsigned short digID = group.first; // DigID
    const auto& readers = group.second; // Vector of BinaryReader pointers

    hitID[digID] = 0; // Initialize hitID for this DigID
    fileID[digID] = 0; 

    BinaryReader* reader = readers[0];
    reader->ReadNextNHitsFromFile();

    for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(saveTrace)); // Decode the hit payload and push it to the event queue

  }

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
            for( int i = 0; i < reader->GetHitSize(); i++)  eventQueue.push(reader->GetHit(i).DecodePayload(saveTrace)); 
          }
        }

        if( reader->GetHitSize() == 0 ) { // load next file if no hits
          if( fileID[digID] < fileGroups[digID].size() - 1 ) {
            reader->DeleteHits(); // Delete the hits from the current reader, free memory
            fileID[digID]++;
            // printf("\033[34m====== No hits in current file for DigID %03d, loading next file...%s\033[0m\n", digID, readers[fileID[digID]]->GetFileName().c_str());
            reader = fileGroups[digID][fileID[digID]];
            reader->ReadNextNHitsFromFile(); // Read more hits from the current file
            hitID[digID] = 0; // Reset hitID for this DigID
            for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(saveTrace)); 
            
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
      
      if( events.size() > MAX_MULTI ) {
        printf("\033[31mWarning: More than %d hits in one event, truncating to %d hits.\033[0m\n", MAX_MULTI, MAX_MULTI);
        //events.resize(MAX_MULTI); // Truncate to MAX_MULTI hits
        numHit = MAX_MULTI; // Set numHit to MAX_MULTI
      }else{
        numHit = events.size(); 
      }

      for( int i = 0; i <  events.size(); i++) {
        id[i]               = events[i].board * 10 + events[i].channel; // Channel ID
        pre_rise_energy[i]  = events[i].pre_rise_energy; // Pre-rise energy
        post_rise_energy[i] = events[i].post_rise_energy; // Post-rise energy
        timestamp[i]        = events[i].timestamp; // Timestamp
#if FULL_OUTPUT
        baseline[i]            = events[i].baseline; // Baseline value
        geo_addr[i]            = events[i].geo_addr; // Geo address
        flags[i]               = events[i].flags; // Flags, bit 0 : external disc, bit 1 : peak valid, bit 2 : offset, bit 3 : sync error, bit 4 : general error, bit 5 : pileup only, bit 6 : pileup
        last_disc_timestamp[i] = events[i].last_disc_timestamp; // Last discriminator timestamp
        peak_timestamp[i]      = events[i].peak_timestamp; // Peak timestamp
        m1_begin_sample[i]     = events[i].m1_begin_sample; // M1 begin sample
        m1_end_sample[i]       = events[i].m1_end_sample; // M1 end sample
        m2_begin_sample[i]     = events[i].m2_begin_sample; // M2 begin sample
        m2_end_sample[i]       = events[i].m2_end_sample; // M2 end sample
        peak_sample[i]         = events[i].peak_sample; // Peak sample
        base_sample[i]         = events[i].base_sample; // Base sample
#endif
        if( saveTrace ){
          traceLen[i] = events[i].traceLength; // Trace length
          // if( events.size() > 1 && eventID == 1 ) printf(" %d |%d | Trace length: %d, id %d, timestamp %llu| ", eventID, i, traceLen[i], id[i], timestamp[i]);
          for( int j = 0 ; j < traceLen[i] && j < MAX_TRACE_LEN; j++){
            trace[i][j] = events[i].trace[j]; // Trace data
            // if( events.size() > 1 && eventID == 1) printf(" %d | %d\n", j, trace[i][j]);
          }
        }
      }
      outTree->Fill(); // Fill the TTree with the current event
    }
    
    double percentage = hitProcessed * 100/ totalNumHits;
    if( percentage >= last_precentage ) {
      size_t memoryUsage = sizeof(Event) * eventQueue.size();
      printf("Processed : %u, %.0f%% | %u/%lu | %.3f Mb", eventID, percentage, hitProcessed, totalNumHits, memoryUsage / (1024. * 1024.));
      printf(" \n\033[A\r");
      last_precentage = percentage + 1.0;
    }
    
    // prepare for the next event
    eventID ++;
    events.clear(); // Clear the events vector for the next event

  }while(!eventQueue.empty()); 

  //*=============== save some marco
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