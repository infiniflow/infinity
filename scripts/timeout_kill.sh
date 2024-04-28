#!/bin/bash

# Check arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <pid> <duration>"
    exit 1
fi

PID=$1
DURATION=$2

# Send SIGTERM
kill -15 $PID

# Wait for the specified duration
sleep $DURATION

# Check if the process is still running
if ps -p $PID > /dev/null; then
   echo "Process $PID is still running. Sending SIGKILL."
   kill -9 $PID
   exit 2  # Return a different value
else
   echo "Process $PID has terminated successfully."
fi