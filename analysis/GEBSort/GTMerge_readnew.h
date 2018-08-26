assert (control.fileActive[whatFile[nextTSpoolIndex]] > 0);

badRead = 1;
while (badRead)
  {
    /* read */

    control.nread++;
//    printf("attemp read # %i..",control.nread);
    st = GTGetDiskEv (whatFile[nextTSpoolIndex], nextTSpoolIndex);
    curTS = Event[nextTSpoolIndex].gd->timestamp;
//  if (control.nread > 2286) exit(0);

#if(0)

    /* debug */

    printf ("TS=%lli from file %i\n", Event[nextTSpoolIndex].gd->timestamp, whatFile[nextTSpoolIndex]);
//    if (Event[nextTSpoolIndex].gd->timestamp > (unsigned long long int) 9129656189)
//      exit (0);
#endif



    /* fail--quit, or decompose */

    if (st != 0)
      {
        printf ("failed to read more data from file %i, nextTSpoolIndex=%i\n", whatFile[nextTSpoolIndex],
                nextTSpoolIndex);
        fflush (stdout);
        if (control.fileActive[whatFile[nextTSpoolIndex]])
          control.nOpenFiles--;
        control.fileActive[whatFile[nextTSpoolIndex]] = 0;
        Event[nextTSpoolIndex].gd->timestamp = ULLONG_MAX;
        if (control.nOpenFiles <= 0)
          {
            printf ("no more data to be read\n");
            goto done;
          };

      };

    /* break out of loop */

    badRead = 0;

    /* zero TS is a scaler, we reassign the last */
    /* know TS to them here and set the tid one higher */

    if (Event[nextTSpoolIndex].gd->timestamp == (long long int) 0)
      {
        Event[nextTSpoolIndex].gd->timestamp = oldFileTS[nextTSpoolIndex];
      };

#if(0)
    if (badRead == 0)
      if (Event[nextTSpoolIndex].tpe >= 0 && Event[nextTSpoolIndex].tpe < MAXTPE)
        if (Event[nextTSpoolIndex].tid >= 0 && Event[nextTSpoolIndex].tid < MAXTID)
          nstat->in_hit[Event[nextTSpoolIndex].tpe][Event[nextTSpoolIndex].tid]++;
#endif


#if(1)

    /* check time stamp did not jump too far forward or backwards */

    dTS = (long long int) Event[nextTSpoolIndex].gd->timestamp - oldFileTS[nextTSpoolIndex];


#if(0)
//if(curTS>=(long long int)4231520787) 
    if (control.nread > 205129)
      {
        printf ("DEBUG STOP!!!!\n");
        exit (0);
      }
#endif


    if (dTS > control.dts_max && st != 2)
      {
        nstat->nTSjumprecover_f[whatFile[nextTSpoolIndex]]++;
        if (nstat->nTSjumprecover_f[whatFile[nextTSpoolIndex]] < 10)
          {
            printf ("timestamp jumped forward %lli for file %i;", dTS, whatFile[nextTSpoolIndex]);
            printf ("now=%lli; ", Event[nextTSpoolIndex].gd->timestamp);
            printf ("old=%lli; ", oldFileTS[nextTSpoolIndex]);
            printf ("nread=%i; ", control.nread);
            printf ("\n");
          };
        printf ("reject TS=%lli; file %i\n", Event[nextTSpoolIndex].gd->timestamp, whatFile[nextTSpoolIndex]);

        /* force rereads */

        badRead = 1;

#if(1)
        /* local count of these events */

        njf++;

        /* breakout */
        /* Idea is that if this happens too many times,  */
        /* it probably was a 'real' jump in time and  */
        /* we should abort our attempts to catch up again.  */
        /* In that case, we reset the counter and set the  */
        /* oldFileTS to the current TS + defeat the  */
        /* badRead we had set above */

        if (njf > control.dtsfabort)
          {
            njf = 0;
            badRead = 0;
            lltmp = oldFileTS[nextTSpoolIndex] - Event[nextTSpoolIndex].gd->timestamp;
            printf ("abort jump forward recovery, oldFileTS %lli --> %lli; ", oldFileTS[nextTSpoolIndex],
                    Event[nextTSpoolIndex].gd->timestamp);
            printf ("DT=%lli\n", -lltmp);
            oldFileTS[nextTSpoolIndex] = Event[nextTSpoolIndex].gd->timestamp;
          };
#endif
      }
    else if (dTS < control.dts_min && st != 2)
      {
        nstat->nTSjumprecover_b[whatFile[nextTSpoolIndex]]++;

        if (nstat->nTSjumprecover_b[whatFile[nextTSpoolIndex]] < 10)
          {
            printf ("TS jumped back %lli; file %i; ", dTS, whatFile[nextTSpoolIndex]);
            printf ("TS=%lli; ", Event[nextTSpoolIndex].gd->timestamp);
            printf ("old=%lli; ", oldFileTS[nextTSpoolIndex]);
            printf ("nread=%i, njb=%i; ", control.nread, njb);
            printf ("\n");
          };
        printf ("reject TS=%lli; file %i\n", Event[nextTSpoolIndex].gd->timestamp, whatFile[nextTSpoolIndex]);


        /* force rereads */

        badRead = 1;

#if(1)
        /* local count of these events */

        njb++;

        /* breakout */
        /* Idea is that if this happens too many times,  */
        /* it probably was a 'real' jump back in time and  */
        /* we should abort our attempts to catch up again.  */
        /* In that case, we reset the counter and set the  */
        /* oldFileTS to the current TS + defeat the  */
        /* badRead we had set above */

        if (njb > control.dtsbabort)
          {
            njb = 0;
            badRead = 0;
            lltmp = oldFileTS[nextTSpoolIndex] - Event[nextTSpoolIndex].gd->timestamp;
            printf ("abort jump back recovery, oldFileTS %lli --> %lli; ", oldFileTS[nextTSpoolIndex],
                    Event[nextTSpoolIndex].gd->timestamp);
            printf ("DT=%lli\n", lltmp);
            oldFileTS[nextTSpoolIndex] = Event[nextTSpoolIndex].gd->timestamp;
          };
#endif
      }
    else
      {
        oldFileTS[nextTSpoolIndex] = Event[nextTSpoolIndex].gd->timestamp;
        njb = 0;
      };

#endif

  }
