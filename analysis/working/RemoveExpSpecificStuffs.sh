#!/bin/sh

echo "==================================================="
echo "  Clean up Experimental data in /working"
echo "---------------------------------------------------"
echo "REMOVE these files: "
echo "   correction_*.dat  "
echo "   reaction.dat  "
echo "   run_Summary.dat"
echo "   example.*"
echo "   RunTimeStamp.dat"
echo "   *.root  "
echo "   *.d  "
echo "   *.so  "
echo "   *.pcm  "
echo "==================================================="
echo " Remove above files? (Yes/No)\c"
read conti

if [ ${conti} == "Yes" ] ; then
  rm -fv correction_*.dat
  rm -fv reaction.dat
  rm -fv run_Summary.dat
  rm -fv example.*
  rm -fv RunTimeStamp.dat
  rm -fv *.root
  rm -fv *.d
  rm -fv *.so
  rm -fv *.pcm
fi
