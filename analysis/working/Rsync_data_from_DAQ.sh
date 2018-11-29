#!/bin/sh

exp=iss631
dir=/Users/heliosdigios/experiments/${exp}
#remote data path
dataloc=/media/DIGIOSDATA3/data/${exp}

#===== directory
DATADIR=$dir/data

rsync -avuht --progress "helios@anldaqrouter:${dataloc}/${exp}_run_*" ${DATADIR}/.

exit 1
