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

  TFile * file[2];
  TTree * tree[2];
  unsigned long size[2];
  UInt_t numHit[2];
  ULong64_t timestamp[2][10000];  

  file[0] = new TFile("test1.root", "READ");
  file[1] = new TFile("test2.root", "READ");

  for( int i = 0; i < 2 ; i++ ){ 
    tree[i] = (TTree*)file[i]->Get("tree");
    size[i] = tree[i]->GetEntries();
    tree[i]->SetBranchAddress("NumHits", &numHit[i]);
    tree[i]->SetBranchAddress("event_timestamp", &timestamp[i]);

  }

  TH1F * h1 = new TH1F("h1", "Timestamp ", 3000, -1000, 2000);

  int ID = 1;
  for( unsigned long i = 0; i < size[ID]-1 ; i++ ){
    tree[ID]->GetEntry(i);

    ULong64_t ts1 = timestamp[ID][0];

    tree[ID]->GetEntry(i+1);

    ULong64_t ts2 = timestamp[ID][0];


    if( ts2 > ts1 ) {
      h1->Fill(ts2 - ts1);
    }else{
      h1->Fill((ts1 - ts2)*(-1.));
    }

    if( ts2 < ts1 ){
      printf("Entry %lu | %llu, %llu\n", i, ts1 , ts2);
    }

    if( ts2-ts1 < 1000 ){
      printf("Entry %lu | %llu, %llu\n", i, ts1 , ts2);
    }

  }

  h1->Draw();

  unsigned long maxEvent = 100;
  // for( unsigned long i = 0; i < size1 && i < size2 && i < maxEvent; i++){
  //   tree1->GetEntry(i);
  //   tree2->GetEntry(i);

  //   if( numHit1 != numHit2 ){
  //     printf("Mismatch at entry %lu: numHit1 = %d, numHit2 = %d\n", i, numHit1, numHit2);
  //     continue;
  //   }

  //   for( int j = 0; j < numHit1; j++){
  //     if( timestamp1[j] != timestamp2[j] ){
  //       printf("Mismatch at entry %lu, hit %d: timestamp1 = %llu, timestamp2 = %llu\n", i, j, timestamp1[j], timestamp2[j]);
  //     }
  //   }

  //   // Add more checks as needed
  // }






}