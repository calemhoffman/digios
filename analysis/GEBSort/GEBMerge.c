
#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <assert.h>
#include <zlib.h>
#include <string.h>
#include <unistd.h>

/* ------- */
/* globals */
/* ------- */

#define CLK_TCK sysconf(_SC_CLK_TCK)


#include "gdecomp.h"
#include "ctk.h"
#include "GTMerge.h"

#define MAXNFIX 10000
#define MAXPAYLOADSIZE 10000

/* globals */

long long int curTS;
MSTAT *nstat;
EVENT Event[MAXCOINEV];
int nBadTestPat = 0;
CONTROL control;

int tlkup[NCHANNELS];
int tid[NCHANNELS];
#if(USEZLIB==0)
off_t inData[MAXFILES];
//int inData[MAXFILES];
#endif
#if(USEZLIB==1)
gzFile zFile[MAXFILES];
#endif

int Counter;
int TEMP[MAXLENINTS];


long long int oldFileTS[MAXCOINEV];
int whatFile[MAXCOINEV];
long long int curT = 0, firstTS = 0;
int NprintEvNo = 0;
int cevent = 0;
int NumToPrint = 10;
FILE *TSlist;
int nTSlist = 0;

#if (USEBREAD)
int *bread_buf[MAXFILES];
int bread_pos[MAXFILES];
int bread_bufsiz[MAXFILES];
#endif

/* big buffer, very simple, so we can sort */

char *bigbuf[MAXBIGBUFSIZ];

/*---------------------------------------------------------------------*/

void
quickSort (char **item, int left, int right, int TSOffinP)
{

  /* specialized qsort that will */
  /* sort on an unsigned long long int */
  /* in the payload of the pointer */

  int i, j;
  char *y;
  unsigned long long int x;

  i = left;
  j = right;
  x = *(unsigned long long int *) (item[(left + right) / 2] + TSOffinP);

  do
    {
      while (*(unsigned long long int *) (item[i] + TSOffinP) < x && i < right)
        i++;
      while (x < *(unsigned long long int *) (item[j] + TSOffinP) && j > left)
        j--;

      if (i <= j)
        {
          if (i!=j)
            {
            y = item[i];
            item[i] = item[j];
            item[j] = y;
            nstat->nswaps++;
            };
          i++;
          j--;
        }
    }
  while (i < j);

  if (left < j)
    quickSort (item, left, j, TSOffinP);
  if (i < right)
    quickSort (item, i, right, TSOffinP);

}

/*----------------------------------------------------------------------------*/

int
#if (USEZLIB == 0 )
bread (int in, char *val, int *pos, int *buf, int *bufsiz)
#endif
#if (USEZLIB == 1 )
bread (gzFile in, int *val, int *pos, int *buf, int *bufsiz)
#endif
{

  /* buffered inter read */

  /* declarations */

  int siz;

  /* read new buffer */

  if (*pos == 0)
    {

      /* read in a buffer of data */

//      printf ("in = %i, bufsiz=%i\n", in,*bufsiz*sizeof(int));

#if (USEZLIB == 0 )
      siz = read (in, (char *) buf, *bufsiz * sizeof (int));
//      printf ("read regular buffer of size %i\n", siz);
#endif
#if (USEZLIB == 1 )
      siz = gzread (in, (char *) buf, *bufsiz * sizeof (int));
//      printf ("read zipped buffer of size %i\n", siz);
#endif

      *bufsiz = siz / sizeof (int);

      if (siz <= 0)
        return (-1);

    };

  /* return a value */

  *val = *(buf + (*pos));
//  printf("pos= %i, *(buf+(*pos))=%i, *val=%i\n", *pos,*(buf+(*pos)),*val );
  (*pos)++;

  /* force buffer read the next time? */

  if (*pos == *bufsiz)
    *pos = 0;

  /* done, report we got 4 bytes like 'read' would */

  return (sizeof (int));

};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
GTGetDiskEv (int FileNo, int storeNo)
{

  /* returns: */
  /*   0: success */
  /*   1: header */
  /*   2: read trouble; end of file */
  /*   5: bad test pattern */

  /* declarations */

  static int nn = 0, mm = 0;
  int siz, i, i1, k;
  unsigned int j;
  unsigned int testPattern = 0, bufPos = 0;
  unsigned int PMET[MAXLENINTS];
  unsigned int t1, t2, t3, t4;
  float r1;

#if(0)
  printf ("entered GTGetDiskEv, mm=%i\n", mm);
  if (mm >= 3)
    {
      printf ("debug stop in GTGetDiskEv\n");
      exit (0);
    };
#endif

  /* attempt to read a GEB header */

#if (USEBREAD)
  siz =
    bread (inData[FileNo], (char *) Event[storeNo].gd, &bread_pos[FileNo], bread_buf[FileNo], &bread_bufsiz[FileNo]);
#else
  siz = read (inData[FileNo], (char *) Event[storeNo].gd, sizeof (GEBDATA));
#endif
  if (siz != sizeof (GEBDATA))
    {
      printf ("failed to read %i bytes for header, got %i\n", sizeof (GEBDATA), siz);

      return (1);
    };
  nstat->inbytes += siz;
  control.filesiz[FileNo] += siz;

  if (mm < 10)
    printf ("\ngot initial header, TS=%lli for storeNo=%i\n", Event[storeNo].gd->timestamp, storeNo);

  /* attempt to read payload */

  i1 = Event[storeNo].gd->length;
#if (USEBREAD)
  siz =
    bread (inData[FileNo], (char *) Event[storeNo].payload, &bread_pos[FileNo], bread_buf[FileNo],
           &bread_bufsiz[FileNo]);
#else
  siz = read (inData[FileNo], (char *) Event[storeNo].payload, i1);
#endif
  if (siz != i1)
    {
      printf ("failed to read %i bytes for payload, got %i\n", i1, siz);
      return (2);
    };
  nstat->inbytes += siz;
  control.filesiz[FileNo] += siz;
  control.fileEventsRead[FileNo]++;

  if (mm < 10)
    printf ("read initial payload of siz=%i into  storeNo=%i\n", siz, storeNo);

  mm++;

  /* done */

  return (0);

}

/* -------------------------------------------------------------------------- */

int
reportStats (struct tms *timesThen, int nfiles, float dtb)
{

  /* declarations */

  int i, nreads = 0, hh, mm, ss;
  float r1, r2;
  double d1;
  int time_stamp (void);
  struct tms timesNow;
  double totalTime;

  times ((struct tms *) &timesNow);

  /* print info */

  printf ("\n**----------------------------------------\n");
  for (i = 0; i < MAXFILES; i++)
    if (control.fileActive[i] || (control.fileEventsRead[i] > 0))
      {
        printf ("file %3i is ", i);
        if (control.fileActive[i])
          printf ("    active; ");
        else
          printf ("NOT active; ");
        printf ("%10i reads ", control.fileEventsRead[i]);
        nreads += control.fileEventsRead[i];
        printf ("; %10i bytes; ", control.filesiz[i]);
        r1 = control.filesiz[i] / 1024.0 / 1024.0 / 1024.0;
        printf ("(%7.3fGB) ", r1);
        if (control.fileActive[i])
          printf ("TS=%18lli", Event[i].gd->timestamp);
        else
          printf (" -- completed");
        printf ("\n");

      };

  printf ("current TS: %12lli; ", curTS);
  printf ("current event no: %9i;\n", control.CurEvNo);
  printf ("events written: %9i; ", control.nwritten);
  printf ("total reads=%9i;", nreads);
//  r1=(float)control.nwritten/(float)nreads;
//  printf(" frac=%f",r1);
  printf ("\n");
  d1 = (double) (curTS - firstTS);
  d1 *= 10e-9;
  hh = d1 / 3600;
  d1 -= hh * 3600;
  mm = d1 / 60;
  d1 -= mm * 60;
  ss = d1;
  printf ("elapsed time..................: %3.3ih%2.2im%2.2is, [t0=%lli, curTS=%lli]\n", hh, mm, ss, firstTS, curTS);
  time_stamp ();
  fflush (stdout);
  printf ("number of open files: %4i\n", control.nOpenFiles);

  d1 = timesNow.tms_utime - timesThen->tms_utime;
  totalTime = d1;
  printf ("CPU time: user %9.3f sec, ", d1 / CLK_TCK);
  r1 = timesNow.tms_stime - timesThen->tms_stime;
  totalTime += d1;
  printf ("system  %9.3f sec, ", d1 / CLK_TCK);
  totalTime /= CLK_TCK;
  printf ("Total  %9.3f sec\n", totalTime);

#if(0)
  for (i = 0; i < nfiles; i++)
    {
      if (nstat->nTSjumprecover_f[i] > 0)
        {
          printf ("nstat->nTSjumprecover_f[%i]=%10i; ", i, nstat->nTSjumprecover_f[i]);
          r1 = 100.0 * (float) nstat->nTSjumprecover_f[i] / (float) nreads;
          printf ("or %9.6f%%\n", r1);
        };

      if (nstat->nTSjumprecover_b[i] > 0)
        {
          printf ("nstat->nTSjumprecover_b[%i]=%10i; ", i, nstat->nTSjumprecover_b[i]);
          r1 = 100.0 * (float) nstat->nTSjumprecover_b[i] / (float) nreads;
          printf ("or %9.6f%%\n", r1);
        };

    };
#endif


  printf ("bigbuf fills..........= %10i\n", nstat->nbigbufreads);
  printf ("time order swaps......= %10i\n", nstat->nswaps);
  r1 = (float) nstat->nswaps / (float) nstat->nbigbufreads;
  printf ("swaps per buffer......= %10.2f\n", r1);
  printf ("bigbuf covers %10.3f ms\n", (float) dtb);


  r1 = nstat->inbytes / 1024.0 / 1024.0 / 1024.0;
  r2 = nstat->inbytes / 1024.0 / 1024.0 / totalTime;
  printf ("# bytes read...: %12lliB, %7.3fGB; %7.3fMB/sec; ", nstat->inbytes, r1, r2);
  printf ("%7.3fGB/hour, ", r2 * 3600. / 1000.);
  printf ("%3.1fTB/day\n", r2 * 3600. / 1000. / 1000. * 24);
  r1 = nstat->outbytes / 1024.0 / 1024.0 / 1024.0;
  r2 = nstat->outbytes / 1024.0 / 1024.0 / totalTime;
  printf ("# bytes written: %12lliB, %7.3fGB; %7.3fMB/sec\n", nstat->outbytes, r1, r2);

  r2 = (nstat->inbytes + nstat->outbytes) / 1024.0 / 1024.0 / totalTime;
  printf ("total IO rate: %7.3fMB/sec or %5.0fMbits/sec\n", r2, r2 * 8);

  r1 = (float) control.chunksiz / 1024.0 / 1024.0 / 1024.0;
  printf ("output chunk # %i (chunksiz=%i bytes, or %6.3f GBytes)\n", control.chunkno, control.chunksiz, r1);

//  for (i = 0; i < NCHANNELS; i++)
//    if (nstat->id_hit[i] > 0)
//      printf ("id=%5i, had %i hits \n", i, nstat->id_hit[i]);

  printf ("^^----------------------------------------\n");
  printf ("\n");
  fflush (stdout);

  /* done */

  return (0);

}

/* ------------------------------------------------------ */

void
CheckNoArgs (int required, int actual, char *str)
{

  if (required < actual)
    {
      printf ("argument problem with chat option\n");
      printf ("--> %s\n", str);
      printf ("required # arguments: %i\n", required - 1);
      printf ("actual   # arguments: %i\n", actual - 1);
      printf ("Please fix and try again, quitting...\n");
      exit (1);
    };

}

/* -------------------------------------------------------------------------- */

int
main (int argc, char **argv)
{

  /* declarations */

  off_t outData;                /* input file */
  gzFile zoutData;
  int nfiles = 0, maxNoEvents = 0, nPoolEvents = 0, siz, firsttime, nbad;
  int i1, i2, i3, i4, badRead, nfe, ii, njb = 0, njf = 0;
  int i, j, st, nextTSpoolIndex, nn, argcoffset = 0;
  DGSHEADER outheader;
  long long int oldListTS, dTS, lltmp, oldTS, lli1;
  unsigned long long int nextTS, ulli1, ulli2;
  float dtbtev[LENSP];
  float ehigain[NGE + 1], ehioffset[NGE + 1], r1, r2, rn;
  FILE *fp1, *fp3, *fp;
  char str[132], str1[512];
  unsigned int seed = 0;
  struct tms timesThen;
  int i7, i8, nGE = 0, nFP = 0, nDSSD = 0, nCHICO2 = 0, nBGO = 0, reportinterval, nGS = 0;
  char *p, *pc;
  int echo = 0, nni, nret, ncib = 0;
  double d1;
  char *tmp_ev;
  int size;
  int truesize;
  int wosize;
  int nused;
  int nprint = 100;

  /* prototypes */

  int wr_spe (char *, int *, float *);
  int printDgsHeader (DGSHEADER);
  int GTPrintHeader (FILE *, GTEVENT *);
  int get_a_seed (unsigned int *);
  int time_stamp (void);
  int GTGetDiskEv (int, int);

  /* help */

  if (argc == 1)
    {
      printf ("use: GEBMerge chatfile outfile     file1  file2  file3  file4 .....\n");
      printf ("eg., GEBMerge gtmerge.chat c.gtd   t1.gtd t2.gtd t3.gtd t4.gtd\n");
      exit (0);
    };


  /* initialize random number generator etc */

  get_a_seed (&seed);
  srand (seed);
  nstat = (MSTAT *) calloc (1, sizeof (MSTAT));
  bzero ((char *) &control, sizeof (CONTROL));
  bzero ((char *) nstat, sizeof (MSTAT));

  control.dtsfabort = 5;
  control.dtsbabort = 5;


  for (i = 0; i < NCHANNELS; i++)
    nstat->id_hit[i] = 0;

  for (i = 0; i <= NGE; i++)
    {
      ehigain[i] = 1;
      ehioffset[i] = 0;
    };

  /* open chat file */

  if ((fp = fopen (argv[1], "r")) == NULL)
    {
      printf ("error: could not open chat file: <%s>\n", argv[1]);
      exit (0);
    };
  printf ("chat file: <%s> open\n", argv[1]);
  printf ("\n");
  fflush (stdout);

  /* read chatfile content and act */

  pc = fgets (str, STRLEN, fp);

  nn = 0;
  while (pc != NULL)
    {
      if (echo)
        printf ("chat->%s", str);
      fflush (stdout);

      /* attemp to interpret the line */

      if ((p = strstr (str, "echo")) != NULL)
        {
          echo = 1;
          if (echo)
            printf ("will echo command lines\n");
          fflush (stdout);

        }
      else if (str[0] == 35)
        {
          /* '#' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 59)
        {
          /* ';' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 10)
        {
          /* empty line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if ((p = strstr (str, "maxNoEvents")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &maxNoEvents);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "reportinterval")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &reportinterval);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "chunksiz")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.chunksiz);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "bigbufsize")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &size);
          CheckNoArgs (nret, 2, str);
          assert (size <= MAXBIGBUFSIZ);
          r1 = (size * sizeof (EVENT) + (size + 1) * sizeof (int)) / 1024.0 / 1024.0;
          printf ("sizeof(EVENT)= %i\n", sizeof (EVENT));
          printf ("will use a bigbuffer size of %i, or %7.3f MBytes\n", size, r1);
        }
      else if ((p = strstr (str, "nprint")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &nprint);
          CheckNoArgs (nret, 2, str);
          printf ("will print information for first %i events\n", nprint);
        }
      else if ((p = strstr (str, "wosize")) != NULL)
        {
          nret = sscanf (str, "%s %f", str1, &r1);
          r1 = (r1 / 100.0 * size);
          wosize = (int) r1;
          CheckNoArgs (nret, 2, str);
          printf ("will use a bigbuffer wosize of %i\n", wosize);
          assert (wosize <= size);
        }
      else if ((p = strstr (str, "startTS")) != NULL)
        {
          nret = sscanf (str, "%s %lli %lli ", str1, &control.startTS_lo, &control.startTS_hi);
          CheckNoArgs (nret, 3, str);
          printf ("startTS from %lli to %lli\n", control.startTS_lo, control.startTS_hi);
          control.startTS = 1;
        }
      else if ((p = strstr (str, "zzipout")) != NULL)
        {
          nret = sscanf (str, "%s", str1);
          CheckNoArgs (nret, 1, str);
          control.zzipout = 1;
          printf ("will zzip output, %i\n", control.zzipout);
          fflush (stdout);

        }
      else if ((p = strstr (str, "TSlistelen")) != NULL)
        {
          nret = sscanf (str, "%s %i %i %i", str1, &control.TSlistelen, &control.TSlist_lo, &control.TSlist_hi);
          CheckNoArgs (nret, 4, str);
        }
      else if ((p = strstr (str, "dts_min")) != NULL)
        {
          nret = sscanf (str, "%s %lli", str1, &control.dts_min);
          CheckNoArgs (nret, 2, str);
          printf ("control.dts_min=%lli\n", control.dts_min);
        }
      else if ((p = strstr (str, "dts_max")) != NULL)
        {
          nret = sscanf (str, "%s %lli", str1, &control.dts_max);
          CheckNoArgs (nret, 2, str);
          printf ("control.dts_max=%lli\n", control.dts_max);
        }
      else if ((p = strstr (str, "dtsfabort")) != NULL)
        {
          nret = sscanf (str, "%s %lli", str1, &control.dtsfabort);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "dtsbabort")) != NULL)
        {
          nret = sscanf (str, "%s %lli", str1, &control.dtsbabort);
          CheckNoArgs (nret, 2, str);
        }
      else
        {

          /* --------------------------- */
          /* chatscript read error point */
          /* --------------------------- */

          printf ("line %2.2i in chat script, option :%s \n__not understood\n", nn, str);
          printf ("%i\n", str[0]);
          printf ("aborting\n");
          fflush (stdout);
          exit (0);
        };

      /* read next line in chat script */

      nn++;                     /* line counter */
      nni++;                    /* instruction counter */
      pc = fgets (str, STRLEN, fp);

    };
  fclose (fp);

  /* extract and repeat parameters */

  printf ("%s: will produce a max of %i events\n", argv[0], maxNoEvents);

  printf ("%s: will write combined data to file \"%s\"\n", argv[0], argv[2]);

  /* offset for data file name reads, find  nfiles etc */

  argcoffset = 3;
  nfiles = (argc - argcoffset);
  printf ("%s: we have %i datafiles to combine\n", argv[0], nfiles);
  fflush (stdout);

  nPoolEvents = nfiles;
  printf ("%s: will keep a pool of %i events to combine from \n", argv[0], nPoolEvents);
  assert (nPoolEvents < MAXCOINEV);

  /* just to be sure */

  assert (nfiles == nPoolEvents);

  /************************/
  /* open all input files */
  /************************/

#if(USEZLIB==0)
  for (i = 0; i < nfiles; i++)
    {
      nn = i + argcoffset;
      inData[i] = open (argv[nn], O_RDONLY, 0);
      if (inData[i] < 0)
        {
          printf ("could not open input data file [%i] %s, quit!\n", nn, argv[nn]);
          exit (1);
        }
      else
        {
          printf ("%s: input data file \"%s\", number %i, is open\n", argv[0], argv[nn], i);
          fflush (stdout);
          control.nOpenFiles++;
          control.fileActive[i] = 1;
#if(USEZLIB==1)
          zFile[i] = gzdopen (inData[i], "r");
#endif
        }
    };
#endif

#if(USEZLIB==1)
  for (i = 0; i < nfiles; i++)
    {
      nn = i + argcoffset;
      zFile[i] = gzopen (argv[nn], "r");
      if (zFile[i] == NULL)
        {
          printf ("could not open input data file [%i] %s, quit!\n", nn, argv[nn]);
          exit (1);
        }
      else
        {
          printf ("%s: gzipped input data file \"%s\", number %i, is open\n", argv[0], argv[nn], i);
          fflush (stdout);
          control.nOpenFiles++;
          control.fileActive[i] = 1;
        }
    };
#endif

#if (USEBREAD)

  for (i = 0; i < nfiles; i++)
    {
      bread_buf[i] = (int *) calloc (BREAD_BUFSIZE, sizeof (int));
      bread_pos[i] = 0;
      bread_bufsiz[i] = BREAD_BUFSIZE;
    };

#endif


  /* ----------- */
  /* output file */
  /* ----------- */


  outData = 0;
  /*                        + name of data file */
  /*                        |                   */
  sprintf (str, "%s_%3.3i", argv[2], control.chunkno);
  outData = open (str, O_WRONLY | O_CREAT | O_TRUNC, PMODE);

  if (outData == 0)
    {
      printf ("could not open output data file %s, quit!\n", str);
      exit (1);
    }
  else
    {
      printf ("%s: output data file \"%s\" is open\n", argv[0], str);
      fflush (stdout);
    };

  if (control.zzipout == 1)
    zoutData = gzdopen (outData, "w");

#if(0)
  /* write output header file */

  bzero ((char *) &outheader, sizeof (DGSHEADER));
  outheader.id = 1000;
  if (control.zzipout == 0)
    siz = write (outData, (char *) &outheader, sizeof (DGSHEADER));
  else
    siz = gzwrite (zoutData, (char *) &outheader, sizeof (DGSHEADER));

  printf ("header written to output file\n");
#endif

  /* -------------------- */
  /* read in the map file */
  /* -------------------- */

  for (i = 0; i < NCHANNELS; i++)
    {
      tlkup[i] = NOTHING;
      tid[i] = NOTHING;
    };

  fp = fopen ("map.dat", "r");
  if (fp == NULL)
    {
      printf ("need a \"map.dat\" file to run\n");
      system("./mkMap > map.dat");
      printf("just made you one...\n");
      fp = fopen ("map.dat", "r");
      assert(fp != NULL);
    };

  printf ("\nmapping\n");

  i2 = fscanf (fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
  printf ("%i %i %i %s\n", i1, i7, i8, str);
  while (i2 == 4)
    {
      tlkup[i1] = i7;
      tid[i1] = i8;
      i2 = fscanf (fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
//      printf ("%i %i %i %s\n", i1, i7, i8, str);
    };
  fclose (fp);


  /* start timer */

  times ((struct tms *) &timesThen);

  /* pool allocations */

  for (i = 0; i < MAXCOINEV; i++)
    {
      Event[i].gd = (GEBDATA *) calloc (sizeof (GEBDATA), 1);
      Event[i].payload = (PAYLOAD *) calloc (MAXPAYLOADSIZE, 1);
    };
  i1 = MAXCOINEV * (sizeof (GEBDATA) + MAXPAYLOADSIZE);
  printf ("allocated %12i bytes for file pools,    or %12i MB\n", i1, i1 / 1024 / 1024);

  /* allocate memory for bigbuf */

  i1 = 0;
  for (i = 0; i <= size; i++)
    {
      bigbuf[i] = (char *) calloc (MAXPAYLOADSIZE + sizeof (GEBDATA), 1);
    };
  i1 = (size + 1) * (MAXPAYLOADSIZE + sizeof (GEBDATA));
  printf ("allocated %12i bytes for bigbuf events, or %12i MB\n", i1, i1 / 1024 / 1024);


  if (control.startTS)
    {
      /* pre-read until we hit the the start interval TS */

      nn = 0;
      for (i = 0; i < nPoolEvents; i++)
        {
          for (j = 0; j < 10000000; j++)
            {
              st = GTGetDiskEv (nn, i);
              if (st == 0)
                {
//      printf("%5i: ", j);
                  dTS = Event[i].gd->timestamp - oldTS;
//      printf("TS=%20lli dTS=%10lli\n",Event[i].gd->timestamp, dTS);
                  if (Event[i].gd->timestamp > control.startTS_lo && Event[i].gd->timestamp < control.startTS_hi)
                    break;
                  oldTS = Event[i].gd->timestamp;
                }
            }
          nn++;
          printf ("found reset at TS=%20lli for PoolEvents=%i after %i reads\n", Event[i].gd->timestamp, i, j);
        };

    };



  /* -------------------------------------------- */
  /* read until we have filled our pool of events */
  /* -------------------------------------------- */

  printf ("filling pool\n");
  nn = 0;
  for (i = 0; i < nPoolEvents; i++)
    {
      printf ("attempting to read event... i=%i ", i);
      st = GTGetDiskEv (nn, i);
      printf ("st=%i\n", st);
      if (st != 0)
        {
          printf ("1: GTGetDiskEv returned %i\n", st);
        }
      if (st == 2 || st == 5)
        {
          printf ("failed to read more data from %i\n", nn);
          exit (0);
        };
      while (st != 0 && st != 2 && st != 5)
        {
          st = GTGetDiskEv (nn, i);
          if (st != 0)
            {
              printf ("2: GTGetDiskEv returned %i\n", st);
            }
        }
      oldFileTS[i] = Event[i].gd->timestamp;
//      printf ("[1]Event[%i].gd->type=%lli\n", i, Event[i].gd->type);
      whatFile[i] = nn;
      nn++;
      if (nn >= nfiles)
        nn = 0;
    };

  /* print first timestamps and check them */

  for (i = 0; i < nPoolEvents; i++)
    {
      printf ("file %3i, first TS %22lli;\n", i, Event[i].gd->timestamp);
    };

  for (i = 0; i < nPoolEvents; i++)
    {

      if (Event[i].gd->timestamp == (long long int) 0)
        printf ("ooops, file %3i has TS = %22lli, will re-read\n", i, Event[i].gd->timestamp);
      {

        while (Event[i].gd->timestamp == (long long int) 0)
          {
            st = GTGetDiskEv (i, i);
            if (st != 0)
              {
                printf ("1: GTGetDiskEv returned %i\n", st);
              }
            if (st == 2 || st == 5)
              {
                printf ("failed to read more data from %i\n", nn);
                exit (0);
              };
            while (st != 0 && st != 2 && st != 5)
              {
                st = GTGetDiskEv (i, i);
                if (st != 0)
                  {
                    printf ("2: GTGetDiskEv returned %i\n", st);
                  }
              }
            printf ("[2]Event[%i].gd->type=%lli\n", i, Event[i].gd->type);
            oldFileTS[i] = Event[i].gd->timestamp;
          };

      };
      d1 = (double) Event[i].gd->timestamp / 100000;
      printf ("file %3i, first TS %22lli or %.3f msec;\n", i, Event[i].gd->timestamp, (float) d1);

    };

  printf ("\n");
  printf ("initial pool of events read (%i events)\n", nPoolEvents);


  /* find the very first time stamp */

  nextTS = ULLONG_MAX;
  nextTSpoolIndex = -1;
  for (i = 0; i < nPoolEvents; i++)
    {
#if(DEBUG1 ||1)
      printf ("Event[%i].gd->timestamp=%lli\n ", i, Event[i].gd->timestamp);
#endif
      if (Event[i].gd->timestamp < nextTS)
        {
          nextTS = Event[i].gd->timestamp;
          nextTSpoolIndex = i;
        };
    };
  firstTS = nextTS;

  printf ("\n");
  printf ("First TS=%lli, from file %i\n", nextTS, nextTSpoolIndex);


  reportStats (&timesThen, nfiles, 0.0);
  fflush (stdout);
//if(1)exit(0);

  /* -------------------------- */
  /* fill bigbuf the first time */
  /* -------------------------- */

  truesize = 0;
  while (truesize < size)
    {

      /* ------------------------------------------------ */
      /* find lowest time stamp of the candicates we have */
      /* ------------------------------------------------ */

      nextTS = ULLONG_MAX;
      nextTSpoolIndex = -1;
      for (i = 0; i < nPoolEvents; i++)
        {
//          printf("Event[%i].gd->timestamp=%lli\n",i,Event[i].gd->timestamp);
          if (Event[i].gd->timestamp < nextTS)
            {
              nextTS = Event[i].gd->timestamp;
              nextTSpoolIndex = i;
            };
        };
//      printf("lowest TS=%lli from nextTSpoolIndex=%i\n",nextTS,nextTSpoolIndex);
      curTS = nextTS;

      /* -------------- */
      /* copy to bigbuf */
      /* -------------- */

      memcpy ((char *) bigbuf[truesize], (char *) Event[nextTSpoolIndex].gd, sizeof (GEBDATA));
      memcpy ((char *) (bigbuf[truesize] + sizeof (GEBDATA)), (char *) Event[nextTSpoolIndex].payload,
              Event[nextTSpoolIndex].gd->length);
      truesize++;

      /* --------------------------- */
      /* read in a replacement event */
      /* --------------------------- */

      if (control.fileActive[whatFile[nextTSpoolIndex]])
        {

#include "GTMerge_readnew.h"
//          st = GTGetDiskEv (whatFile[nextTSpoolIndex], nextTSpoolIndex);
        }

#if(0)
      printf ("after replacement\n");
      for (i = 0; i < nPoolEvents; i++)
        printf ("__Event[%i].gd->timestamp=%lli\n", i, Event[i].gd->timestamp);
#endif

    };


  printf ("GEBMerge: big buf is now completely primed with %i events...\n", truesize);
  nstat->nbigbufreads++;
  ulli1 = *(unsigned long long int *) (bigbuf[0] + 2 * sizeof (int));
  printf ("first fill: bigbuf[%4i]->gd->timestamp-curTS=%lli\n", 0, ulli1);
  ulli2 = *(unsigned long long int *) (bigbuf[truesize - 1] + 2 * sizeof (int));
  printf ("last  fill: bigbuf[%4i]->gd->timestamp-curTS=%lli\n", truesize - 1, ulli2);
  d1 = (double) ulli2 - (double) ulli1;
  d1 /= 1000000000;
  printf ("bigbuf covers %f seconds\n", (float) d1);
  printf ("\n\n");


  for (i = 0; i < LENSP; i++)
    dtbtev[i] = 0;

  /* ---------------------------------------------- */
  /* at this point we have a filled bigbuf. we are  */
  /* now going to process it and refill it until we  */
  /* have no data left to process */
  /* ---------------------------------------------- */

  while (control.CurEvNo < maxNoEvents)
    {
      nused = 0;

#if(0)
      printf ("--------\n");
      printf ("start of loop\n");
      printf ("truesize=%i\n", truesize);
      printf ("nused=%i\n", nused);
      printf ("size=%i\n", size);
      fflush (stdout);
#endif

#if(0)
      printf ("\nbeginning buffer\n");
      printf ("size=%i, nused=%i, (size - nused)=%i\n", size, nused, (size - nused));
      i1 = 10;
      i2 = nused;
#include "pbuf.h"
#endif

#if(0)
      for (i = 0; i < 5; i++)
        printf ("1-> bigbuf[%4i]->gd->timestamp=%lli\n", i, bigbuf[i]->gd->timestamp);
      for (i = (truesize - 1 - 5); i < truesize; i++)
        printf ("1-> bigbuf[%4i]->gd->timestamp=%lli\n", i, bigbuf[i]->gd->timestamp);
#endif

      /* now time order */
      /* bigbuf by swapping pointers.
      else if ((p = strstr (str, "bigbufsize")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &size);
          CheckNoArgs (nret, 2, str);
          assert (size <= MAXBIGBUFSIZ);
          r1 = (size * sizeof (EVENT) + (size + 1) * sizeof (int)) / 1024.0 / 1024.0;
          printf ("sizeof(EVENT)= %i\n", sizeof (EVENT));
          printf ("will use a bigbuffer size of %i, or %7.3f MBytes\n", size, r1);
        } */

#if(0)

      /* Simple 'bubble sort' now, maybe */
      /* use better later */

      /* Note: it might be tempting to say:  */
      /* just sort from nused and up, but  */
      /* we might read in something that  */
      /* happened in the past so we cannot  */
      /* do that! That would be against what  */
      /* bigbuf was installed for in the  */
      /* first place */

      assert (0);

      for (i = 0; i < truesize; i++)
        for (j = i + 1; j < truesize; j++)
          {
            ulli1 = *(unsigned long long int *) (bigbuf[j] + 2 * sizeof (int));
            ulli2 = *(unsigned long long int *) (bigbuf[i] + 2 * sizeof (int));
            if (ulli1 < ulli2)
              {
//                printf ("swap %i and %i\n", i, j);
//                printf ("swap: TS[%i]=%lli < TS[%i]=%lli\n", i,bigbuf[j]->gd->timestamp, j,bigbuf[i]->gd->timestamp);

                /* swap the pointers to the headers and payloads */

                tmp_ev = bigbuf[i];
                bigbuf[i] = bigbuf[j];
                bigbuf[j] = tmp_ev;

                nstat->nswaps++;

              };
          };
//      printf ("nstat->nbigbufreads=%i, nstat->nswaps=%i\n", nstat->nbigbufreads, nstat->nswaps);

#else

      /* use specialized quick sort routine */

      quickSort (bigbuf, 0, truesize -1, 2 * sizeof (int));

#endif

#if(0)
      for (i = 0; i < 5; i++)
        printf ("2-> bigbuf[%4i]->gd->timestamp=%lli\n", i, bigbuf[i]->gd->timestamp);
      for (i = (truesize - 1 - 5); i < truesize; i++)
        printf ("2-> bigbuf[%4i]->gd->timestamp=%lli\n", i, bigbuf[i]->gd->timestamp);
      if (1)
        exit (0);
#endif
#if(1)
      /* check sanity of buffer */

      ulli1=*(unsigned long long int *) (bigbuf[0] + 2 * sizeof (int));
      for (i = 1; i < truesize; i++)
        {
          ulli2=*(unsigned long long int *) (bigbuf[i] + 2 * sizeof (int));
          assert (ulli2>=ulli1);
          dTS = (long long int)ulli2-(long long int)ulli1;
          ulli1=ulli2;
          if (control.CurEvNo < 100 /*nprint*/)
            {
            if (dTS > 300)
              printf ("\n");
	                printf ("_%5i: TS=%20lli, dTS=%10lli\n", i, ulli1, dTS);
            };
        }
//      exit (1);
#endif


      /* update simple TS.list file */
      /*
      if (nstat->nbigbufreads >= control.TSlist_lo && nstat->nbigbufreads <= control.TSlist_hi)
        {
          sprintf (str, "TS.list_%3.3i", nstat->nbigbufreads);
          TSlist = fopen (str, "w");
          nTSlist = 0;
          fprintf (TSlist, "TS dump of buffer # %i\n", nstat->nbigbufreads);

          if (nTSlist < control.TSlistelen)
            {

//        fprintf (TSlist, "read # %5i> ", control.nread);

//          fprintf (TSlist, "(%10lli,", (long long int)bigbuf[i]->.LEDts);
//          fprintf (TSlist, "%10lli)", oldFileTS[nextTSpoolIndex]);

              for (i = 0; (i < truesize) && (nTSlist < control.TSlistelen); i++)
                {

                  dTS = *(unsigned long long int *) (bigbuf[i] + 2 * sizeof (int)) - oldListTS;
                  if (dTS > (long long int) 300)
                    fprintf (TSlist, "\n");

                  fprintf (TSlist, "%i, TS=%10lli;  ", i, *(unsigned long long int *) (bigbuf[i] + 2 * sizeof (int)));

                  if (nTSlist == 0)
                    oldListTS = *(unsigned long long int *) (bigbuf[i] + 2 * sizeof (int));
                  else
                    {
                      fprintf (TSlist, "dTS=%10lli; ", dTS);
                      oldListTS = *(unsigned long long int *) (bigbuf[i] + 2 * sizeof (int));
                    }
                  fprintf (TSlist, "\n");
                  fflush (TSlist);
                  nTSlist++;

                };

            };
          fclose (TSlist);
        };
      */

      /* write statistics in the beginning */

      if (nstat->nbigbufreads < 50)
        {
          d1 = *(unsigned long long int *) (bigbuf[truesize - 1] + 2 * sizeof (int));
          d1 -= *(unsigned long long int *) (bigbuf[0] + 2 * sizeof (int));
          d1 /= 100000;
          printf ("buf# %3i, bigbuf covers %9.3f ms;\n ", nstat->nbigbufreads, (float) d1);
//          printf ("bigbuf[%4i]->gd->timestamp=%lli\n", 0, bigbuf[0]->gd->timestamp);
//          printf ("bigbuf[%4i]->gd->timestamp=%lli\n", truesize - 1,
//              bigbuf[truesize - 1]->gd->timestamp);
          fflush (stdout);
        };


      /* write out the first part of the buffer */

      assert (outData > 0);

      i1 = 0;
      ii = 0;
//      nused = 0;
      while (ii < wosize)
        {
          i2 = *(unsigned long long int *) (bigbuf[ii] + sizeof (int));
          i2 += sizeof (GEBDATA);
//          printf("write %i bytes out, payload size: %i\n", i2,*(unsigned long long int *)(bigbuf[ii]+sizeof(int)));
          siz = write (outData, (char *) bigbuf[ii], i2);
          i1 += siz;
          nstat->outbytes += siz;
          ii++;
//        printf("%i/%i\n", ii, wosize);
          nused++;
        };
      control.CurEvNo += wosize;
      control.nwritten += wosize;

//      printf ("wrote %i bytes to outData\n", i1);


      /* update dtbtev for the event we used */

      for (i = 1; i < nused; i++)
        {
          lli1 = *(long long int *) (bigbuf[i] + 2 * sizeof (int));
          lli1 -= *(long long int *) (bigbuf[i - 1] + 2 * sizeof (int));
          i1 = (int) lli1;
          if (i1 < LENSP && i1 > 0)
            dtbtev[i1]++;
        }

      /* --------------------------  */
      /* we have processed bigbuf    */
      /* used first 'nused' elements */
      /* now we will replenish it    */
      /* --------------------------  */


#if(0)
      printf ("processed bigbuf\n");
      printf ("truesize=%i\n", truesize);
      printf ("nused=%i\n", nused);
      printf ("size=%i\n", size);
      fflush (stdout);
#endif

#if(0)
      printf ("\nbefore refill\n");
      printf ("size=%i, nused=%i, (size - nused)=%i\n", size, nused, (size - nused));
//#include "pbuf.h"
#endif


      /* ... but first we move the top of */
      /* the remaining buffer down, so we have */
      /* to sort less; it should be faster than having */
      /* the sort do the same job in a double loop */

      i = 0;
      for (j = nused; j < size; j++)
        {

          tmp_ev = bigbuf[j];
          bigbuf[j] = bigbuf[i];
          bigbuf[i] = tmp_ev;

          i++;
        }
#if(0)
      printf ("copied end of buffer to start of buffer %i..%i to %i...%i\n", nused, size, 0, i);
      printf ("truesize=%i\n", truesize);
      printf ("nused=%i\n", nused);
      printf ("size=%i\n", size);
      fflush (stdout);
#endif

#if(0)
      /* debug print the buffer */

      printf ("\nafter moving top to bottom \n");
      printf ("size=%i, nused=%i, (size - nused)=%i\n", size, nused, (size - nused));
#endif

      /* TBD */

      truesize = size - nused;

#if(0)
      printf ("ready to refil buffer\n");
      printf ("truesize=%i\n", truesize);
      printf ("nused=%i\n", nused);
      printf ("size=%i\n", size);
      fflush (stdout);
#endif


      i1 = size - nused;
      i2 = size;
      truesize == i1;
//      printf ("refill into %i...%i\n", i1, i2 - 1);

      for (nfe = i1; nfe < i2; nfe++)
        {

          /* ------------------------------------------------ */
          /* find lowest time stamp of the candidates we have */
          /* ------------------------------------------------ */

          nextTS = ULLONG_MAX;
          nextTSpoolIndex = -1;
          for (i = 0; i < nPoolEvents; i++)
            {
              if (Event[i].gd->timestamp < nextTS)
                {
                  nextTS = Event[i].gd->timestamp;
                  nextTSpoolIndex = i;
                };
            };

          /* -------------- */
          /* copy to bigbuf */
          /* -------------- */

          memcpy ((char *) bigbuf[nfe], (char *) Event[nextTSpoolIndex].gd, sizeof (GEBDATA));
          memcpy ((char *) (bigbuf[nfe] + sizeof (GEBDATA)), (char *) Event[nextTSpoolIndex].payload,
                  Event[nextTSpoolIndex].gd->length);
          truesize++;

          /* --------------------------- */
          /* read in a replacement event */
          /* --------------------------- */

          if (control.fileActive[whatFile[nextTSpoolIndex]])
            {
#include "GTMerge_readnew.h"
            }

        };
      nused = 0;
#if(0)
      /* debug print the buffer */

      printf ("\nafter refill \n");
      printf ("size=%i, nused=%i, (size - nused)=%i\n", size, nused, (size - nused));
      i1 = 10;
      i2 = size - nused;
//#include "pbuf.h"
#endif

      nstat->nbigbufreads++;
//    printf("%i: truesize=%i,size=%i \n",nstat->nbigbufreads,truesize, size);
//   reportStats (&timesThen,nfiles);

#if(0)
      printf ("bottom of loop\n");
      printf ("truesize=%i\n", truesize);
      printf ("nused=%i\n", nused);
      printf ("size=%i\n", size);
      fflush (stdout);
//      exit (0);
#endif

      if ((nstat->nbigbufreads % reportinterval) == 0)
        {
          ulli1 = *(unsigned long long int *) (bigbuf[0] + 2 * sizeof (int));
          ulli2 = *(unsigned long long int *) (bigbuf[truesize - 1] + 2 * sizeof (int));
          d1 = (double) ulli2 - (double) ulli1;
          d1 /= 100;            /* us */
          d1 /= 1000;           /* ms */
          reportStats (&timesThen, nfiles, (float) d1);
        };
    };



done:

  /* close all files */

  close (outData);
  for (i = 0; i < nfiles; i++)
#if(USEZLIB==0)
    close (inData[i]);
#endif
#if(USEZLIB==1)
  gzclose (zFile[i]);
#endif

  printf ("closed all files\n");
  printf ("\n");

  /* done */

  printf ("\n");
  printf ("hit statistics hit/Compton-suppress fraction\n");
  printf ("\n");
  for (i = 0; i <= NGE; i++)
    if (nstat->ge_hit[i] > 0)
      {
        r1 = 100.0 * (float) nstat->ge_cs[i] / (float) nstat->ge_hit[i];
        printf ("%3i: %10i %10i %5.1f%%\n", i, nstat->ge_hit[i], nstat->ge_cs[i], r1);
      };


  i = LENSP;
  //  wr_spe ("dtbtev.spe", &i, dtbtev);
  //  printf ("wrote \"dtbtev.spe\"\n");
  //  printf ("\n");
  reportStats (&timesThen, nfiles, 0.0);

  printf ("\n");
  r1 = (size + 1) * sizeof (EVENT) / 1024 / 1024.0 / 1024.0;
  printf ("used big buf of size %9.3f GBytes\n", r1);
  printf ("\n");
  printf ("\n");
  printf ("wrote %i events to::\n", control.nwritten);
  fflush (stdout);
//sprintf (str, "%s_%3.3i", argv[2], control.chunkno);
  sprintf (str, "file %s_*; ls -l %s_*", argv[2], argv[2]);
//  printf("executing \"%s\"", str);
  system (str);
  fflush (stdout);
  printf ("\n");
  fflush (stdout);

  printf ("\n");
  printf ("Boniva Sancta ;-) >> GEBMerge3 did not crash << !!\n");
  printf ("\n");
  printf ("\n");

#if(0)
  printf ("sizeof(GTEVENT)=%i\n", sizeof (GTEVENT));
  printf ("sizeof(EVENT)=%i\n", sizeof (EVENT));
  printf ("DGSEVENT_BASELEN=%i\n", DGSEVENT_BASELEN);
#endif

  printf ("\n");
  fflush (stdout);
  exit (0);

}
