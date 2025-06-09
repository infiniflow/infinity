#!/bin/bash

# Function to run a command repeatedly in an infinite loop
function run_repeatedly() {
    local cmd="$*"

    if [ -z "$cmd" ]; then
        echo "Error: Please provide a command to execute"
        echo "Usage: $0 <command>"
        exit 1
    fi

    echo "Will repeatedly execute command: $cmd"
    echo "Press Ctrl+C to stop"

    while true; do
        # Execute the command and capture output
        echo "=== Running at: $(date) ==="
        eval "$cmd"

        # Delay after command execution (adjust as needed)
        sleep 1

        # Add separator to distinguish between runs
        echo -e "\n----------------------------------------\n"
    done
}

# Check if arguments were provided
if [ $# -eq 0 ]; then
    echo "Error: No command provided to execute repeatedly"
    echo "Example usage:"
    echo "  $0 ls -l"
    echo "  $0 'ping -c 3 example.com'"
    exit 1
fi

# Call the function
run_repeatedly "$@"