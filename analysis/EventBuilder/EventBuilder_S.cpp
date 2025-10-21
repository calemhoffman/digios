/*************************************** 
 * 
 * created by Ryan Tang 2025, July 10
 * 
 * Base on the EventBuilder_Q.cpp,
 * This event builder apply the mapping 
 * S for super
 * 
****************************************/

#include "BinaryReader.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TString.h"
#include "TGraph.h"
#include "TMacro.h"
#include "TF1.h"
#include "TClonesArray.h"
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory> // added for unique_ptr

#define MAX_TRACE_LEN 1250 
#define MAX_TRACE_MULTI 60
#define MAX_READ_HITS 100000 // Maximum hits to read at a time
#define MAX_QUEUE_SIZE 10000 // Maximum size of the data queue

#include <sys/time.h> /** struct timeval, select() */
inline unsigned int getTime_us(){
  unsigned int time_us;
  struct timeval t1;
  struct timezone tz;
  gettimeofday(&t1, &tz);
  time_us = (t1.tv_sec) * 1000 * 1000 + t1.tv_usec;
  return time_us;
}

#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multifit_nlinear.h>

struct trace_data {
  size_t n;
  double *y;
};

int fit_model(const gsl_vector * x, void *data, gsl_vector * f){
  size_t n = ((struct trace_data *)data)->n;
  double *y = ((struct trace_data *)data)->y;

  double A        = gsl_vector_get(x, 0);
  double T0       = gsl_vector_get(x, 1);
  double riseTime = gsl_vector_get(x, 2);
  double B        = gsl_vector_get(x, 3);

  for (size_t i = 0; i < n; ++i){
    double t = i;
    double Yi = A / (1 + exp(-(t - T0) / riseTime)) + B;
    gsl_vector_set(f, i, Yi - y[i]);
  }

  return GSL_SUCCESS;
}


// Comparator for min-heap (smallest timestamp on top)
struct CompareEvent {
  bool operator()(const Event& a, const Event& b) const {
    return a.timestamp > b.timestamp;
  }
};
  
#include "../working/GeneralSortMapping.h" // Include the mapping header file

class Data {
public:
  unsigned short runID = 0;

  unsigned int evID = 0 ;

  float Energy[NARRAY];
  float XF[NARRAY];
  float XN[NARRAY];
  float Ring[NARRAY];
  uint64_t EnergyTimestamp[NARRAY];
  uint64_t XFTimestamp[NARRAY];
  uint64_t XNTimestamp[NARRAY];
  uint64_t RingTimestamp[NARRAY];
  
  // trace
  unsigned short                            traceCount = 0; 
  unsigned short           traceDetID[MAX_TRACE_MULTI] = {0};
  unsigned short          traceKindID[MAX_TRACE_MULTI] = {0};
  unsigned short             traceLen[MAX_TRACE_MULTI] = {0};
  unsigned short trace[MAX_TRACE_MULTI][MAX_TRACE_LEN] = {0};

  // trace analysis
  float tracePara[MAX_TRACE_MULTI][4]; // trace parameters, 0 = Amplitude, 1 = Rise time, 2 = Timestamp, 3 = Baseline
  float traceChi2[MAX_TRACE_MULTI]; // trace fit chi2

  float te[NARRAY]; // trace energy
  float te_t[NARRAY]; // trace energy timestamp
  float te_r[NARRAY]; // trace energy rise time
  float te_chi2[NARRAY]; // trace energy chi2

  float txf[NARRAY]; // trace XF
  float txf_t[NARRAY]; // trace XF timestamp
  float txf_r[NARRAY]; // trace XF rise time
  float txf_chi2[NARRAY]; // trace XF chi2

  float txn[NARRAY]; // trace XN
  float txn_t[NARRAY]; // trace XN timestamp
  float txn_r[NARRAY]; // trace XN rise time
  float txn_chi2[NARRAY]; // trace XN chi2
  
#if NRDT > 0
  float RDT[NRDT];
  uint64_t RDTTimestamp[NRDT];
  
  float trdt[NRDT];
  float trdt_t[NRDT];
  float trdt_r[NRDT];
  float trdt_chi2[NRDT];
#endif
#if NTAC > 0
  float TAC[NTAC];
  uint64_t TACTimestamp[NTAC];
#endif
#if NELUM > 0
  float ELUM[NELUM];
  uint64_t ELUMTimestamp[NELUM];
#endif
#if NEZERO > 0
  float EZERO[NEZERO];
  uint64_t EZEROTimestamp[NEZERO];
#endif
#if NCRDT > 0
  float CRDT[NCRDT];
  uint64_t CRDTTimestamp[NCRDT];
#endif
#if NAPOLLO > 0
  float APOLLO[NAPOLLO];
  uint64_t APOLLOTimestamp[NAPOLLO];
#endif

  Data(){}
  ~Data(){}

  void Print(){
    printf("Energy: ");
    for (int i = 0; i < NARRAY; i++) {
      if( Energy[i] > 0  ) printf("%d, %f ", i, Energy[i]);
    }
    printf("\nXF: ");
    for (int i = 0; i < NARRAY; i++) {
      if( XF[i] > 0 ) printf("%d, %f ", i, XF[i]);
    }
    printf("\nXN: ");
    for (int i = 0; i < NARRAY; i++) {
      if( XN[i] > 0 ) printf("%d, %f ", i, XN[i]);
    }
    printf("\n");
  }

  void Reset() {
    // runID = 0;

    for (int i = 0; i < NARRAY; i++) {
      Energy[i] = TMath::QuietNaN();
      XF[i] = TMath::QuietNaN();
      XN[i] = TMath::QuietNaN();
      Ring[i] = TMath::QuietNaN();
      EnergyTimestamp[i] = 0;
      XFTimestamp[i] = 0;
      XNTimestamp[i] = 0;
      RingTimestamp[i] = 0;

      te[i] = TMath::QuietNaN();
      te_t[i] = TMath::QuietNaN();
      te_r[i] = TMath::QuietNaN();
    }

    traceCount = 0; 
    for (int i = 0; i < MAX_TRACE_MULTI; i++) {
      traceDetID[i] = 0;
      traceKindID[i] = 0;
      traceLen[i] = 0;
      for (int j = 0; j < MAX_TRACE_LEN; j++) trace[i][j] = 0;
      for (int j = 0; j < 4; j++) tracePara[i][j] = TMath::QuietNaN();
    }

#if NRDT > 0
    for (int i = 0; i < NRDT; i++) {
      RDT[i] = TMath::QuietNaN();
      RDTTimestamp[i] = 0;
      
      trdt[i] = TMath::QuietNaN();
      trdt_t[i] = TMath::QuietNaN();
      trdt_r[i] = TMath::QuietNaN();
    }
#endif
#if NTAC > 0
    for (int i = 0; i < NTAC; i++) {
      TAC[i] = TMath::QuietNaN();
      TACTimestamp[i] = 0;
    }
#endif
#if NELUM > 0
    for (int i = 0; i < NELUM; i++) {
      ELUM[i] = TMath::QuietNaN();
      ELUMTimestamp[i] = 0;
    }
#endif
#if NEZERO > 0    
    for (int i = 0; i < NEZERO; i++) {
      EZERO[i] = TMath::QuietNaN();
      EZEROTimestamp[i] = 0;
    }
#endif
#if NCRDT > 0
    for (int i = 0; i < NCRDT; i++) {
      CRDT[i] = TMath::QuietNaN();
      CRDTTimestamp[i] = 0;
    }
#endif
#if NAPOLLO > 0
    for (int i = 0; i < NAPOLLO; i++) {
      APOLLO[i] = TMath::QuietNaN();
      APOLLOTimestamp[i] = 0;
    }
#endif
  }

  void FillData(std::vector<Event> events, bool saveTrace){

    for( int i = 0; i <  events.size(); i++) {
      int id                    = events[i].board * 10 + events[i].channel - 1010;
      uint32_t pre_rise_energy  = events[i].pre_rise_energy; // Pre-rise energy
      uint32_t post_rise_energy = events[i].post_rise_energy; // Post-rise energy
      uint64_t timestamp        = events[i].timestamp; // Timestamp

      int idDet = idDetMap[id]; 
      int idKind = idKindMap[id]; 

      if( idDet < 0 ) continue;

      float eee = ((float)post_rise_energy - (float)pre_rise_energy) / MWIN; 

      // printf(" post_rise_energy : %u, pre_rise_energy : %u, eee : %.3f \n", post_rise_energy, pre_rise_energy, eee);

      if( 0 <= idDet && idDet < NARRAY ) { // Check if the ID is within the range of NARRAY
        switch (idKind ){
        case 0: // Energy signal
          Energy[idDet] = eee; // Calculate energy
          EnergyTimestamp[idDet] = timestamp; // Set the timestamp for energy
          break;
        case 1: // XF
          XF[idDet] = eee * POLARITY_XFXN; // Calculate XF
          XFTimestamp[idDet] = timestamp; // Set the timestamp for XF
          break;
        case 2: // XN
          XN[idDet] = eee * POLARITY_XFXN; // Calculate XN
          XNTimestamp[idDet] = timestamp; // Set the timestamp for XN
          break;
        case 3: // Ring
          Ring[idDet] = eee; // Calculate Ring
          RingTimestamp[idDet] = timestamp; // Set the timestamp for Ring
          break;
        }
      }

    #if NRDT > 0
      if( 100 <= idDet && idDet < 100 + NRDT ) { // Recoil
        int rdtID = idDet - 100; // Recoil ID
        RDT[rdtID] = eee * POLARITY_RDT; // Calculate RDT
        RDTTimestamp[rdtID] = timestamp; // Set the timestamp for RDT
      }
    #endif

    #if NELUM > 0 
      if( 200 <= idDet && idDet < 200 + NELUM ) { // ELUM
        int elumID = idDet - 200; // ELUM ID
        ELUM[elumID] = eee;
        ELUMTimestamp[elumID] = timestamp; // Set the timestamp for ELUM
      }
    #endif

    #if NEZERO > 0
      if( 300 <= idDet && idDet < 300 + NEZERO ) { // EZERO
        int ezeroID = idDet - 300; // EZERO ID
        EZERO[ezeroID] = eee;
        EZEROTimestamp[ezeroID] = timestamp; // Set the timestamp for EZERO
      }
    #endif

    #if NTAC > 0
      if( 400 <= idDet && idDet < 400 + NTAC ) { // TAC
        int tacID = idDet - 400; // TAC ID
        TAC[tacID] = eee;
        TACTimestamp[tacID] = timestamp; // Set the timestamp for TAC
      }
    #endif

    #if NCRDT > 0
      if( 500 <= idDet && idDet < 500 + NCRDT ) { // Circular Recoil
        int crdtID = idDet - 500; // Circular Recoil ID
        CRDT[crdtID] = eee;
        CRDTTimestamp[crdtID] = timestamp; // Set the timestamp for CRDT
      }
    #endif

    #if NAPOLLO > 0
      if( 600 <= idDet && idDet < 600 + NAPOLLO ) { // APOLLO
        int apolloID = idDet - 600; // APOLLO ID
        APOLLO[apolloID] = eee;
        APOLLOTimestamp[apolloID] = timestamp; // Set the timestamp for APOLLO
      }
    #endif

      if( saveTrace ){

        if( traceCount >= MAX_TRACE_MULTI ) {
          printf("\033[31mWarning: More than %d traces, truncating to %d traces. (event size %lu)\033[0m\n", MAX_TRACE_MULTI, MAX_TRACE_MULTI, events.size());
        }else{

          bool isArray = (0 <= idDet && idDet < NARRAY); 
          bool isRDT = (NRDT > 0 && 100 <= idDet && idDet < 100 + NRDT); 
          if( isArray || isRDT ) { 

            // printf("iDet: %d, idKind: %d, traceLength: %d\n", idDet, idKind, events[i].traceLength);

            traceLen[traceCount] = events[i].traceLength;
            traceDetID[traceCount] = idDet;
            traceKindID[traceCount] = idKind;
            uint16_t regulatedValue = 0;
            for( int j = 0; j < traceLen[traceCount]; j++){
              if( events[i].trace[j] < 16000 ) regulatedValue = events[i].trace[j]; 
              trace[traceCount][j] = regulatedValue; 
            }

            traceCount++;
          }
        }
      }

    }

  }

  int displayCount = 0;

  void TraceAnalysis(){
    if( traceCount <= 0 ) return; // No traces to analyze
    
    for( int i = 0; i < traceCount; i++ ){
      if( traceLen[i] <= 0 ) continue; // Skip if trace length is zero
      
      // set the trace_data 
      struct trace_data d = { (size_t)traceLen[i], new double[traceLen[i]] };
      for( int j = 0; j < traceLen[i]; j++) d.y[j] = trace[i][j];
      
      gsl_multifit_nlinear_fdf f;
      f.f = &fit_model;
      f.df = NULL;   // Use finite differences
      f.n = d.n;
      f.p = 4;
      f.params = &d;
      
      gsl_vector *x = gsl_vector_alloc(4);
      gsl_vector_set(x, 0, 100.0); // Initial guess for A
      gsl_vector_set(x, 1, 100.0); // Initial guess for T0
      gsl_vector_set(x, 2, 1.0);   // Initial guess for rise time
      gsl_vector_set(x, 3, 8000.0); // Initial guess for baseline
      
      const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
      gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
      gsl_multifit_nlinear_workspace *w = gsl_multifit_nlinear_alloc(T, &fdf_params, d.n, 4);
      gsl_multifit_nlinear_init(x, &f, w);

      // Solve the system using iteration
      int status;
      size_t iter = 0, max_iter = 100;
      do {
        status = gsl_multifit_nlinear_iterate(w);
        if (status) break;
        status = gsl_multifit_test_delta(w->dx, w->x, 1e-5, 1e-5);
        iter++;
      } while (status == GSL_CONTINUE && iter < max_iter);

      // Store the results
      gsl_vector *result = gsl_multifit_nlinear_position(w);
      tracePara[i][0] = gsl_vector_get(result, 0);
      tracePara[i][1] = gsl_vector_get(result, 1);
      tracePara[i][2] = gsl_vector_get(result, 2);
      tracePara[i][3] = gsl_vector_get(result, 3);

      // Calculate chi2
      gsl_vector *f_res = gsl_multifit_nlinear_residual(w);
      double chi2 = 0.0;
      for (size_t k = 0; k < f_res->size; k++) {
        double r = gsl_vector_get(f_res, k);
        chi2 += r * r;
      }
      traceChi2[i] = chi2;

      if( 0 <= traceDetID[i] && traceDetID[i] < NARRAY ){

        if ( traceKindID[i] == 0 ){ // Energy
          te[traceDetID[i]] = tracePara[i][0]; // Amplitude
          te_t[traceDetID[i]] = tracePara[i][1]; // Timestamp
          te_r[traceDetID[i]] = tracePara[i][2]; // Rise time
          te_chi2[traceDetID[i]] = traceChi2[i]; // Chi2
        }else if ( traceKindID[i] == 1 ){ // XF
          txf[traceDetID[i]] = tracePara[i][0]; // Amplitude
          txf_t[traceDetID[i]] = tracePara[i][1]; // Timestamp
          txf_r[traceDetID[i]] = tracePara[i][2]; // Rise time
          txf_chi2[traceDetID[i]] = traceChi2[i]; // Chi2
        }else if ( traceKindID[i] == 2 ){ // XN
          txn[traceDetID[i]] = tracePara[i][0]; // Amplitude
          txn_t[traceDetID[i]] = tracePara[i][1]; // Timestamp
          txn_r[traceDetID[i]] = tracePara[i][2]; // Rise time
          txn_chi2[traceDetID[i]] = traceChi2[i]; // Chi2
        }

      }

      if( 100 <= traceDetID[i] && traceDetID[i] < 100 + NRDT ){
        int rdtID = traceDetID[i] - 100; // Recoil ID
        trdt[rdtID] = tracePara[i][0]; // Amplitude
        trdt_t[rdtID] = tracePara[i][1]; // Timestamp
        trdt_r[rdtID] = tracePara[i][2]; // Rise time
        trdt_chi2[rdtID] = traceChi2[i]; // Chi2
      }

      gsl_multifit_nlinear_free(w);
      delete[] d.y;
    }
  }
  
  void PrintTraceAnalysisResult(){
    for( int i = 0; i < traceCount; i++ ){
      if( traceLen[i] <= 0 ) continue; // Skip if trace length is zero
      
      for(int j = 0; j < traceLen[i]; j++) {
        printf("Trace[%d]| %3d %5u\n", i, j, trace[i][j]);
      }
      printf("======== Trace: %d A = %.2f, T0 = %.2f, Rise time = %.2f, Baseline = %.2f\n", 
            i, tracePara[i][0], tracePara[i][1], tracePara[i][2], tracePara[i][3]);
    }
  }

};


//^============================================
int main(int argc, char* argv[]) {

  printf("=======================================================\n");
  printf("===        Event Builder S  raw data --> root       ===\n");
  printf("=======================================================\n");  

  if( argc <= 4){
    printf("%s [outfile] [timeWindow] [save Trace] [trace Analysis] [file-1] [file-2] ... \n", argv[0]);
    printf("        outfile : output root file name\n");
    printf("     timeWindow : tick; if < 0, no event build\n"); 
    printf("     Save Trace : 0 : no, 1 : yes\n");
    printf(" trace Analysis : 0 : no, 1 : yes (single core), >1 : multi-core\n");
    printf("         file-X : the input file(s)\n");
    return -1;
  }

  unsigned int runStartTime = getTime_us();

  TString outFileName = argv[1];
  int timeWindow = atoi(argv[2]);
  const bool saveTrace = atoi(argv[3]);
  const short nWorkers = atoi(argv[4]);

  Data data;

  const int nFile = argc - 5;
  TString inFileName[nFile];
  for( int i = 0 ; i < nFile ; i++){
    inFileName[i] = argv[i+5];
  }

  printf(" out file : \033[1;33m%s\033[m\n", outFileName.Data());
  if ( timeWindow < 0 ){
    printf(" Event building time window : no event build\n");
  }else{
    printf(" Event building time window : %d ticks \n", timeWindow);
  }
  printf("       Save Trace ? %s\n", saveTrace ? "Yes" : "No");
  printf("   Trace Analysis ? %s %s\n", nWorkers ? "Yes" : "No", nWorkers > 0 ? Form("(%d-core)", nWorkers) : "");
  if( saveTrace ){
    printf("  MAX Trace Length : %d\n", MAX_TRACE_LEN);
    printf("   MAX Trace Multi : %d\n", MAX_TRACE_MULTI);
  }
  printf("     MAX Hits read : %d\n", MAX_READ_HITS);
  printf("    MAX Queue Size : %d\n", MAX_QUEUE_SIZE);
  printf(" Number of input file : %d \n", nFile);
  
  //*=============== setup reader
  printf(" Scaning input files...\n");
  uint64_t totalNumHits = 0;
  uint64_t totFileSize = 0; // Total file size in bytes
  BinaryReader ** reader = new BinaryReader *[nFile];

  unsigned short globalRunID = 0;

  std::vector<std::thread> threads;

  for( int i = 0 ; i < nFile ; i++){
    reader[i] = new BinaryReader((MAX_READ_HITS)); 
    reader[i]->Open(inFileName[i].Data());
    globalRunID = reader[i]->GetRunID(); // Get the run ID from the first file
    if( i > 0 && globalRunID != reader[i]->GetRunID() ) {
      printf("\033[31mWarning: Run ID mismatch between files. Using the first file's Run ID: %d.\033[0m\n", globalRunID);
    }
    threads.emplace_back([](BinaryReader* reader) { 
      reader->Scan(true); 
      printf("%s | %6.1f MB | # hit : %10d (%d)\n", reader->GetFileName().c_str(), reader->GetFileSize()/1024./1024., reader->GetNumData(), reader->GetMaxHitSize());
    }, reader[i]);
  }

  uint64_t globalEarliestTime = UINT64_MAX; // Global earliest timestamp
  uint64_t globalLastTime = 0; // Global last timestamp

  // Wait for all threads to finish
  for (int i = 0; i < threads.size(); ++i) {
    threads[i].join();
    totalNumHits += reader[i]->GetNumData();
    totFileSize += reader[i]->GetFileSize();

    if (reader[i]->GetGlobalEarliestTime() < globalEarliestTime) globalEarliestTime = reader[i]->GetGlobalEarliestTime();
    if (reader[i]->GetGlobalLastTime() > globalLastTime) globalLastTime = reader[i]->GetGlobalLastTime();
  }

  threads.clear();
  
  //*=============== group files by DigID and sort the fileIndex
  std::map<unsigned short, std::vector<BinaryReader*>> fileGroups;
  for( int i = 0 ; i < nFile ; i++){
    unsigned short digID = reader[i]->GetDigID();
    if (!fileGroups.count(digID) ) {
      fileGroups[digID] = std::vector<BinaryReader*>();
    }
    fileGroups[digID].push_back(reader[i]);
  }
  // printf("Found %zu DigIDs\n", fileGroups.size());
  
  // printf out the file groups
  // for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
  //   unsigned short digID = group.first; // DigID
  //   const auto& readers = group.second; // Vector of BinaryReader pointers
  //   printf("----- DigID %03d has %zu files\n", digID, readers.size());
  //   for (size_t j = 0; j < readers.size(); j++) {
  //     printf("  File %zu: %s | %6.1f MB | num. of hit : %10d\n", j, readers[j]->GetFileName().c_str(), readers[j]->GetFileSize()/1024./1024., readers[j]->GetNumData());
  //   }
  // }
  
  printf("================= Total number of hits: %lu\n", totalNumHits);
  printf("                       Total file size: %.1f MB\n", totFileSize / (1024.0 * 1024.0));
  printf("                        Total Run Time: %.3f s = %.3f min\n", (globalLastTime - globalEarliestTime) / 1e8, (globalLastTime - globalEarliestTime) / 1e8 / 60.0);


  //*=============== create output file and setup TTree
  TFile * outFile = TFile::Open(outFileName.Data(), "RECREATE");
  TTree * outTree = new TTree("gen_tree", outFileName.Data());
  outTree->SetDirectory(outFile);

  outTree->Branch("runID",   &data.runID, "runID/s");
  outTree->Branch("evID",   &data.evID, "evID/i");
  
  outTree->Branch("e",             data.Energy, Form("e[%d]/F", NARRAY));
  outTree->Branch("e_t",  data.EnergyTimestamp, Form("e_t[%d]/l", NARRAY));
  outTree->Branch("xf",                data.XF, Form("xf[%d]/F", NARRAY));
  outTree->Branch("xf_t",     data.XFTimestamp, Form("xf_t[%d]/l", NARRAY));
  outTree->Branch("xn",                data.XN, Form("xn[%d]/F", NARRAY));
  outTree->Branch("xn_t",     data.XNTimestamp, Form("xn_t[%d]/l", NARRAY));
  outTree->Branch("ring",            data.Ring, Form("ring[%d]/F", NARRAY));
  outTree->Branch("ring_t", data.RingTimestamp, Form("ring_t[%d]/l", NARRAY));
  
  printf(" -----  %d array det.\n", NARRAY);

#if NRDT > 0
    outTree->Branch("rdt", data.RDT, Form("rdt[%d]/F", NRDT));
    outTree->Branch("rdt_t", data.RDTTimestamp, Form("rdt_t[%d]/l", NRDT));
#else
    printf(" -----  no recoil.\n");
#endif

#if NTAC > 0
  outTree->Branch("tac", data.TAC, Form("tac[%d]/F", NTAC));
  outTree->Branch("tac_t", data.TACTimestamp, Form("tac_t[%d]/l", NTAC));
#else
  printf(" -----  no TAC.\n");
#endif

#if NELUM > 0
  outTree->Branch("elum", data.ELUM, Form("elum[%d]/F", NELUM));
  outTree->Branch("elum_t", data.ELUMTimestamp, Form("elum_t[%d]/l", NELUM));
#else
  printf(" -----  no ELUM.\n");
#endif

#if NEZERO > 0
  outTree->Branch("ezero", data.EZERO, Form("ezero[%d]/F", NEZERO));
  outTree->Branch("ezero_t", data.EZEROTimestamp, Form("ezero_t[%d]/l", NEZERO));
#else
  printf(" -----  no EZERO.\n");
#endif

#if NCRDT > 0
  outTree->Branch("crdt", data.CRDT, Form("crdt[%d]/F", NCRDT));
  outTree->Branch("crdt_t", data.CRDTTimestamp, Form("crdt_t[%d]/l", NCRDT));
#else
  printf(" -----  no CRDT.\n");
#endif

#if NAPOLLO > 0
  outTree->Branch("apollo", data.APOLLO, Form("apollo[%d]/F", NAPOLLO));
  outTree->Branch("apollo_t", data.APOLLOTimestamp, Form("apollo_t[%d]/l", NAPOLLO));
#else
  printf(" -----  no APOLLO.\n");
#endif

  if( saveTrace ){
    outTree->Branch( "traceCount",  &data.traceCount, "traceCount/s");
    outTree->Branch( "traceDetID",   data.traceDetID, "traceDetID[traceCount]/s");
    outTree->Branch("traceKindID",  data.traceKindID, "traceKindID[traceCount]/s");
    outTree->Branch(   "traceLen",     data.traceLen, "traceLen[traceCount]/s");
    outTree->Branch(      "trace",        data.trace, Form("trace[traceCount][%d]/s", MAX_TRACE_LEN));
  }
  if( nWorkers > 0 ){ 
    outTree->Branch("tracePara", data.tracePara, "tracePara[traceCount][4]/F");
    outTree->Branch("traceChi2", data.traceChi2, "traceChi2[traceCount]/F");

    outTree->Branch("te", data.te, Form("te[%d]/F", NARRAY));
    outTree->Branch("te_t", data.te_t, Form("te_t[%d]/F", NARRAY));
    outTree->Branch("te_r", data.te_r, Form("te_r[%d]/F", NARRAY));
    outTree->Branch("te_chi2", data.te_chi2, Form("te_chi2[%d]/F", NARRAY));

    outTree->Branch("txf", data.txf, Form("txf[%d]/F", NARRAY));
    outTree->Branch("txf_t", data.txf_t, Form("txf_t[%d]/F", NARRAY));
    outTree->Branch("txf_r", data.txf_r, Form("txf_r[%d]/F", NARRAY));
    outTree->Branch("txf_chi2", data.txf_chi2, Form("txf_chi2[%d]/F", NARRAY));

    outTree->Branch("txn", data.txn, Form("txn[%d]/F", NARRAY));
    outTree->Branch("txn_t", data.txn_t, Form("txn_t[%d]/F", NARRAY));
    outTree->Branch("txn_r", data.txn_r, Form("txn_r[%d]/F", NARRAY));
    outTree->Branch("txn_chi2", data.txn_chi2, Form("txn_chi2[%d]/F", NARRAY));

#if NRDT > 0
    outTree->Branch("trdt", data.trdt, Form("trdt[%d]/F", NRDT));
    outTree->Branch("trdt_t", data.trdt_t, Form("trdt_t[%d]/F", NRDT));
    outTree->Branch("trdt_r", data.trdt_r, Form("trdt_r[%d]/F", NRDT));
    outTree->Branch("trdt_chi2", data.trdt_chi2, Form("trdt_chi2[%d]/F", NRDT));
#endif

  }

  //*=============== print ID map
  for(int i = 0 ; i < 88; i++ ) printf("=");
  printf(" ID-MAP:\n");
  printf("%11s|", ""); 
  for(int i = 0 ; i < 10; i++ ) printf("%7d|", i);
  printf("\n");
  for(int i = 0 ; i < 96; i++ ) printf("-");
  for(int i = 0 ; i < 160; i ++){
    if( (i) % 10 == 0 ) {
      printf("\n");
      if(((i+1)/10)/4+1 < 5) printf("%11s|", Form("VME%d-Dig%d", ((i+1)/10)/4+1, ((i+1)/10)%4+1)); 
    }
    if( 100 + NRDT >= idDetMap[i] && idDetMap[i] >= 100){
      printf("\033[36m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Recoil, Cyan
    }else if( 200+NELUM >= idDetMap[i] && idDetMap[i] >= 200){
      printf("\033[91m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Elum, 
    }else if( 300+NEZERO >= idDetMap[i] && idDetMap[i] >= 300){
      printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// EZERO, 
    }else if( 400+NTAC >= idDetMap[i] && idDetMap[i] >= 400){
      printf("\033[93m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// TAC, 
    }else if( 500+NCRDT >= idDetMap[i] && idDetMap[i] >= 500){
      printf("\033[100m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Circular Recoil
    }else if( 600+NAPOLLO >= idDetMap[i] && idDetMap[i] >= 600){
      printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// APOLLO
    }else if(  NARRAY > idDetMap[i] && idDetMap[i] >= 0){    
      switch (idKindMap[i]) {
        case -1: printf("%7s|", ""); break;
        case  0: printf("\033[31m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// RED
        case  1: printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Green
        case  2: printf("\033[33m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Yellow
        case  3: printf("\033[34m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Blue
        case  4: printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Magenta
        default: printf("%3d(%2d)|", idDetMap[i], idKindMap[i]); break; /// no color
      }
    }else{
      printf("%7s|", "");
    }
  }
  printf("\n");
  for(int i = 0 ; i < 96; i++ ) printf("=");
  printf("\n");


  //*=============== read n data from each file
  std::map<unsigned short, unsigned int> hitID; // store the hit ID for the current reader for each DigID
  std::map<unsigned short, short> fileID; // store the file ID for each DigID
  
  std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue; // Priority queue to store events for each thread

  for( const std::pair<const unsigned short, std::vector<BinaryReader*>>& group : fileGroups) { // looping through the map
    unsigned short digID = group.first; // DigID
    const auto& readers = group.second; // Vector of BinaryReader pointers

    hitID[digID] = 0; // Initialize hitID for this DigID
    fileID[digID] = 0; 

    BinaryReader* reader = readers[0];
    reader->ReadNextNHitsFromFile(); // Read 10,000 hits at a time

    for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(saveTrace)); // Decode the hit payload and push it to the event queue

  }

  //*=============== event building
  std::vector<Event> events; // Vector to store events
  unsigned int eventID = 0;
  
  unsigned int hitProcessed = 0; // Number of hits processed
  double last_precentage = 0.0; // Last percentage printed

  std::mutex queueMutex;
  using DataPtr = std::unique_ptr<Data>;
  std::queue<DataPtr> dataQueue; // Queue of unique_ptr to avoid expensive Data copies
  std::mutex outQueueMutex; // Mutex for output queue and file writing
  std::queue<DataPtr> outputQueue;
  std::condition_variable trace_cv; // Condition variable for thread synchronization
  std::atomic<bool> done(false); // to flag all data is processed. to tell the threads to finish

  std::atomic<bool> allEventProcessed(false);
  std::condition_variable fileCv; // Condition variable for file writing
  std::thread outTreeThread;

  DataPtr tempData; // used for multi-threaded processing (allocated when needed)

  if( nWorkers > 1 ) {
       
    for (int i = 0; i < nWorkers; i++) {
      threads.emplace_back([i, &dataQueue, &outputQueue, &queueMutex, &trace_cv, &fileCv, &done, &outQueueMutex]() {
        DataPtr localData;
        int count = 0;
        while (true) {

          {
            std::unique_lock<std::mutex> lock(queueMutex);
            trace_cv.wait(lock, [&] { return !dataQueue.empty() || done; });
            if (dataQueue.empty() && done) break;
            localData = std::move(dataQueue.front()); // move pointer out of queue (no Data copy)
            dataQueue.pop();
          }
          // Process data
          if (localData) localData->TraceAnalysis(); // Perform trace analysis if enabled
          count++;

          {// push processed data to output queue
            std::lock_guard<std::mutex> lock(outQueueMutex);
            outputQueue.push(std::move(localData)); // move processed pointer to output queue (no copy)
            fileCv.notify_one(); // Notify the output tree thread to write data
          }
        }

        printf("Trace worker %2d finished processing. total processed event : %d\n", i, count);
      });
    }

    // Create a thread to write the output tree
    outTreeThread = std::thread([&]() {
      int count = 0;
      while (true) {
        
        std::unique_lock<std::mutex> lock(outQueueMutex);   
        fileCv.wait(lock, [&] { return !outputQueue.empty() || allEventProcessed; }); // Wait for data to be available in the output queue or all events are processed
        if (allEventProcessed && outputQueue.empty()) {
          printf("Writing data to output tree: %d items processed, out Queue size %ld\n", count, outputQueue.size());
          break; // Exit if all data is processed and output queue is empty
        }
        DataPtr temp_data = std::move(outputQueue.front()); // take ownership (no copy)
        outputQueue.pop();
        lock.unlock(); // Release outQueueMutex as soon as possible

        // Copy into the tree-fill object (one copy here)
        if (temp_data) data = *temp_data;
         
        data.runID = globalRunID; // Set the run ID
        outTree->Fill(); // Fill the tree with the data
        outFile->cd(); // Ensure the output file is set as the current directory

        count++;
      }
      outTree->Write(); // Write the tree to the output file
      printf("Output tree written to file: %s. total processed event : %d\n", outFileName.Data(), count);
    });

  }

  do{

    ///============================== forming events and check is need to load more hits from files
    do{

      events.push_back(eventQueue.top()); 
      int digID = events.back().board; 
      hitID[digID]++;
      hitProcessed ++;

      if( fileID[digID] >= 0  ){ // check shoudl read the next N data or next file for this DigID

        BinaryReader* reader = fileGroups[digID][fileID[digID]];
        if( hitID[digID] >= reader->GetHitSize() ) { // If the hitID exceeds the number of hits in the current file
          reader->ReadNextNHitsFromFile(); // Read more hits from the current file
          if( reader->GetHitSize() > 0 ) {
            hitID[digID] = 0; // Reset hitID for this DigID
            for( int i = 0; i < reader->GetHitSize(); i++)  eventQueue.push(reader->GetHit(i).DecodePayload(saveTrace)); 
          }
        }

        if( reader->GetHitSize() == 0 ) { // load next file if no hits
          if( fileID[digID] < fileGroups[digID].size() - 1 ) {
            reader->DeleteHits(); // Delete the hits from the current reader, free memory
            fileID[digID]++;
            // printf("\033[34m====== No hits in current file for DigID %03d, loading next file...%s\033[0m\n", digID, readers[fileID[digID]]->GetFileName().c_str());
            reader = fileGroups[digID][fileID[digID]];
            reader->ReadNextNHitsFromFile(); // Read more hits from the next file

            hitID[digID] = 0; // Reset hitID for this DigID
            for( int i = 0; i < reader->GetHitSize(); i++) eventQueue.push(reader->GetHit(i).DecodePayload(saveTrace)); 
            
          } else {
            fileID[digID] = -1; // Mark that there are no more files for this DigID
          }
        }
      }      

      if( timeWindow >= 0 && events.size() > 0 && events.back().timestamp - events.front().timestamp > timeWindow) {
        events.pop_back(); // Remove the last event if it exceeds the time window
        hitID[digID]--; // Decrement the hitID for this DigID
        hitProcessed--; // Decrement the hitProcessed count
        break;
      }

      eventQueue.pop();
      if( timeWindow < 0 ) break;

    }while(!eventQueue.empty()); // Loop until the event queue is empty

    ///============================== process the events
    if( events.size() > 0 ) {
      std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return a.timestamp < b.timestamp; // Sort events by timestamp
      });
      
      if( nWorkers > 1 ) {
        // Multi-threaded processing
        while(true){

          std::unique_lock<std::mutex> lock(queueMutex); // Lock the queue mutex
          if (dataQueue.size() < MAX_QUEUE_SIZE) { // Check if the queue size is within the limit
            tempData.reset(new Data()); // allocate new Data once per push
            tempData->Reset();
            tempData->evID = eventID; // Set the event ID
            // printf("Main thread pushing event %u to dataQueue (size: %ld)\n", eventID, dataQueue.size());
            tempData->FillData(events, saveTrace); // Fill data with the events
            dataQueue.push(std::move(tempData)); // move the pointer into the queue (no copy)
            lock.unlock(); // Unlock the queue mutex
            trace_cv.notify_one(); // Notify one of the worker threads to start processing
            break;
          }
          lock.unlock(); // Unlock the queue mutex 
          if( dataQueue.size() >= MAX_QUEUE_SIZE ) {
            //wait for 10 miniseconds before checking the queue again
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Sleep for
            continue;
          }

        }

      } else if( nWorkers == 1){
        data.Reset();
        data.runID = globalRunID; // Set the run ID
        data.evID = eventID; // Set the event ID
        data.FillData(events, saveTrace); // Fill data with the events
        data.TraceAnalysis(); // Perform trace analysis if enabled

        outTree->Fill(); // Fill the tree with the processed data
        
      } else {
        // Single-threaded processing, o trace analysis
        data.Reset();
        data.runID = globalRunID; // Set the run ID
        data.evID = eventID; // Set the event ID
        data.FillData(events, saveTrace);

        outTree->Fill(); // Fill the tree
      }

    }
    
    ///=============== print progress
    double percentage = hitProcessed * 100/ totalNumHits;
    if( percentage >= last_precentage ) {
      size_t memoryUsage = sizeof(Event) * eventQueue.size();
      printf("Processed : %u, %.0f%% | %u/%lu | %.3f Mb", eventID, percentage, hitProcessed, totalNumHits, memoryUsage/ (1024. * 1024.));
      printf(" \n\033[A\r");
      last_precentage = percentage + 1.0;
    }
    
    // prepare for the next event
    eventID ++;
    events.clear(); // Clear the events vector for the next event

  }while(!eventQueue.empty()); 
  
  if ( nWorkers > 1 ){

    printf("\nWait for all threads to finish processing...\n");
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      printf("dataQueue size before finishing: %ld\n", dataQueue.size());
    }
    done = true; // All data is processed, set the done flag to true
    trace_cv.notify_all();
    for( int i = 0; i < nWorkers; i++) {
      threads[i].join(); // Wait for all threads to finish
    }
    printf("All trace analysis threads finished processing.\n");
    allEventProcessed = true;
    fileCv.notify_all(); // Notify the output tree thread to finish writing data
    {
      std::unique_lock<std::mutex> lock(outQueueMutex);
      printf("outQueue size before finishing: %ld\n", outputQueue.size());
    }
    outTreeThread.join(); // Wait for the output tree thread to finish
    printf("Output tree thread finished writing to file.\n");
  }

  //*=============== save some marco
  //Save the global earliest and last timestamps as a TMacro
  TMacro macro("info", "Earliest and Last Timestamps");
  macro.AddLine(Form("globalEarliestTime = %lu", globalEarliestTime));
  macro.AddLine(Form("globalLastTime = %lu", globalLastTime));
  macro.AddLine(Form("totalNumHits = %lu", totalNumHits));
  macro.AddLine(Form("totFileSizeMB = %.1f", totFileSize / (1024.0 * 1024.0))); // Convert to MB
  macro.Write("info");

  TMacro macro2("trace_info", "Maximum Trace Length"); //this macro is for read Raw trace
  macro2.AddLine(Form("%d", MAX_TRACE_LEN));
  macro2.Write("trace_info");

  //*=============== summary
  printf("=======================================================\n");
  printf("===          Event Builder S finished               ===\n");
  printf("=======================================================\n");
  unsigned int runEndTime = getTime_us();
  printf("              Total time taken: %.3f s = %.3f min\n", (runEndTime - runStartTime) / 1000000.0, (runEndTime - runStartTime) / 1000000.0 / 60.0);
  printf("Total number of hits processed: %10u (%lu)\n", hitProcessed, totalNumHits);
  printf("                  Events / sec: %10.2f\n", eventID / ((runEndTime - runStartTime) / 1000000.0));
  printf("  Total number of events built: %10u\n", eventID);
  printf("     Number of entries in tree: %10lld\n", outTree->GetEntries());
  //clean up
  outFile->Write();
  outFile->Close();
  for( int i = 0 ; i < nFile ; i++){
    delete reader[i]; // Delete each BinaryReader
  }
  delete[] reader; // Delete the array of BinaryReader pointers
  printf("Output file \033[31m%s\033[0m created successfully.\n", outFileName.Data());

  return 0;
}