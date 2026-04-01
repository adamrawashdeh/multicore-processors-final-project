//https://learn.microsoft.com/en-us/cpp/parallel/openmp/reference/openmp-directives?view=msvc-170&utm_source=chatgpt.com
//https://www.intel.com/content/www/us/en/docs/advisor/user-guide/2024-1/openmp-reduction-operations.html?utm_source=chatgpt.com

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, const char *argv[]) {
    if (argc != 4) {
        printf("Invalid input\n");
        return 1;
    }

    long long iterations = atoll(argv[1]);
    const char* mode;

    if (strcmp(argv[2], "atomic") == 0) {
        mode = "atomic";
    }
    else if (strcmp(argv[2], "critical") == 0) {
        mode = "critical";
    }
    else if (strcmp(argv[2], "reduction") == 0) {
        mode = "reduction";
    }
    else {
        fprintf(stderr, "Invalid mode");
        return EXIT_FAILURE;
    }

    int threads = atoi(argv[3]);

    if (iterations <= 0 || threads <= 0) {
        fprintf(stderr, "iterations and threads must be positive.\n");
        return EXIT_FAILURE;
    }

    omp_set_num_threads(threads);

    long long counter = 0;
    double start = omp_get_wtime();

    if (strcmp(mode, "reduction") == 0) {
        long long sum = 0;

        #pragma omp parallel for reduction(+:sum) schedule(static)
        for (long long i = 0; i < iterations; i++) {
            sum += 1;
        }

        counter = sum;
    } else if (strcmp(mode, "atomic") == 0) {
        #pragma omp parallel for schedule(static) 
        for (long long i = 0; i < iterations; i++) {
            #pragma omp atomic
            counter += 1;
        }
    } else {
        #pragma omp parallel for schedule(static)  
            for (long long i = 0; i < iterations; i++) {
                #pragma omp critical
                {
                    counter += 1;
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