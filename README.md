-------------------------------------------------------------------------------
ATOMIC OPERATIONS BENCHMARK: SEQUENTIAL, ATOMIC, CRITICAL, AND REDUCTION
-------------------------------------------------------------------------------

This directory contains C implementations of the Synchronization benchmarks and a 
comprehensive automation script for performance profiling.

FILES:
- atomicOperations.c : Benchmark implementation supporting sequential, atomic,
                       critical, and reduction modes.
- run_tests.sh       : Bash script to compile the program, execute multiple 
                       runs, and report average timing and speedup.

-------------------------------------------------------------------------------
1. COMPILATION
-------------------------------------------------------------------------------

Compile using GCC with optimization and OpenMP support:

gcc-15 -O2 -fopenmp atomicOperations.c -o atomicOperations

-------------------------------------------------------------------------------
2. RUNNING INDIVIDUAL FILES
-------------------------------------------------------------------------------

After compilation, you can run the program manually. The program requires 
three arguments:

1. iterations : Number of loop iterations to execute
2. mode       : Execution mode (seq, atomic, critical, reduction)
3. threads    : Number of OpenMP threads to use

RUNNING THE SEQUENTIAL VERSION:
./atomicOperations 1000000 seq 1
./atomicOperations 100000000 seq 1

RUNNING THE REDUCTION VERSION:
./atomicOperations 1000000 reduction 4
./atomicOperations 100000000 reduction 8

RUNNING THE ATOMIC VERSION:
./atomicOperations 1000000 atomic 4
./atomicOperations 100000000 atomic 8

RUNNING THE CRITICAL VERSION:
./atomicOperations 100000 critical 4
./atomicOperations 10000000 critical 8

PROGRAM OUTPUT:
The program prints:
- mode       : Selected execution mode
- threads    : Number of threads used
- iterations : Number of iterations executed
- result     : Final counter value
- time       : Total execution time in seconds

-------------------------------------------------------------------------------
3. AUTOMATED BENCHMARKING (run_tests.sh)
-------------------------------------------------------------------------------

The provided script 'run_tests.sh' automates benchmarking across multiple 
iteration counts and thread counts.

USAGE:
chmod +x run_tests.sh
./run_tests.sh

The script performs the following:
* Compiles the program using:
  /opt/homebrew/bin/gcc-15 -O2 -fopenmp atomicOperations.c -o atomicOperations
* Repeats each configuration 5 times.
* Computes the average execution time for each run.
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
For each mode and iteration count, the script prints a table containing:
* Threads
* Iterations
* Average Time
* Speedup

-------------------------------------------------------------------------------
4. PERFORMANCE ANALYSIS
-------------------------------------------------------------------------------

This benchmark compares the cost of different synchronization modes in 
OpenMP:

* seq       : Single-threaded baseline.
* reduction : Uses OpenMP reduction to combine partial results efficiently.
* atomic    : Uses atomic updates to safely increment the shared counter.
* critical  : Uses a critical section to protect the shared counter update.

The benchmark is designed to illustrate the relative overhead of each 
synchronization mechanism and how performance changes as thread count increases.

-------------------------------------------------------------------------------
5. HARDWARE PERFORMANCE PROFILING
-------------------------------------------------------------------------------

You can collect hardware performance counters using the Linux 'perf' tool.

Example command for all modes:
perf stat -r 5 -e cycles,instructions,cache-references,cache-misses \
./atomicOperations 100000000 reduction 8

perf stat -r 5 -e cycles,instructions,cache-references,cache-misses \
./atomicOperations 100000000 atomic 8

perf stat -r 5 -e cycles,instructions,cache-references,cache-misses \
./atomicOperations 10000000 critical 8

Explanation:
* -r 5 : Runs the benchmark 5 times and averages results.
* cycles, instructions : CPU execution metrics.
* cache-references, cache-misses : Memory hierarchy behavior.
-------------------------------------------------------------------------------