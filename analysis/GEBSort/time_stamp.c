/* $Id: time_stamp.c,v 1.1 2013/08/29 15:20:31 tl Exp $ */

#include <time.h>
#include <stddef.h>
#include <stdio.h>

int
#ifdef OLDSUNOS
time_stamp ()
#else
time_stamp (void)
#endif
{
  /* declarations */

  struct tm *local;
  time_t t;

  /* get current time and print */

  t = time (NULL);
  local = localtime (&t);
  printf ("%s", asctime (local));

  /* done */

  return (0);

}
