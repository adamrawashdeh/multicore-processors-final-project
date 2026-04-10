#!/bin/bash

set -u

SIZES=(1000000 10000000 100000000 1000000000)
THREADS=(1 2 4 8 16 32 64)

SEQ_SRC="stream_seq.c"
OMP_SRC="stream_omp.c"
SEQ_BIN="./stream_seq"
OMP_BIN="./stream_omp"

RESULTS_DIR="results_stream"
TIMING_CSV="$RESULTS_DIR/timings.csv"
PERF_CSV="$RESULTS_DIR/perf.csv"

# For perf_event_paranoid=2, use user-space-only hardware events.
PERF_EVENTS="cycles:u,instructions:u,cache-references:u,cache-misses:u,branches:u,branch-misses:u,task-clock,context-switches,cpu-migrations"

mkdir -p "$RESULTS_DIR"

compile_all() {
    echo "Compiling benchmarks..."
    gcc -O3 -std=c11 -march=native -fopenmp "$SEQ_SRC" -o "$SEQ_BIN" -lm
    gcc -O3 -std=c11 -march=native -fopenmp "$OMP_SRC" -o "$OMP_BIN" -lm
}

write_csv_headers() {
    echo "size,mode,threads,copy_time,scale_time,add_time,triad_time,checksum" > "$TIMING_CSV"
    echo "size,mode,threads,status,cycles,instructions,cache_references,cache_misses,branches,branch_misses,task_clock_ms,context_switches,cpu_migrations" > "$PERF_CSV"
}

extract_time() {
    local label="$1"
    local text="$2"
    echo "$text" | grep "$label" | grep -oE "[0-9]+\.[0-9]+"
}

# Match exact event field in perf -x ';' output
get_perf_value() {
    local event_name="$1"
    local file="$2"
    awk -F';' -v ev="$event_name" '$3 == ev {print $1}' "$file" | tail -n 1
}

run_timing_bench() {
    local cmd="$1"
    local size="$2"
    local mode="$3"
    local threads="$4"

    local output
    output=$($cmd "$size" 2>&1)
    local rc=$?

    if [ $rc -ne 0 ]; then
        printf "%-12s | %-10s | %-7s | %-12s | %-12s | %-12s | %-12s\n" \
               "$size" "$mode" "$threads" "ERR" "ERR" "ERR" "ERR"
        echo "Timing run failed for size=$size mode=$mode threads=$threads" >&2
        echo "$output" >&2
        return 1
    fi

    local copy_val scale_val add_val triad_val checksum_val
    copy_val=$(extract_time "Average Copy time" "$output")
    scale_val=$(extract_time "Average Scale time" "$output")
    add_val=$(extract_time "Average Add time" "$output")
    triad_val=$(extract_time "Average Triad time" "$output")
    checksum_val=$(echo "$output" | grep "Checksum" | grep -oE "[0-9]+\.[0-9]+" | tail -n 1)

    printf "%-12s | %-10s | %-7s | %-12s | %-12s | %-12s | %-12s\n" \
           "$size" "$mode" "$threads" "$copy_val" "$scale_val" "$add_val" "$triad_val"

    echo "$size,$mode,$threads,$copy_val,$scale_val,$add_val,$triad_val,$checksum_val" >> "$TIMING_CSV"
    return 0
}

run_perf_bench() {
    local cmd="$1"
    local size="$2"
    local mode="$3"
    local threads="$4"

    local perf_out_file="$RESULTS_DIR/perf_${mode}_${threads}_${size}.txt"

    perf stat --no-big-num -x ';' -e "$PERF_EVENTS" "$cmd" "$size" > /dev/null 2> "$perf_out_file"
    local rc=$?

    local raw_text
    raw_text=$(cat "$perf_out_file")

    local status="OK"
    if echo "$raw_text" | grep -qiE "not supported|not counted|no permission|permission denied|access"; then
        status="UNAVAILABLE"
    elif [ $rc -ne 0 ]; then
        status="ERROR"
    fi

    local cycles instructions cache_refs cache_misses branches branch_misses task_clock ctx_switches cpu_migrations

    cycles=$(get_perf_value "cycles:u" "$perf_out_file")
    instructions=$(get_perf_value "instructions:u" "$perf_out_file")
    cache_refs=$(get_perf_value "cache-references:u" "$perf_out_file")
    cache_misses=$(get_perf_value "cache-misses:u" "$perf_out_file")
    branches=$(get_perf_value "branches:u" "$perf_out_file")
    branch_misses=$(get_perf_value "branch-misses:u" "$perf_out_file")
    task_clock=$(get_perf_value "task-clock" "$perf_out_file")
    ctx_switches=$(get_perf_value "context-switches" "$perf_out_file")
    cpu_migrations=$(get_perf_value "cpu-migrations" "$perf_out_file")

    cycles=${cycles:-NA}
    instructions=${instructions:-NA}
    cache_refs=${cache_refs:-NA}
    cache_misses=${cache_misses:-NA}
    branches=${branches:-NA}
    branch_misses=${branch_misses:-NA}
    task_clock=${task_clock:-NA}
    ctx_switches=${ctx_switches:-NA}
    cpu_migrations=${cpu_migrations:-NA}

    echo "$size,$mode,$threads,$status,$cycles,$instructions,$cache_refs,$cache_misses,$branches,$branch_misses,$task_clock,$ctx_switches,$cpu_migrations" >> "$PERF_CSV"
    return 0
}

compile_all
write_csv_headers

export OMP_PLACES=cores
export OMP_PROC_BIND=close

echo
echo "Comprehensive STREAM Benchmark"
echo "================================================================================================================"
printf "%-12s | %-10s | %-7s | %-12s | %-12s | %-12s | %-12s\n" "Size" "Mode" "Threads" "Copy(s)" "Scale(s)" "Add(s)" "Triad(s)"
echo "----------------------------------------------------------------------------------------------------------------"

for size in "${SIZES[@]}"
do
    run_timing_bench "$SEQ_BIN" "$size" "SEQ" "1"
    run_perf_bench   "$SEQ_BIN" "$size" "SEQ" "1"

    for t in "${THREADS[@]}"
    do
        export OMP_NUM_THREADS="$t"
        run_timing_bench "$OMP_BIN" "$size" "OMP" "$t"
        run_perf_bench   "$OMP_BIN" "$size" "OMP" "$t"
    done

    echo "----------------------------------------------------------------------------------------------------------------"
done

echo
echo "Done."
echo "Timing CSV : $TIMING_CSV"
echo "Perf CSV   : $PERF_CSV"
echo "Raw perf files stored in: $RESULTS_DIR/"
