#include "nn.h"

float *flatten(float ***input, int inputSize, int depth)
{

    //DONE

    float *flattened_output = (float *)malloc(inputSize * inputSize * depth * sizeof(float));

    // flatten: assume the image is inputsize * inputsize? right, because pooled feature map is a square necessarily
    int index = 0;
    for (int d = 0; d < depth; d++)
    {
        for (int i = 0; i < inputSize; i++)
        {
            for (int j = 0; j < inputSize; j++)
            {
                flattened_output[index++] = input[d][i][j];
            }
        }
    }

    return flattened_output;
}

void destroyConvOutput(float ***convOutput, int convOutputSize)
{
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < convOutputSize; j++)
        {
            free(convOutput[i][j]);
        }
        free(convOutput[i]);
    }
    free(convOutput);
}

int forwardPass(float ***image, int numChannels, float ****conv1WeightsData, float **fc1WeightsData, float **fc2WeightsData, float *conv1BiasData, float *fc1BiasData, float *fc2BiasData)
{
    // Perform the convolution operation
    int numFilters = sizeof(conv1WeightsData) / sizeof(conv1WeightsData[0]);
    int inputSize = sizeof(image[0]) / sizeof(image[0][0]);
    int kernelSize = sizeof(conv1WeightsData[0][0]) / sizeof(conv1WeightsData[0][0][0]);

    float ***convOutput = convolution(image, numChannels, conv1WeightsData, conv1BiasData, numFilters, inputSize, kernelSize);

    // Flatten the output
    int depth = numFilters; // depth should be the number of filters
    float *flattenedOutput = flatten(convOutput, inputSize, depth);

    // Perform the fully connected operations

    // layer 1
    int fc1InputSize = inputSize * inputSize * depth;
    int fc1OutputSize = sizeof(fc1BiasData) / sizeof(fc1BiasData[0]);

    float* out_1 = linear(flattenedOutput, fc1WeightsData, fc1BiasData, fc1InputSize, fc1OutputSize);

    // layer 2
    int fc2InputSize = fc1OutputSize;
    int fc2OutputSize = sizeof(fc2BiasData) / sizeof(fc2BiasData[0]);

    float* out_2 = linear(out_1, fc2WeightsData, fc2BiasData, fc2InputSize, fc2OutputSize);

    // Apply the final softmax activation
    float* softmaxed = softmax(out_2, fc2OutputSize);

    // Make predictions
    int numClasses = fc2OutputSize;
    int prediction = predict(softmaxed, numClasses);

    // Clean up the memory usage
    destroyConvOutput(convOutput, inputSize); //must specially destroy it because it is a 3d tensor
    free(flattenedOutput);
    free(out_1);
    free(out_2);
    free(softmaxed);

    return prediction;
}

int predict(float *probabilityVector, int numClasses)
{
    // Implement the prediction function

    float maxProbability = -INFINITY; // use -INFINITY for negative infinity

    int predictedClass = -1;
    for (int i = 0; i < numClasses; i++){
        if (probabilityVector[i] > maxProbability){
            maxProbability = probabilityVector[i];
            predictedClass = i;
        }
    }

    return predictedClass;
}