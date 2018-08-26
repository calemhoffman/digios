/*-----------------------------------------------------*/
// Class to hold the DGSEVENT data
// Will be passed to TTree
class EvtList {
 public:

  //Constructor
 EvtList(int MaxHits) {
    //All variables to be included in the TTree
    this->MaxHits = MaxHits;
    NumHits = MaxHits;
    RunNumber;
    id = new short int[MaxHits]; // board_id*10 + chan_id

    tpe = new unsigned short int[MaxHits]; //Requires a correct map file [will remove]
    tid = new unsigned short int[MaxHits]; //Requires a correct map file [will remove]
    chan_id = new unsigned short int[MaxHits];
    board_id = new unsigned short int[MaxHits];
    geo_addr = new unsigned short int[MaxHits];
    packet_length = new unsigned short int[MaxHits];
    header_type = new unsigned short int[MaxHits];
    event_type = new unsigned short int[MaxHits];
    header_length = new unsigned short int[MaxHits];
    
    event_timestamp = new unsigned long long int[MaxHits]; // long long int
    last_disc_timestamp = new unsigned long long int[MaxHits]; //long long int
    peak_timestamp = new unsigned long long int[MaxHits]; // long long int

    timestamp_match_flag = new unsigned short int[MaxHits]; // For CFD
    external_disc_flag = new unsigned short int[MaxHits];
    cfd_valid_flag = new unsigned short int[MaxHits]; // For CFD
    pileup_only_flag = new unsigned short int[MaxHits];
    offset_flag = new unsigned short int[MaxHits];
    sync_error_flag = new unsigned short int[MaxHits];
    general_error_flag = new unsigned short int[MaxHits];
    peak_valid_flag = new unsigned short int[MaxHits];
    pileup_flag = new unsigned short int[MaxHits];

    sampled_baseline = new int[MaxHits];
    cfd_sample_0 = new int[MaxHits]; // For CFD
    cfd_sample_1 = new int[MaxHits]; // For CFD
    cfd_sample_2 = new int[MaxHits]; // For CFD
    pre_rise_energy = new int[MaxHits];
    post_rise_energy = new int[MaxHits];

    m1_begin_sample = new unsigned short int[MaxHits];
    m1_end_sample = new unsigned short int[MaxHits];
    m2_begin_sample = new unsigned short int[MaxHits];
    m2_end_sample = new unsigned short int[MaxHits];
    peak_sample = new unsigned short int[MaxHits];
    base_sample = new unsigned short int[MaxHits];

    baseline = new int[MaxHits]; // (m1_begin_sample + m1_end_sample) / 2

    trace_length = new unsigned short int[MaxHits];
    trace = new short int[MaxHits][1024];
  };
  
  //Reset Function
  void Reset() {
    for (int i=0;(i<NumHits&&i<MaxHits);i++) {
      id[i]=-1;
      tpe[i]=0;
      tid[i]=0;
      chan_id[i]=0;
      board_id[i]=0;
      geo_addr[i]=0;
      packet_length[i]=0;
      header_type[i]=0;
      event_type[i]=0;
      header_length[i]=0;
      event_timestamp[i]=0;
      last_disc_timestamp[i]=0;
      peak_timestamp[i]=0;
      timestamp_match_flag[i]=0;
      external_disc_flag[i]=0;
      cfd_valid_flag[i]=0;
      pileup_only_flag[i]=0;
      offset_flag[i]=0;
      sync_error_flag[i]=0;
      general_error_flag[i]=0;
      peak_valid_flag[i]=0;
      pileup_flag[i]=0;
      sampled_baseline[i]=0;
      cfd_sample_0[i]=0;
      cfd_sample_1[i]=0;
      cfd_sample_2[i]=0;
      pre_rise_energy[i]=0;
      post_rise_energy[i]=0;
      m1_begin_sample[i]=0;
      m1_end_sample[i]=0;
      m2_begin_sample[i]=0;
      m2_end_sample[i]=0;
      peak_sample[i]=0;
      base_sample[i]=0;
      baseline[i]=0;
      trace_length[i]=0;
      for (Int_t j=0;j<1024;j++)
	trace[i][j]=-1000;
    }
    NumHits=0;
    return;
  };

  //Members
  int MaxHits;
  int NumHits;
  int RunNumber;
  short int* id;
  unsigned short int* tpe;
  unsigned short int* tid;
  unsigned short int* chan_id;
  unsigned short int* board_id;
  unsigned short int* geo_addr;
  unsigned short int* packet_length;
  unsigned short int* header_type;
  unsigned short int* event_type;
  unsigned short int* header_length;
  unsigned long long int* event_timestamp;
  unsigned long long int* last_disc_timestamp;
  unsigned long long int* peak_timestamp;
  unsigned short int* timestamp_match_flag;
  unsigned short int* external_disc_flag;
  unsigned short int* cfd_valid_flag;
  unsigned short int* pileup_only_flag;
  unsigned short int* offset_flag;
  unsigned short int* sync_error_flag;
  unsigned short int* general_error_flag;
  unsigned short int* peak_valid_flag;
  unsigned short int* pileup_flag;
  int* sampled_baseline;
  int* cfd_sample_0;
  int* cfd_sample_1;
  int* cfd_sample_2;
  int* pre_rise_energy;
  int* post_rise_energy;
  unsigned short int* m1_begin_sample;
  unsigned short int* m1_end_sample;
  unsigned short int* m2_begin_sample;
  unsigned short int* m2_end_sample;
  unsigned short int* peak_sample;
  unsigned short int* base_sample;
  int* baseline;
  unsigned short int* trace_length;
  short int (*trace)[1024];

};
