# ./EventBuilder test.root -1 1000 `\ls -1 run_30*`

# ./EventBuilder test.root 1000 0 run_30.gtd04_000_0113

# ./EventBuilder test.root 1000 0 `\ls -1 run_107*`
# ./EventBuilder test.root 1000 0 run_107.gtd04_000_0113 run_107.gtd04_001_0113

# ./EventBuilder test1.root 1000 0 `\ls -1 ../data/*run_040*`
# ./EventBuilder test_trace.root 100000000 1 ../data/h093_21F_d3He_run_040.gtd02_000_0105

./EventBuilder test2.root 1000 1  10 `\ls -1 ../data/*run_040*`
# ./EventBuilder_Q_ca test2.root 1000 1 `\ls -1 ../data/*run_040*`
# ./EventBuilder test2.root 1000 1 `\ls -1 ../data/*run_040*`

# ./EventBuilder test2.root -1 1 `\ls -1 s008_29Mg_*`

exit

#======== GEB Merge and Sorte
start=$(date +%s%N)
../GEBSort/GEBMerge ../working/GEBMerge.chat  merged_file `\ls -1 ../data/*run_040*`

../GEBSort/EventBuilder_trace -input disk merged_file_000 -rootfile  trace.root -chat ../working/GEBSort.chat

end=$(date +%s%N)
elapsed=$(( (end - start) / 1000 ))  # convert nanoseconds to microseconds
echo -e "\033[31m====================== GEBMerge and GEBSort DONE\033[0m"
echo "Elapsed time: ${elapsed} microseconds = $(echo "scale=2; $elapsed/1000000" | bc) seconds"