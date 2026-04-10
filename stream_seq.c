#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE 100000000UL
#endif

#ifndef NTIMES
#define NTIMES 10
#endif

#ifndef ALIGNMENT
#define ALIGNMENT 64
#endif

static void *aligned_malloc_or_die(size_t alignment, size_t size) {
    void *ptr = NULL;
    int rc = posix_memalign(&ptr, alignment, size);
    if (rc != 0) {
        fprintf(stderr, "posix_memalign failed: %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }
    return ptr;
}

int main(int argc, char *argv[]) {
    size_t N = ARRAY_SIZE;
    const double scalar = 3.0;

    if (argc > 1) {
        char *endptr = NULL;
        errno = 0;
        unsigned long long tmp = strtoull(argv[1], &endptr, 10);
        if (errno != 0 || endptr == argv[1] || *endptr != '\0' || tmp == 0) {
            fprintf(stderr, "Invalid array size: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
        N = (size_t) tmp;
    }

    const size_t bytes_per_array = N * sizeof(double);

    double *a = (double *) aligned_malloc_or_die(ALIGNMENT, bytes_per_array);
    double *b = (double *) aligned_malloc_or_die(ALIGNMENT, bytes_per_array);
    double *c = (double *) aligned_malloc_or_die(ALIGNMENT, bytes_per_array);

    double copy_sum = 0.0, scale_sum = 0.0, add_sum = 0.0, triad_sum = 0.0;

    for (size_t i = 0; i < N; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
        c[i] = 0.0;
    }

    for (int k = 0; k < NTIMES + 1; k++) {
        double start, end, t;

        start = omp_get_wtime();
        for (size_t i = 0; i < N; i++) {
            c[i] = a[i];
        }
        end = omp_get_wtime();
        t = end - start;
        if (k > 0) copy_sum += t;

        start = omp_get_wtime();
        for (size_t i = 0; i < N; i++) {
            b[i] = scalar * c[i];
        }
        end = omp_get_wtime();
        t = end - start;
        if (k > 0) scale_sum += t;

        start = omp_get_wtime();
        for (size_t i = 0; i < N; i++) {
            c[i] = a[i] + b[i];
        }
        end = omp_get_wtime();
        t = end - start;
        if (k > 0) add_sum += t;

        start = omp_get_wtime();
        for (size_t i = 0; i < N; i++) {
            a[i] = b[i] + scalar * c[i];
        }
        end = omp_get_wtime();
        t = end - start;
        if (k > 0) triad_sum += t;
    }

    double checksum = 0.0;
    for (size_t i = 0; i < N; i++) {
        checksum += a[i];
    }

    printf("Array size: %zu\n", N);
    printf("Threads used: 1\n");
    printf("Average Copy time : %f sec\n", copy_sum / NTIMES);
    printf("Average Scale time: %f sec\n", scale_sum / NTIMES);
    printf("Average Add time  : %f sec\n", add_sum / NTIMES);
    printf("Average Triad time: %f sec\n", triad_sum / NTIMES);
    printf("Checksum: %f\n", checksum);

    free(a);
    free(b);
    free(c);
    return 0;
}
