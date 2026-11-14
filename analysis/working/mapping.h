/***********************************************************************
 * 
 * mapping.h : the channel mapping (digitizer channel -> detector) for
 *             analysis/EventBuilder, and the array sizes (NARRAY, NRDT, ...)
 *             for Monitors / Cali macros, which include this file.
 *
 * ---- If the experiment needs more than one mapping (e.g. cabling for the
 *      alpha-source runs differs from the beam runs) :
 *
 *   1. copy this file to mapping_xxx.h (e.g. mapping_alpha.h) and edit only
 *      idDetMap / idKindMap there. Keep every #define (NARRAY, NRDT, NELUM,
 *      ..., POLARITY_*, MWIN) the SAME as in this file : Monitors / Cali
 *      macros are compiled with mapping.h only (process_Sort warns if the
 *      #define differ).
 *   2. create working/mappingByRun.sh to set MAPPING for each run, e.g.
 *         alphaRuns="1 2"   -> MAPPING=../working/mapping_alpha.h
 *         other runs        -> MAPPING=../working/mapping.h
 *      Armory/process_Sort sources it; without that file, all runs use mapping.h.
 *   3. process_Sort then builds the EventBuilder with that mapping
 *      ( make EventBuilder MAPPING=../working/mapping_xxx.h ), rebuilding only
 *      when the mapping changes. The mapping used is printed and saved in
 *      gen_runXXX.root as TMacro "mapping".
 *   Do not sort runs of different mappings at the same time (one binary).
 * 
 * idDetMap = 0XX ; PSD
 *          = 1XX ; Recoil,  dE of the Recoil MUST be odd
 *          = 2XX ; ELUM
 *          = 3XX ; EZERO
 *          = 4XX ; TAC & RF-Timing
 *          = 5XX ; Circular Recoil
 *          = 6XX ; APOLLO
 * 
 * idKindMap = 0 ; Energy (E)
 *           = 1 ; XF  (front position signal)
 *           = 2 ; XN  (near/back position signal)
 *           = -1 ; not in use
 * 
 * See EventBuilder/EventBuilder.cpp: case 0=Energy, case 1=XF, case 2=XN
 * 
***********************************************************************/

#ifndef MAPPING
#define MAPPING

#define NARRAY  24
#define NRDT    8
#define NELUM   16
#define NEZERO  0
#define NTAC    0
#define NCRDT   0
#define NAPOLLO 0

#define POLARITY_E   -1
#define POLARITY_XFXN   +1
#define POLARITY_RDT    -1
#define POLARITY_ELUM   +1
#define POLARITY_EZERO  +1
#define POLARITY_CRDT   +1
#define POLARITY_APOLLO +1

///TODO, trigger time for trace analysis, that is the initial guess for the fit

#define MWIN 100 ///M value for energy filter from digi setting

int idConst = 1010;

int idDetMap[160] = { 
                        101, 100, 103, 102, 105, 104, 107, 106,  -1,  -1,  //VME1-DIG1, RECOIL E, dE (VME 1 is top right)
                         -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  //VME1-DIG2, not in use
                         -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  //VME1-DIG3, not in use
                         -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  //VME1-DIG4, not in use


                         1,   0,   5,   4,   3,   2,   1,   0,  -1,  -1,   // 1, VME2-DIG1, array
                         3,   2,   1,   0,   5,   4,   3,   2,  -1,  -1,   // 2, VME2-DIG2, array
                        11,  10,   9,   8,   7,   6,   5,   4,  -1,  -1,   // 3, VME2-DIG3, array
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //10, VME2-DIG4, not in use
                        
                         7,   6,  11,  10,   9,   8,   7,   6,  -1,  -1,   // 4, VME3-DIG1, array
                        15,  14,  13,  12,  11,  10,   9,   8,  -1,  -1,   // 5, VME3-DIG2, array
                        17,  16,  15,  14,  13,  12,  17,  16,  -1,  -1,   // 6, VME3-DIG3, arrays
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //11, VME3-DIG4, not in use
                        
                       200, 201, 202, 203, 204, 205, 206, 207,  -1,  -1,   // 7, VME4-DIG1, array
                        21,  -1,  -1,  -1,  -1,  21,  21,  -1,  -1,  -1,   // 8, VME4-DIG2, array
                       208, 209, 210, 211, 212, 213, 214, 215,  -1,  -1,   // 9, VME4-DIG3, array
                       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,    //12, VME4-DIG4, not in use
                   
    
};


int idKindMap[160] = {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         1,   1,   0,   0,   0,   0,   0,   0,  -1,  -1,//1
                         2,   2,   2,   2,   1,   1,   1,   1,  -1,  -1,//2
                         0,   0,   0,   0,   0,   0,   2,   2,  -1,  -1,//3
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                         2,   2,   1,   1,   1,   1,   1,   1,  -1,  -1,//4
                         0,   0,   0,   0,   2,   2,   2,   2,  -1,  -1,//5
                         1,   1,   1,   1,   1,   1,   0,   0,  -1,  -1,//6
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//7
                         0,  -1,  -1,  -1,  -1,   1,   2,  -1,  -1,  -1,//8
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//9
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};


#endif
