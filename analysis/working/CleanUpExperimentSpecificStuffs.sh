#!/bin/sh

echo "==================================================="
echo "  Clean up Experimental data in /working"
echo "---------------------------------------------------"
echo "REMOVE these files: "
echo "   correction_*.dat  "
echo "   reaction.dat  "
echo "   run_Summary.dat"
echo "   example.*"
echo "   *.root  "
echo "   *.d  "
echo "   *.so  "
echo "   *.pcm  "
echo "==================================================="
echo " Remove above files? (Yes/No)\c"
read conti

if [ ${conti} == "Yes" ] ; then
  rm -fvi correction_*.dat
  rm -fvi reaction.dat
  rm -fvi run_Summary.dat
  rm -fvi example.*
  rm -fvi *.root
  rm -fvi *.d
  rm -fvi *.so
  rm -fvi *.pcm
fi
