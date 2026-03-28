//https://learn.microsoft.com/en-us/cpp/parallel/openmp/reference/openmp-directives?view=msvc-170&utm_source=chatgpt.com
//https://www.intel.com/content/www/us/en/docs/advisor/user-guide/2024-1/openmp-reduction-operations.html?utm_source=chatgpt.com

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse(const char *s) {
    if (strcmp(s, "atomic") == 0) return 0;
    if (strcmp(s, "critical") == 0) return 1;
    if (strcmp(s, "reduction") == 0) return 2;

    fprintf(stderr, "Invalid mode: %s\n", s);
    exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[]) {
    if (argc != 4) {
        printf("Invalid input\n");
        return 1;
    }

    long long iterations = atoll(argv[1]);
    int mode = parse(argv[2]);
    int threads = atoi(argv[3]);

    if (iterations <= 0 || threads <= 0) {
        fprintf(stderr, "iterations and threads must be positive.\n");
        return EXIT_FAILURE;
    }

    omp_set_num_threads(threads);

    long long counter = 0;
    double start = omp_get_wtime();

    if (mode == 2) {
        long long sum = 0;

        #pragma omp parallel for reduction(+:sum) schedule(static)
        for (long long i = 0; i < iterations; i++) {
            sum += 1;
        }

        counter = sum;
    } else if (mode == 0) {
        #pragma omp parallel
        {
            #pragma omp for schedule(static)
            for (long long i = 0; i < iterations; i++) {
                #pragma omp atomic
                counter += 1;
            }
        }
    } else {
        #pragma omp parallel
        {
            #pragma omp for schedule(static)
            for (long long i = 0; i < iterations; i++) {
                #pragma omp critical
                {
                    counter += 1;
                }
            }
        }
    }

    double end = omp_get_wtime();

    printf("mode=%s\n", argv[2]);
    printf("threads=%d\n", threads);
    printf("iterations=%lld\n", iterations);
    printf("result=%lld\n", counter);
    printf("time=%f\n", end - start);

    return 0;
}