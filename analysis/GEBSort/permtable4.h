/* $Id: permtable4.h,v 1.1 2013/08/29 15:20:31 tl Exp $ */

  ndet = 4;
  nn = 0;
  printf("--> order= %i <------------------\n", ndet);
  for (i1 = 0; i1 < ndet; i1++)
    for (i2 = 0; i2 < ndet; i2++)
      if (i2 != i1)
	for (i3 = 0; i3 < ndet; i3++)
	  if (i3 != i1)
	    if (i3 != i2)
	      for (i4 = 0; i4 < ndet; i4++)
		if (i4 != i1)
		  if (i4 != i2)
		    if (i4 != i3)
		    {
		      assert(nn < MAXPERM);
		      assert(ndet < MAXNOSEG);
		      Pars.permlkup[ndet][nn][0] = i1;
		      Pars.permlkup[ndet][nn][1] = i2;
		      Pars.permlkup[ndet][nn][2] = i3;
		      Pars.permlkup[ndet][nn][3] = i4;
		      nn++;
		    };
