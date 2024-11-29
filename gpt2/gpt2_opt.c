typedef struct {
    int batch_size;
    int sequence_length;
    int features;
    float *data __attribute__((aligned(64))); // data[batch_size * sequence_length * features]
} Tensor3D;

typedef struct {
    int rows;
    int cols;
    float *data __attribute__((aligned(64))); // data[rows * cols]
} Tensor2D;

typedef struct {
    float **weights __attribute__((aligned(64))); // weights[fcOutputSize][fcInputSize]
    float *biases __attribute__((aligned(64)));   // biases[fcOutputSize]
    int fcInputSize;
    int fcOutputSize;
} LinearLayer;

typedef struct {
    LinearLayer q_mlp;
    LinearLayer k_mlp;
    LinearLayer v_mlp;
    LinearLayer first_block_MLP;
    LinearLayer second_block_MLP;
} BlockWeights;

typedef struct {
    float **wpe __attribute__((aligned(64)));         // Positional embeddings
    float **wte __attribute__((aligned(64)));         // Token embeddings
    BlockWeights *blocks __attribute__((aligned(64)));
    LinearLayer logits_mlp;
} GPT2Weights;
