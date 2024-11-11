#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_ops.h"

float **generate_random_matrix(int rows, int cols) {
    float **matrix = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float *)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++) {
            // 50% chance to assign 0, otherwise assign a random value between 0 and 1 - KEY DIFFERENCE, WE ARE GENERATING SPARSE MATRICES
            if (rand() % 2 == 0) {
                matrix[i][j] = 0.0f;
            } else {
                matrix[i][j] = (float)rand() / RAND_MAX;
            }
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
    int A_rows = 5, A_cols = 5, B_rows = 5, B_cols = 5;
    
    if (A_cols != B_rows) {
        printf("Matrix dimensions incompatible for multiplication.\n");
        return 1;
    }
    
    float **A = generate_random_matrix(A_rows, A_cols);
    float **B = generate_random_matrix(B_rows, B_cols);
    float **C = NULL;
    

    float **A_CSR = csr_alloc(A, A_rows, A_cols);
    float **B_CSR = csr_alloc(B, B_rows, B_cols);
    
    printf("Performing sparse multiplication...\n");
    for (int i = 0; i < 10000; i++) {
        C = matmul_sparse_step2(A_CSR, B_CSR, A_rows, A_cols, B_rows, B_cols);
        if (C != NULL) {  
            free_matrix(C, A_rows);
        }
        C = NULL;  
    }
    
    // Cleanup
    free_matrix(A, A_rows);
    free_matrix(B, B_rows);
    
    // Free CSR matrices if you're using them
    if (A_CSR != NULL) {
        free(A_CSR[0]); 
        free(A_CSR[1]); 
        free(A_CSR[2]); 
        free(A_CSR);
    }
    
    if (B_CSR != NULL) {
        free(B_CSR[0]); 
        free(B_CSR[1]); 
        free(B_CSR[2]); 
        free(B_CSR);
    }
    
    printf("Sparse multiplications completed.\n");
    return 0;
}