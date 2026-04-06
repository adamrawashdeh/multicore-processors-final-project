#!/bin/bash

/opt/homebrew/bin/gcc-15 -O2 -fopenmp atomicOperations.c -o atomicOperations

RUNS=5
THREADS=(1 2 4 8 16 32 64)

REDUCTION_ITERS=(1000000 10000000 100000000)
ATOMIC_ITERS=(1000000 10000000 100000000)
CRITICAL_ITERS=(100000 1000000 10000000)

measure_avg_time() {
    local iterations=$1
    local mode=$2
    local threads=$3
    local sum=0

    for r in $(seq 1 $RUNS); do
        output=$(./atomicOperations "$iterations" "$mode" "$threads")
        time=$(echo "$output" | awk -F= '/^time=/{print $2}')
        sum=$(echo "$sum + $time" | bc -l)
    done

    echo "$(echo "scale=8; $sum / $RUNS" | bc -l)"
}

run_benchmark() {
    local mode=$1
    shift
    local iterations_list=("$@")

    echo "================ $(echo "$mode" | tr '[:lower:]' '[:upper:]') ================="

    for iterations in "${iterations_list[@]}"; do
        local tseq=$(measure_avg_time "$iterations" seq 1)

        echo "Sequential baseline for this mode: iterations=$iterations, Tseq=$tseq"
        printf "%-10s %-15s %-15s %-15s\n" "Threads" "Iterations" "Avg Time" "Speedup"

        for t in "${THREADS[@]}"; do
            avg=$(measure_avg_time "$iterations" "$mode" "$t")
            speedup=$(echo "scale=8; $tseq / $avg" | bc -l)
            printf "%-10d %-15s %-15s %-15s\n" "$t" "$iterations" "$avg" "$speedup"
        done

        echo ""
    done
}

run_benchmark reduction "${REDUCTION_ITERS[@]}"
run_benchmark atomic "${ATOMIC_ITERS[@]}"
run_benchmark critical "${CRITICAL_ITERS[@]}"