#!/bin/bash -l

# Define the delay time in seconds (2 hours = 7200 seconds)
DELAY=7200

# Print a message indicating the wait time
echo "Waiting for 2 hours before stopping the DAQ..."

# Sleep for the specified delay time
sleep $DELAY

# Run the specified program
echo "Stop the DAQ now..."

source ${HELIOSSYS}/daq/edm/scripts/stop_run.sh "Auto stop after 2 hours"
