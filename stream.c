#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE 100000000UL
#endif

#ifndef NTIMES
#define NTIMES 10
#endif

int main(int argc, char *argv[]) {
    size_t N = ARRAY_SIZE;
    double scalar = 3.0;

    if (argc > 1) {
        N = strtoull(argv[1], NULL, 10);
    }

    double *a = (double *) malloc(N * sizeof(double));
    double *b = (double *) malloc(N * sizeof(double));
    double *c = (double *) malloc(N * sizeof(double));

    if (!a || !b || !c) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    double start, end;
    double copy_time = 0.0, scale_time = 0.0, add_time = 0.0, triad_time = 0.0;

    // initialization
    #pragma omp parallel for default(none) shared(a, b, c, N) schedule(static)
    for (size_t i = 0; i < N; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
        c[i] = 0.0;
    }

    for (int k = 0; k < NTIMES; k++) {
        start = omp_get_wtime();
        #pragma omp parallel for default(none) shared(a, c, N) schedule(static)
        for (size_t i = 0; i < N; i++) {
            c[i] = a[i];                
        }
        end = omp_get_wtime();
        copy_time += (end - start);

        start = omp_get_wtime();
        #pragma omp parallel for default(none) shared(b, c, scalar, N) schedule(static)
        for (size_t i = 0; i < N; i++) {
            b[i] = scalar * c[i];      
        }
        end = omp_get_wtime();
        scale_time += (end - start);

        start = omp_get_wtime();
        #pragma omp parallel for default(none) shared(a, b, c, N) schedule(static)
        for (size_t i = 0; i < N; i++) {
            c[i] = a[i] + b[i];         
        }
        end = omp_get_wtime();
        add_time += (end - start);

        start = omp_get_wtime();
        #pragma omp parallel for default(none) shared(a, b, c, scalar, N) schedule(static)
        for (size_t i = 0; i < N; i++) {
            a[i] = b[i] + scalar * c[i]; 
        }
        end = omp_get_wtime();
        triad_time += (end - start);
    }

    double checksum = 0.0;
    #pragma omp parallel for default(none) shared(a, N) reduction(+:checksum) schedule(static)
    for (size_t i = 0; i < N; i++) {
        checksum += a[i];
    }

    printf("Array size: %zu\n", N);
    printf("Threads used: %d\n", omp_get_max_threads());
    printf("Average Copy time : %f sec\n", copy_time / NTIMES);
    printf("Average Scale time: %f sec\n", scale_time / NTIMES);
    printf("Average Add time  : %f sec\n", add_time / NTIMES);
    printf("Average Triad time: %f sec\n", triad_time / NTIMES);
    printf("Checksum: %f\n", checksum);

    free(a);
    free(b);
    free(c);
    return 0;
}
