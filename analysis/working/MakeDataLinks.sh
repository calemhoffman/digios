#!/bin/sh

echo "=============================================== "
echo "============  MakeDataLink.sh ================= "
echo "----------------------------------------------- "
echo " This bash script will make symoblic links for"
echo "       1) analysis/data "
echo "       2) analysis/merged_data "
echo "       3) analysis/root_data "
echo "----------------------------------------------- "

RED='\033[0;31m'
YELLOW='\033[1;33m'
ORANGE='\033[0;33m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
Cyan='\033[0;36m'
NC='\033[0m'

PCNAME=${HOSTNAME}

#================ Find computer
if [ $PCNAME == "phywl162.phy.anl.gov" ]; then
   echo " Detecting computer : ${RED}HELIOS iMac${NC}"
   
   branch=$(git symbolic-ref HEAD)
   branch=${branch#refs/heads/}
   
   echo " Current Git Branch : ${RED}$branch ${NC}"
   
   DATAPATH=~/experiment
  
   if [ $branch == "master" ] ; then exp=iss631; fi
   if [ $branch == "ISS631" ] ; then exp=iss631; fi
   if [ $branch == "ISS000" ] ; then exp=iss000; fi
   
fi

if [ $PCNAME == "phywl154.phy.anl.gov" ]; then
  echo " Detecting computer : ${RED}Ryan Mac Air${NC}"
  
  branch=$(git symbolic-ref HEAD)
  branch=${branch#refs/heads/}
   
  echo " Current Git Branch : ${RED}$branch${NC}"
  echo "----- no need to do anything."
  
  exit

fi

if [ ${PCNAME:0:5} == "bebop" ]; then
   echo " Detecting computer : BEBOP"
   
   branch=$(git symbolic-ref HEAD)
   branch=${branch#refs/heads/}
   
   echo " Current Git Branch : $branch"
   
   DATAPATH=/lcrc/project/HELIOS
  
   if [ $branch == "master" ] ; then exp=iss631; fi
   if [ $branch == "ISS631" ] ; then exp=iss631; fi
   if [ $branch == "ISS000" ] ; then exp=iss000; fi

fi

#================= create links
echo "--------- remove exist links"
rm -f ../data
rm -f ../merged_data
rm -f ../root_data

echo "--------- create links"
ln -s ${DATAPATH}/${exp}/data         ../data
ln -s ${DATAPATH}/${exp}/merged_data  ../merged_data
ln -s ${DATAPATH}/${exp}/root_data    ../root_data

echo "---------------------- check."
ls -lhtr ../
echo "---------------------- done."
