/* Header file for GRETA decomposition routines
   Author:  D.C. Radford    Aug 2004
 */

#ifndef _gdecomp_h
#define _gdecomp_h

#define SUMS_FILE   "precalc.dat"       /* file containing precalculated sums */

#define GRID_PTS   228254  /* CHANGEME number of grid points in the basis */
#define GRID_SEGS  37      /* number of signals calculated for each basis point */
#define MEAS_SEGS  37      /* number of signals measured for each event */
#define TIME_STEPS 50      /* number of time steps calculated/measured for each segment */
#define TOT_SEGS   36      /* total number of segments in crystal */
#define MAX_AGS    1000  /* CHANGEME max. no. of points in coarse grid for AGS */

#define MAX_SEGS   8              /* max. number of segments to take in events */
#define MAX_PARS   (8*MAX_SEGS+1) /* max. number of parameters to fit */
#define MAX_INTPTS (2*MAX_SEGS)   /* max. number of interaction points */
#define SSEG       TOT_SEGS       /* range of seg in basis grid */
#define SRAD       34             /* range of r in basis grid */
#define SPHI       13             /* range of phi in basis grid */
#define SZZZ       22             /* range of z in basis grid */

#define COAL_DIST_DEFAULT 2.0


/* data structure for measured event signals */
typedef struct {
  float total_energy;                    /* total energy in crystal */
  float seg_energy[TOT_SEGS];            /* net energy in each segment */
  float signal[MEAS_SEGS][TIME_STEPS];   /* actual measured signals */
#ifdef TIMESTAMP
  long long int time;                    /* timestamp for this crystal event */
#endif
} Event_Signal;

/* data structure for calculated basis signals */
typedef struct {
  char  iseg, ir, ip, iz;             /* integer cylindrical coordinates of grid point */
  float x, y, z;                      /* cartesian coordinates of grid point */
  float signal[GRID_SEGS][50];        /* actual basis signals */
  int   lo_time[GRID_SEGS], hi_time[GRID_SEGS];     /* limits for non-zero signal */
} Basis_Point;

/* data structure for Adaptive Grid Search coarse grid */
typedef struct {
  int    npts;                /* number of AGS points for this segment */
  int    grid_pos[MAX_AGS];   /* pointer to basis-signal-ID for each AGS point */
  double *da;                 /* precalculated sums */
  float  *db;                 /* precalculated sums */
} Adaptive_Grid;

/* data structure for interactions */
typedef struct {
  int    seg;                 /* segment */
  int    pos;                 /* basis signal position, if applicable, else -1 */
  double r, p, z, e;          /* parameters */
  double dr, dp, dz, de;      /* uncertainties */
} Interaction;

extern Basis_Point   *basis;                 /* basis-signal data */
extern int           grid_pos_lu[SSEG][SRAD][SPHI][SZZZ];    /* basis-grid position look-up table */
extern int           maxir[SSEG], maxip[SSEG], maxiz[SSEG];  /* max. values of ir, ip, iz for each segment */
extern Adaptive_Grid ags1[SSEG];             /* Adaptive Grid Search coarse grid, 1 segment */

extern int           quiet;                  /* set to 1 to prevent output of diagnostic info */
extern int           average_sigs;           /* set to 1 to output averaged observed and 
						fitted signals for single-segment (net=1) events */
extern int           *bad_segs;              /* list of segment numbers that should be disabled */


/* ===================
   function prototypes
   =================== */

/* many of these modules could be combined... */

/* in read_basis.c: */
int    read_basis(char *basis_file_name);

/* in gdecomp.c: */
void   pclock(int print_flag);

/* in decompose.c: */
/* returns number of best-fit interactions found */
int decompose_1(const Event_Signal *asig,  /* observed signals */
		Event_Signal *bsig, /* fitted signals */
		int seg, Interaction *ints, double *t0, double *chisq_out, /* parameters */
		int grid2, int fit0, int fit1, int fit2, int fit3,         /* control */
		int fit4, int final_fit, int coalesce, double min_e_fraction);  /* control */
/* returns number of best-fit interactions found */
int decompose_n(const Event_Signal *asig,  /* observed signals*/
		Event_Signal *bsig, /* fitted signals*/
		int nseg, int *seg, int coalesce, 
		Interaction *ints, double *t0, double *chisq_out);  /* parameters */
/* returns final number of interactions found */
int postprocess_events(Interaction *ints, int nints, float total_e,
		       int ppflag, float coal_dist,
		       double *x, double *y, double *z, double *e);

/* in grid.c: */
int    grid_init(void);
double coarse_grid_1(const Event_Signal *asig,  /* observed signals */
		     int seg, Interaction *ints,
		     double *chisq0, double min_e_fraction);
double refine_grid_1(const Event_Signal *asig,  /* observed signals */
		     double chisq, double chisq0, double min_e_fraction,
		     Interaction *ints);

/* in fitter.c: */
double fitter(const Event_Signal *asig,  /* observed signals */
	      Event_Signal *bsig, /* fitted signals */
	      Interaction *ints, double *t0, int nints, int flag);

/* in eval.c: */
int    eval(const Event_Signal *asig,  /* observed signals */
	    Event_Signal *bsig, /* fitted signals */
	    Interaction *ints, double t0, int nints,
	    double *chisq_out, double beta[MAX_PARS],
	    double alpha[MAX_PARS][MAX_PARS], int calc_deriv, int *ssel);
double eval_int_pos(const Event_Signal *asig,  /* observed signals */
		    Event_Signal *bsig, /* fitted signals */
		    Interaction *ints, double t0, int nints);

/* in interpolate.c: */
int    nearest_grid_points(int seg, double rin, double pin, double zin,
			   float *rdiff, float *pdiff, float *zdiff, int pos_out[8]);
int    interpolate(int seg, double rin, double pin, double zin, Basis_Point *sig,
		   Basis_Point sigderiv[3], int calc_deriv, int *ssel);
int    cyl_to_cart(int seg, double *pars, double*x, double *y, double*z, double *e);

/* in matinv.c: */
int    matinv(double *array, int norder, int dim);

/* data structs added to allow multithreading */

struct decomp_errcnt {
  int nonet;
  int toomanynet;
  int sumener;
  int badchisq;
};

struct decomp_state {
  Event_Signal *bsig;
  Interaction  *ints;
  int   cnt;
  float coal_dist;	
  struct decomp_errcnt *err;
};

#if(0)

/*--------------------------------------*/
/* format from before SEP/OCT 2011......*/
/* ER-{1-6}.............................*/
/*--------------------------------------*/

typedef struct DCR_INTPTS {
    float x, y, z;
    float e;
    } DCR_INTPTS;

struct crys_intpts {
  int   num;
  int crystal_id;
  float tot_e;
  float t0;
  float chisq;
  float norm_chisq;
  long long int timestamp;
  DCR_INTPTS intpts[MAX_INTPTS];
};

#endif

#define GEB_TYPE_DECOMP         1
#define GEB_TYPE_RAW            2
#define GEB_TYPE_TRACK          3
#define GEB_TYPE_BGS            4
#define GEB_TYPE_S800_RAW       5   
#define GEB_TYPE_NSCLnonevent   6
#define GEB_TYPE_GT_SCALER      7 
#define GEB_TYPE_GT_MOD29       8
#define GEB_TYPE_S800PHYSDATA   9
#define GEB_TYPE_NSCLNONEVTS   10
#define GEB_TYPE_G4SIM         11
#define GEB_TYPE_CHICO         12
#define GEB_TYPE_DGS           14
#define GEB_TYPE_DGSTRIG       15
#define GEB_TYPE_DFMA          16

#define MAX_GEB_TYPE           17   

/* this structure tells what follows */

struct gebData {
  int type; /* type of data following */
  int length;
  long long timestamp;
};

#if(0)

/*--------------------------------*/
/* format after SEP/OCT 2011......*/
/* SHE/BGS runs...................*/
/*--------------------------------*/



typedef struct DCR_INTPTS {
    float x, y, z, e;       /* here e refers to the fraction */
    int seg;                /* segment hit */
    float seg_ener;         /* energy of hit segment */
    } DCR_INTPTS;

struct crys_intpts {
  int type;                                             /* typically: abcd1234 */
  int crystal_id;
  int num;
  float tot_e;
  long long int timestamp;
  long long trig_time;
  float t0;
  float cfd;
  float chisq;
  float norm_chisq;
  float baseline;
  int pad;                      /* to ensure 8-byte alignment of struct */
  DCR_INTPTS intpts[MAX_INTPTS];
};

#endif

#if(1)

/*----------------------------------*/
/* format user at MSU with the S800 */
/*----------------------------------*/


typedef struct DCR_INTPTS {
    float x, y, z, e;       /* here e refers to the fraction */
    int seg;                /* segment hit */
    float seg_ener;         /* energy of hit segment */
    } DCR_INTPTS;


struct crys_intpts {
  int type;          /* defined as abcd5678 */
  int crystal_id;
  int num;           /* # of int pts from decomp, or # of nets on decomp error */
  float tot_e;       /* dnl corrected */
  int core_e[4];     /* 4 raw core energies from FPGA filter (no shift) */
  long long int timestamp;
  long long trig_time;    /* not yet impl */
  float t0;
  float cfd;
  float chisq;
  float norm_chisq;
  float baseline;
  float prestep;    /* avg trace value before step */
  float poststep;   /* avg trace value following step */
  int pad;          /* non-0 on decomp error, value gives error type */
  DCR_INTPTS intpts[MAX_INTPTS];
};


/* Meaning of error code (pad) */

/*    pad = 1   a null pointer was passed to dl_decomp() */
/*        = 2   total energy below threshold */
/*        = 3   no net charge segments in evt */
/*        = 4   too many net charge segments */
/*        = 5   chi^2 is bad following decomp (in this case */
/*              crys_intpts is non-zero but post-processing step is not applied)  */    
   
#endif

typedef struct crys_intpts CRYS_INTPTS;
typedef struct gebData GEBDATA;

/* external data can be bigger than GRETINA data */
/* so allocate more space for external data */

#define MAXDATASIZE 10000

#define DCRHDRLEN sizeof(CRYS_INTPTS)-MAX_INTPTS*sizeof(DCR_INTPTS)

/* in decompose.c: */

struct decomp_state *dl_decomp_init(char *basis_file_name, int set_quiet);
/* global decomp init call; use once per program */
int dl_decomp_init_global(char *basisfile, int quiet);
/* use the next init call once per thread */
struct decomp_state *dl_decomp_init_thread();

struct crys_intpts *dl_decomp(struct decomp_state *di, Event_Signal *asig);
char *dl_crys_intpts_2s(struct crys_intpts *x);
void dl_set_coal_dist(struct decomp_state *inst, float d);
int num_net(Event_Signal *asig);


#endif	/* _gdecomp_h */
