#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_ops.h"

float **generate_random_matrix(int rows, int cols) {
    float **matrix = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float *)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (float)rand() / RAND_MAX;
        }
    }
    return matrix;
}

void free_matrix(float **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    srand(time(NULL));
    int A_rows = 1024, A_cols = 1024, B_rows = 1024, B_cols = 1024;

    if (A_cols != B_rows) {
        printf("Matrix dimensions incompatible for multiplication.\n");
        return 1;
    }

    float **A = generate_random_matrix(A_rows, A_cols);
    float **B = generate_random_matrix(B_rows, B_cols);
    float **C = NULL;

    // Step 1: Create CSR representations of the input matrices (separately)
    float **A_CSR = csr_alloc(A, A_rows, A_cols); 
    float **B_CSR = csr_alloc(B, B_rows, B_cols);

    printf("Performing sparse multiplication...\n");
    

    for (int i = 0; i < 5; i++){

        float **C = matmul_sparse_step2(A, B, A_rows, A_cols, B_rows, B_cols);
        free_matrix(C, A_rows);
        
    }
    // Cleanup
    free_matrix(A, A_rows);
    free_matrix(B, B_rows);
    free_matrix(C, A_rows);

    printf("Naive multiplications completed.\n");

    return 0;
}
