#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

#include "Hit.h"  

class BinaryReader {
public:
  BinaryReader(){ Init(); }
  BinaryReader(const std::string& filename) { Init(); Open(filename); }
  ~BinaryReader() {
    if (file.is_open()) file.close();
  }

  void Open(const std::string& filename);
  std::string GetFileName() const { return fileName; }  // Get the file name
  unsigned short GetDigID() const { return DigID; }  // Get the last three digits of the file name
  unsigned short GetFileIndex() const { return fileIndex; }  // Get the second last three digits of the file name
  size_t GetFileSize() const { return fileSize; }  // Get size of the file in bytes

  template <typename T> T Read();
  template <typename T> std::vector<T> ReadArray(std::size_t count);
  std::vector<char> ReadBytes(std::size_t count);
  
  bool Eof() const { return file.eof(); }  // Check if end of file is reached
  bool IsGood() const { return file.good(); }  // Check if file is good
  size_t Tell() { return static_cast<size_t>(file.tellg()); }  // Get current file position
  void Seek(std::streampos pos) { file.seekg(pos); }   // Seek to position
  
  void Scan(bool quick = false, bool debug = false);  // Scan the file to count data blocks
  unsigned int GetNumData() const { return totalNumHits; }  // Get number of data blocks found
  const std::vector<Hit>& GetHits() const { return hits; }  // Get vector of hits
  Hit GetHit(unsigned int index) const {
    if (index < hits.size()) {
      return hits[index];  // Return the hit at the specified index
    } else {
      throw std::out_of_range("Index out of range ");
    }
  }

  void ClearHits() { hits.clear(); }  // Clear the hits vector
  size_t GetMemoryUsageBytes();

  void ReadNextNHitsFromFile(unsigned int n);

private:
  std::ifstream file;
  std::string fileName;
  size_t fileSize; // Size of the file in bytes
  unsigned short DigID; // the last three digits of the file name, e.g. 001, 002, etc.
  unsigned short fileIndex; // the 2nd last three digits of the file name, e.g. 000, 001, etc. to indicate the next file from the same DigID.

  unsigned int totalNumHits;
  unsigned int hitID; // current hit ID

  std::vector<Hit> hits; // Vector to store hits

  void Init(){
    fileSize = 0;
    DigID = 0;
    fileIndex = 0;
    totalNumHits = 0;
    hitID = 0;
    hits.clear();
  }
};

//########################################################################

inline void BinaryReader::Scan(bool quick, bool debug) {

  Hit hit;
  totalNumHits = 0;
  unsigned filePos = 0; // byte
  uint32_t type = 0;
  uint64_t old_timestamp = 0;
  unsigned int timestamp_error_counter = 0;
  do{
    hit.header = Read<GEBHeader>();
    if( totalNumHits == 0) type = hit.header.type;
    
    // printf("Data ID : %d \n", totalNumHits);
    // header.Print();
    
    if( hit.header.type != type) {
      printf("header error at Data ID : %d \n", totalNumHits);
      break;
    }

    if( hit.header.timestamp < old_timestamp) {
      if( debug ) printf("timestamp error at Data ID : %d \n", totalNumHits);
      if( debug ) printf("old timestamp : %16lu \n", old_timestamp);
      if( debug ) printf("new timestamp : %16lu \n", hit.header.timestamp);
      timestamp_error_counter ++;
    }
    old_timestamp = hit.header.timestamp;
    
    if( quick ) {
      file.seekg( hit.header.payload_lenght_byte, std::ios::cur );
    }else {
      hit.payload = ReadArray<uint32_t>(hit.header.payload_lenght_byte / sizeof(uint32_t));
      hits.push_back(hit); // Store the hit in the vector
    }

    totalNumHits ++;
  }while(Tell() < fileSize);

  if( debug ) printf("number of Hit Found : %d \n", totalNumHits);
  if( timestamp_error_counter > 0 && debug ) printf("timestamp error found for %u times.\n", timestamp_error_counter);
  file.seekg(0, std::ios::beg);
  hitID = 0; // Reset hitID to 0 after scanning

}

inline void BinaryReader::ReadNextNHitsFromFile(unsigned int n) {

  printf("Reading next %u hits from file: %s |", n, fileName.c_str());
  hits.clear();  // Clear previous hits
  uint64_t old_timestamp = 0;
  unsigned int timestamp_error_counter = 0;
  for (unsigned int i = 0; i < n && Tell() < fileSize; ++i) {
    Hit hit;
    hit.header = Read<GEBHeader>();

    if( hit.header.timestamp < old_timestamp) {
      printf("timestamp error at Data ID : %d \n", i);
      printf("old timestamp : %16lu \n", old_timestamp);
      printf("new timestamp : %16lu \n", hit.header.timestamp);
      timestamp_error_counter++;
    }
    old_timestamp = hit.header.timestamp;

    if (hit.header.payload_lenght_byte > 0) {
      hit.payload = ReadArray<uint32_t>(hit.header.payload_lenght_byte / sizeof(uint32_t));
      hits.push_back(hit);  // Store the hit in the vector
    }
  }

  //sort hits by timestamp
  if( timestamp_error_counter > 0 ) {
    printf("timestamp error found for %u times. Sort data.\n", timestamp_error_counter);
    std::sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
      return a.header.timestamp < b.header.timestamp;
    });
  }
  hitID += n;  // Update the hitID to the next position

  printf(" first timestamp: %lu, last timestamp: %lu, total hits read: %zu\n", 
         hits.front().header.timestamp, 
         hits.back().header.timestamp, 
         hits.size());
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
  // printf("============== Opened file: %s\n", filename.c_str());
  this->fileName = filename;
  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  if (fileSize < sizeof(GEBHeader)) {
    throw std::runtime_error("File is too small to contain a GEBHeader.");
  }
  //decode the DigID and fileIndex from the filename
  std::string baseName = filename.substr(filename.find_last_of("/\\") + 1);
  if (baseName.length() >= 6) {
    try {
      DigID = std::stoi(baseName.substr(baseName.length() - 3, 3));
      fileIndex = std::stoi(baseName.substr(baseName.length() - 7, 3));
    } catch (const std::invalid_argument & e) {
      throw std::runtime_error("Failed to parse DigID and fileIndex from filename: " + filename);
    }
  } else {
    throw std::runtime_error("Filename is too short to contain DigID and fileIndex: " + filename);
  }
  // printf("Opened file: %s\n", filename.c_str());
  // printf("DigID: %03d, fileIndex: %03d\n", DigID, fileIndex);
  // if (fileSize > 0) {
  //   printf("File size: %zu bytes\n", fileSize);
  // } else {
  //   throw std::runtime_error("File is empty: " + filename); 
  // }
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

