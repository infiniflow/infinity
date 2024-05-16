#!/bin/bash

# Check arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <pid> <duration_in_seconds>"
    exit 1
fi

PID=$1
DURATION=$2

# Send SIGTERM
kill -15 $PID

# Start a timer
start_time=$(date +%s)
end_time=$((start_time + DURATION))

# Wait and check process status in a loop
while true; do
    # Check if the process is still running
    if ! ps -p $PID > /dev/null; then
        echo "Process $PID has terminated successfully."
        exit 0
    fi

    # Check if the duration has elapsed
    current_time=$(date +%s)
    if [ $current_time -ge $end_time ]; then
        echo "Process $PID did not terminate in time. Sending SIGKILL."
        kill -9 $PID
        exit 2  # Return a different value
    fi

    # Sleep briefly to avoid a tight loop
    sleep 1
done
