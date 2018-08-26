/* $Id: permtable3.h,v 1.1 2013/08/29 15:20:31 tl Exp $ */

  ndet = 3;
  nn = 0;
  printf("--> order= %i <------------------\n", ndet);
  for (i1 = 0; i1 < ndet; i1++)
    for (i2 = 0; i2 < ndet; i2++)
      if (i2 != i1)
	for (i3 = 0; i3 < ndet; i3++)
	  if (i3 != i1)
	    if (i3 != i2)
	    {
	      printf("%4i: %i %i %i\n", nn, i1, i2, i3);
	      assert(nn < MAXPERM);
	      assert(ndet < MAXNOSEG);
	      Pars.permlkup[ndet][nn][0] = i1;
	      Pars.permlkup[ndet][nn][1] = i2;
	      Pars.permlkup[ndet][nn][2] = i3;
	      nn++;
	    };

