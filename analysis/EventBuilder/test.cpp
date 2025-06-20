#include "BinaryReader.h"

BinaryReader haha(10000);

void test(){
  
  haha.Open("run_30.gtd03_000_0109");

  haha.Scan(false);

  haha.ReadNextNHitsFromFile(true);

  size_t memSize = haha.GetMemoryUsageBytes();
  printf("Memory %zu bytes = %.3f MB\n", memSize, memSize / (1024.0 * 1024.0));  

  // haha.GetHit(0).Print();


  for( int i = 0; i < 20 ; i++){
    Hit hit = haha.GetHit(i);
    printf(" %3d | %lu \n", i,   hit.header.timestamp);
  }

  


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


}