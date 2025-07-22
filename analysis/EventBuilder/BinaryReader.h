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
  BinaryReader(unsigned int maxHitSize){ // Constructor with hit size
    Init(); 
    CreateHits(maxHitSize);
  }  
  BinaryReader(const std::string& filename, unsigned int maxHitSize) {  // Constructor with filename and hit size
    Init(); 
    Open(filename); 
    CreateHits(maxHitSize);
  } 
  ~BinaryReader() {
    if (file.is_open()) file.close();
    DeleteHits();
  }

  void Open(const std::string& filename);
  std::string GetFileName() const { return fileName; }  // Get the file name
  unsigned short GetRunID() const { return runID; }  // Get the run ID
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
  
  void ResetFile() {
    file.clear();  // Clear any error flags
    file.seekg(0, std::ios::beg);  // Seek to the beginning of the file
    hitID = 0;  // Reset hit ID
    hitSize = 0;  // Reset hit size
    lastTimestampOfHits = 0;  // Reset last timestamp of hits
  }

  uint64_t GetGlobalEarliestTime() const { return globalEarliestTime; }  
  uint64_t GetGlobalLastTime() const { return globalLastTime; } 

  void ReadNextNHitsFromFile(bool debug = false); 
  unsigned int GetMaxHitSize() const { return maxHitSize; }  // Get the maximum size of the hits vector
  unsigned int GetHitSize() const { return hitSize; }  // Get the size of the hits vector
  const Hit * GetHits() const { return hits; }  // Get vector of hits
  Hit GetHit(unsigned int index) const {
    if (index < maxHitSize) {
      return hits[index];  // Return the hit at the specified index
    } else {
      throw std::out_of_range("Index out of range ");
    }
  }

  void CreateHits(unsigned int size) {
    if (hits != nullptr) {
      delete[] hits;  // Delete the existing hits array
    }
    hits = new Hit[size];  // Create a new hits array with the specified size
    maxHitSize = size;  // Update the maximum hit size
    hitSize = 0;  // Reset hit size
  }

  void DeleteHits() { 
    delete[] hits;  // Delete the hits array
    hits = nullptr;  // Set hits to nullptr
    maxHitSize = 0;  // Reset hit size
  } 
  size_t GetMemoryUsageBytes();


private:
  std::ifstream file;
  std::string fileName;
  size_t fileSize; // Size of the file in bytes
  unsigned short runID;
  unsigned short DigID; // the last three digits of the file name, e.g. 001, 002, etc.
  unsigned short fileIndex; // the 2nd last three digits of the file name, e.g. 000, 001, etc. to indicate the next file from the same DigID.

  uint64_t globalEarliestTime; // Earliest timestamp in the file
  uint64_t globalLastTime; // Last timestamp in the file

  unsigned int totalNumHits;
  unsigned int hitID; // current hit ID

  // std::vector<Hit> hits; // Vector to store hits

  Hit * hits;
  unsigned int maxHitSize; // Size of the hits array
  unsigned int hitSize; // Number of hits read from the file
  uint64_t lastTimestampOfHits; // Last timestamp of hits read from the file

  void Init(){
    fileSize = 0;
    runID = 0;
    DigID = 0;
    fileIndex = 0;
    totalNumHits = 0;
    hitID = 0;
    hits = nullptr;
    maxHitSize = 0;
    hitSize = 0;
    lastTimestampOfHits = 0; 
    globalEarliestTime = UINT64_MAX; 
    globalLastTime = 0; 
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

  unsigned short count = 0;
  std::vector<uint64_t> timestampList; // Store timestamps for error checking

  uint64_t last_timestamp = 0; // Last timestamp of hits

  do{
    hit.header = Read<GEBHeader>();
    if( totalNumHits == 0) type = hit.header.type;
    
    // printf("Data ID : %d \n", totalNumHits);
    // header.Print();
    
    if( hit.header.type != type) {
      printf("header error at Data ID : %d \n", totalNumHits);
      break;
    }

    timestampList.push_back(hit.header.timestamp); 
 
    if( timestampList.size() >= maxHitSize ){ // check the timestamp error every maxHitSize hits
      std::sort(timestampList.begin(), timestampList.end()); // Sort the timestamps for error checking
      last_timestamp = timestampList.back(); // Get the last timestamp in the sorted list
      
      if( timestampList.front() < globalEarliestTime) globalEarliestTime = timestampList.front(); // Update the global earliest time
      if( timestampList.back() > globalLastTime) globalLastTime = timestampList.back(); // Update the global last time 
      
      if( count > 0 && timestampList.front() < old_timestamp) {
        maxHitSize *= 2;
        // printf("\033[33m=== Increasing maxHitSize to %u due to timestamp not in order. %s \033[0m\n", maxHitSize, fileName.c_str());
        CreateHits(maxHitSize); // Adjust the hit size to the maximum difference
      }
      timestampList.clear(); // Clear the list for the next batch
      count++;
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
    }

    totalNumHits ++;
  }while(Tell() < fileSize);

  std::sort(timestampList.begin(), timestampList.end()); // Sort the timestamps for error checking
  if( timestampList.front() < globalEarliestTime) globalEarliestTime = timestampList.front(); // Update the global earliest time
  if( timestampList.back() > globalLastTime) globalLastTime = timestampList.back(); // Update the global last time  

  if( debug ) printf("number of Hit Found : %d \n", totalNumHits);
  if( timestamp_error_counter > 0 && debug) printf("%s : timestamp error found for %u times.\n", fileName.c_str(), timestamp_error_counter);

  file.seekg(0, std::ios::beg);
  hitID = 0; // Reset hitID to 0 after scanning

}

inline void BinaryReader::ReadNextNHitsFromFile(bool debug) {

  if( debug ) printf("Reading next %u hits from file: %s |", maxHitSize, fileName.c_str());
  uint64_t old_timestamp = 0;
  unsigned int timestamp_error_counter = 0;
  hitSize = 0; // Reset hit size before reading new hits
  for (unsigned int i = 0; i < maxHitSize && Tell() < fileSize; ++i) {
    hits[i].header = Read<GEBHeader>();

    if( hits[i].header.timestamp < old_timestamp) {
      if( debug ) printf("timestamp error at Data ID : %d \n", i);
      if( debug ) printf("old timestamp : %16lu \n", old_timestamp);
      if( debug ) printf("new timestamp : %16lu \n", hits[i].header.timestamp);
      timestamp_error_counter++;
    }
    old_timestamp = hits[i].header.timestamp;


    if (hits[i].header.payload_lenght_byte > 0) {
      hits[i].payload = ReadArray<uint32_t>(hits[i].header.payload_lenght_byte / sizeof(uint32_t));
    }
    hitSize++;  // Increment hit size for each hit read
    hitID ++;  // Update the hitID to the next position
  }

  //sort hits by timestamp
  if( hitSize > 0 && timestamp_error_counter > 0 ) {
    if( debug ) printf("timestamp error found for %u times. Sort data.\n", timestamp_error_counter);

    //sort the hits by timestamp
    std::sort(hits, hits + hitSize, [](const Hit& a, const Hit& b) {
      return a.header.timestamp < b.header.timestamp;
    });

    uint64_t firstTimestamp = hits[0].header.timestamp; // First timestamp of hits
    if( firstTimestamp < lastTimestampOfHits){
      printf("\033[33m=== In file: %s \033[0m\n", fileName.c_str());
      printf("\033[31mWarning: First timestamp (%lu) of this read is less than the last timestamp (%lu) of the last read. \033[0m\n", 
             firstTimestamp, lastTimestampOfHits);
    }

    lastTimestampOfHits = hits[hitSize - 1].header.timestamp; // Update the last timestamp of hits
  }


  if( debug ){
    if( hitSize == 0 ) {
      printf(" No hits to read\n");
    }else{
      printf(" first timestamp: %lu, last timestamp: %lu, hits read: %u\n", 
          hits[0].header.timestamp, 
          hits[maxHitSize-1].header.timestamp, 
          hitSize);
    }
  }
}

inline size_t BinaryReader::GetMemoryUsageBytes() {
  size_t total = sizeof(BinaryReader);
  total += maxHitSize * sizeof(Hit);
  for (int i = 0; i < maxHitSize; ++i) {
      total += sizeof(GEBHeader);
      total += hits[i].payload.capacity() * sizeof(uint32_t);
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

      // find position of "run_" in the filename
      size_t runPos = baseName.find("run_");
      if (runPos != std::string::npos) {
        runID = std::stoi(baseName.substr(runPos + 4, 3));
      } else {
        runID = 0; // Default value if "run_" is not found
      }

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

