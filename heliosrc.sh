# heliosrc
# this script for establishing bash enviroment 
# user should source this script at .bashrc
# if multiple digios copies exist, source heliosrc.sh at ~/ to change the HELIOSSYS

#=================== git and PS1
set_GitPS1()
{
    function git_branch {
	ref=$(git symbolic-ref HEAD 2> /dev/null) || return;
	echo "("${ref#refs/heads/}")";
    }
    
    function git_since_last_commit {
	now=`date +%s`;
	last_commit=$(git log --pretty=format:%at -1 2> /dev/null) || return;
	seconds_since_last_commit=$((now-last_commit));
	minutes_since_last_commit=$((seconds_since_last_commit/60));
	hours_since_last_commit=$((minutes_since_last_commit/60));
	minutes_since_last_commit=$((minutes_since_last_commit%60));
	
	echo "${hours_since_last_commit}h${minutes_since_last_commit}m";
    }

    #========== PS1 is variable of appearance of the prompt
    # \u user name
    # \W Working directory
    # \w long working directory
    # \h computer name
    # \! number in history
    #\e[X:XXm start of color code
    #\e[m end of color
    PS1="\h:\e[1;32m\w\e[m \e[0;35m\u\e[m|\t>\n[\!]\[\033[1;36m\]\$(git_branch)\[\033[0;33m\]\$(git_since_last_commit)\[\033[0m\]$"
}


#=================== History control
set_HistCtrl()
{
    export HISTTIMEFORMAT="%d/%m/%y %T "
    export PROMPT_COMMAND='history -a'  #save all history from all terminals
    HISTSIZE=50000
}

#=================== Main
unset HELIOSSYS
unset HELIOSDAQ
unset HELIOSANA

unset HELIOSSYS
SOURCE=${BASH_ARGV[0]}
if [ $(pwd) == $HOME ]; then 
  HELIOSSYS=$(dirname ${SOURCE})
else
  HELIOSSYS=$(pwd)
fi

#-------- optional
set_GitPS1
set_HistCtrl

############################ Setting of HELIOS-DIGIOS
export HELIOSSYS
HELIOSDAQ=${HELIOSSYS}/daq
HELIOSANA=${HELIOSSYS}/analysis/working

export HELIOSDAQ
export HELIOSANA

export PATH=${HELIOSSYS}/analysis/Armory:$PATH
#make the daq script avalible everywhere
export PATH=${HELIOSSYS}/daq/edm/scripts/:$PATH

source ${HELIOSSYS}/expName.sh
alias gotoRawData='cd ${daqDataPath}/${expName}; pwd'


alias gotoExp='cd ${HELIOSSYS}; pwd'
alias gotoDaq='cd ${HELIOSDAQ}; pwd'
alias gotoAna='cd ${HELIOSANA}; pwd'
alias sshMac='ssh -XY heliosdigios@192.168.1.164'  #iMac2020
alias sshDB='ssh -XY heliosdatabase@heliosDB.onenet'


alias Helios_Database='${HELIOSSYS}/daq/edm/scripts/helios_database'
alias SetThreshold='${HELIOSSYS}/daq/edm/scripts/SetVMEThreshold'
alias SetTrigger='${HELIOSSYS}/daq/edm/scripts/SetVMETrigger'

alias ShowRunTimeStamp='cat ${HELIOSSYS}/analysis/data/RunTimeStamp.dat'
alias ShowLastDB="cat ${HELIOSSYS}/daq/tempDB.txt; ls -lht ${HELIOSSYS}/daq/tempDB.txt"
alias AutoStartStop='${HELIOSSYS}/daq/edm/scripts/AutoStartStop'
alias ShowPVList="cat ${HELIOSSYS}/daq/edm/screens/pv_list.txt"

alias LastRunNum='source ${HELIOSSYS}/expName.sh; echo -e "========= ExpName:\033[0;31m${expName}\033[0m LastRunNum:\033[0;31m${LastRunNum}\033[0m"'

function ListHeliosFunctions {
    echo "LastRunNum"
    echo "ShowRunSize"
    echo "Helios_Database = start the database accuqsition"
    echo "ShowRunTimeStamp"
    echo "ShowLastDB = Show last pushed data to database"
    echo "SetThreshold"
    echo "SetTrigger"
    echo "ShowPVList"
    echo "AutoStartStop"
    echo "GlobusUpload = upload all data to LCRC"
    echo "GlobusDownload = download all data to local from LCRC"
    echo "DeleteSlackBotMsg, need SLACKTOKEN"
    echo "DeleteSlackBotFile, need SLACKTOKEN"
}

function ShowRunSize {
    if [ $# -ne 1 ]; then
	echo 'Please set run number '
       return 0
    fi
    source ${HELIOSSYS}/expName.sh
    RUN=$1
    if [ ${RUN} = "latest" ]; then
	RUN=${LastRunNum}
    fi
    runLen=${#RUN}
    if [ ${runLen} -eq 1 ]; then
	RUN="00"${RUN}
    elif [ ${runLen} -eq 2 ]; then
	RUN="0"${RUN}
    fi
    du -hsc ${HELIOSSYS}/analysis/data/${expName}_run_$RUN*
}

function GlobusUpload {
    source ~/digios/expName.sh
    DAQEndpoint='d907d6b0-986e-11e9-bf5e-0e4a062367b8'
    LCRCEndpoint='57b72e31-9f22-11e8-96e1-0a6d4e044368'

    fromPath=${DAQEndpoint}":"${daqDataPath}"/"${expName}"/data/"
    toPath=${LCRCEndpoint}":/lcrc/project/HELIOS/experimentsData/"${expName}"/data/"

    echo $fromPath
    echo $toPath

    globus transfer -r -s checksum  $fromPath $toPath

    globus task list

}

function GlobusDownload {
    source ~/digios/expName.sh
    DAQEndpoint='d907d6b0-986e-11e9-bf5e-0e4a062367b8'
    LCRCEndpoint='57b72e31-9f22-11e8-96e1-0a6d4e044368'

    toPath=${DAQEndpoint}":"${daqDataPath}"/"${expName}"/root_data/"
    fromPath=${LCRCEndpoint}":/lcrc/project/HELIOS/experimentsData/"${expName}"/root_data/"

    echo $fromPath
    echo $toPath

    globus transfer -r -s checksum  $fromPath $toPath

    globus task list

}


#============= Slack
function DeleteSlackBotMsg {

    if [ $# -ne 1 ]; then
        echo "please input channel_name"
        return
    fi

    CHANNEL=$1
    
    slack-cleaner --token ${SLACKTOKEN} --message --bot --rate 1 --channel $CHANNEL

    read -p "Are you want to delete them all? [y/n] " isDelete

    if [ $isDelete = "y" ]; then
        slack-cleaner --token ${SLACKTOKEN} --message --bot --rate 1 --channel $CHANNEL --perform
    fi

}

function DeleteSlackBotFile {
    
    slack-cleaner --token ${SLACKTOKEN} --file --rate 3 --user grafana_alert_bot
    
    read -p "Are you want to delete them all? [y/n] " isDelete
    
    if [ $isDelete = "y" ]; then
        slack-cleaner --token ${SLACKTOKEN} --file --rate 3 --user grafana_alert_bot --perform
    fi

}


unset -f setGitPS1
unset -f setHistCtrl
