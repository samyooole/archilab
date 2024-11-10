#include "conv.h"

// Im2col algorithm
float **im2col(float ***image, int numChannels, int imageSize, int kernelSize, int stride, int *outputSize)
{
    // TODO: Implement the im2col algorithm

    // calculate output dimensions
    int outputDim = (imageSize - kernelSize) / stride + 1;
    int patchSize = numChannels * kernelSize * kernelSize;  // rows in output matrix
    *outputSize = outputDim * outputDim;  // columns in output matrix
    
    // allocate memory for the output matrix
    float **cols = (float **)malloc(patchSize * sizeof(float *));
    for (int i = 0; i < patchSize; i++) {
        cols[i] = (float *)malloc((*outputSize) * sizeof(float));
    }
    
    int col_idx = 0;
    // for each possible patch position
    for (int i = 0; i <= imageSize - kernelSize; i += stride) {
        for (int j = 0; j <= imageSize - kernelSize; j += stride) {
            // for each channel
            int row_idx = 0;
            for (int c = 0; c < numChannels; c++) {
                // for each element in the patch
                for (int ki = 0; ki < kernelSize; ki++) {
                    for (int kj = 0; kj < kernelSize; kj++) {
                        cols[row_idx][col_idx] = image[c][i + ki][j + kj];
                        row_idx++;
                    }
                }
            }
            col_idx++;
        }
    }
    
    return cols;
}

// Im2col algorithm's inverse
float ***col2im(float **result, int num_kernels, int conv_rows, int conv_cols)
{
    // TODO: Implement the col2im algorithm

    // allocate memory for the output image
    float ***output = (float ***)malloc(num_kernels * sizeof(float **));
    for (int i = 0; i < num_kernels; i++) {
        output[i] = (float **)malloc(conv_rows * sizeof(float *));
        for (int j = 0; j < conv_rows; j++) {
            output[i][j] = (float *)malloc(conv_cols * sizeof(float));
        }
    }
    
    // copy data from columns back to image format
    for (int k = 0; k < num_kernels; k++) {
        for (int i = 0; i < conv_rows; i++) {
            for (int j = 0; j < conv_cols; j++) {
                output[k][i][j] = result[k][i * conv_cols + j];
            }
        }
    }
    
    return output;
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
float ***convolution_im2col(float ***image, int numChannels, float ****kernel, float *biasData, int numFilters, int inputSize, int kernelSize, MatmulType matmul_type) {
    int outputSize;
    
    // 1. apply im2col to the input image
    float **image_cols = im2col(image, numChannels, inputSize, kernelSize, 1, &outputSize);
    
    // 2. flatten kernels
    float **flat_kernels = kernel_flatten(kernel, numFilters, kernelSize);
    
    // 3. perform matrix multiplication
    int conv_size = inputSize - kernelSize + 1;

    float **result;
    
    switch (matmul_type) {
        case MATMUL_BASE:
            // basic matrix mult
            result = matmul(flat_kernels, numFilters, kernelSize * kernelSize * numChannels, image_cols, kernelSize * kernelSize * numChannels, outputSize);
            break;
            
        case MATMUL_SPARSE:
            // sparse matrix mult
            result = matmul_sparse(flat_kernels, numFilters, kernelSize * kernelSize * numChannels, image_cols, kernelSize * kernelSize * numChannels, outputSize);
            break;
    }
    
    // 4. add bias, apply relu
    for (int i = 0; i < numFilters; i++) {
        for (int j = 0; j < outputSize; j++) {
            result[i][j] = relu(result[i][j] + biasData[i]);
        }
    }
    
    // 5. reshape back to image format using col2im
    float ***output = col2im(result, numFilters, conv_size, conv_size);
    
    // cleanup
    for (int i = 0; i < numChannels * kernelSize * kernelSize; i++) {
        free(image_cols[i]);
    }
    free(image_cols);
    
    for (int i = 0; i < numFilters; i++) {
        free(flat_kernels[i]);
        free(result[i]);
    }
    free(flat_kernels);
    free(result);
    
    return output;
}