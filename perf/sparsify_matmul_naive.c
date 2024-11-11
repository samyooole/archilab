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
    if (matrix == NULL) return;  // Add NULL check
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
    
    printf("Performing naive multiplication...\n");

    
    for (int i = 0; i < 10; i++) {
        C = matmul(A, B, A_rows, A_cols, B_rows, B_cols);
        if (C != NULL) { 
            free_matrix(C, A_rows);
        }
        C = NULL;  
    }
    
    // Cleanup
    free_matrix(A, A_rows);
    free_matrix(B, B_rows);

    
    printf("Naive multiplications completed.\n");
    return 0;
}