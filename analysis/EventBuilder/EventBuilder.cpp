#include "BinaryReader.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TString.h"
#include "TMacro.h"

#include <sys/time.h> /** struct timeval, select() */
inline unsigned int getTime_us(){
  unsigned int time_us;
  struct timeval t1;
  struct timezone tz;
  gettimeofday(&t1, &tz);
  time_us = (t1.tv_sec) * 1000 * 1000 + t1.tv_usec;
  return time_us;
}


#define MAX_MULTI 64
#define MAX_TRACE_LEN 1250 ///  = 10 us

//unsigned long long                        evID = 0;
unsigned int                            numHit = 0;
unsigned short                   id[MAX_MULTI] = {0}; 
unsigned short      pre_rise_energy[MAX_MULTI] = {0};  
unsigned short     post_rise_energy[MAX_MULTI] = {0};  
unsigned long long        timestamp[MAX_MULTI] = {0};
unsigned int               traceLen[MAX_MULTI] = {0};
unsigned short trace[MAX_MULTI][MAX_TRACE_LEN] = {0};

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

  //*=============== setup reader
  uint64_t totalNumHits = 0;
  BinaryReader * reader = new BinaryReader[nFile];
  for( int i = 0 ; i < nFile ; i++){
    reader[i].Open(inFileName[i].Data());
    reader[i].Scan(true);
    totalNumHits += reader[i].GetNumData();
    // printf("File %s has %u hits\n", inFileName[i].Data(), reader[i].GetNumData());
  }
  
  //*=============== group files by DigID and sort the fileIndex
  std::map<unsigned short, std::vector<BinaryReader*>> fileGroups;
  for( int i = 0 ; i < nFile ; i++){
    unsigned short digID = reader[i].GetDigID();
    if (!fileGroups.count(digID) ) {
      fileGroups[digID] = std::vector<BinaryReader*>();
    }
    fileGroups[digID].push_back(&reader[i]);
  }
  printf("Found %zu DigIDs\n", fileGroups.size());

  // printf out the file groups
  for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
    unsigned short digID = group.first; // DigID
    const auto& readers = group.second; // Vector of BinaryReader pointers
    printf("----- DigID %03d has %zu files\n", digID, readers.size());
    for (size_t j = 0; j < readers.size(); j++) {
      printf("  File %zu: %s | num. of hit : %d\n", j, readers[j]->GetFileName().c_str(), readers[j]->GetNumData());
    }
  }

  printf("================= Total number of hits: %lu\n", totalNumHits);

  //*=============== create output file and setup TTree
  // TFile * outFile = TFile::Open(outFileName.Data(), "RECREATE");
  
  // TTree * outTree = new TTree("tree", outFileName.Data());
  // outTree->SetAutoSave(10000000); // autosave every 10 million bytes
  // outTree->SetAutoFlush(1000000); // auto-flush every 1 million bytes 
  // outTree->SetDirectory(outFile);

  // outTree->Branch("numHit", &numHit, "numHit/i");
  // outTree->Branch("id", id, "id[numHit]/s");
  // outTree->Branch("pre_rise_energy", pre_rise_energy, "pre_rise_energy[numHit]/s");
  // outTree->Branch("post_rise_energy", post_rise_energy, "post_rise_energy[numHit]/s");
  // outTree->Branch("timestamp", timestamp, "timestamp[numHit]/l");
  // if( saveTrace ){
  //   outTree->Branch("traceLen", traceLen, "traceLen[numHit]/i");
  //   outTree->Branch("   trace",    trace, "trace[numHit][traceLen[numHit]]/s");
  // } 


  //*=============== read n data from each file
  const unsigned int MAX_READ_HITS = 10000; // Maximum hits to read at a time
  
  
  uint64_t earlistTime = UINT64_MAX; 
  int earlistDigID = -1; // Earliest DigID with the earliest timestamp
  std::map<unsigned short, unsigned int> hitID; // store the hit ID for the current reader for each DigID
  std::map<unsigned short, short> fileID; // store the file ID for each DigID
  
  for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
    unsigned short digID = group.first; // DigID
    const auto& readers = group.second; // Vector of BinaryReader pointers

    hitID[digID] = 0; // Initialize hitID for this DigID
    fileID[digID] = 0; 

    BinaryReader* reader = readers[0];
    reader->ReadNextNHitsFromFile(10000); // Read 10,000 hits at a time

    if( reader->GetHit(0).header.timestamp < earlistTime ) {
      earlistTime = reader->GetHit(0).header.timestamp; // Update the earliest timestamp
      earlistDigID = digID; // Update the earliest DigID
    }

    totalNumHits += reader->GetNumData();
  }

  
  //*=============== event building
  std::vector<Event> events; // Vector to store events
  unsigned int eventID = 0;
  
  int debug = 0;
  int debug2 = 10; 
  do{

    printf("##### Earliest timestamp: %lu from DigID %03d\n", earlistTime, earlistDigID);

    for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
      unsigned short digID = group.first; // DigID
      if( fileID[digID] < 0 ) continue; // Skip if no more files for this DigID
      const std::vector<BinaryReader*>& readers = group.second; // Vector of BinaryReader pointers

      const std::vector<Hit> hits = readers[fileID[digID]]->GetHits();
      
      if( hitID[digID] >= hits.size() ) {
        readers[0]->ReadNextNHitsFromFile(MAX_READ_HITS); // Read more hits if needed
        hitID[digID] = 0; // Reset hitID for this DigID
      }

      if( hits.empty() ) { // load next file if no hits
        if( fileID[digID] < readers.size() - 1 ) {
          fileID[digID]++;
          readers[fileID[digID]]->ReadNextNHitsFromFile(MAX_READ_HITS); // Read more hits from the next file
          hitID[digID] = 0; // Reset hitID for this DigID
        } else {
          printf("No more files for DigID %03d\n", digID);
          fileID[digID] = -1; // Mark that there are no more files for this DigID
          continue; // No more files to read for this DigID
        }
      }

      
      if( timeWindow < 0 ) {
        printf("Processing DigID %03d, Hit ID %u, file ID %d, Timestamp %lu\n", digID, hitID[digID], fileID[digID], hits[hitID[digID]].header.timestamp);
        events.push_back(hits[hitID[digID]].DecodePayload()); // Decode the hit payload and store it in the events vector
        numHit++;
        hitID[digID]++; // Move to the next hit
        break;
      }
      
      for (int i = hitID[digID]; i < hits.size(); i++) {  
        printf("Processing DigID %03d, Hit ID %u, file ID %d, Timestamp %lu\n", digID, hitID[digID], fileID[digID], hits[hitID[digID]].header.timestamp);
        if( hits[i].header.timestamp - earlistTime  <= timeWindow) {
          events.push_back(hits[i].DecodePayload()); // Decode the hit payload and store it in the events vector
          numHit++;
          hitID[digID]++; // Move to the next hit
        }else{
          printf("----- break\n");
          break;
        }      
      }
    }

    printf("EventID : %u, Number of Hits in this event : %zu| Total hits read: %u\n", eventID, events.size(), numHit);

    //TODO save to TTree
    if( events.size() > 0 ) {
      std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return a.timestamp < b.timestamp; // Sort events by timestamp
      });
    }


    // prepare for the next event
    eventID ++;
    events.clear(); // Clear the events vector for the next event

    // find the next earliest timestamp
    earlistTime = UINT64_MAX;
    earlistDigID = -1; // Reset the earliest DigID
    for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
      unsigned short digID = group.first; // DigID
      const auto& readers = group.second; // Vector of BinaryReader pointers    

      if( fileID[digID] < 0 ) continue; // Skip if no more files for this DigID

      BinaryReader* reader = readers[fileID[digID]];
      if( hitID[digID] < reader->GetNumData() ) {
        if( reader->GetHit(hitID[digID]).header.timestamp < earlistTime ) {
          earlistTime = reader->GetHit(hitID[digID]).header.timestamp; // Update the earliest timestamp
          earlistDigID = digID; // Update the earliest DigID
        }
      } else {
        if( fileID[digID] < readers.size() - 1 ) {
          fileID[digID]++;
          reader = readers[fileID[digID]];
          if( reader->GetHit(0).header.timestamp < earlistTime ) {
            earlistTime = reader->GetHit(0).header.timestamp; // Update the earliest timestamp
            earlistDigID = digID; // Update the earliest DigID
          }
        }else{
          fileID[digID] = -1; // Mark that there are no more files for this DigID
        }
      }
    }

  }while(debug < debug2 && earlistDigID >= 0);
  
  
  return 0;
}