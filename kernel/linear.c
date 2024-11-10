#include "linear.h"

float *linear(float *input, float **weights, float *biases, int inputSize, int outputSize)
{
    // DONE: Implement the linear function (fully connected layer)

    // Wx + b
    // need relu actication funtion?

    float *output = (float *)malloc(outputSize * sizeof(float));


    for (int i = 0; i < outputSize; i++){

        float weight_term = 0.0f;
        for (int j = 0; j < inputSize; j++){

            weight_term += weights[i][j] * input[j];


        }

        output[i] = weight_term + biases[i];
    }

    return output;

}