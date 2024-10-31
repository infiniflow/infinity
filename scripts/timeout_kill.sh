#!/bin/bash

# Check arguments
if [ $# -lt 1 ]; then
    echo "Usage: $0 <duration_in_seconds> [pids]"
    exit 1
fi

DURATION=$1
# Check if no PIDs are provided
if [ $# -eq 1 ]; then
    echo "No PIDs provided. Exiting."
    exit 0
fi
# kill all infinity process
for pid in "${@:2}"; do
    # Send SIGTERM
    kill -15 $pid
done

# Start a timer
start_time=$(date +%s)
end_time=$((start_time + DURATION))

# Wait and check all processes status in a loop
while true; do
    # Check if all processes are still running
    all_dead=true
    for pid in "${@:2}"; do
        if ps -p $pid > /dev/null; then
            all_dead=false
            break
        fi
    done

    if $all_dead; then
        echo "All processes have terminated successfully."
        exit 0
    fi

    # Check if the duration has elapsed
    current_time=$(date +%s)
    if [ $current_time -ge $end_time ]; then
        echo "Some processes did not terminate in time. Sending SIGKILL."
        for pid in "${@:2}"; do
            kill -9 $pid
        done
        exit 2  # Return a different value
    fi

    # Sleep briefly to avoid a tight loop
    sleep 1
done
