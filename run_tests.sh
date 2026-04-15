#!/bin/bash

gcc -O3 -fopenmp primeCounting.c -lm -o primeCounting

for N in 100000 1000000 10000000 100000000; do
  echo "===== N = $N ====="

  for t in 1 2 4 8 16 32 64; do
    echo "Threads = $t"
    OMP_NUM_THREADS=$t ./primeCounting $N
  done

  echo
done