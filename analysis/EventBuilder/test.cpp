#include "BinaryReader.h"

BinaryReader haha(10000);

void test(){
  
  // haha.Open("run_30.gtd04_000_0113");
  haha.Open("merged_file_000_000");

  haha.Scan(false);

  printf(" hit count: %d \n", haha.GetNumData());

  haha.ReadNextNHitsFromFile(true);

  size_t memSize = haha.GetMemoryUsageBytes();
  printf("Memory %zu bytes = %.3f MB\n", memSize, memSize / (1024.0 * 1024.0));  

  // haha.GetHit(0).Print();


  for( int i = 0; i < 1 ; i++){
    Hit hit = haha.GetHit(i);
    printf("0x%08X \n", ntohl(hit.payload[7]));
    printf("0x%08X \n", ntohl(hit.payload[8]));
    Event event = hit.DecodePayload();
    event.Print();
    // printf(" %3d | %lu | %d | %d %d\n", i,   hit.header.timestamp, event.channel, event.pre_rise_energy, event.post_rise_energy);
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