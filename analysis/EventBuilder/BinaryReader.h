#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

#include "Hit.h"  

class BinaryReader {
public:
  BinaryReader(){ Init(); }
  BinaryReader(const std::string& filename) { Init(); Open(filename); }
  ~BinaryReader() {
    if (file.is_open()) file.close();
  }

  void Open(const std::string& filename);
  size_t GetFileSize() const { return fileSize; }  // Get size of the file in bytes
  template <typename T> T Read();
  template <typename T> std::vector<T> ReadArray(std::size_t count);
  std::vector<char> ReadBytes(std::size_t count);
  
  bool Eof() const { return file.eof(); }  // Check if end of file is reached
  bool IsGood() const { return file.good(); }  // Check if file is good
  size_t Tell() { return static_cast<size_t>(file.tellg()); }  // Get current file position
  void Seek(std::streampos pos) { file.seekg(pos); }   // Seek to position
  
  void Scan(bool quick = false);  // Scan the file to count data blocks
  unsigned int GetNumData() const { return numData; }  // Get number of data blocks found
  const std::vector<Hit>& GetHits() const { return hits; }  // Get vector of hits
  Hit GetHit(unsigned int index) const {
    if (index < hits.size()) {
      return hits[index];  // Return the hit at the specified index
    } else {
      throw std::out_of_range("Index out of range ");
    }
  }

  size_t GetMemoryUsageBytes();

private:
  std::ifstream file;
  size_t fileSize; // Size of the file in bytes

  unsigned int numData;

  std::vector<Hit> hits; // Vector to store hits

  void Init(){
    fileSize = 0;
    numData = 0;
  }
};

//########################################################################

inline void BinaryReader::Scan(bool quick) {

  Hit hit;
  numData = 0;
  unsigned filePos = 0; // byte
  uint32_t type = 0;
  uint64_t old_timestamp = 0;
  unsigned int timestamp_error_counter = 0;
  do{
    hit.header = Read<GEBHeader>();
    if( numData == 0) type = hit.header.type;
    
    // printf("Data ID : %d \n", numData);
    // header.Print();
    
    if( hit.header.type != type) {
      printf("header error at Data ID : %d \n", numData);
      break;
    }

    if( hit.header.timestamp < old_timestamp) {
      printf("timestamp error at Data ID : %d \n", numData);
      printf("old timestamp : %16lu \n", old_timestamp);
      printf("new timestamp : %16lu \n", hit.header.timestamp);
      timestamp_error_counter ++;
    }
    old_timestamp = hit.header.timestamp;
    
    if( quick ) {
      file.seekg( hit.header.payload_lenght_byte, std::ios::cur );
    }else {
      hit.payload = ReadArray<uint32_t>(hit.header.payload_lenght_byte / sizeof(uint32_t));
      hits.push_back(hit); // Store the hit in the vector
    }

    numData ++;
  }while(Tell() < fileSize);

  printf("number of Data Found : %d \n", numData);
  if( timestamp_error_counter > 0 ) printf("timestamp error found for %u times.\n", timestamp_error_counter);
  printf("go back to beginging of the file\n");
  file.seekg(0, std::ios::beg);

}


inline size_t BinaryReader::GetMemoryUsageBytes() {
  size_t total = sizeof(BinaryReader);
  total += hits.capacity() * sizeof(Hit);
  for (const auto& hit : hits) {
      total += sizeof(GEBHeader);
      total += hit.payload.capacity() * sizeof(uint32_t);
  }
  return total;
}


inline void BinaryReader::Open(const std::string& filename){
  file.open(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filename);
  }
  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  if (fileSize < sizeof(GEBHeader)) {
    throw std::runtime_error("File is too small to contain a GEBHeader.");
  }
}

template <typename T> inline T BinaryReader::Read() {
  T value;
  file.read(reinterpret_cast<char*>(&value), sizeof(T));
  if (!file) {
    throw std::runtime_error("Failed to read from file.");
    printf(" file position : %zu \n", Tell());
  }
  return value;
}

template <typename T> inline std::vector<T> BinaryReader::ReadArray(std::size_t count) {
  std::vector<T> buffer(count);
  file.read(reinterpret_cast<char*>(buffer.data()), count * sizeof(T));
  if (!file) {
    throw std::runtime_error("Failed to read array from file.");
  }
  return buffer;
}

inline std::vector<char> BinaryReader::ReadBytes(std::size_t count) {
  std::vector<char> buffer(count);
  file.read(buffer.data(), count);
  if (!file) {
    throw std::runtime_error("Failed to read bytes from file.");
  }
  return buffer;
}

