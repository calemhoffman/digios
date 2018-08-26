/* $Id: get_a_seed.c,v 1.1 2013/08/29 15:20:31 tl Exp $ */

/* generate a randowm seed from the clock */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

int
get_a_seed (unsigned int *seed)
{
  /* delarations */

  struct timeval tp;
  int i;
  unsigned short int i1;

  /* make the random seed */

  for (i = 0; i < 31; i++)
    {
      gettimeofday (&tp, NULL);
      i1 = (unsigned int) tp.tv_usec;
      i1 &= 0x0001;
      *seed += i1 << i;
    };
  /* printf("get_a_seed:: %i\n", *seed); */

  return (0);

}
