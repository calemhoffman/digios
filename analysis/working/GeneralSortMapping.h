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
#define NELUM   0
#define NEZERO  0
#define NTAC    0
#define NCRDT   0
#define NAPOLLO 0

#define POLARITY_XFXN   +1
#define POLARITY_RDT    -1
#define POLARITY_ELUM   +1
#define POLARITY_EZERO  +1
#define POLARITY_CRDT   +1
#define POLARITY_APOLLO +1

///TODO, trigger time for trace analysis, that is the initial guess for the fit

#define MWIN 100 ///M value for energy filter from digi setting

Int_t idConst = 1010;

Int_t idDetMap[160] = { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //VME1-DIG1, STUB (not used on this Mac)
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //VME1-DIG2, STUB (not used on this Mac)
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //VME1-DIG3, STUB (not used on this Mac)
                        101,  100,  103,  102, 105,  104,  107,  106,  -1,  -1,   //VME1-DIG4, RECOIL
                   
                    //     22,  23,  18,  19,  20,  21,  22,  23,  -1,  -1,   // 1, VME2-DIG1, array
                    //     20,  21,  22,  23,  18,  19,  20,  21,  -1,  -1,   // 2, VME2-DIG2, array
                    //     12,  13,  14,  15,  16,  17,  18,  19,  -1,  -1,   // 3, VME2-DIG3, array
                    //     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //10, VME2-DIG4, *dead
                        
                    //     16, 17,  12,  13,  14,  15,  16,  17,  -1,  -1,   // 4, VME3-DIG1, array
                    //      8,  9,  10,  11,  12,  13,  14,  15,  -1,  -1,   // 5, VME3-DIG2, array
                    //      6,  7,   8,   9,  10,  11,   6,   7,  -1,  -1,   // 6, VME3-DIG3, array
                    //     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //11, VME3-DIG4, empty
                        
                    //      4,   5,   6,  7,  8,  9, 10, 11,  -1,  -1,   // 7, VME4-DIG1, array
                    //      2,   3,   4,  5,  0,  1,  2,  3,  -1,  -1,   // 8, VME4-DIG2, array
                    //      0,   1,   2,  3,  4,  5,  0,  1,  -1,  -1,   // 9, VME4-DIG3, array
                    //    200, 201,  202,  -1,  -1,  -1,  -1,  -1,  -1,  -1};  //12, VME4-DIG4, ELUM
                      
                        1,   0,   11,   10,   9,   8,   7,   6,  -1,  -1,   // 1, VME2-DIG1, array
                         3,   2,   1,   0,   5,   4,   3,   2,  -1,  -1,   // 2, VME2-DIG2, array
                        5,  4,   3,   2,   1,   0,   5,   4,  -1,  -1,   // 3, VME2-DIG3, array
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //10, VME2-DIG4, *dead

                        // 1,   0,   5,   4,   3,   2,   1,   0,  -1,  -1,   // 1, VME2-DIG1, array, left
                        //  3,   2,   1,   0,   5,   4,   3,   2,  -1,  -1,   // 2, VME2-DIG2, array
                        // 11,  10,   9,   8,   7,   6,   5,   4,  -1,  -1,   // 3, VME2-DIG3, array, bottom
                        // -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //10, VME2-DIG4, *dead


                         7,   6,  11,  10,   9,   8,   7,   6,  -1,  -1,   // 4, VME3-DIG1, array
                        15,  14,  13,  12,  11,  10,   9,   8,  -1,  -1,   // 5, VME3-DIG2, array
                        17,  16,  15,  14,  13,  12,  17,  16,  -1,  -1,   // 6, VME3-DIG3, array
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //11, VME3-DIG4, empty
                        
                        19,  18,  17,  16,  15,  14,  13,  12,  -1,  -1,   // 7, VME4-DIG1, array
                        21,  20,  19,  18,  23,  22,  21,  20,  -1,  -1,   // 8, VME4-DIG2, array
                        23,  22,  21,  20,  19,  18,  23,  22,  -1,  -1,   // 9, VME4-DIG3, array
                       -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};  //12, VME4-DIG4, ELUM



Int_t idKindMap[160] = { 0,   0,   0,   0,   0,   0,   0,   0,  -1,  -1,
                         1,   1,   1,   1,   1,   1,   1,   1,  -1,  -1,
                         2,   2,   2,   2,   2,   2,   2,   2,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         1,   1,   0,   0,   0,   0,   0,   0,  -1,  -1,//1
                         2,   2,   2,   2,   1,   1,   1,   1,  -1,  -1,//2
                         0,   0,   0,   0,   0,   0,   2,   2,  -1,  -1,//3
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         2,   2,   1,   1,   1,   1,   1,   1,  -1,  -1,//4
                         0,   0,   0,   0,   2,   2,   2,   2,  -1,  -1,//5
                         1,   1,   1,   1,   1,   1,   0,   0,  -1,  -1,//6
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         0,   0,   2,   2,   2,   2,   2,   2,  -1,  -1,//7
                         1,   1,   1,   1,   0,   0,   0,   0,  -1,  -1,//8
                         2,   2,   2,   2,   2,   2,   1,   1,  -1,  -1,//9
                        0,  1,  2,  -1,  -1,  -1,  -1,  -1,  -1,  -1};

#endif
