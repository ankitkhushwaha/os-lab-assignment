#!/bin/bash

# Script to run the virtual memory simulator with different parameters
# and save the results to a CSV file.

EXECUTABLE="./vmm_simulator"
TRACE_FILES=("combined_short.trace" "combined.trace")
PAGE_SIZES=(1024 4096 16384) # 1KB, 4KB, 16KB
FRAME_COUNTS=(64 256 1024)
REPL_POLICIES=("FIFO" "LRU" "RANDOM" "OPTIMAL")
ALLOC_POLICIES=("GLOBAL" "LOCAL")
RESULTS_FILE="results.csv"

# Check if the executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Simulator executable '$EXECUTABLE' not found. Please compile it first with 'make'."
    exit 1
fi

# Create the CSV header
echo "TraceFile,PageSize,NumFrames,ReplPolicy,AllocPolicy,TotalPageFaults,P0_Faults,P1_Faults,P2_Faults,P3_Faults" > $RESULTS_FILE

# Run the experiments
for trace in "${TRACE_FILES[@]}"; do
    if [ ! -f "$trace" ]; then
        echo "Warning: Trace file '$trace' not found. Skipping."
        continue
    fi
    for ps in "${PAGE_SIZES[@]}"; do
        for fc in "${FRAME_COUNTS[@]}"; do
            for repl in "${REPL_POLICIES[@]}"; do
                for alloc in "${ALLOC_POLICIES[@]}"; do
                    echo "Running: $trace, $ps, $fc, $repl, $alloc"
                    
                    # Run the simulator and capture the output
                    output=$($EXECUTABLE $ps $fc $repl $alloc $trace)
                    
                    # Parse the output to extract page fault counts
                    total_faults=$(echo "$output" | grep "Total Page Faults" | awk '{print $4}')
                    p0_faults=$(echo "$output" | grep "Process 0 Page Faults" | awk '{print $5}')
                    p1_faults=$(echo "$output" | grep "Process 1 Page Faults" | awk '{print $5}')
                    p2_faults=$(echo "$output" | grep "Process 2 Page Faults" | awk '{print $5}')
                    p3_faults=$(echo "$output" | grep "Process 3 Page Faults" | awk '{print $5}')

                    # Append the results to the CSV file
                    echo "$trace,$ps,$fc,$repl,$alloc,$total_faults,$p0_faults,$p1_faults,$p2_faults,$p3_faults" >> $RESULTS_FILE
                done
            done
        done
    done
done

echo "All experiments finished. Results saved to $RESULTS_FILE"
