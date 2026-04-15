-------------------------------------------------------------------------------
PRIME COUNTING BENCHMARK: SEQUENTIAL, OPENMP, AND PERFORMANCE ANALYSIS
-------------------------------------------------------------------------------

This directory contains C implementations of the Prime Counting Application (Compute-Bound)
and a comprehensive automation script for performance profiling.

FILES:
- primeCounting.c : Combined sequential and OpenMP implementation of 
                    prime counting.
- run_tests.sh    : Bash script to compile, execute multiple runs with 
                    varying input sizes and thread counts.

-------------------------------------------------------------------------------
1. COMPILATION
-------------------------------------------------------------------------------

Compile using GCC with optimization and OpenMP support:

gcc -O3 -fopenmp primeCounting.c -lm -o primeCounting

-------------------------------------------------------------------------------
2. RUNNING INDIVIDUAL FILES
-------------------------------------------------------------------------------

After compilation, you can run the program manually. The program requires 
two arguments:

1. N     : Upper bound for prime search
2. mode  : Execution mode (0 = sequential, 1 = OpenMP)

RUNNING THE SEQUENTIAL VERSION:
./primeCounting 100000000 0
./primeCounting 50000000 0

RUNNING THE OPENMP VERSION:
# Set threads before running
export OMP_NUM_THREADS=4
./primeCounting 100000000 1
./primeCounting 200000000 1

-------------------------------------------------------------------------------
3. AUTOMATED BENCHMARKING (run_tests.sh)
-------------------------------------------------------------------------------

The provided script 'run_tests.sh' automates benchmarking across multiple input 
sizes and thread counts.

USAGE:
chmod +x run_tests.sh
./run_tests.sh

The script performs the following:
* Compiles the program using:
  gcc -O3 -fopenmp primeCounting.c -lm -o primeCounting
* Runs the benchmark for multiple values of N:
  (100,000 → 100,000,000)
* Varies thread counts:
  (1, 2, 4, 8, 16, 32, 64)
* Executes both sequential and OpenMP modes (depending on usage).
* Prints execution time and prime counts for each configuration.

-------------------------------------------------------------------------------
4. HARDWARE PERFORMANCE PROFILING
-------------------------------------------------------------------------------

You can collect hardware performance counters using the Linux 'perf' tool.

Example command:
perf stat -r 5 -e cycles,instructions,cache-references,cache-misses \
./primeCounting 100000000 1

Explanation:
* -r 5 : Runs the benchmark 5 times and averages results.
* cycles, instructions : CPU execution metrics.
* cache-references, cache-misses : Memory hierarchy behavior.

You may also test different thread counts:
OMP_NUM_THREADS=8 perf stat -r 5 -e cycles,instructions \
./primeCounting 100000000 1

-------------------------------------------------------------------------------