#include "matrix_ops.h"
#include <pthread.h>
#include <string.h>

float **matmul(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols) 
{
    // check if matrix dims compatible with matmul
    if (A_cols != B_rows) {
        fprintf(stderr, "Matrix dimensions not compatible for matmul\n");
        return NULL;
    }

    // alloc memory: output matrix C
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)malloc(B_cols * sizeof(float));
    }

    
    for (int a_row = 0; a_row < A_rows; a_row++) {
        for (int b_col = 0; b_col < B_cols; b_col++) {
            C[a_row][b_col] = 0; 
            for (int a_col = 0; a_col < A_cols; a_col++) {
                C[a_row][b_col] += A[a_row][a_col] * B[a_col][b_col];
            }
        }
    }

    return C;
}

// free matrix memory
/*
void free_matrix(float **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
*/




typedef struct {
    // helper struct holds references to the relevant input and output matrices
    float **A;
    float **B;
    float **C;
    int A_rows;
    int A_cols;
    int B_cols;
    int start_row;
    int end_row;
    char padding[64]; //padding to avoid false sharing
} thread_data_t;

void *matmul_worker(void *arg) {
    // pass in a pointer that will reference the special thread_data_t struct we defined
    thread_data_t *data = (thread_data_t *)arg;
    
    // transpose B: row major, better cache locality
    float *B_transposed = malloc(data->A_cols * data->B_cols * sizeof(float));
    for (int j = 0; j < data->B_cols; j++) {
        for (int k = 0; k < data->A_cols; k++) {
            B_transposed[j * data->A_cols + k] = data->B[k][j];
        }
    }

    // we are doing the actual matrix multiplication (naive algorithm) on a subset of the full matrices here

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->B_cols; j++) {
            float sum = 0.0f;
            // use transposed matrix
            for (int k = 0; k < data->A_cols; k++) {
                sum += data->A[i][k] * B_transposed[j * data->A_cols + k];
            }
            data->C[i][j] = sum;
        }
    }

    // free matrix, exit pthread
    free(B_transposed);
    pthread_exit(NULL);
}

float **matmul_thread(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols) {

    // incompatible dimension sanity check
    if (A_cols != B_rows) {
        fprintf(stderr, "Matrix dimensions not compatible for matmul\n");
        return NULL;
    }

    // alloc output matrix with aligned memory
    // the principle behind this is that we want to fetch data in alignment with the cache line
    // checking the ecelinux server:
    // `getconf LEVEL1_DCACHE_LINESIZE`
    // reveals that the L1 cache line size is indeed 64 bytes
    float **C = (float **)aligned_alloc(64, A_rows * sizeof(float *)); 
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)aligned_alloc(64, B_cols * sizeof(float));
        memset(C[i], 0, B_cols * sizeof(float));
    }


    int num_threads = 8; // since we are automatically allocated 8 cores, we shall run 8 threads
    pthread_t threads[num_threads]; // declare an array of pthread_t objects, one for each thread
    // this is a less verbose way of writing 
    // pthread_t *threads;
    // threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    thread_data_t thread_data[num_threads]; // declare an array of thread_data_t objects - to hold data pointers

    int rows_per_thread = A_rows / num_threads;
    int remaining_rows = A_rows % num_threads; // handle edge rows

    for (int i = 0; i < num_threads; i++) {
        // populate data pointer array
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C;
        thread_data[i].A_rows = A_rows;
        thread_data[i].A_cols = A_cols;
        thread_data[i].B_cols = B_cols;
        
        // distribute rows evenly
        thread_data[i].start_row = i * rows_per_thread + 
            (i < remaining_rows ? i : remaining_rows);
        thread_data[i].end_row = thread_data[i].start_row + rows_per_thread + 
            (i < remaining_rows ? 1 : 0);


        // KEY: create threads
        pthread_create(&threads[i], NULL, matmul_worker, (void *)&thread_data[i]);
    }

    // sync threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return C;
}