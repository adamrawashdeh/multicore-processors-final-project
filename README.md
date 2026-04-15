-------------------------------------------------------------------------------
ATOMIC OPERATIONS BENCHMARK: SEQUENTIAL, ATOMIC, CRITICAL, AND REDUCTION
-------------------------------------------------------------------------------

This directory contains C implementations of the synchronization benchmarks 
and a comprehensive automation script for performance profiling.

FILES:
- atomicOperations.c : Benchmark implementation supporting sequential, atomic,
                       critical, and reduction modes.
- run_tests.sh       : Bash script to automate multiple runs, vary iteration 
                       counts and thread counts, and compute average runtime 
                       and speedup.

-------------------------------------------------------------------------------
1. ENVIRONMENT VARIABLES
-------------------------------------------------------------------------------

OpenMP environment variables can be used to control thread behavior. However, 
in this benchmark, thread count is explicitly set within the program using:

    omp_set_num_threads(threads);

Therefore, external variables such as OMP_NUM_THREADS are not required.

-------------------------------------------------------------------------------
2. COMPILATION
-------------------------------------------------------------------------------

Compile using GCC with optimization and OpenMP support:

gcc -O2 -fopenmp atomicOperations.c -o atomicOperations

-------------------------------------------------------------------------------
3. RUNNING INDIVIDUAL FILES
-------------------------------------------------------------------------------

After compilation, you can run the program manually. The program requires 
three arguments:

1. iterations : Number of loop iterations to execute
2. mode       : Execution mode (seq, atomic, critical, reduction)
3. threads    : Number of OpenMP threads to use

RUNNING THE SEQUENTIAL VERSION:
./atomicOperations 1000000 seq 1

RUNNING THE REDUCTION VERSION:
./atomicOperations 1000000 reduction 8

RUNNING THE ATOMIC VERSION:
./atomicOperations 1000000 atomic 8

RUNNING THE CRITICAL VERSION:
./atomicOperations 100000 critical 8

PROGRAM OUTPUT:
The program prints:
- mode       : Selected execution mode
- threads    : Number of threads used
- iterations : Number of iterations executed
- result     : Final counter value
- time       : Total execution time in seconds

-------------------------------------------------------------------------------
4. AUTOMATED PROFILING (run_tests.sh)
-------------------------------------------------------------------------------

The provided script 'run_tests.sh' automates the benchmarking process across 
multiple iteration counts and thread counts.

USAGE:
chmod +x run_tests.sh
./run_tests.sh

The script performs the following:
* Compiles the program using:
  gcc -O2 -fopenmp atomicOperations.c -o atomicOperations
* Runs each configuration 5 times.
* Computes the average runtime for each run.
* Uses the sequential version as the baseline for speedup calculation.
* Tests the following thread counts:
  (1, 2, 4, 8, 16, 32, 64)

BENCHMARKED ITERATION COUNTS:
* reduction:
  (1,000,000, 10,000,000, 100,000,000)
* atomic:
  (1,000,000, 10,000,000, 100,000,000)
* critical:
  (100,000, 1,000,000, 10,000,000)

OUTPUT:
* Printed tables showing:
  - Threads
  - Iterations
  - Average execution time
  - Speedup relative to sequential execution

-------------------------------------------------------------------------------
5. HARDWARE COUNTER PERMISSIONS
-------------------------------------------------------------------------------

You can collect hardware performance counters using the Linux 'perf' tool.

Example commands:

perf stat -r 5 -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
./atomicOperations 1000000 reduction 8

perf stat -r 5 -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
./atomicOperations 1000000 atomic 8

perf stat -r 5 -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
./atomicOperations 100000 critical 8

If hardware counters are unavailable, your system may restrict access. You can 
temporarily lower the restriction (requires sudo):

sudo sysctl -w kernel.perf_event_paranoid=0

-------------------------------------------------------------------------------