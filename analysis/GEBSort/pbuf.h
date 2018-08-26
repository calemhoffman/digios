      for (i = 0; i < i1; i++)
        {
          printf ("i=%4i, TS=%lli; ", i, bigbuf.ev[i]->TSTYPE);
          if (i > 0)
            printf (" DT= %lli\n", bigbuf.ev[i]->TSTYPE - bigbuf.ev[i - 1]->TSTYPE);
          else
            printf ("\n");
        };
      printf ("..........\n");

      if (i2>i1)
      {
      for (i = (i2 - i1); i < i2 ; i++)
        {
          printf ("i=%4i, TS=%lli; ", i, bigbuf.ev[i]->TSTYPE);
          if (i > 0)
            printf (" DT= %lli\n", bigbuf.ev[i]->TSTYPE - bigbuf.ev[i - 1]->TSTYPE);
          else
            printf ("\n");
        };
      printf ("-----watermark %i\n", i2);
      for (i = i2; i < (i2+i1) ; i++)
        {
          printf ("i=%4i, TS=%lli; ", i, bigbuf.ev[i]->TSTYPE);
          if (i > 0)
            printf (" DT= %lli\n", bigbuf.ev[i]->TSTYPE - bigbuf.ev[i - 1]->TSTYPE);
          else
            printf ("\n");
        };
     };

      printf ("..........\n");
      for (i = (bigbuf.size - i1); i < bigbuf.size; i++)
        {
          printf ("i=%4i, TS=%lli; ", i, bigbuf.ev[i]->TSTYPE);
          if (i > 0)
            printf (" DT= %lli\n", bigbuf.ev[i]->TSTYPE - bigbuf.ev[i - 1]->TSTYPE);
          else
            printf ("\n");
        };

