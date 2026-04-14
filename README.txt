

-------------------------------------------------------------------------------
STREAM BENCHMARK: SEQUENTIAL, OPENMP, AND PERFORMANCE ANALYSIS
-------------------------------------------------------------------------------

This directory contains C implementations of the STREAM benchmark and a 
comprehensive automation script for performance profiling.

FILES:
- stream_seq.c    : Single-threaded sequential implementation.
- stream_omp.c    : Multi-threaded OpenMP implementation.
- pref_run_all.sh : Bash script to automate multiple runs, vary thread counts, 
                    and collect hardware performance counters via 'perf'.

-------------------------------------------------------------------------------
1. ENVIRONMENT VARIABLES
-------------------------------------------------------------------------------

To ensure reproducible results and optimal performance, the following 
environment variables are recommended (and used by the automation script):

* OMP_NUM_THREADS : Sets the number of parallel threads to use.
* OMP_PLACES      : Specifies where threads should be placed (e.g., 'cores').
* OMP_PROC_BIND   : Sets the thread affinity policy (e.g., 'close').

Example setup:
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close

-------------------------------------------------------------------------------
2. COMPILATION
-------------------------------------------------------------------------------

Compile using GCC with optimization and OpenMP support:

Sequential:
gcc -O3 -std=c11 -march=native -fopenmp stream_seq.c -o stream_seq -lm

OpenMP:
gcc -O3 -std=c11 -march=native -fopenmp stream_omp.c -o stream_omp -lm

-------------------------------------------------------------------------------
3. RUNNING INDIVIDUAL FILES
-------------------------------------------------------------------------------

After compilation, you can run the binaries manually. Both programs accept 
an optional argument to specify the array size (N).

RUNNING THE SEQUENTIAL VERSION:
./stream_seq               (Uses default size: 100,000,000)
./stream_seq 50000000      (Uses custom size: 50,000,000)

RUNNING THE OPENMP VERSION:
# Set threads before running
export OMP_NUM_THREADS=4
./stream_omp               (Uses default size: 100,000,000)
./stream_omp 200000000     (Uses custom size: 200,000,000)

-------------------------------------------------------------------------------
4. AUTOMATED PROFILING (pref_run_all.sh)
-------------------------------------------------------------------------------

The provided script 'pref_run_all.sh' automates the benchmarking process across 
multiple array sizes and thread counts.

USAGE:
chmod +x pref_run_all.sh
./pref_run_all.sh

OUTPUT DIRECTORY (results_stream_partial/):
* timings.csv : Table of execution times for each kernel.
* perf.csv    : Hardware performance counter data.
* perf_*.txt  : Raw output from the 'perf' command.

-------------------------------------------------------------------------------
5. HARDWARE COUNTER PERMISSIONS
-------------------------------------------------------------------------------

If the script reports "UNAVAILABLE" for performance counters, your system 
may have restricted access to hardware events. You can temporarily 
lower the restriction (requires sudo):

sudo sysctl -w kernel.perf_event_paranoid=0

The script targets user-space events (:u) to work under standard security 
settings.

-------------------------------------------------------------------------------
