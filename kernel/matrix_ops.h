#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H

#include <stdio.h>
#include <stdlib.h>

float **matmul(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols);
float **matmul_sparse(float **A, float **B, int A_rows, int A_cols, int B_rows, int B_cols);
float **csr_alloc(float **A, int A_rows, int A_cols);
float **matmul_sparse_step2(float **A_CSR, float **B_CSR, int A_rows, int A_cols, int B_rows, int B_cols);

#endif /* MATRIX_OPS_H */