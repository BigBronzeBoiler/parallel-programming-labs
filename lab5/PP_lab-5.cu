#include <iostream>
#include <fstream>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;

// CUDA-ядро для перемножения матриц
__global__ void matrixMulKernel(double* A, double* B, double* C, int n) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < n && col < n) {
        double sum = 0;
        for (int k = 0; k < n; ++k) {
            sum += A[row * n + k] * B[k * n + col];
        }
        C[row * n + col] = sum;
    }
}

// Вспомогательная функция для чтения (одномерный массив)
void readMatrixFlat(const string& filename, double* mat, int n) {
    ifstream file(filename);
    for (int i = 0; i < n * n; ++i) file >> mat[i];
}

void saveMatrixFlat(const string& filename, double* mat, int n) {
    ofstream file(filename);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            file << mat[i * n + j] << " ";
        file << '\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    int n = atoi(argv[1]);
    size_t size = n * n * sizeof(double);

    // Выделение памяти на CPU (Host)
    double* h_A = new double[n * n];
    double* h_B = new double[n * n];
    double* h_C = new double[n * n];

    readMatrixFlat("A.txt", h_A, n);
    readMatrixFlat("B.txt", h_B, n);

    // Выделение памяти на GPU (Device)
    double *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_B, size);
    cudaMalloc(&d_C, size);

    auto start = chrono::high_resolution_clock::now();

    // Копирование данных на Device
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    // Настройка сетки потоков
    // Используем блоки 16x16 или 32x32
    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((n + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (n + threadsPerBlock.y - 1) / threadsPerBlock.y);

    // Запуск ядра
    matrixMulKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, n);

    // Ожидание завершения и копирование результата назад
    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    saveMatrixFlat("result.txt", h_C, n);

    cout << "GPU Size: " << n << "x" << n << '\n';
    cout << "Total GPU Execution Time: " << duration.count() << " mcs" << '\n';

    // Освобождение ресурсов
    cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
    delete[] h_A; delete[] h_B; delete[] h_C;

    return 0;
}