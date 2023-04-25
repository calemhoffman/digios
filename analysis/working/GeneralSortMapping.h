/***********************************************************************
 * 
 * This is the Mapping for Armory/GeneralSort.C
 * 
 * idDetMap = 0XX ; PSD
 *          = 1XX ; Recoil,  dE of the Recoil MUST be odd
 *          = 2XX ; ELUM
 *          = 3XX ; EZERO
 *          = 4XX ; TAC & RF-Timing
 *          = 5XX ; Circular Recoil
 *          = 6XX ; APOLLO
 * 
***********************************************************************/

#ifndef MAPPING
#define MAPPING

#define NARRAY  30
#define NRDT    8
#define NELUM   1
#define NEZERO  0
#define NTAC    1
#define NCRDT   0
#define NAPOLLO 0

#define POLARITY_RDT    +1
#define POLARITY_ELUM   +1
#define POLARITY_EZERO  +1
#define POLARITY_CRDT   +1
#define POLARITY_APOLLO +1

///TODO, trigger time for trace analysis, that is the initial guess for the fit

#define MWIN -100 ///M value for energy filter from digi setting


Int_t idConst = 1010;

Int_t idDetMap[160] = {  4,   3,   2,   1,   0,   4,   3,   2,   1,   0,   //VME1-DIG1-TL-E,R
                         4,   3,   2,   1,   0,   4,   3,   2,   1,   0,   //VME1-DIG2-TL-F,N
                        29,  28,  27,  26,  25,  29,  28,  27,  26,  25,   //VME1-DIG3-BL-E,R
                       200, 201, 202, 203, 204, 205, 206, 207,  -1,  -1,   //VME1-DIG4
                        29,  28,  27,  26,  25,  29,  28,  27,  26,  25,   //VME2-DIG1-BL-F,N
                        24,  23,  22,  21,  20,  24,  23,  22,  21,  20,   //VME2-DIG2-BB-E,R
                        24,  23,  22,  21,  20,  24,  23,  22,  21,  20,   //VME2-DIG3-BB-F,N
                       400,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //VME2-DIG4
                        19,  18,  17,  16,  15,  19,  18,  17,  16,  15,   //VME3-DIG1-BR-E,R
                        19,  18,  17,  16,  15,  19,  18,  17,  16,  15,   //VME3-DIG2-BR-F,N
                        14,  13,  12,  11,  10,  14,  13,  12,  11,  10,   //VME3-DIG3-TR-E,R
                       100, 101, 102, 103, 104, 105, 106, 107,  -1,  -1,   //VME3-DIG4
                        14,  13,  12,  11,  10,  14,  13,  12,  11,  10,   //VME4-DIG1-TR-F,N
                         9,   8,   7,   6,   5,   9,   8,   7,   6,   5,   //VME4-DIG2-TT-E,R
                         9,   8,   7,   6,   5,   9,   8,   7,   6,   5,   //VME4-DIG3-TT-F,N
                       208, 209, 210, 211, 212, 213, 214, 215,  -1,  -1};  //VME4-DIG4

//Kind Map, 0 = energy, 1 = Far, 2 = Near, 3 = Ring
//kind map for other detectors is not used.
Int_t idKindMap[160] = { 0,   0,   0,   0,   0,   3,   3,   3,   3,   3,//VME1
                         2,   2,   2,   2,   2,   1,   1,   1,   1,   1,
                         0,   0,   0,   0,   0,   3,   3,   3,   3,   3,
                        -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
                         2,   2,   2,   2,   2,   1,   1,   1,   1,   1,//VME2
                         0,   0,   0,   0,   0,   3,   3,   3,   3,   3,
                         2,   2,   2,   2,   2,   1,   1,   1,   1,   1,
                        -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
                         0,   0,   0,   0,   0,   3,   3,   3,   3,   3,//VME3
                         2,   2,   2,   2,   2,   1,   1,   1,   1,   1,
                         0,   0,   0,   0,   0,   3,   3,   3,   3,   3,
                        -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
                         2,   2,   2,   2,   2,   1,   1,   1,   1,   1,//VME4
                         0,   0,   0,   0,   0,   3,   3,   3,   3,   3,
                         2,   2,   2,   2,   2,   1,   1,   1,   1,   1,
                        -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2};

#endif

#define NARRAY 30
#define NRDT 8
#define NELUM 16
#define NEZERO 5
#define NTAC 3
#define NCRDT 0
#define NARRAY 30
#define NRDT 8
#define NELUM 16
#define NEZERO 5
#define NTAC 3
#define NCRDT 0
#define NARRAY 30
#define NRDT 8
#define NELUM 16
#define NEZERO 5
#define NTAC 3
#define NCRDT 0
#define NARRAY 30
#define NRDT 8
#define NELUM 16
#define NEZERO 5
#define NTAC 3
#define NCRDT 0
#define NARRAY 30
#define NRDT 8
#define NELUM 16
#define NEZERO 5
#define NTAC 3
#define NCRDT 0
#define NARRAY 30
#define NRDT 8
#define NELUM 16
#define NEZERO 5
#define NTAC 3
#define NCRDT 0
#define NARRAY 30
#define NRDT 8
#define NELUM 16
#define NEZERO 5
#define NTAC 3
#define NCRDT 0
