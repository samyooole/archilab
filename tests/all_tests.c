#include "unity/unity.h"
#include "test_conv.h"
#include "test_nn.h"
#include "test_functional.h"
#include "test_linear.h"
#include "test_matrix_ops.h"
#include "test_attention.h"

void setUp(void) {
    /* Code here will run before each test */
}

void tearDown(void) {
    /* Code here will run after each test */
}

int main(void) {
    UNITY_BEGIN();

    // Only testing matrix_ops
    RUN_TEST(test_matmul_square_matrices);
    RUN_TEST(test_matmul_rectangular_matrices);
    RUN_TEST(test_matmul_incompatible_dimensions);

    return UNITY_END();
}