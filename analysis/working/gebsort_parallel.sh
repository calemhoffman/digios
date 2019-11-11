#!/bin/bash
count=0
echo "GEBSorting in parallel"

sort_func(){
    sleep 1
    echo "Working file $1"
    ./process_run $i 1 1 1 0
    wait
    echo "Done with file $1"
    }

# For loop
for i in {25..205}
do
    sort_func $i & #
    count=$((count+1))
    echo "Counter status: $count"
    if [ $count -eq 35 ]; then
	#echo "inside"
	wait
	count=0
    fi
done
wait
echo "GEBSort all done"
