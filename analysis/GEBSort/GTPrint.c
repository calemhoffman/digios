/* $Id: GTPrint.c,v 1.1 2013/08/29 15:20:31 tl Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include "GTMerge.h"
#include "GEBSort.h"
#include "gdecomp.h"

//extern CONTROL control;
extern PARS Pars;
/*-----------------------------------------------------------------------------*/

int
GTPrintEvent2 (FILE * fp, int ii, DGSEVENT * DGSEvent)
{

  /* declarations */

  int i;

  /* print event */

  fprintf (fp, "-----------\n");
  fprintf (fp, "bigbuf pos=%i\n", ii);
  fprintf (fp, "board= %8i; ", DGSEvent->board_id);
  fprintf (fp, "chan= %8i; ", DGSEvent->chan_id);
  fprintf (fp, "id= %8i; ", DGSEvent->id);
  fprintf (fp, "tpe= %8i; ", DGSEvent->tpe);
  fprintf (fp, "tid= %8i\n", DGSEvent->tid);

  if (DGSEvent->tpe == GE)
    fprintf (fp, "is germanium (%x)", DGSEvent->flag);
  else if (DGSEvent->tpe == BGO)
    fprintf (fp, "is bgo");
  else if (DGSEvent->tpe == SIDE)
    fprintf (fp, "is side");
  else if (DGSEvent->tpe == AUX)
    fprintf (fp, "is aux");
  else if (DGSEvent->tpe == DSSD)
    fprintf (fp, "is DSSD");
  else if (DGSEvent->tpe == FP)
    fprintf (fp, "is FP");
  else if (DGSEvent->tpe == XARRAY)
    fprintf (fp, "is XARRAY");
  else
    printf ("is not assigned");
  fprintf (fp, "# %3i, ", DGSEvent->tid);

  //fprintf (fp, "e= %6i; ", DGSEvent->ehi);

  fprintf (fp, " baseline %i ", DGSEvent->base_sample);
  fprintf (fp, "\n");


  /* time stamps */

  fprintf (fp, "__TS=%lli\n", DGSEvent->event_timestamp);
  fprintf (fp, "__LEDts=0x%8.8x|0x%8.8x; ", (unsigned int) (DGSEvent->event_timestamp >> 32),
           (unsigned int) (DGSEvent->event_timestamp & 0xffffffff));
 // fprintf (fp, "CFDts=0x%8.8x|%8.8x\n", (unsigned int) (DGSEvent->CFDts >> 32),
 //          (unsigned int) (DGSEvent->CFDts & 0xffffffff));


  /* done */

  fflush (fp);
  return (0);

}

/*-----------------------------------------------------------------------------*/

int
GTPrintEvent (FILE * fp, GTEVENT * Event, DGSEVENT * DGSEvent)
{

  /* declarations */

  int i;

  /* print event */

  fprintf (fp, "-----------\n");
  fprintf (fp, "ev %8i> ", Pars.CurEvNo);
  fprintf (fp, "board= %8i; ", DGSEvent->board_id);
  fprintf (fp, "chan= %8i; ", DGSEvent->chan_id);
  fprintf (fp, "id= %8i; ", DGSEvent->id);
  //fprintf (fp, "e= %6i; ", DGSEvent->ehi);

  if (DGSEvent->tpe == GE)
    fprintf (fp, "is germanium");
  else if (DGSEvent->tpe == BGO)
    fprintf (fp, "is bgo");
  else if (DGSEvent->tpe == SIDE)
    fprintf (fp, "is side");
  else if (DGSEvent->tpe == AUX)
    fprintf (fp, "is aux");
  else if (DGSEvent->tpe == DSSD)
    fprintf (fp, "is DSSD");
  else if (DGSEvent->tpe == FP)
    fprintf (fp, "is FP");
  else if (DGSEvent->tpe == XARRAY)
    fprintf (fp, "is XARRAY");
  else
    printf ("is not assigned");
  fprintf (fp, "# %3i", DGSEvent->tid);
  fprintf (fp, " baseline %i ", DGSEvent->base_sample);
  fprintf (fp, "\n");
  /* print a little of the trace */

  fprintf (fp, "tr: ");
  for (i = 0; i < 5; i++)
    fprintf (fp, "%6i ", Event->trace[i]);
  fprintf (fp, "...");
  for (i = Event->traceLen / 2 - 5; i < Event->traceLen / 2; i++)
    fprintf (fp, "%6i ", Event->trace[i]);
  fprintf (fp, ";\n");

  /* time stamps */

  fprintf (fp, "__TS=%lli\n", DGSEvent->event_timestamp);
  fprintf (fp, "__LEDts=0x%8.8x|0x%8.8x; ", (unsigned int) (DGSEvent->event_timestamp >> 32),
           (unsigned int) (DGSEvent->event_timestamp & 0xffffffff));
//  fprintf (fp, "CFDts=0x%8.8x|%8.8x\n", (unsigned int) (DGSEvent->CFDts >> 32),
//           (unsigned int) (DGSEvent->CFDts & 0xffffffff));

  /* bits */

//  fprintf (fp, "__pu=%i, ", Event->pu);
//  fprintf (fp, "CFDvalid=%i, ", Event->CFDvalid);
//  fprintf (fp, "ExtTrig=%i, ", Event->ExtTrig);
//  fprintf (fp, "LEDsign=%i\n", Event->LEDsign);

  /* pos and such */

//  fprintf (fp, "__pos=%12i, len= %5i, tracelen=%6i\n", Event->pos, Event->len, Event->traceLen);

  /* print the event id */

  if (DGSEvent->tpe == GE && !DGSEvent->flag)
    fprintf (fp, "__clean germanium\n");
  if (DGSEvent->tpe == GE && DGSEvent->flag)
    fprintf (fp, "__dirty germanium\n");
  if (DGSEvent->tpe == BGO)
    fprintf (fp, "__bgo\n");

  /* done */

  fflush (fp);
  return (0);

}

/*----------------------------------------------------------------------------*/

int
GTPrintHeader (FILE * fp, GTEVENT * Event)
{

  /* declarations */

  int i, j, k;
  char txt[12][80] = {
    "len",
    "ID",
    "extTS H",
    "extTS L",
    "Ehi L",
    "extTS H",
    "CFD TS L",
    "Ehi H",
    "CFD TS H",
    "CFD TS M",
    "CFD point 1",
    "CFD point 2"
  };

  /* print */

  fprintf (fp, "-------------------------------------------------------------------------\n");
  for (i = 0; i < 12; i++)
    {
      fprintf (fp, "hdr[%2i]=%5i,0x%4.4x ", i, Event->hdr[i], Event->hdr[i]);
      j = 32768;
      fprintf (fp, "|");
      for (k = 0; k < 16; k++)
        {
          if ((Event->hdr[i] & j) == j)
            fprintf (fp, "1");
          else
            fprintf (fp, "0");
          j = j / 2;
          if ((k + 1) % 4 == 0)
            fprintf (fp, "|");
        };
      fprintf (fp, " \"%11s\"", txt[i]);
      if (i % 2)
        fprintf (fp, "\n");
      else
        fprintf (fp, " | ");
    };

  /* done */

  fflush (stdout);
  return (0);

}
