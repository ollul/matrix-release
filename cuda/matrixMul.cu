#include <stdio.h>
#include <cuda_runtime.h>

__global__ void matrixMul(const float *A, const float *B, float *C, int M, int N, int K) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < M && col < N) {
        float sum = 0;
        for (int i = 0; i < K; i++) {
            sum += A[row * K + i] * B[i * N + col];
        }
        C[row * N + col] = sum;
    }
}
void printMatrix(float *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main() {
    // Set matrix dimensions
    int M = 10000;
    int N = 30000;
    int K = 20000;

    // Allocate memory on the host for input and output matrices
    float *A, *B, *C;
    A = (float *)malloc(M * K * sizeof(float));
    B = (float *)malloc(K * N * sizeof(float));
    C = (float *)malloc(M * N * sizeof(float));

    // Initialize input matrices with random values
    for (int i = 0; i < M * K; i++) A[i] = rand() / (float)RAND_MAX;
    for (int i = 0; i < K * N; i++) B[i] = rand() / (float)RAND_MAX;

    // printf("MatrixA:\n");
    // printMatrix(A, M, K);

    // printf("MatrixB:\n");
    // printMatrix(B, M, K);

    // Allocate memory on the device for input and output matrices
    float *d_A, *d_B, *d_C;
    cudaMalloc((void **)&d_A, M * K * sizeof(float));
    cudaMalloc((void **)&d_B, K * N * sizeof(float));
    cudaMalloc((void **)&d_C, M * N * sizeof(float));

    // Copy input matrices from host to device memory
    cudaMemcpy(d_A, A, M * K * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, K * N * sizeof(float), cudaMemcpyHostToDevice);

      // Create and record events
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // Set block and grid dimensions
    dim3 blockDim(16, 16);
    dim3 gridDim((N + blockDim.x - 1) / blockDim.x, (M + blockDim.y - 1) / blockDim.y);

    // Launch kernel on the device
    matrixMul<<<gridDim, blockDim>>>(d_A, d_B, d_C, M, N, K);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    // Calculate elapsed time
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    

    // Copy output matrix from device to host memory
    cudaMemcpy(C, d_C, M * N * sizeof(float), cudaMemcpyDeviceToHost);

    // printf("Matrix C:\n");
    // printMatrix(C, M, N);

    printf("Execution Time: %f ms\n", elapsedTime);

    // Free device memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    // Free host memory
    free(A);
    free(B);
    free(C);

    return 0;
}