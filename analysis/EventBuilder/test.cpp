#include "BinaryReader.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

// BinaryReader haha(10000);

void test(){
  
  // haha.Open("../data/h093_21F_d3He_run_040.gtd02_000_0105");
  // // haha.Open("merged_file_000_000");

  // haha.Scan(false);

  // printf(" hit count: %d \n", haha.GetNumData());

  // haha.ReadNextNHitsFromFile(true);

  // size_t memSize = haha.GetMemoryUsageBytes();
  // printf("Memory %zu bytes = %.3f MB\n", memSize, memSize / (1024.0 * 1024.0));  

  // // haha.GetHit(0).Print();


  // for( int i = 0; i < 10000; i++){
  //   Hit hit = haha.GetHit(i);
  //   if( hit.header.timestamp == 78478520128183 ){

  //     printf(" ====================== i = %d\n", i);

  //     hit.Print();
  //     Event event = hit.DecodePayload(true);
  //     event.Print();
  //     // printf(" %3d | %lu | %d | %d %d\n", i,   hit.header.timestamp, event.channel, event.pre_rise_energy, event.post_rise_energy);
  //   }
  // }

  


  // Event event = hit.DecodePayload();

  // event.Print();


  // GEBHeader header;

  // header = haha.Read<GEBHeader>();
  // header.Print();

  // haha.Seek( sizeof(GEBHeader) + header.payload_lenght_byte );

  // header = haha.Read<GEBHeader>();
  // header.Print();


  
  // uint32_t word;

  // for( int i = 0; i< 220 ; i++){
  //   word = haha.read<uint32_t>();
  //   printf("%3d | 0x%08X\n", i, word);
  // }


  //############# compare tree


  TFile * file1 = new TFile("test1.root", "READ");
  TTree * tree1 = (TTree*)file1->Get("tree");
  
  unsigned long size1 = tree1->GetEntries();

  UInt_t numHit1;
  ULong64_t timestamp1[10000];  

  tree1->SetBranchAddress("NumHits", &numHit1);
  tree1->SetBranchAddress("event_timestamp", timestamp1);

  TFile * file2 = new TFile("test2.root", "READ");
  TTree * tree2 = (TTree*)file2->Get("tree");

  unsigned long size2 = tree2->GetEntries();

  UInt_t numHit2;
  ULong64_t timestamp2[10000];  

  tree2->SetBranchAddress("NumHits", &numHit2);
  tree2->SetBranchAddress("event_timestamp", timestamp2);

  unsigned long maxEvent = 100;

  for( unsigned long i = 0; i < size1 && i < size2 && i < maxEvent; i++){
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    if( numHit1 != numHit2 ){
      printf("Mismatch at entry %lu: numHit1 = %d, numHit2 = %d\n", i, numHit1, numHit2);
      continue;
    }

    for( int j = 0; j < numHit1; j++){
      if( timestamp1[j] != timestamp2[j] ){
        printf("Mismatch at entry %lu, hit %d: timestamp1 = %llu, timestamp2 = %llu\n", i, j, timestamp1[j], timestamp2[j]);
      }
    }

    // Add more checks as needed
  }






}