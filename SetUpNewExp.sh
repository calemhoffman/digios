#!/bin/bash -l
echo "#############################################"
echo "#     Set-up for new Experiment"
echo "#############################################"

expName=$1

if [ $# -ne 0 ]; then
    if [ ${expName} == "master" ] || [ ${expName} == "Master" ] ; then
      expName="ARR01"
    fi
fi
    
echo -e "--- Git Fetch"
echo "when password is needed, please edit .git/config"
git fetch

echo -e "--- Checking git repository is clean or not...."
gitCheck=`git status --porcelain --untracked-files=no | wc -l`
if [ ${gitCheck} -eq 0 ]; then
    echo "---- clean."
else
    git status
    echo "=============== Please fix the git status"
    exit 1;
fi
#read -p "========== Have you git the present experiment (Y/N)?" isGit
#if [ ${isGit} == "N" ]; then
#    echo "Please git it."
#    exit 1
#fi

if [ $# -eq 0 ]; then
   git branch -a
   read -p 'Enter the new experiment name: ' expName    
fi

if [ ${expName} == "master" ] || [ ${expName} == "Master" ] ; then
   expName="ARR01"
   echo -e "----------- new experiment name : \033[0;31m${expName}\033[0m = master"
else
   echo -e "----------- new experiment name : \033[0;31m${expName}\033[0m"
fi

PCName="$(hostname)"

#------ Set up data folder, check disk space
echo "=================== Checking disk space."
echo "PC name : ${PCName}"

if [ ${PCName} == "digios1" ]; then  #DAQ
   DATAPATH=/media/DIGIOSDATA3
   expDIR=~/digios
   
   space=`df -ml | grep ${DATAPATH} | awk '{print $4}'` #in mb
   spacePrecent=`df -ml | tail -1 | awk '{print $5}'`
   spacePrecent="${spacePrecent:0:2}"
   echo "Free Space : ${space} MB |  ${spacePrecent}%-free"

elif [ ${PCName:0:5} == "phywl" ]; then #MAC
   DATAPATH=~/experiments
   expDIR=~/digios

   space=`df -ml | grep "/dev/disk2" | awk '{print $4}'` #in mb
   spacePrecent=`df -ml | grep "/dev/disk2" | awk '{print $5}'`
   spacePrecent="${spacePrecent:0:2}"
   echo "Free Space : ${space} MB |  ${spacePrecent}%-free"

elif [ ${PCName:0:5} == "bebop" ]; then #LCRC-Bebop
   DATAPATH=/lcrc/project/HELIOS/
   space=100000000
   expDIR=/lcrc/project/HELIOS/digios

else
   path=$(pwd)
   read -p "Please enter absolute DATAPATH (e.g. ${path}) " DATAPATH
   echo "DATAPATH for    raw data : ${DATAPATH}/${expName}/data"
   echo "DATAPATH for merged_data : ${DATAPATH}/${expName}/merged_data"
   echo "DATAPATH for   root_data : ${DATAPATH}/${expName}/root_data"   

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
      exit
   fi
fi


#------ check is there any GIt branch in repository, 
#       if not create Git Branch, if yes, checkout
echo "=================== Checking/Create Git Branch"
pullFlag="K"
isBranchExist=`git branch -a | grep ${expName} | wc -l`
if [ ${expName} == "ARR01" ]; then
   echo "this is master experiment name. no branch create."
   git checkout master
   isBranchExist=1
else

   if [ ${isBranchExist} -eq 0 ]; then
       git checkout -b ${expName}
   else
       echo "Experimental Name (${expName}) already in use."
       echo "Please take another name or git pull origin ${expName}"
       read -p "Do you want to checkout origin/${expName} (Y/N):"  pullFlag
       if [ ${pullFlag} == "N" ]; then
	   exit
       else
	   git checkout ${expName}
       fi
   fi
fi

#------ set up expName.sh, so that all experimental Name is refered to this name
if [ ${isBranchExist} -eq 0 ]; then
    DigiosDir="$(pwd)"
    echo "=================== Setting up ${DigiosDir}/expName.sh"
    touch ${DigiosDir}/expName.sh
    echo "#!/bin/bash -l" > ${DigiosDir}/expName.sh
    echo "expName=${expName}" >> ${DigiosDir}/expName.sh
    echo "LastRunNum=0" >> ${DigiosDir}/expName.sh
fi

echo "=================== making new folders in ${DATAPATH}/${expName}"
Data=${DATAPATH}/${expName}/data
mergedData=${DATAPATH}/${expName}/merged_data
rootData=${DATAPATH}/${expName}/root_data

mkdir -vp ${Data}
mkdir -vp ${mergedData}
mkdir -vp ${rootData}


#===== create symbolic links
echo "=================== creating symbolic links"
rm -f ${expDIR}/analysis/data
rm -f ${expDIR}/analysis/merged_data
rm -f ${expDIR}/analysis/root_data

ln -sfv ${Data} ${expDIR}/analysis/data
ln -sfv ${mergedData} ${expDIR}/analysis/merged_data
ln -sfv ${rootData} ${expDIR}/analysis/root_data

echo "=================== done."

#===== clean up working if it is new
if [ ${isBranchExist} -eq 0 ]; then

    echo "======== Clean up working directory "
    rm -fv ${expDIR}/analysis/working/correction_*.dat
    rm -fv ${expDIR}/analysis/working/reaction.dat
    rm -fv ${expDIR}/analysis/working/run_Summary.dat
    rm -fv ${expDIR}/analysis/working/example.*
    rm -fv ${expDIR}/analysis/working/RunTimeStamp.dat
    rm -fv ${expDIR}/analysis/working/*.root
    rm -fv ${expDIR}/analysis/working/*.d
    rm -fv ${expDIR}/analysis/working/*.so
    rm -fv ${expDIR}/analysis/working/*.pcm

    echo "======== git commit "
    git add -A
    git commit -m "new experiment ${expName}"

    echo "======== git push "
    git push origin ${expName}

fi 
