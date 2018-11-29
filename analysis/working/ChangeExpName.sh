#!/bin/sh

echo "============= Change Experimental Name ======"
read -p 'Old Experiment Name: ' expNameOld
read -p 'New Experiment Name: ' expName

sed -i '' -e "s/${expNameOld}/${expName}/g" gebmerge.sh
sed -i '' -e "s/${expNameOld}/${expName}/g" gebsortmerged.sh
sed -i '' -e "s/${expNameOld}/${expName}/g" process_run.C
sed -i '' -e "s/${expNameOld}/${expName}/g" process_run.sh
sed -i '' -e "s/${expNameOld}/${expName}/g" process_run_simple.sh

grep -nrw ${expName} .

echo "========= need to reset the symbolic link."
