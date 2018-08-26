#/bin/bash -l

if [ $# -eq 1 ]
then
    cmd=$1
    case $cmd in
       1)	xterm -e "file_count.sh" ;;
       2)	xterm -e "bufs.sh" "1" ;;
    esac
    exit
fi
