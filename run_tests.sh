#!/bin/bash

gcc -O3 -fopenmp primeCounting.c -lm -o primeCounting

RUNS=5
THREADS=(1 2 4 8 16 32 64)
INPUT_SIZES=(100000 1000000 10000000)

measure_avg_time() {
    local n=$1
    local mode=$2
    local threads=$3
    local sum=0

    for r in $(seq 1 $RUNS); do
        if [ "$mode" = "0" ]; then
            output=$(./primeCounting "$n" 0)
        else
            output=$(OMP_NUM_THREADS=$threads ./primeCounting "$n" 1)
        fi

        time=$(echo "$output" | awk '/Time =/ {print $(NF-1)}')
        sum=$(echo "$sum + $time" | bc -l)
    done

    echo "$(echo "scale=8; $sum / $RUNS" | bc -l)"
}

run_benchmark() {
    echo "================ PRIME COUNTING ================="

    for n in "${INPUT_SIZES[@]}"; do
        local tseq=$(measure_avg_time "$n" 0 1)

        echo "Sequential baseline: N=$n, Tseq=$tseq"
        printf "%-10s %-15s %-15s %-15s\n" "Threads" "N" "Avg Time" "Speedup"

        for t in "${THREADS[@]}"; do
            avg=$(measure_avg_time "$n" 1 "$t")
            speedup=$(echo "scale=8; $tseq / $avg" | bc -l)
            printf "%-10d %-15s %-15s %-15s\n" "$t" "$n" "$avg" "$speedup"
        done

        echo ""
    done
}

run_benchmark