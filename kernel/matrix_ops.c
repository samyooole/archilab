#include "matrix_ops.h"


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


float **matmul_sparse(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols)
{
    // check if matrix dims compatible with matmul
    if (A_cols != B_rows) {
        fprintf(stderr, "Matrix dimensions not compatible for matmul\n");
        return NULL;
    }


    // Step 1: Create CSR representations of the input matrices
    float **A_CSR = csr_alloc(A, A_rows, A_cols); 
    float **B_CSR = csr_alloc(B, B_rows, B_cols);

    // alloc output matrix C's memory
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)calloc(B_cols, sizeof(float));  // Initialize to 0
    }

    // Step 2: Perform sparse matrix multiplication
    for (int i = 0; i < A_rows; i++) {
        int row_start = A_CSR[1][i];
        int row_end = A_CSR[1][i + 1];

        for (int j = row_start; j < row_end; j++) {
            float A_value = A_CSR[0][j];
            int A_col = A_CSR[2][j];

            // multiply with the non-zero elements in the B matrix row corresponding to A_col
            int B_row_start = B_CSR[1][A_col];
            int B_row_end = B_CSR[1][A_col + 1];

            for (int k = B_row_start; k < B_row_end; k++) {
                float B_value = B_CSR[0][k];
                int B_col = B_CSR[2][k];

                // accumulate the product in the C matrix
                C[i][B_col] += A_value * B_value;
            }
        }
    }

    // free csr matrices
    free(A_CSR[0]); free(A_CSR[1]); free(A_CSR[2]); free(A_CSR);
    free(B_CSR[0]); free(B_CSR[1]); free(B_CSR[2]); free(B_CSR);

    return C;
}


float **csr_alloc(float **A, int A_rows, int A_cols)
{
    // allocate memory gradually - we first begin with extracting all the non-zero values

    // take the dynamic array approach
    int initial_size = 10;
    int current_size = initial_size;
    int nonzerovalue_count = 0;

    // allocate initial memory

    float **CSR_array = (float **)malloc(3 * sizeof(float *)); // we have 3 rows
    if (!CSR_array) return NULL; // graceful exception handling


    CSR_array[0] = (float *)malloc(current_size * sizeof(float));
    CSR_array[2] = (float *)malloc(current_size * sizeof(float));
    CSR_array[1] = (float *)malloc( (A_rows + 1) * sizeof(float));

    // graceful exception handling
    if (!CSR_array[0] || !CSR_array[1] || !CSR_array[2]) {
        free(CSR_array[0]);
        free(CSR_array[1]);
        free(CSR_array[2]);
        free(CSR_array);
        return NULL;
    }

    for (int a_row = 0; a_row < A_rows; a_row++) {

        // after each row is done, get cumulative # of non-zero values and put in CSR_array[1]
        CSR_array[1][a_row] = nonzerovalue_count;
        for (int a_col = 0; a_col < A_cols; a_col++) {
            if (A[a_row][a_col] != 0) {
                // if the current count exceeds the allocated size, reallocate with more space
                if (nonzerovalue_count >= current_size) {
                    current_size *= 2; // Double the size
                    

                    float *new_values = (float *)realloc(CSR_array[0], current_size * sizeof(float));
                    float *new_columns = (float *)realloc(CSR_array[2], current_size * sizeof(float));
                    
                    if (!new_values || !new_columns) {
                        free(CSR_array[0]);
                        free(CSR_array[1]);
                        free(CSR_array[2]);
                        free(CSR_array);
                        return NULL;
                    }
                    CSR_array[0] = new_values;
                    CSR_array[2] = new_columns;

                }
                // append non-zero element to array_one
                
                CSR_array[0][nonzerovalue_count] = A[a_row][a_col];
                CSR_array[2][nonzerovalue_count] = a_col;
                nonzerovalue_count++;
            }
        }

        CSR_array[1][a_row+1] = nonzerovalue_count; // final entry for the row pointer array


    }

    // trim array to the exact size needed - necessary?
    /*
    CSR_array[0] = (float *)realloc(CSR_array[0], nonzerovalue_count * sizeof(float));
    CSR_array[1] = (float *)realloc(CSR_array[1], nonzerovalue_count * sizeof(float));
    CSR_array[2] = (float *)realloc(CSR_array[0], nonzerovalue_count * sizeof(float));
    */
    

    return CSR_array;
}
