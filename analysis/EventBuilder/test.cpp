#include "BinaryReader.h"

BinaryReader haha(10000);

void test(){
  
  haha.Open("../data/h093_21F_d3He_run_040.gtd02_000_0105");
  // haha.Open("merged_file_000_000");

  haha.Scan(false);

  printf(" hit count: %d \n", haha.GetNumData());

  haha.ReadNextNHitsFromFile(true);

  size_t memSize = haha.GetMemoryUsageBytes();
  printf("Memory %zu bytes = %.3f MB\n", memSize, memSize / (1024.0 * 1024.0));  

  // haha.GetHit(0).Print();


  for( int i = 0; i < 10000; i++){
    Hit hit = haha.GetHit(i);
    if( hit.header.timestamp == 78478520128183 ){

      printf(" ====================== i = %d\n", i);

      hit.Print();
      Event event = hit.DecodePayload(true);
      event.Print();
      // printf(" %3d | %lu | %d | %d %d\n", i,   hit.header.timestamp, event.channel, event.pre_rise_energy, event.post_rise_energy);
    }
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