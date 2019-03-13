#!/bin/bash -l
echo "#############################################"
echo "#     Set-up for new Experiment"
echo "#############################################"

expName=$1
if [ $# -eq 0 ]; then
   read -p 'Enter the new experiment name: ' expName
fi

PCName="$(hostname)"

if [ ${PCName} == "phywl183.phy.anl.gov" ]; then 
   echo "----------- new experiment name : ${expName}" #for MAC
else
   echo -e "----------- new experiment name : \033[0;31${expName}\033[0m"
fi

#------ Set up data folder, check disk space
echo "=================== Checking disk space."


if [ ${PCName} == "digios1" ]; then  #DAQ
   DATAPATH=/media/DIGIOSDATA3
   
   space=`df -ml | tail -1 | awk '{print $4}'` #in mb
   spacePrecent=`df -ml | tail -1 | awk '{print $5}'`
   spacePrecent="${spacePrecent:0:2}"
   echo "Free Space : ${space} MB |  ${spacePrecent}%-free"
fi

if [ ${PCName} == "phywl183.phy.anl.gov" ]; then #MAC
   DATAPATH=~/experiments
   
   space=`df -ml | tail -1 | awk '{print $4}'` #in mb
   spacePrecent=`df -ml | tail -1 | awk '{print $5}'`
   spacePrecent="${spacePrecent:0:2}"
   echo "Free Space : ${space} MB |  ${spacePrecent}%-free"
fi

if [ "${space}" -le 512000 ]; then
   echo "The avalible space is less then 500 GB."
   read -p  '-------------------------- Continue? (Y/N) ' OKFlag
   
   if [ ${OKFlag} == "N" ]; then
      echo "xxxxxxxxxxxxxx Abort SetUpNewExp.sh."
      exit 0;
   fi
fi

#------ set up expName.sh, so that all experimental Name is refered to this name
echo "=================== Setting up ~/digios/expName.sh"
touch ~/digios/expName.sh
echo "#!/bin/bash -l" > expName.sh
echo "expName=${expName}" >> expName.sh

#------ check is there any GIt branch in repository, 
#       if not create Git Branch, if yes, checkout
#echo "=================== Checking/Create Git Branch"
#if [ ${expName} == "ARR01" ]; then
#   echo "this is master experiment name. no branch create."
#else
#   git checkout -b ${expName}
#fi


echo "=================== making new folders in ${DATAPATH}/${expName}"
Data=${DATAPATH}/${expName}/data
mergedData=${DATAPATH}/${expName}/merged_data
rootData=${DATAPATH}/${expName}/root_data

mkdir -vp ${Data}
mkdir -vp ${mergedData}
mkdir -vp ${rootData}


#===== create symbolic links
echo "=================== creating symbolic links"
expDIR=~/digios
rm -f ${expDIR}/analysis/data
rm -f ${expDIR}/analysis/merged_data
rm -f ${expDIR}/analysis/root_data

ln -sfv ${Data} ${expDIR}/analysis/data
ln -sfv ${mergedData} ${expDIR}/analysis/merged_data
ln -sfv ${rootData} ${expDIR}/analysis/root_data

echo "=================== done."
