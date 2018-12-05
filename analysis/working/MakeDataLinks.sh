#!/bin/sh

echo "=============================================== "
echo "============  MakeDataLink.sh ================= "
echo "----------------------------------------------- "
echo " This bash script will make symoblic links for"
echo "       1) analysis/data "
echo "       2) analysis/merged_data "
echo "       3) analysis/root_data "
echo "----------------------------------------------- "
echo " Detecting computer : \c"

if [ $HOSTNAME == "phywl162.phy.anl.gov" ]; then
   echo "  HELIOS iMac "
   
   branch=$(git symbolic-ref HEAD)
   branch=${branch#refs/heads/}
   
   echo " Current Git Branch :   $branch "
   
   DATAPATH=~/experiment
  
   if [ $branch == "master" ] ; then exp=iss631; fi
   
   if [ $branch == "ISS631" ] ; then exp=iss631; fi
   
   if [ $branch == "ISS000" ] ; then exp=iss000; fi
   
fi





ln -shfv ${DATAPATH}/${exp}/data         ../data
ln -shfv ${DATAPATH}/${exp}/merged_data  ../merged_data
ln -shfv ${DATAPATH}/${exp}/root_data    ../root_data
