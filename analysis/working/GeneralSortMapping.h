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

#define NARRAY  24
#define NRDT    8
#define NELUM   3
#define NEZERO  2
#define NTAC    0
#define NCRDT   0
#define NAPOLLO 0

#define POLARITY_RDT    -1
#define POLARITY_ELUM   +1
#define POLARITY_EZERO  +1
#define POLARITY_CRDT   +1
#define POLARITY_APOLLO +1

///TODO, trigger time for trace analysis, that is the initial guess for the fit

#define MWIN 100 ///M value for energy filter from digi setting

Int_t idConst = 1010;
//                       0    1    2    3    4     5    6    7    8    9
Int_t idDetMap[160] = { -1,  -1,  -1, 202, 201,  -1, 200,  -1,  -1,  -1,   //VME1-DIG1, elum
                       100, 101, 102, 103, 104, 105, 106, 107,  -1,  -1,   //VME1-DIG2, recoils
                        -1, 300, 301,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //VME1-DIG3
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //VME1-DIG4
                   
                         1,   0,   5,   4,   3,   2,   1,   0,  -1,  -1,   // 1, VME2-DIG1, array
                         3,   2,   1,   0,   5,   4,   3,   2,  -1,  -1,   // 2, VME2-DIG2, array
                        11,  10,   9,   8,   7,   6,   5,   4,  -1,  -1,   // 3, VME2-DIG3, array
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //10, VME2-DIG4
                        
                         7,   6,  11,  10,   9,   8,   7,   6,  -1,  -1,   // 4, VME3-DIG1, array
                        15,  14,  13,  12,  11,  10,   9,   8,  -1,  -1,   // 5, VME3-DIG2, array
                        17,  16,  15,  14,  13,  12,  17,  16,  -1,  -1,   // 6, VME3-DIG3, array
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //11, VME3-DIG4
                        
                        19,  18,  17,  16,  15,  14,  13,  12,  -1,  -1,   // 7, VME4-DIG1, array
                        21,  20,  19,  18,  23,  22,  21,  20,  -1,  -1,   // 8, VME4-DIG2, array
                        23,  22,  21,  20,  19,  18,  23,  22,  -1,  -1,   // 9, VME4-DIG3, array
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};  //12, VME4-DIG4


Int_t idKindMap[160] = {-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         1,   1,   0,   0,   0,   0,   0,   0,  -1,  -1,//1
                         2,   2,   2,   2,   1,   1,   1,   1,  -1,  -1,//2
                         0,   0,   0,   0,   0,   0,   2,   2,  -1,  -1,//3
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         2,   2,   1,   1,   1,   1,   1,   1,  -1,  -1,//4
                         0,   0,   0,   0,   2,   2,   2,   2,  -1,  -1,//5
                         2,   2,   2,   2,   2,   2,   0,   0,  -1,  -1,//6
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         0,   0,   1,   1,   1,   1,   1,   1,  -1,  -1,//7
                         1,   1,   1,   1,   0,   0,   0,   0,  -1,  -1,//8
                         2,   2,   2,   2,   2,   2,   1,   1,  -1,  -1,//9
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};

#endif
