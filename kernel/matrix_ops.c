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

    // reference: http://alvinwan.com/how-to-tile-matrix-multiplication/


    /*
    We are given two matrices, A, and B

    A: m * k (A_rows * A_cols)
    B: k * n (B_rows * B_cols)

    Which produce an output matrix

    C: m * n (A_rows * B_cols)

    Tiling means determining some

    m_t < m
    n_t < n

    Such that we use A(m_t * k) x B(k * n_t) in order to produce the output submatrix C(m_t * n_t)

    With the goal of minimizing fetches
    
    */

   if (A_cols != B_rows) {
        fprintf(stderr, "Matrix dimensions not compatible for matmul\n");
        return NULL;
    }

    // allocate output matrix C
    float **C = (float **)malloc(A_rows * sizeof(float *));
    for (int i = 0; i < A_rows; i++) {
        C[i] = (float *)malloc(B_cols * sizeof(float));  // Initialize to 0
    }


    int M_T = A_rows/2;
    int N_T = B_cols/2;
    int K = A_cols;


    // Handle the case where dimensions are odd
    int row_remainder = A_rows % M_T;
    int col_remainder = B_cols % N_T;

    // allocate local tiles
    float *tile_C = (float *)malloc(M_T * N_T * sizeof(float));

    // Main tiled multiplication
    for (int mm = 0; mm < A_rows / M_T; mm++) {
        for (int nn = 0; nn < B_cols / N_T; nn++) {
            // Clear tile_C for accumulation
            memset(tile_C, 0, M_T * N_T * sizeof(float));
            
            // Calculate tile boundaries
            int row_start = mm * M_T;
            int row_end = (mm + 1) * M_T;
            int col_start = nn * N_T;
            int col_end = (nn + 1) * N_T;
            
            // Perform multiplication for current tile
            for (int i = row_start; i < row_end; i++) {
                for (int j = col_start; j < col_end; j++) {
                    float sum = 0.0f;
                    for (int k = 0; k < K; k++) {
                        sum += A[i][k] * B[k][j];
                    }
                    tile_C[(i - row_start) * N_T + (j - col_start)] = sum;
                }
            }
            
            // Copy tile_C back to C
            for (int i = 0; i < M_T; i++) {
                for (int j = 0; j < N_T; j++) {
                    C[row_start + i][col_start + j] = tile_C[i * N_T + j];
                }
            }
        }
    }

    // Handle row remainder if it exists
    if (row_remainder > 0) {
        int row_start = A_rows - row_remainder;
        
        // Allocate tile for row remainder
        float *tile_row = (float *)malloc(row_remainder * N_T * sizeof(float));
        
        for (int nn = 0; nn < B_cols / N_T; nn++) {
            memset(tile_row, 0, row_remainder * N_T * sizeof(float));
            
            int col_start = nn * N_T;
            int col_end = (nn + 1) * N_T;
            
            // Compute remainder rows
            for (int i = row_start; i < A_rows; i++) {
                for (int j = col_start; j < col_end; j++) {
                    float sum = 0.0f;
                    for (int k = 0; k < K; k++) {
                        sum += A[i][k] * B[k][j];
                    }
                    tile_row[(i - row_start) * N_T + (j - col_start)] = sum;
                }
            }
            
            // Copy results back
            for (int i = 0; i < row_remainder; i++) {
                for (int j = 0; j < N_T; j++) {
                    C[row_start + i][col_start + j] = tile_row[i * N_T + j];
                }
            }
        }
        
        free(tile_row);
    }

    // Handle column remainder if it exists
    if (col_remainder > 0) {
        int col_start = B_cols - col_remainder;
        
        // Allocate tile for column remainder
        float *tile_col = (float *)malloc(M_T * col_remainder * sizeof(float));
        
        for (int mm = 0; mm < A_rows / M_T; mm++) {
            memset(tile_col, 0, M_T * col_remainder * sizeof(float));
            
            int row_start = mm * M_T;
            int row_end = (mm + 1) * M_T;
            
            // Compute remainder columns
            for (int i = row_start; i < row_end; i++) {
                for (int j = col_start; j < B_cols; j++) {
                    float sum = 0.0f;
                    for (int k = 0; k < K; k++) {
                        sum += A[i][k] * B[k][j];
                    }
                    tile_col[(i - row_start) * col_remainder + (j - col_start)] = sum;
                }
            }
            
            // Copy results back
            for (int i = 0; i < M_T; i++) {
                for (int j = 0; j < col_remainder; j++) {
                    C[row_start + i][col_start + j] = tile_col[i * col_remainder + j];
                }
            }
        }
        
        free(tile_col);
    }

    // Handle corner case (if both remainders exist)
    if (row_remainder > 0 && col_remainder > 0) {
        int row_start = A_rows - row_remainder;
        int col_start = B_cols - col_remainder;
        
        // Compute corner directly
        for (int i = row_start; i < A_rows; i++) {
            for (int j = col_start; j < B_cols; j++) {
                float sum = 0.0f;
                for (int k = 0; k < K; k++) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

    free(tile_C);
    return C;


}