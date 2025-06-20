#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <arpa/inet.h>

class GEBHeader{
public:
  uint32_t type;
  uint32_t payload_lenght_byte;
  uint64_t timestamp;

  void Print() const{
    printf("           type : 0x%08X = %d\n", type, type);
    printf(" payload lenght : 0x%08X = %d bytes\n", payload_lenght_byte, payload_lenght_byte);
    printf("      timestamp : 0x%016lX = %lu\n", timestamp, timestamp);
  }
};

class Event{
public:
  unsigned channel;  // Channel ID
  unsigned board;    // Board ID
  
  uint64_t timestamp;  // Timestamp in nanoseconds
  uint32_t pre_rise_energy;  // Pre-rise energy
  uint32_t post_rise_energy;  // Post-rise energy
  
  uint16_t traceLength;  // trace length in samples
  std::vector<uint16_t> trace;  // Trace data
  
  // not very useful, but kept for compatibility
  uint32_t baseline;  // Baseline value
  unsigned geo_addr;  // Geo address
  unsigned short flags;  // Flags, bit 0 : external disc, bit 1 : peak valid, bit 2 : offset, bit 3 : sync error, bit 4 : general error, bit 5 : pileup only, bit 6 : pileup
  uint64_t last_disc_timestamp;  // Last discriminator timestamp
  uint32_t peak_timestamp;  // Peak timestamp
  unsigned m1_begin_sample;  // M1 begin sample
  unsigned m1_end_sample;  // M1 end sample
  unsigned m2_begin_sample;  // M2 begin sample  
  unsigned m2_end_sample;  // M2 end sample
  unsigned peak_sample;  // Peak sample
  unsigned base_sample;  // Base sample

  void Print() const {

    printf("         Channel ID: %u\n", channel);
    printf("           Board ID: %u\n", board);      
    printf("        Geo Address: %u\n", geo_addr);
    printf("        flags (bit): %u\n", flags);
    printf("Last Disc Timestamp: %lu\n", last_disc_timestamp);
    printf("           Baseline: %u\n", baseline);
    printf("    Pre Rise Energy: %u\n", pre_rise_energy);
    printf("   Post Rise Energy: %u\n", post_rise_energy);
    printf("     Peak Timestamp: %u\n", peak_timestamp);
    printf("    M1 Begin Sample: %u\n", m1_begin_sample);
    printf("      M1 End Sample: %u\n", m1_end_sample);
    printf("    M2 Begin Sample: %u\n", m2_begin_sample);
    printf("      M2 End Sample: %u\n", m2_end_sample);
    printf("        Peak Sample: %u\n", peak_sample);
    printf("        Base Sample: %u\n", base_sample);

    printf("       Trace Length: %u samples\n", traceLength);
    for( int i = 0; i < traceLength && i < 1000; i++){
      printf("Trace[%3d]: %6u\n", i, trace[i]);
    }
  }
};

class Hit {
public:
  GEBHeader header;  // Header information
  std::vector<uint32_t> payload;  // Payload data

  void Print() const {
    header.Print();
    printf("Payload size: %zu words\n", payload.size());
    for (size_t i = 0; i < payload.size(); ++i) {
      printf("%3zu: 0x%08X\n", i, ntohl(payload[i]));
    }
  }

  Event DecodePayload(bool saveTrace = false) const {
    // Implement payload decoding logic here if needed
    // For now, just printing the payload size
    // printf("Decoding payload of size: %zu words\n", payload.size());

    Event event;
    
    event.timestamp = header.timestamp; // Use the timestamp from the header

    unsigned int word = ntohl(payload[0]);
    event.channel       = word & 0xF; 
    event.board         = (word >> 4) & 0xFFF;
    event.geo_addr      = (word >> 27) & 0x1F;
    uint16_t packet_length = (word >> 16) & 0x7FF;
    
    word = ntohl(payload[2]);
    unsigned short header_type   = (word >> 16) & 0xF; // for data type
    unsigned short event_type    = (word >> 22) & 0xF;
    unsigned short header_length = (word >> 26) & 0x3F;

    if( header_type == 2 ){
      word = ntohl(payload[3]);
      bool timestamp_match_flag = (word >> 7) & 0x1;
      bool external_disc_flag   = (word >>  8) & 0x1;
      bool peak_valid_flag      = (word >>  9) & 0x1;
      bool offset_flag          = (word >> 10) & 0x1;
      bool sync_error_flag      = (word >> 12) & 0x1;
      bool general_error_flag   = (word >> 13) & 0x1;
      bool pileip_only_flag     = (word >> 14) & 0x1;
      bool pileup_flag          = (word >> 15) & 0x1;
      
      event.flags = external_disc_flag | 
                    (peak_valid_flag << 1) | 
                    (offset_flag << 2) | 
                    (sync_error_flag << 3) | 
                    (general_error_flag << 4) | 
                    (pileip_only_flag << 5) | 
                    (pileup_flag << 6) |
                    (timestamp_match_flag << 7);


      event.last_disc_timestamp = (word & 0xFFFF0000) >> 16; 
      word = ntohl(payload[4]);
      event.last_disc_timestamp |= (static_cast<uint64_t>(word) << 16);

      //not Complete

    }

    if( header_type == 3 ){
  
      word = ntohl(payload[3]);
      bool external_disc_flag = (word >>  8) & 0x1;
      bool peak_valid_flag    = (word >>  9) & 0x1;
      bool offset_flag        = (word >> 10) & 0x1;
      bool sync_error_flag    = (word >> 12) & 0x1;
      bool general_error_flag = (word >> 13) & 0x1;
      bool pileip_only_flag   = (word >> 14) & 0x1;
      bool pileup_flag        = (word >> 15) & 0x1;
      
      event.flags = external_disc_flag | 
                    (peak_valid_flag << 1) | 
                    (offset_flag << 2) | 
                    (sync_error_flag << 3) | 
                    (general_error_flag << 4) | 
                    (pileip_only_flag << 5) | 
                    (pileup_flag << 6);
      
      event.last_disc_timestamp = (word & 0xFFFF0000) >> 16; 
      word = ntohl(payload[4]);
      event.last_disc_timestamp |= (static_cast<uint64_t>(word) << 16); 
    
      event.baseline         =  ntohl(payload[5]) & 0x00FFFFFF; 
      event.pre_rise_energy  =  ntohl(payload[7]) & 0x00FFFFFF;
      event.post_rise_energy = (ntohl(payload[7]) >> 28) & 0xFF   // should be  24 ??
                                | ((ntohl(payload[8]) & 0x0000FFFF) << 8);
    
      event.peak_timestamp = ntohl(payload[8])  >> 16;
    
      event.m1_begin_sample =  ntohl(payload[10]) & 0x00003FFF;
      event.m1_end_sample   = (ntohl(payload[10]) >> 16) & 0x3FFF;
      event.m2_begin_sample =  ntohl(payload[11]) & 0x00003FFF;
      event.m2_end_sample   = (ntohl(payload[11]) >> 16) & 0x3FFF;
      event.peak_sample     =  ntohl(payload[12]) & 0x00003FFF;
      event.base_sample     = (ntohl(payload[12]) >> 16) & 0x3FFF;

    }
  
    if( saveTrace ){  
      event.traceLength = (packet_length - 13)*2;
      for( int i = 13; i < payload.size(); i++){
        uint32_t word = ntohl(payload[i]);
        event.trace.push_back(word & 0xFFFF);
        event.trace.push_back(word >> 16);
      }
    }

    return event;

  }

};