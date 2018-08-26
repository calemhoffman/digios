
/* write a gf2 spe file from a c program        */
/* note wee need to play with the front and end */
/* record numbers that fortran is using         */
/* see fortran manual page 138 (reference man)  */
/* we write in this format so fortran fg2 pro-  */
/* gam by radford can read the spectra!!!       */

#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef LINUX
#include <string.h>
#else
#include <strings.h>
#endif

#define         PMODE 0644

/*-------------------------------------------------------------*/

int
wr_spe (char *fn, int *dim, float *sp)
{

  /* declarations */

  int st, i1;
  int rec1, rec2;
  int exa;
  int siz = 0;
  int idim1, idim2, ired1, ired2;

  /* open file */

  exa = open (fn, O_WRONLY | O_CREAT | O_TRUNC, PMODE);
  if (exa <= 0)
    {
      printf ("wr_spe: could not open spectrum file %s\n", fn);
      return (0);
    };

  /* write fortran rec1 */

  rec1 = 24;
  i1 = 4;
  siz = write (exa, &rec1, i1);
  if (i1 != siz)
    {
      printf ("tried to write %i\n", i1);
      printf ("actually wrote %i\n", siz);
      close (exa);
      return (-1);
    };

  /* write spectrum name */

  siz = write (exa, fn, 8);

  /* write dim variable */

  idim1 = *dim;
  siz = write (exa, &idim1, 4);
  idim2 = 1;
  siz = write (exa, &idim2, 4);
  ired1 = 1;
  siz = write (exa, &ired1, 4);
  ired2 = 1;
  siz = write (exa, &ired2, 4);

  /* write fortran rec2 */

  rec2 = 24;
  siz = write (exa, &rec2, 4);

  /* write fortran rec1 */

  rec1 = *dim * 4;
  siz = write (exa, &rec1, 4);

  /* write spectrum */

  i1 = *dim * 4;
  siz = write (exa, (char *) sp, i1);
  if (i1 != siz)
    {
      printf ("tried to write %i\n", i1);
      printf ("actually wrote %i\n", siz);
      close (exa);
      return (-1);
    };

  /* write fortran rec2 */

  rec2 = *dim * 4;
  siz = write (exa, &rec2, 4);

  /* done */

  st = close (exa);
  if (st != 0)
    printf ("could not close [%s]\n", fn);
  return (0);

}

/* ========================================================= */

/* rd_spe.c  tl/93/5/18 */
/* read a gf2 spe file from a c program         */
/* note we need to play with the front and end  */
/* record numbers that fortran is using         */
/* see fortran manual page 138 (reference man)  */

int
rd_spe (char *fn, int *dim, float *sp)
/* specify the file name of the spectrum in fn (input)  */
/* as much as dim channels are read. The routine will   */
/* return the actual number of channels read. The spec- */
/* trum is returned in sp                               */
{

  /* declarations */

  int i1;
  int rec1, rec2;
  int exa;
  int siz;
  char namesp[8];
  int idim1, idim2, ired1, ired2;
  int numch;

  /* trim filename */

#if(0)

  i1 = strlen (fn);
  i1--;
  if (i1 < 0)
    i1 = 0;
  while (fn[i1] == 32 && i1 > 0)
    i1--;
  i1++;
  fn[i1] = 0;
#endif

#if(0)

  for (i = 0; i < i1; i++)
    printf ("- %3i,%c,%i\n", i, fn[i], fn[i]);
  i = 0;
#endif

  /* open file */

  exa = open (fn, O_RDONLY, 0);
  if (exa <= 0)
    {
      printf ("could not open spe file <%s>\n", fn);
      return (-1);
    };

  /* read fortran rec1 */

  siz = read (exa, &rec1, 4);
  if (siz != 4)
    {
      close (exa);
      return (-1);
    };

  /* read spectrum name */

  siz = read (exa, namesp, 8);

  /* read dim variable */

  siz = read (exa, &idim1, 4);
  siz = read (exa, &idim2, 4);
  siz = read (exa, &ired1, 4);
  siz = read (exa, &ired2, 4);

  /* read fortran rec2 */

  siz = read (exa, &rec2, 4);

  /* find size of spectrum */

  numch = idim1 * idim2;
  if (numch > *dim)
    numch = *dim;

  /* read fortran rec1 */

  siz = read (exa, &rec1, 4);

  /* read spectrum */

  i1 = numch * 4;
  siz = read (exa, sp, i1);
  if (i1 != siz)
    {
      close (exa);
      return (-1);
    };
  *dim = siz / 4;

  /* read fortran rec2 */

  siz = read (exa, &rec2, 4);

  /* done */

  close (exa);
  return (0);

}
