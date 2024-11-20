#include "unity/unity.h"
#include "../kernel/kernel.h"
#include "test_matrix_ops.h"


#define EPSILON 0.000001f

void assert_float_array_equal_matmul(float **expected, float **actual, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            UNITY_TEST_ASSERT_FLOAT_WITHIN(EPSILON, expected[i][j], actual[i][j], __LINE__, "Arrays Not Equal!");
        }
    }
}

void test_matmul_square_matrices(void)
{
    // Setup
    float **A = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        A[i] = (float *)malloc(2 * sizeof(float));
    }
    A[0][0] = 1.0f;
    A[0][1] = 2.0f;
    A[1][0] = 3.0f;
    A[1][1] = 4.0f;

    float **B = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        B[i] = (float *)malloc(2 * sizeof(float));
    }
    B[0][0] = 2.0f;
    B[0][1] = 0.0f;
    B[1][0] = 1.0f;
    B[1][1] = 2.0f;

    float **expected = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        expected[i] = (float *)malloc(2 * sizeof(float));
    }
    expected[0][0] = 4.0f;
    expected[0][1] = 4.0f;
    expected[1][0] = 10.0f;
    expected[1][1] = 8.0f;

    // Run function under test
    float **C = matmul(A, B, 2, 2, 2, 2);

    // Check expectations
    assert_float_array_equal_matmul(expected, C, 2, 2);

    // Cleanup
    for (int i = 0; i < 2; i++)
    {
        free(A[i]);
        free(B[i]);
        free(C[i]);
        free(expected[i]);
    }
    free(A);
    free(B);
    free(C);
    free(expected);
}

void test_matmul_incompatible_dimensions(void)
{
    // Setup
    float **A = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        A[i] = (float *)malloc(3 * sizeof(float));
    }

    float **B = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        B[i] = (float *)malloc(2 * sizeof(float));
    }

    // Run function under test
    float **C = matmul(A, B, 2, 3, 2, 2);

    // Check expectations
    UNITY_TEST_ASSERT_NULL(C, __LINE__, "Expected NULL!");

    // Cleanup
    for (int i = 0; i < 2; i++)
    {
        free(A[i]);
        free(B[i]);
    }
    free(A);
    free(B);
}

void test_matmul_rectangular_matrices(void)
{
    // Setup
    float **A = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        A[i] = (float *)malloc(5 * sizeof(float));
    }
    A[0][0] = 1.0f;
    A[0][1] = 2.0f;
    A[0][2] = 3.0f;
    A[0][3] = 4.0f;
    A[0][4] = 5.0f;
    A[1][0] = 6.0f;
    A[1][1] = 7.0f;
    A[1][2] = 8.0f;
    A[1][3] = 9.0f;
    A[1][4] = 10.0f;

    float **B = (float **)malloc(5 * sizeof(float *));
    for (int i = 0; i < 5; i++)
    {
        B[i] = (float *)malloc(3 * sizeof(float));
    }
    B[0][0] = 1.0f;
    B[0][1] = 2.0f;
    B[0][2] = 3.0f;
    B[1][0] = 4.0f;
    B[1][1] = 5.0f;
    B[1][2] = 6.0f;
    B[2][0] = 7.0f;
    B[2][1] = 8.0f;
    B[2][2] = 9.0f;
    B[3][0] = 10.0f;
    B[3][1] = 11.0f;
    B[3][2] = 12.0f;
    B[4][0] = 13.0f;
    B[4][1] = 14.0f;
    B[4][2] = 15.0f;

    float **expected = (float **)malloc(2 * sizeof(float *));
    for (int i = 0; i < 2; i++)
    {
        expected[i] = (float *)malloc(3 * sizeof(float));
    }
    expected[0][0] = 135.0f;
    expected[0][1] = 150.0f;
    expected[0][2] = 165.0f;
    expected[1][0] = 310.0f;
    expected[1][1] = 350.0f;
    expected[1][2] = 390.0f;

    // Run function under test
    float **C = matmul_sparse(A, B, 2, 5, 5, 3);

    // Check expectations
    assert_float_array_equal_matmul(expected, C, 2, 3);

    // Cleanup
    for (int i = 0; i < 2; i++)
    {
        free(A[i]);
        free(expected[i]);
    }
    for (int i = 0; i < 5; i++)
    {
        free(B[i]);
    }
    free(A);
    free(B);
    free(C);
    free(expected);
}