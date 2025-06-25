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
  unsigned short geo_addr;  // Geo address
  unsigned short flags;  // Flags, bit 0 : external disc, bit 1 : peak valid, bit 2 : offset, bit 3 : sync error, bit 4 : general error, bit 5 : pileup only, bit 6 : pileup
  uint64_t last_disc_timestamp;  // Last discriminator timestamp
  uint32_t peak_timestamp;  // Peak timestamp
  unsigned short m1_begin_sample;  // M1 begin sample
  unsigned short m1_end_sample;  // M1 end sample
  unsigned short m2_begin_sample;  // M2 begin sample  
  unsigned short m2_end_sample;  // M2 end sample
  unsigned short peak_sample;  // Peak sample
  unsigned short base_sample;  // Base sample

  unsigned short cfd_sample_0;
  unsigned short cfd_sample_1;
  unsigned short cfd_sample_2;

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
    printf("  Payload size: %zu words\n", payload.size());
    unsigned short header_type   = (ntohl(payload[2]) >> 16) & 0xF; // for data type
    uint16_t packet_length = (ntohl(payload[0]) >> 16) & 0x7FF;
    printf("  header type : %u\n", header_type);
    printf("packet length : %u words\n", packet_length);
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
    event.channel          = word & 0xF; 
    event.board            = (word >> 4) & 0xFFF;
    event.geo_addr         = (word >> 27) & 0x1F;
    uint16_t packet_length = (word >> 16) & 0x7FF;
    
    word = ntohl(payload[2]);
    unsigned short header_type   = (word >> 16) & 0xF; // for data type
    unsigned short event_type    = (word >> 22) & 0xF;
    unsigned short header_length = (word >> 26) & 0x3F;

    //^====================== Original LED header (Compatibility mode)
    if( header_type == 0 ){ 

      word = ntohl(payload[3]);
      bool external_disc_flag     = 0 ;
      bool pileup_only_flag       = 0 ;
      bool peak_valid_flag        = ((word & 0x00000200) >> 9);                               // Word 3: 9
      bool offset_flag            = ((word & 0x00000400) >> 10);                              // Word 3: 10
      bool sync_error_flag        = ((word & 0x00001000) >> 12);                              // Word 3: 12
      bool general_error_flag     = ((word & 0x00002000) >> 13);                              // Word 3: 13
      bool pileup_flag            = ((word & 0x00008000) >> 15);                              // Word 3: 15


      event.flags = external_disc_flag | 
                    (peak_valid_flag << 1) | 
                    (offset_flag << 2) | 
                    (sync_error_flag << 3) | 
                    (general_error_flag << 4) | 
                    (pileup_only_flag << 5) | 
                    (pileup_flag << 6);
                    
      event.last_disc_timestamp    = (((unsigned long long int)(word & 0xFFFF0000)) >> 16 );   // Word 3: 31..16 & 
      event.last_disc_timestamp   += (((unsigned long long int)(ntohl(payload[4]) & 0xFFFFFFFF)) << 16);    // Word 4 :31..0  
      
      event.baseline               = ((ntohl(payload[5]) & 0x00FFFFFF) >> 0);                               // Word 5: 23..0
      event.pre_rise_energy        = ((ntohl(payload[7]) & 0x00FFFFFF) >> 0);                               // Word 7: 23..0
      event.post_rise_energy       = ((ntohl(payload[7]) & 0xFF000000) >> 28) |                             // Word 7: 31..24 & 
                                     ((ntohl(payload[8]) & 0x0000FFFF) << 8);                               // Word 8: 15..0 
      event.peak_timestamp         = (((unsigned long long int)(ntohl(payload[8]) & 0xFFFF0000)) >> 16 )|   // Word 8: 31..16 & 
                                     (((unsigned long long int)(ntohl(payload[9]) & 0xFFFFFFFF)) << 16);    // Word 9 :31..0  
      event.m1_begin_sample        = ((ntohl(payload[10]) & 0x00003FFF) >> 0);                              // Word 10:13..0
      event.m1_end_sample          = ((ntohl(payload[10]) & 0x3FFF0000) >> 16);                             // Word 10:29..16
      event.m2_begin_sample        = ((ntohl(payload[11]) & 0x00003FFF) >> 0);                              // Word 11:13..0
      event.m2_end_sample          = ((ntohl(payload[11]) & 0x3FFF0000) >> 16);                             // Word 11:29..16
      event.peak_sample            = ((ntohl(payload[12]) & 0x00003FFF) >> 0);                              // Word 12:13..0
      event.base_sample            = ((ntohl(payload[12]) & 0x3FFF0000) >> 16);                             // Word 12:29..16
      
    }

    //^====================== LED header
    if( header_type == 1 ){ 
      bool timestamp_match_flag   = 0;
      bool cfd_valid_flag         = 0;
                        
      word = ntohl(payload[3]);
      bool external_disc_flag     = ((word & 0x00000100) >> 8);                               // Word 3: 8
      bool peak_valid_flag        = ((word & 0x00000200) >> 9);                               // Word 3: 9
      bool offset_flag            = ((word & 0x00000400) >> 10);                              // Word 3: 10
      bool sync_error_flag        = ((word & 0x00001000) >> 12);                              // Word 3: 12
      bool general_error_flag     = ((word & 0x00002000) >> 13);                              // Word 3: 13
      bool pileup_only_flag       = ((word & 0x00004000) >> 14);                              // Word 3: 14
      bool pileup_flag            = ((word & 0x00008000) >> 15);                              // Word 3: 15  
      
      event.flags = external_disc_flag | 
                    (peak_valid_flag << 1) | 
                    (offset_flag << 2) | 
                    (sync_error_flag << 3) | 
                    (general_error_flag << 4) | 
                    (pileup_only_flag << 5) | 
                    (pileup_flag << 6);

      event.last_disc_timestamp    = (((unsigned long long int)(word & 0xFFFF0000)) >> 16 );   // Word 3: 31..16 & 
      event.last_disc_timestamp   += (((unsigned long long int)(ntohl(payload[4]) & 0xFFFFFFFF)) << 16);    // Word 4 :31..0  

      event.baseline               = ((ntohl(payload[5]) & 0x00FFFFFF) >> 0);                               // Word 5: 23..0
      event.pre_rise_energy        = ((ntohl(payload[7]) & 0x00FFFFFF) >> 0);                               // Word 7: 23..0
      event.post_rise_energy       = ((ntohl(payload[7]) & 0xFF000000) >> 28) |                             // Word 7: 31..24 & 
                                    ((ntohl(payload[8]) & 0x0000FFFF) << 8);                               // Word 8: 15..0 
      event.peak_timestamp         = (((unsigned long long int)(ntohl(payload[8]) & 0xFFFF0000)) >> 16 )|   // Word 8: 31..16 & 
                                    (((unsigned long long int)(ntohl(payload[9]) & 0xFFFFFFFF)) << 16);    // Word 9 :31..0  
      event.m1_begin_sample        = ((ntohl(payload[10]) & 0x00003FFF) >> 0);                              // Word 10:13..0
      event.m1_end_sample          = ((ntohl(payload[10]) & 0x3FFF0000) >> 16);                             // Word 10:29..16
      event.m2_begin_sample        = ((ntohl(payload[11]) & 0x00003FFF) >> 0);                              // Word 11:13..0
      event.m2_end_sample          = ((ntohl(payload[11]) & 0x3FFF0000) >> 16);                             // Word 11:29..16
      event.peak_sample            = ((ntohl(payload[12]) & 0x00003FFF) >> 0);                              // Word 12:13..0
      event.base_sample            = ((ntohl(payload[12]) & 0x3FFF0000) >> 16);                             // Word 12:29..16

    }

    //^====================== CFD
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


      event.last_disc_timestamp  = (word & 0xFFFF0000) >> 16; 
      event.last_disc_timestamp |= (static_cast<uint64_t>(ntohl(payload[4])) << 16);

      event.cfd_sample_0           = ((ntohl(payload[4]) & 0x3FFF0000) >> 16);                              // Word 4: 29..16
      event.baseline               = ((ntohl(payload[5]) & 0x00FFFFFF) >> 0);                               // Word 5: 23..0
      event.cfd_sample_1           = ((ntohl(payload[6]) & 0x00003FFF) >> 0);                               // Word 6: 13..0
      event.cfd_sample_2           = ((ntohl(payload[6]) & 0x3FFF0000) >> 16);                              // Word 6: 29..16
      event.pre_rise_energy        = ((ntohl(payload[7]) & 0x00FFFFFF) >> 0);                               // Word 7: 23..0
      event.post_rise_energy       = ((ntohl(payload[7]) & 0xFF000000) >> 28) |                             // Word 7: 31..24 & 
                                         ((ntohl(payload[8]) & 0x0000FFFF) << 8);                               // Word 8: 15..0 

      event.peak_timestamp         = (((unsigned long long int)(ntohl(payload[8]) & 0xFFFF0000)) >> 16 )|   // Word 8: 31..16 & 
      (((unsigned long long int)(ntohl(payload[9]) & 0xFFFFFFFF)) << 16);    // Word 9 :31..0  
      event.m1_begin_sample        = ((ntohl(payload[10]) & 0x00003FFF) >> 0);                              // Word 10:13..0
      event.m1_end_sample          = ((ntohl(payload[10]) & 0x3FFF0000) >> 16);                             // Word 10:29..16
      event.m2_begin_sample        = ((ntohl(payload[11]) & 0x00003FFF) >> 0);                              // Word 11:13..0
      event.m2_end_sample          = ((ntohl(payload[11]) & 0x3FFF0000) >> 16);                             // Word 11:29..16
      event.peak_sample            = ((ntohl(payload[12]) & 0x00003FFF) >> 0);                              // Word 12:13..0
      event.base_sample            = ((ntohl(payload[12]) & 0x3FFF0000) >> 16);                             // Word 12:29..16

    }

    //^====================== HELIOS 2015/16 LED 
    if( header_type == 3 ){ 
      word = ntohl(payload[3]);
      bool external_disc_flag = (word >>  8) & 0x1;
      bool peak_valid_flag    = (word >>  9) & 0x1;
      bool offset_flag        = (word >> 10) & 0x1;
      bool sync_error_flag    = (word >> 12) & 0x1;
      bool general_error_flag = (word >> 13) & 0x1;
      bool pileup_only_flag   = (word >> 14) & 0x1;
      bool pileup_flag        = (word >> 15) & 0x1;
      
      event.flags = external_disc_flag | 
                    (peak_valid_flag << 1) | 
                    (offset_flag << 2) | 
                    (sync_error_flag << 3) | 
                    (general_error_flag << 4) | 
                    (pileup_only_flag << 5) | 
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