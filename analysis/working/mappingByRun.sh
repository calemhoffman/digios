#!/bin/bash
#######################################################################
#  experiment-specific : which GeneralSortMapping header to compile
#  into EventBuilder for a given run. Sourced by Armory/process_Sort
#  with RUN set (integer). Must set MAPPING (path relative to EventBuilder/).
#
#  if this file does not exist, process_Sort uses GeneralSortMapping.h
#
#  NOTE: all mapping headers must keep the same #define N* and POLARITY_*
#        as GeneralSortMapping.h (used by Monitors / Cali macros).
#######################################################################

alphaRuns="1 2"   # alpha source runs, add more here, e.g. "1 2 57 58"

MAPPING=../working/GeneralSortMapping.h   # beam (default)
for _alphaRun in $alphaRuns; do
  if [ "$((10#$RUN))" -eq "$_alphaRun" ]; then MAPPING=../working/GeneralSortMapping_alpha.h; fi
done
unset _alphaRun
