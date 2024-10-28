#include "matrix_ops.h"

#ifndef BLOCK_SIZE_I
#define BLOCK_SIZE_I 169
#endif

#ifndef BLOCK_SIZE_J
#define BLOCK_SIZE_J 32
#endif

#ifndef BLOCK_SIZE_K
#define BLOCK_SIZE_K 9
#endif

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
void free_matrix(float **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


// Matmul with blocking optimization
float **matmul_blocking(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols)
{
    // TODO: Implement matrix multiplication with blocking (loop tiling)
}