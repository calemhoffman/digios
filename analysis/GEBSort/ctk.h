
#ifndef CTKHINCLUDED
#define CTKHINCLUDED

#include <stdio.h>
#include <stdint.h>

#define MAXFLOAT 3.40282347e+38
#define MAXINT   2147483647

/* if you change  MAXPERM, must also modify MAXPERM */
/* MAXNOSEG=9 is about maximum we can handle */

#define MAXNOSEG 9
#define MAXPERM MAXNOSEG*8*7*6*5*4*3*2
#define MAXFACULTY 20

#define MAXDETPOS 30
#define MAXCRYSTALNO 3
#define NRETURNCODES 30

#define MODULENOOUTOFRANGE 501
#define CRYSTALNOOUTOFRANGE 502
#define DEBUGSTOP 99
#define TOOMANYHITS 98
#define BADPAD 97
#define DOUBLEHITS 200

#define STRLEN 512
#define SHORTLEN 1024
#define LONGLEN 16384
#define PMODE 0644
#define MAXSHELLHITS 500
#define MAXCLUSTERHITS 200
#define HWHMtoSIG 2.3548205
#define EPS 0.001

#define PI      3.14159265
#define PIHALF  PI/2
#define RAD2DEG 0.01745329

#define SIMULATED 0
#define MAXDETNO 200
#define MAXFOM 101
#define BADFOM MAXFOM+1
#define MAX_GAMMA_RAYS 30

#define MAX_SEG TOT_SEGS+1
#define MAXBIGBUFSIZ 50000

/*-------------------------*/
/* basic interaction point */
/*-------------------------*/
/* now comes from "gdecomp.h" */

#include "gdecomp.h"

/*---------------------------------*/
/* new/final crystal hit structure */
/*---------------------------------*/

typedef struct PAYLOAD {
  char p[MAXDATASIZE];
} PAYLOAD;

typedef struct TRACK_STRUCT {
  int n; 
  GEBDATA *gd;
  PAYLOAD *payload;
} TRACK_STRUCT;

#define MAXTRACK (MAXDETPOS+1)*(MAXCRYSTALNO+1)*2
#define MAX_inp_SIZE  MAXTRACK*sizeof(CRYS_INTPTS)


typedef struct EVENT {
  GEBDATA *gd;
  PAYLOAD *payload;
} EVENT;

typedef struct bigbuf_struct
  {
  char *ev[MAXBIGBUFSIZ];
  } BIGBUF;

/*-----------------------------------*/
/* after the clustering task the     */
/* clusters of gamma rays are stored */
/* in this structure. each cluster   */
/* is then tracked.                  */
/*-----------------------------------*/
/* note: we used originally MAX_NDET  */
/* and not MAX_INTPTS to have */
/* extra space. Not sure it it is necessary */
/* --- it is necessary it seems */

#define MAX_NDET 30



typedef struct CL_INTPTS {
    float xx, yy, zz;
    float edet;
    int   order;
    long long int timestamp;
    int   shellHitPos;
    int   detno;
#if SIMULATED
    float esumOrig;
    int   origPos;
#endif
    } CL_INTPTS;

typedef struct CLUSTER_INTPTS {
  int   valid;
  int   ndet;
  int   tracked;
  float fom;
  float esum;
  int   trackno;
  int   bestPermutation;
  int   processsed;
  CL_INTPTS intpts[MAX_NDET];
} CLUSTER_INTPTS;

/* structure for the tracked gamma rays */
/* written to the output with geb ID GEB_TYPE_TRACK==3 */

typedef struct TRACKED_GAMMA_RAY {
    float esum;              /* gamma ray energy */
    int ndet;                /* number of interactions */
    float fom;               /* figure of merit */
    int   tracked;           /* 1==if tracked */
    long long int timestamp; /* timestap of first interaction point */
    float x0, y0, z0, e0;    /* first interaction point */
    float x1, y1, z1, e1;    /* second interaction point */
    } TRACKED_GAMMA_RAY;

typedef struct TRACKED_GAMMA_HIT {
      int ngam;
      int pad;
      TRACKED_GAMMA_RAY gr[MAX_GAMMA_RAYS];
    } TRACKED_GAMMA_HIT;


/* for Dirk's stuff */

typedef struct COOR {
  float x;
  float y;
  float z;
  float theta;
  float phi;
} COOR;

#define CLUSTERHDRLEN sizeof(CLUSTER_INTPTS)-MAX_NDET*sizeof(CL_INTPTS)

/*------------------------------*/
/* GRETINA shell hit input data */
/* as we store it in cltk.c     */
/*------------------------------*/

/* this is the workhorse of the tracking task */

/* for simulated data knownclusterNumber contains    */
/* the real cluster number in the sequence      */
/* 0...nclusters. For real data we do not       */
/* know this at the time of input; that is what */
/* we are going to determine. ncluster same     */
/* story */

/*--------------------------------------*/
/* tracking parameters from chat script */
/*--------------------------------------*/
/* they are described in the chat file */

typedef struct TRACKINGPAR
{
  long long int dtwin;
  int  nprint;
  int  maxevents;
  int  ntracks;
  int  dataType;
  int  useCCEnergy;
  int  useSegEnergy;
  char data[STRLEN];
  float alpha[MAXSHELLHITS];
  float gamma;
  float fomJump;
  float fomGoodenough;
  float snglsFom;
  float target_x;
  float target_y;
  float target_z;
  int fixdoublehits;
  int fixdoublehitsCode;
  int nocrystaltoworldrot;
  float CCcal_gain[MAXDETNO+1];
  float CCcal_offset[MAXDETNO+1];
  float SEGcal_gain[MAXDETNO + 1][MAXCRYSTALNO + 1];
  float SEGcal_offset[MAXDETNO + 1][MAXCRYSTALNO + 1];

  int splitclusters1;
  float splitclusters_kickoutfom1;
  float splitclusters_thresh1;
  int splitclusters_minn1;
  int splitclusters_maxn1;
  int splitclusters_maxtry1;

  int splitclusters2;
  float splitclusters_kickoutfom2;
  float splitclusters_thresh2;
  int splitclusters_minn2;
  int splitclusters_maxn2;
  int splitclusters_maxtry2;

  int recluster1;
  float recluster_kickoutfom1;
  float recluster_thresh1;
  int recluster_minn1;
  int recluster_maxtry1;
  float recluster_reduxfactor1;

  int recluster2;
  float recluster_kickoutfom2;
  float recluster_thresh2;
  int recluster_minn2;
  int recluster_maxtry2;
  float recluster_reduxfactor2;

  float untracked_fom_kickout;

  int combineclusters;
  float combineclusters_kickoutfom;
  float goodImproveFraction;
  float combineclusters_thresh;
  int combineclusters_maxn;
  float combineMaxDist;

  int matchmaker;
  float matchmakerMaxDist;
  float matchmaker_kickoutfom;

  int ndetmin;
  int trackDataStream;
  int trackOps[MAXNOSEG];
  int nperm[MAXNOSEG];
  float ndetElim_lo[MAXNOSEG];
  float ndetElim_hi[MAXNOSEG];
  float ndetElim_fom[MAXNOSEG];
  int permlkup[MAXNOSEG][MAXPERM][MAXNOSEG];
  int jmpGrpLen[MAXNOSEG];
  int   marksinglehitrange;
  float singlehitrange[LONGLEN];
  float singlehitrangeFOM;
  int   itterations;
  float crmat[MAXDETPOS + 1][MAXCRYSTALNO + 1][4][4];
  long long fac[MAXFACULTY];

  int enabled[MAXDETNO+1];
  int detNo[MAXDETPOS];

  float dirk_rcf[MAXDETNO];

} TRACKINGPAR;

/*------------*/
/* statistics */
/*------------*/

typedef struct STAT
{
  int nhit[164],nsfom_nt;
  long trackGetCalls;
  long trackPassCount;
  long trackFMok;
  long badpad;
  long doubleSegHits;
  long SegHits;

  double sfom, sfom_nt;
  
  /*int  permHit[MAXNOSEG][MAXPERM];*/
  long nperm;
  long numInputDataLines;
  long goodtrak;
  long badtrack;
  long notTracked;
  unsigned int noTrueClusters;
  unsigned int      nEvents;
  unsigned int      nClusters;
  long long int ndetToLarge;

  unsigned int      TrackingCalls;

  unsigned int      firstClusterTrackCalls;
  unsigned int      ctktk0_errors[NRETURNCODES];
  unsigned int      ctktk1_errors[NRETURNCODES];
  unsigned int      ctktk3_errors[NRETURNCODES];
  unsigned int      ctktk4_errors[NRETURNCODES];
  unsigned int      ctktk5_errors[NRETURNCODES];

  unsigned int      splitClusterReturns1[NRETURNCODES];
  unsigned int      splitClusterReturns2[NRETURNCODES];
  unsigned int      reClusterReturns1[NRETURNCODES];
  unsigned int      reClusterReturns2[NRETURNCODES];
  unsigned int      combineClusterReturns[NRETURNCODES];
  unsigned int      matchMakerReturns[NRETURNCODES];

  unsigned int      splitClusterTrackCalls1[NRETURNCODES];
  unsigned int      splitClusterTrackCalls2[NRETURNCODES];
  unsigned int      reClusterTrackCalls1[NRETURNCODES];
  unsigned int      reClusterTrackCalls2[NRETURNCODES];
  unsigned int      combineClusterTrackCalls[NRETURNCODES];
  unsigned int      matchMakerTrackCalls[NRETURNCODES];

  long long int     fullEnergyEv;
  long long int     comptonLossEnergyEv;

  long long int     singlehitoutofrange;
  long long int     badndetElim;

  long long int     gammasForWriteout;
  long long int     gammasForWriteout_OK1;
  long long int     gammasForWriteout_OK2;
  long long int     gammasWritten;
  long long int     trackedEventsWritten;

} STAT;

#define IDLEN 16384
typedef struct GTID {
  int len;
  char IDSTR1[7];
  int ID; 
  char  payload[IDLEN-2*sizeof(int)-2*7];
  char IDSTR2[7];
} GTID;

/* use the GTID structure as something like this:  */
/*   GTID idbuf; */
/*   sprintf(idbuf.IDSTR1,"GRETINA"); */
/*   sprintf(idbuf.IDSTR2,"GRETINA"); */
/*   idbuf.len=IDLEN; */
/*   then fill payload with ID specific data */

typedef struct SHELLHIT
{
  /* raw inputs */

  int nhit;
  
  float t0[MAXSHELLHITS]; 
  float chisq[MAXSHELLHITS];  
  float norm_chisq[MAXSHELLHITS];
  long long int timestamp[MAXSHELLHITS];
  int crystal_id[MAXSHELLHITS]; 
  float XX[MAXSHELLHITS];
  float YY[MAXSHELLHITS];
  float ZZ[MAXSHELLHITS];
  float edet[MAXSHELLHITS];
  float esum[MAXSHELLHITS];
  int   origPos[MAXSHELLHITS];
  int   detno[MAXSHELLHITS];
  
  /* unpacked identification */
  
  int module[MAXSHELLHITS];
  int crystaltype[MAXSHELLHITS];
  
  /* normalized vectors */

  float nXX[MAXSHELLHITS];
  float nYY[MAXSHELLHITS];
  float nZZ[MAXSHELLHITS];
  
  /* angles, relative angles and distance */
  
  float relAng[MAXSHELLHITS][MAXSHELLHITS];
  float polAng[MAXSHELLHITS];
  float aziAng[MAXSHELLHITS];
  float rLen[MAXSHELLHITS];
  
  /* deduced cluster information */
  
  int   NumClusters;
  int   ClusterNumber[MAXSHELLHITS];  
  
  /* for simulated data only */
  /* where we know true answer */

  int   knownNumClusters;
  int   knownClusterNumber[MAXSHELLHITS];
  
} SHELLHIT;

/*----------------------------------*/
/* anlysis parameters, not strictly */
/* part of the tracking task        */
/*----------------------------------*/

typedef struct ANAPAR
{
 float beta;
 char  rootFileName[STRLEN];
 int   nBinCh2d;
 int   nSMAP;
 float loBinVal;
 float hiBinVal;  
 char  testdatafn[STRLEN];
 int   havetestdata;
 int   nprint;
 int   rayno;
 int   evno;
 float target_x;
 float target_y;
 float target_z;
 float greta_ata;
 float greta_bta;
 float ytascale;
 float fomlo[MAXDETNO];
 float fomhi[MAXDETNO];
 int ndetlimlo,ndetlimhi;
 int enabled[MAXDETNO+1];
 int detNo[MAXDETPOS];
 int EXT_tac_hi;
 int EXT_tac_lo;
 int EXT_e_hi;
 int EXT_e_lo;
 int gateexternal;
 float detpolang[MAXDETNO+1];
 float detdopfac[MAXDETNO+1];
 float beamdir[3];
 int havePIDwin;
 char PIDwinfile[STRLEN];
 int simpleDopplerCor;
 int S800_PID_NX;
 float S800_PID_XLO;
 float S800_PID_XHI;
 int S800_PID_NY;
 float S800_PID_YLO;
 float S800_PID_YHI;
 int GGMAX;
 float rmin;
 float rmax;
 float minazi, maxazi, minpol, maxpol;
 float crmat[MAXDETNO + 1][MAXCRYSTALNO + 1][4][4];
} ANAPAR;

/*---------------*/
/* BGS structure */
/*---------------*/

typedef  struct BGS_STRUCT {
  int nEsi;
  int Esi[200];
  } BGS_STRUCT;

/*---------------------------------------------*/
/* struture for use with splitting of clusters */
/*---------------------------------------------*/

typedef  struct PM {
    int np1;			/* number of members of group 1 */
    int np2;			/* number of members of group 2 */
    int p1[20];			/* members of group 1 */
    int p2[20];			/* members of group 2 */
    float s1;
    float s2;
    float likely1;
    float likely2;
    float dist;
    float fom1;
    float fom2;
  } PM;

/*
 * S800 pre-processed (currently poor Dirk's version)
 *
 * PID plot made from tof_obje1/xfpe1/rfe1 versus ic _de
 *
 * ata/bta/dta/yta used for Doppler reconstruction
 *
 * gap = 1073mm, zfp = 0.
 * fp_afp = atan((crdc2_x - crdc1_x) / gap)
 * fp_bfp = atan((crdc2_y - crdc1_y) / gap)
 * fp_xfp = crdc1_x / 1000 + zfp * tan(fp_afp);
 * fp_yfp = crdc1_y / 1000 + zfp * tan(fp_bfp);
 *
 *
 */
#define S800PHYSDATA_NOTVALIDDATA -9999.999
#define S800PHYSDATA_TYPETAG  0xABCD1234
struct S800_physicsdata {
  int32_t type;    /* defined abcd1234 for indicating this version */
  float crdc1_x;   /* Crdc x/y positions in mm */
  float crdc1_y;
  float crdc2_x;
  float crdc2_y;
  float ic_sum;    /* ion chamber energy loss         */
  float tof_xfp;   /* TOF scintillator after A1900    */
  float tof_obj;   /* TOF scintillator in object box  */
  float rf;        /* Cyclotron RF for TOF            */ 
  int32_t trigger; /* Trigger register bit pattern    */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* from here corrected values extracted from data above */ 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  float ic_de;
  /* TOF values with TOF correction applied (from afp/crdc x) */
  float tof_xfpe1;
  float tof_obje1;
  float tof_rfe1;
  /* Trajectory information at target position calculated from 
     a map and afp/bfp/xfp/yfp. New map and you need to re-calc */
  float ata; /* dispersive angle        */
  float bta; /* non-dispersive angle    */
  float dta; /* dT/T T:kinetic energy   */
  float yta; /* non-dispersive position */
};
typedef struct S800_physicsdata S800_PHYSICSDATA;


/*------------*/
/* prototypes */
/*------------*/

 int ctk_pr_trackPar(void);
 int ctkgttrack(int);
 int prCurTrack(int);
 int ctkinit();
 int ctksort(int,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *nClusters);
 int findVector(float,float,float,float,float,float,float*,float*,float*);
 int findAngle(float *,float*, float*);
 int findCAngle(float,float,float*);
 int ctkRmat();
 int ctkBin();
 int ctkWriteSpectra();
 int ctktk0(int, float target_pos[3], STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *);
 int ctktk1(int, float target_pos[3], STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *);
 int ctktk2(int, float target_pos[3], STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *);
 int ctktk3(int, float target_pos[3], STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *);
 int ctktk4(int, float target_pos[3], STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *);
 int ctktk5(int, float target_pos[3], STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *);
 int ctkTrackOpt(int);
 int ctkBin();
 int ctkWriteSpectra();
 int ctkanaBin();
 int ctkROOTBin(TRACK_STRUCT *,float polang[MAXCLUSTERHITS], float doppler_factor[MAXCLUSTERHITS],BGS_STRUCT *, long long int);
 int ctkROOTexit();
 int ctkROOTSetup();
 int printTrack(TRACK_STRUCT *);
 int ctkStats();
 int wr_spe(char *, int *, float *);
 int printEvent(FILE *, int , TRACK_STRUCT *);
 int printTrackedEvent(FILE *, int);
 int printCluster(int ,  FILE *,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS]);
 int splitCluster (int , int, STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *nClusters, float target_pos[3], float, int, float);
 int combineCluster (int , int, STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *nClusters, float target_pos[3]);
 int clusterUpdate ( int,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS] );
 int printAllClusters (char *, char *,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int * );
 int matchMaker(int, int , int , STAT *ctkStat,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *nClusters, float target_pos[3]);
 int readChatFile ();
 int setupTrack ();
 int trackEvent (float target_pos[3], TRACK_STRUCT *, STAT *ctkStat, SHELLHIT *,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *);
 int writeTrack (int, TRACK_STRUCT * track,CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *, STAT *);
 void *serializeTrack (TRACK_STRUCT * track, CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *nClusters, int *bufferlen);
 int printShellHit (FILE *, SHELLHIT *);
 int streamlineClstr( int *, CLUSTER_INTPTS , CLUSTER_INTPTS );
 int reCluster (int , int , STAT *, CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], int *, float target_pos[3],
	   SHELLHIT *, float , int, float );
 int plugClstr (int *, CLUSTER_INTPTS Clstr[MAXCLUSTERHITS], STAT * ctkStat);
 int printCRYS_INTPTS (FILE *, CRYS_INTPTS *, GEBDATA *);
 int print_tracked_gamma_rays (FILE *, TRACKED_GAMMA_HIT *);
#endif

