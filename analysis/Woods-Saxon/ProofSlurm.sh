#!/bin/bash

#==================================== User Input

isotope=17O
IsNeutron=1;

searchRangeFile="wsSearch_FineRange.txt"

runTime="12:00:00"

userName="ttang"


#=====================================

jobName=WS${isotope}
outFile="ws_${isotope}.root"

echo "####################################"
echo "Job Name     : "${jobName}
echo "is neutron   : "${IsNeutron}
echo "search Range : "${searchRangeFile}
echo "output file  : "${outFile}
echo "####################################"

scriptName="slurm.sh"

#========== Generating the script
echo "#!/bin/bash" > ${scriptName}
echo ""           >> ${scriptName}
echo "#SBATCH -J ${jobName}"          >> ${scriptName}
echo "#SBATCH -p bdwall"              >> ${scriptName}
echo "#SBATCH -N 1 "                  >> ${scriptName}
echo "#SBATCH --ntasks-per-node=36 "  >> ${scriptName}
echo "#SBATCH -t ${runTime}"          >> ${scriptName}
echo ""                               >> ${scriptName}
echo root -b "'"'ProofWSSearch.C+("'${isotope}'",'${IsNeutron}',"'${searchRangeFile}'","'${outFile}'")'"'" >> ${scriptName}

#====================== run the script
sbatch ${scriptName}
#source ${scriptName}

squeue -u ${userName} -l

lcrc-sbank -q balance
