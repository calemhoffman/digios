#!/bin/sh

echo "==================================================="
echo "  Clean up Experimental data in /working"
echo "---------------------------------------------------"
echo "REMOVE these files: "
#echo "   correction_*.dat  "
echo "   reaction.dat  "
echo "   run_Summary.dat"
echo "   DWBA.*"
ls -l DWBA.*
echo "   RunTimeStamp.dat"
#echo "   *.root  "
echo "   *.pdf  "
ls -l *.pdf
echo "   *.png  "
ls -l *.png
echo "   *.d  "
ls -l *.d 
echo "   *.so  "
ls -l *.so
echo "   *.pcm  "
ls -l *.pcm
echo "==================================================="
echo " Remove above files? (Yes/No)"
read conti

if [ ${conti} == "Yes" ] ; then
#  rm -fv correction_*.dat
  rm -fv reaction.dat
  rm -fv run_Summary.dat
  rm -fv DWBA.*
  rm -fv RunTimeStamp.dat
#  rm -fv *.root
  rm -fv *.pdf
  rm -fv *.png
  rm -fv *.d
  rm -fv *.so
  rm -fv *.pcm
fi
