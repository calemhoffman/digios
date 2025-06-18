#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

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

class Hit {
public:
  GEBHeader header;  // Header information
  std::vector<uint32_t> payload;  // Payload data

  void Print() const {
    header.Print();
    printf("Payload size: %zu words\n", payload.size());
    for (size_t i = 0; i < payload.size(); ++i) {
      printf("%3zu: 0x%08X\n", i, payload[i]);
    }
  }

  void DecodePayload(){
    // Implement payload decoding logic here if needed
    // For now, just printing the payload size
    printf("Decoding payload of size: %zu words\n", payload.size());
  }

};