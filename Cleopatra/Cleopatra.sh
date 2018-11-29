#!/bin/bash


########################################################################
# 
#  This is Cleopatra.sh, a scripted version for Cleopatra
#
#  Using bash script provide flexibility that user can add difference
#      compoenents during the calculation
#
#  A full package includes fellowing:
#    1) create a in-file for ptolemy
#    2) run ptolemy from that in-file and output an out-file
#    3) extract cross-section distribution from the out-file
#                 save as txt or root TGraph format
#    4) call ROOT to draw the TGraph
#    5) load possible experimental Xsec and fit with Ptolemy calulation
#
#  User can easily select/comment-out different component 
#      to suit their needs
#-------------------------------------------------------
#  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
#  email: goluckyryan@gmail.com
########################################################################

#for User, please Modify the path for thisroot.h
source /Applications/root/bin/thisroot.sh

#================================ User Defualt Control
CreateInFile=0   # 0 = false, 1 = true
RunPtolemy=0
IsExtractXSec=0
PlotResult=0
#============================================ USER don't need to change thing below

if [ $# -eq 0 ] ; then
  echo "$./Cleopatra in-file X  X  X  X"
  echo "                     |  |  |  |"
  echo "                     |  |  |  PlotResult? (1/0)"
  echo "                     |  |  Extract cross-section? (1/0)"
  echo "                     |  Run Ptolemy? (1/0)"
  echo "                     Create infile? (1/0)"
  exit 1
fi;

loadfile=$1
infile=$1".in"
outfile=$1".out"
rootfile=$1".root"

if [ $# -eq 2 ]; then
  CreateInFile=$2
fi;
if [ $# -eq 3 ]; then
  CreateInFile=$2
  RunPtolemy=$3
fi;
if [ $# -eq 4 ]; then
  CreateInFile=$2
  RunPtolemy=$3
  IsExtractXSec=$4
fi;
if [ $# -eq 5 ]; then
  CreateInFile=$2
  RunPtolemy=$3
  IsExtractXSec=$4
  PlotResult=$5
fi;

echo "#################################################################"
echo "##   @@@@ @@    @@@@  @@@@  @@@@@  @@@@  @@@@@@ @@@@@   @@@@   ##"
echo "##  @@    @@    @@   @@  @@ @@ @@ @@  @@   @@   @@ @@  @@  @@  ##"
echo "##  @@    @@    @@@@ @@  @@ @@@@@ @@@@@@   @@   @@@@@  @@@@@@  ##"
echo "##  @@    @@    @@   @@  @@ @@    @@  @@   @@   @@ @   @@  @@  ##"
echo "##   @@@@ @@@@@ @@@@  @@@@  @@    @@  @@   @@   @@  @  @@  @@  ##"
echo "#################################################################"
echo "#####        Cleopatra, Ptolemy for (d,p),(p,d)             #####"
echo "#################################################################"
echo ""
echo "USER OPTION:"
echo " --- Is Create Ptolemy infile ? " ${CreateInFile}
echo " --- Is Run Ptolemy           ? " ${RunPtolemy}
echo " --- Is Extract Cross-Section ? " ${IsExtractXSec}
echo " --- Is Plot Results          ? " ${PlotResult}
echo "================================================================="

#if [ ${CreateInFile} -eq 1 ] ; then 
#  echo "infile ----> "${loadfile}
#fi;
#
#if [ ${RunPtolemy} -eq 1 ] ; then 
#  echo "Ptolemy  infile ----> "${infile}
#  echo "Ptolemy outfile ----> "${outfile}
#fi;

if [ ${CreateInFile} -eq 1 ] ; then 
  ./InFileCreator ${loadfile} 0.0 50.0 0.5
fi;

if [ ${RunPtolemy} -eq 1 ] ; then 
  echo "================================================================="
  echo "=====   Ptolemy Calcualtion   ==================================="
  echo "================================================================="
  ./ptolemy <${infile}> ${outfile}
fi;

#===== Extracting XSec and save into *txt and *root
if [ ${IsExtractXSec} -eq 1 ] ; then 
  ./ExtractXSec ${outfile}
fi;

if [ ${PlotResult} -eq 1 ] ; then 
  #===== Plot the result from the *.root
  #./PlotTGraphTObjArray ${rootfile}
  #--- other way within ROOT
  echo "================================================================="
  echo "=====   Plot Result from ${rootfile}"
  echo "================================================================="
  com='PlotTGraphTObjArray.h("'${rootfile}'")'
  echo ${com}
  root -l ${com}
fi;
