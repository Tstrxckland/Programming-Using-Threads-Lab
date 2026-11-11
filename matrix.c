#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20
#define NUM_THREADS 10

int matA[MAX][MAX];
int matB[MAX][MAX];

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

int matrixSize = MAX;

// Fill a matrix with random values.
void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Print a matrix.
void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Parallel matrix addition.
void* computeSum(void* args) {
    int threadID = *(int*)args;

    int start = threadID * matrixSize / NUM_THREADS;
    int end = (threadID + 1) * matrixSize / NUM_THREADS;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }

    return NULL;
}

// Parallel matrix subtraction.
void* computeDiff(void* args) {
    int threadID = *(int*)args;

    int start = threadID * matrixSize / NUM_THREADS;
    int end = (threadID + 1) * matrixSize / NUM_THREADS;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }

    return NULL;
}

// Parallel matrix multiplication (dot product).
void* computeProduct(void* args) {
    int threadID = *(int*)args;

    int start = threadID * matrixSize / NUM_THREADS;
    int end = (threadID + 1) * matrixSize / NUM_THREADS;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < matrixSize; j++) {
            int sum = 0;

            for (int k = 0; k < matrixSize; k++) {
                sum += matA[i][k] * matB[k][j];
            }

            matProductResult[i][j] = sum;
        }
    }

    return NULL;
}

// Create and join 10 threads for one operation.
int runThreads(void* (*function)(void*)) {
    pthread_t threads[NUM_THREADS];
    int threadIDs[NUM_THREADS];
    int created = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        threadIDs[i] = i;

        if (pthread_create(&threads[i], NULL,
                           function, &threadIDs[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            break;
        }
        created++;
    }

    int success = (created == NUM_THREADS);

    for (int i = 0; i < created; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            success = 0;
        }
    }

    return success;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    // 0. Get matrix size from command line.
    if (argc > 1) {
        char* endptr;
        long requested = strtol(argv[1], &endptr, 10);

        if (*endptr != '\0' || requested < 1 || requested > MAX) {
            fprintf(stderr, "Matrix size must be 1 to 20.\n");
            return 1;
        }

        matrixSize = (int)requested;
    }

    // 1. Fill both matrices.
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);

    printf("Matrix B:\n");
    printMatrix(matB);

    // 3-5. Create, run, and join threads.
    if (!runThreads(computeSum)) {
        return 1;
    }

    if (!runThreads(computeDiff)) {
        return 1;
    }

    if (!runThreads(computeProduct)) {
        return 1;
    }

    // 6. Print results.
    printf("Results:\n");

    printf("Sum:\n");
    printMatrix(matSumResult);

    printf("Difference:\n");
    printMatrix(matDiffResult);

    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}
