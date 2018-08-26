/* $Id: GEBHeader.c,v 1.1 2013/08/29 15:20:31 tl Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "GTMerge.h"

/*----------------------------------------------------------------------------*/

int
printDgsHeader (DGSHEADER dgsHeader)
{

  if (dgsHeader.id == 0xaaaaaaaa)
    {
      printf ("oops: file has no valid header (old data?)\n");
      return (-1);
    }
  else
    {
      printf ("header ID of data is %i(0x%x)\n", dgsHeader.id, dgsHeader.id);
    };


  /* done */

  fflush (stdout);
  return (0);

}
