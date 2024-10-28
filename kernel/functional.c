#include "functional.h"
#include <math.h>

// this file is done

float relu(float x)
{
    // DONE: Implement relu

    return x > 0 ? x : 0;
}

void applyRelu(float *input, int inputSize)
{
    for (int i = 0; i < inputSize; i++)
    {
        input[i] = relu(input[i]);
    }
}

float *softmax(float *input, int inputSize)
{
    // done: Implement softmax

    // Find maximum of input vector

    float maxInput = input[0];
    for (int i = 1; i < inputSize; i++)
    {
        if (input[i] > maxInput)
        {
            maxInput = input[i];
        }
    }

    // alloc mem for output
    float *output = (float*)malloc(inputSize * sizeof(float));
    if (output == NULL) {
        // handle mem alloc failure
        return NULL;
    }
    

    // Compute exp of input - maxInput to avoid underflow
    float sumExp = 0.0;
    for (int i = 0; i < inputSize; i++) {
        output[i] = exp(input[i] - maxInput);
        sumExp += output[i];
    }
   

    
    // Normalize and apply logarithm
    for (int i = 0; i < inputSize; i++) {
        //output[i] = log(output[i] / sumExp); //im a bit confused here. in the provided softmax fn documentation there is no log. what's going on?
        output[i] = log(output[i] / sumExp);
    }

    return output;
}