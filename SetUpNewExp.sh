#!/bin/bash -l

echo "#############################################"
echo "#     Set-up for new Experiment"
echo "#############################################"

read -p 'Enter the new experiment name: ' expName

echo "----------- new experiment name : ${expName}"

EXPPATH=/home/helios/experiments
DATAPATH=/media/DIGIOSDATA3
OLDexpName=iss631

expDIR=${EXPPATH}/${expName}

#===== make new folders
#echo "====== Creaing ${expDIR} in ${EXPPATH}"
#mkdir -vp ${expDIR}

echo "====== making new folder in ${DATAPATH}"
Data=${DATAPATH}/data/${expName}
mergedData=${DATAPATH}/merged_data/${expName}
rootData=${DATAPATH}/root_data/${expName}

mkdir -vp ${Data}
mkdir -vp ${mergedData}
mkdir -vp ${rootData}

#===== cp to expfolder
#echo "======= copy folder to ${expDIR}"
#cp -rfln ${EXPPATH}/clean/* ${expDIR}/.

#===== replace string
echo "======= replacing string"
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/process_run.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/process_run.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/process_run_local.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/process_run_simple.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/gebmerge_local.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/gebsortmerged.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/gebmerge.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/process_run.C
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/process_run_local.C
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/analysis/working/gebsortmerged_local.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/daq/edm/scripts/start_run.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/daq/edm/scripts/helios_setup_digitizer
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/daq/edm/scripts/open_r.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/daq/edm/scripts/close_r.sh
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/daq/edm/scripts/helios_setup_trigger
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/daq/edm/scripts/helios_setup_other
sed -i "s/${OLDexpName}/${expName}/g" ${expDIR}/daq/heliosCommander

#===== check replace string
echo "========= check the replace"
grep -rnw ${expDIR} -e "${expName}"

#===== replace .bashrc
echo "======== replace .bashrc"
sed -i "s/${OLDexpName}/${expName}/g" ~/.bashrc

#===== create symbolic links
echo "======== creating symbolic links"
rm -f ${expDIR}/analysis/data
rm -f ${expDIR}/analysis/merged_data
rm -f ${expDIR}/analysis/root_data

ln -sf ${Data} ${expDIR}/analysis/data
ln -sf ${mergedData} ${expDIR}/analysis/merged_data
ln -sf ${rootData} ${expDIR}/analysis/root_data

#===== delete SetUpNewExp.sh
#rm -fv ${expDIR}/SetUpNewExp.sh

echo "============ done."
