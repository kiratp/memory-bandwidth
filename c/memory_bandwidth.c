#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int NUM_THREADS = 8; // Set the number of threads to run in parallel
const size_t BUFFER_SIZE = 100000000; // Set the buffer size
const int NUM_ITERATIONS = 10;        // Set the number of iterations

typedef struct {
    double *buffer;
    int id;
} ThreadData;

void *measure_bandwidth(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int i;
    double dummy = 0;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        int j;
        for (j = 0; j < BUFFER_SIZE; j++) {
            dummy += data->buffer[j];
        }
    }

    printf("Thread %d completed. Accumulated value: %f\n", data->id, dummy);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    double *buffers[NUM_THREADS];
    int i;

    srand(time(NULL)); // Seed the random number generator

    for (i = 0; i < NUM_THREADS; i++) {
        buffers[i] = (double *)malloc(BUFFER_SIZE * sizeof(double));
        int j;
        for (j = 0; j < BUFFER_SIZE; j++) {
            buffers[i][j] =
                (double)rand() /
                RAND_MAX; // Initialize buffer elements with random values
        }
        thread_data[i].buffer = buffers[i];
        thread_data[i].id = i;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, measure_bandwidth, &thread_data[i]);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Elapsed time: %f seconds\n", elapsed_time);

    double bandwidth = (double)NUM_THREADS * BUFFER_SIZE * NUM_ITERATIONS *
                       sizeof(double) / elapsed_time;
    printf("Memory bandwidth: %f GB/s\n", bandwidth / 1e9);

    for (i = 0; i < NUM_THREADS; i++) {
        free(buffers[i]);
    }

    return 0;
}
