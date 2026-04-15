-------------------------------------------------------------------------------
PRIME COUNTING BENCHMARK: SEQUENTIAL, OPENMP, AND PERFORMANCE ANALYSIS
-------------------------------------------------------------------------------

This directory contains a C implementation of a compute-bound prime counting 
benchmark and an automation script for performance profiling.

FILES:
- primeCounting.c : Combined sequential and OpenMP implementation for counting primes.
- run_tests.sh    : Bash script to automate multiple runs, vary input sizes and 
                    thread counts, and compute average runtime and speedup.

-------------------------------------------------------------------------------
1. ENVIRONMENT VARIABLES
-------------------------------------------------------------------------------

To ensure reproducible results and controlled parallel execution, the following 
environment variable is used:

* OMP_NUM_THREADS : Sets the number of parallel threads to use.

Example setup:
export OMP_NUM_THREADS=8

Note:
Thread count is also controlled directly within the automation script using:
OMP_NUM_THREADS=<threads> ./primeCounting ...

-------------------------------------------------------------------------------
2. COMPILATION
-------------------------------------------------------------------------------

Compile using GCC with optimization and OpenMP support:

gcc -O3 -fopenmp primeCounting.c -lm -o primeCounting

-------------------------------------------------------------------------------
3. RUNNING INDIVIDUAL FILES
-------------------------------------------------------------------------------

After compilation, you can run the program manually. The program requires 
two arguments:

1. N     : Upper bound for prime search
2. mode  : Execution mode (0 = sequential, 1 = OpenMP)

RUNNING THE SEQUENTIAL VERSION:
./primeCounting 10000000 0
./primeCounting 5000000 0

RUNNING THE OPENMP VERSION:
# Set threads before running
export OMP_NUM_THREADS=4
./primeCounting 10000000 1
./primeCounting 20000000 1

-------------------------------------------------------------------------------
4. AUTOMATED PROFILING (run_tests.sh)
-------------------------------------------------------------------------------

The provided script 'run_tests.sh' automates the benchmarking process across 
multiple input sizes and thread counts.

USAGE:
chmod +x run_tests.sh
./run_tests.sh

The script performs the following:
* Compiles the program using:
  gcc -O3 -fopenmp primeCounting.c -lm -o primeCounting
* Runs each configuration 5 times.
* Computes the average runtime for each run.
* Uses the sequential version as the baseline for speedup calculation.
* Evaluates performance for input sizes:
  (100,000, 1,000,000, 10,000,000)
* Tests thread counts:
  (1, 2, 4, 8, 16, 32, 64)

OUTPUT:
* Printed tables showing:
  - Threads
  - Input size (N)
  - Average execution time
  - Speedup relative to sequential execution

-------------------------------------------------------------------------------
5. HARDWARE COUNTER PERMISSIONS
-------------------------------------------------------------------------------

If hardware performance counters are unavailable when using 'perf', your system 
may restrict access to these events. You can temporarily lower the restriction 
(requires sudo):

sudo sysctl -w kernel.perf_event_paranoid=0

Example profiling command:
OMP_NUM_THREADS=8 perf stat -r 5 -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
./primeCounting 10000000 1

-------------------------------------------------------------------------------