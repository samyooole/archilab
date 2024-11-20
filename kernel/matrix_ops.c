#include "matrix_ops.h"
#include <pthread.h>


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
    float **A;
    float **B;
    float **C;
    int A_rows;
    int A_cols;
    int B_cols;
    int start_row;
    int end_row;
} thread_data_t;

void *matmul_worker(void *arg) {
    // This worker performs the standard matrix multiplication

    // Passing data in as a pointer
    thread_data_t *data = (thread_data_t *)arg;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->B_cols; j++) {
            data->C[i][j] = 0;
            for (int k = 0; k < data->A_cols; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

float **matmul_thread(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols) {
    if (A_cols != B_rows) {
        fprintf(stderr, "Matrix dimensions not compatible for matmul\n");
        return NULL;
    }


    // alloc output matrix
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)malloc(B_cols * sizeof(float));
    }

    // spin up threads
    int num_threads = 4; // change # of threads here
    pthread_t threads[num_threads];
    thread_data_t thread_data[num_threads];

    int rows_per_thread = A_rows / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C;
        thread_data[i].A_rows = A_rows;
        thread_data[i].A_cols = A_cols;
        thread_data[i].B_cols = B_cols;
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == num_threads - 1) ? A_rows : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, matmul_worker, (void *)&thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return C;
}