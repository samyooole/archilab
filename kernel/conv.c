#include "conv.h"

// Im2col algorithm
float **im2col(float ***image, int numChannels, int imageSize, int kernelSize, int stride, int *outputSize)
{
    // TODO: Implement the im2col algorithm
}

// Im2col algorithm's inverse
float ***col2im(float **result, int num_kernels, int conv_rows, int conv_cols)
{
    // TODO: Implement the col2im algorithm
}

float **kernel_flatten(float ****kernel, int num_kernels, int kernel_size)
{
    float **flattened_kernels = (float **)malloc(num_kernels * sizeof(float *));
    for (int i = 0; i < num_kernels; i++)
    {
        flattened_kernels[i] = (float *)malloc(kernel_size * kernel_size * sizeof(float));
    }

    for (int k = 0; k < num_kernels; k++)
    {
        for (int i = 0; i < kernel_size; i++)
        {
            for (int j = 0; j < kernel_size; j++)
            {
                flattened_kernels[k][i * kernel_size + j] = kernel[k][0][i][j];
            }
        }
    }

    return flattened_kernels;
}

// Basic convolution operation
float ***convolution(float ***image, int numChannels, float ****kernel, float *biasData, int numFilters, int inputSize, int kernelSize)
{
    // DONE: Implement the convolution operation

    // also has relu 

    // image: a 3d-array

    // kernel[3][1][2][1] is the weight at the (2,1) position for the 3rd filter's 1st channel
    // kernel[filter][channel][row][column]
    // results will be summed across all CHANNELS, so output will be 3-d array (FILTER, row, column)

    // mem alloc

    float ***convOutput = (float ***)malloc(numFilters * sizeof(float **));
    for (int i = 0; i < numFilters; i++) {
        convOutput[i] = (float **)malloc((inputSize - kernelSize + 1) * sizeof(float *));
        for (int j = 0; j < (inputSize - kernelSize + 1); j++) {
            convOutput[i][j] = (float *)malloc((inputSize - kernelSize + 1) * sizeof(float));
        }
    }


    for (int filter = 0; filter < numFilters; filter++){
        //outermost loop

        // per channel

        // my assumptions: image channel = inputSize * inputSize
        // filter window = kernelSize * kernelSize

        for (int outRow = 0; outRow < inputSize - kernelSize + 1; outRow++){
            for (int outCol = 0; outCol < inputSize - kernelSize + 1; outCol++){


                float sum = 0.0; //init the filter


                for (int channel = 0; channel < numChannels; channel++) {

                    // want to sum across all channels


                    for (int kRow = 0; kRow < kernelSize; kRow++) {
                        for (int kCol = 0; kCol < kernelSize; kCol++) {

                            int inRow = outRow + kRow;
                            int inCol = outCol + kCol;

                            sum += image[channel][inRow][inCol] * kernel[filter][channel][kRow][kCol];

                        }
                    }
                }
                convOutput[filter][outRow][outCol] = relu(sum + biasData[filter]);
            }
        }


    }

    return convOutput;


}

// Convolution with im2col algorithm
float ***convolution_im2col(float ***image, int numChannels, float ****kernel, float *biasData, int numFilters, int inputSize, int kernelSize, MatmulType matmul_type)
{
    // TODO: Implement the convolution operation with im2col algorithm
    // Flatten kernel

    // Apply im2col

    // Apply matmul

    // Apply col2im

    // Add bias and apply ReLU

    // Cleanup
}