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

#===== Call thisroot.h
ROOTPATH=$(which root)
len=${#ROOTPATH}
ROOTSOURCE=${ROOTPATH:0:$len-4}"thisroot.sh"
echo $ROOTSOURCE
source $ROOTSOURCE

#===== go to Cleopatra and make
cd ../Cleopatra
make
cd ../working

#================================ User Defualt Control
CreateInFile=0   # 0 = false, 1 = true
RunPtolemy=0
PlotResult=0
#============================================ USER don't need to change thing below

if [ $# -eq 0 ] ; then
  echo "$./Qwack in-file X  X  X  angMin angMax angStep TNA-file"
  echo "                 |  |  |"
  echo "                 |  |  |"
  echo "                 |  |  |"
  echo "                 |  |  |"
  echo "                 |  |  |"
  echo "                 |  |  PlotResult? (1/0)"
  echo "                 |  |"
  echo "                 |  Run DWUCK? (1/0)"
  echo "                 |  "
  echo "                 Create infile? (1/0)"
  echo "                                                            "
  echo "This can be run only for one reaction at a time!"
  echo "                                                            "
  exit 1
fi;

loadfile=$1
TNAfile=$8
infile=$1".in"
outfile=$1".out"
rootfile=$1".root"
exFile=$1".Ex.txt"

if [ $# -eq 8 ]; then
  CreateInFile=$2
  RunPtolemy=$3
  PlotResult=$4
  angMin=$5
  angMax=$6
  angStep=$7
  else
    echo "The syntax is incorrect"
    exit 1;
fi;


cat << "EOF"
##################################################################
##      _          _          _          _          _           ##
##    >(')____,  >(')____,  >(')____,  >(')____,  >(') ___,     ##
##      (` ~~~/    (` ~~~/    (` ~~~/    (` ~~~/    (` ~~~/     ##
##^~^~^~^`---'~^~^~^`---'~^~^~^`---'~^~^~^`---'~^~^~^`---'~^~^~ ##
##################################################################
#####   Qwack, DWUCK4 for microscopic two nucleon transfer   #####
##################################################################
EOF
echo "Supported reactions:"
echo "2-nucleon transfer"
echo "================================================================="
echo "USER OPTION:"
echo " --- Is Create DWUCK4 infile ? " ${CreateInFile}
echo " --- Is Run DWUCK4           ? " ${RunPtolemy}
echo " --- Is Plot Results         ? " ${PlotResult}
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
  if [ $# -eq 8 ]; then
    ../Cleopatra/DWInFileCreator ${loadfile} $angMin $angMax $angStep ${TNAfile}
    inputCreatorOUTPUT=$?
  else
    echo "You're doing something wrong with the syntax here"
  fi
  
  #echo "File reading exit code : " $inputCreatorOUTPUT
  if [ ${inputCreatorOUTPUT} -eq 1 ] ; then
      echo "Input files read "
  else
      echo "Input files not read, check ${loadfile} or ${TNAfile}"
      exit 1
   fi
   
fi;


if [ ${RunPtolemy} -eq 1 ] ; then 
  echo "================================================================="
  echo "=====   DWUCK4 Calcualtion   ==================================="
  echo "================================================================="
  
  #check is linux or Mac
  
  arch=$(uname)
  
  if [ ${arch} == "Darwin" ] ; then
    ../Cleopatra/dwuck4/DWUCK4 <${infile}> ${outfile}
    ptolemyOUTPUT=$?
  else  
    ../Cleopatra/dwuck4/DWUCK4 <${infile}> ${outfile}
    ptolemyOUTPUT=$?
  fi
  
  echo "DWUCK4 exit code : " $ptolemyOUTPUT
  if [ ${ptolemyOUTPUT} -eq 0 ] ; then
      echo "DWUCK4 finished without problem. "
  else
      echo "DWUCK4 has error, check ${infile} or ${outfile}"
      exit 1;
  fi

fi;


if [ ${PlotResult} -eq 1 ] ; then 
  ../Cleopatra/dwuck4/DWUCK4 ${infile}
fi;
