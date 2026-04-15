#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Usage: %s <N> <mode>\n", argv[0]);
        printf("mode: 0 = sequential, 1 = OpenMP\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int mode = atoi(argv[2]);

    long long totalPrimes = 0;

    double start = omp_get_wtime();

    if (mode == 0) {
        for (int i = 2; i <= N; i++) {
            int is_prime = 1;

            if (i == 2) {
                is_prime = 1;
            } else if (i % 2 == 0) {
                is_prime = 0;
            } else {
                int potentialPrime = (int)sqrt((double)i);
                for (int odds = 3; odds <= potentialPrime; odds += 2) {
                    if (i % odds == 0) {
                        is_prime = 0;
                        break;
                    }
                }
            }

            totalPrimes += is_prime;
        }
    } else {
        #pragma omp parallel for reduction(+:totalPrimes)
        for (int i = 2; i <= N; i++) {
            int is_prime = 1;

            if (i == 2) {
                is_prime = 1;
            } else if (i % 2 == 0) {
                is_prime = 0;
            } else {
                int potentialPrime = (int)sqrt((double)i);
                for (int odds = 3; odds <= potentialPrime; odds += 2) {
                    if (i % odds == 0) {
                        is_prime = 0;
                        break;
                    }
                }
            }

            totalPrimes += is_prime;
        }
    }

    double end = omp_get_wtime();

    if (mode == 0)
        printf("SEQ | N = %d | Primes = %lld | Time = %f seconds\n", N, totalPrimes, end - start);
    else
        printf("OMP | N = %d | Primes = %lld | Time = %f seconds\n", N, totalPrimes, end - start);

    return 0;
}