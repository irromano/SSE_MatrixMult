// SSE_MatrixMult.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <xmmintrin.h>
#include <chrono>

#define N (1 << 6)

void matrixMult(float* A, float* B, float* C)
{
    int row, col, iter;

    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            for (iter = 0; iter < N; iter++)
            {
                C[row * N + col] += A[row * N + iter] * B[iter * N + col];
            }
        }
    }
}

void matrixMultSSE(float* A, float* B, float* C)
{
    __m128 regA;
    __m128 regB;
    __m128 regC;
    __m128 regTemp;

    int row, col, iter;
    float temp[4];

    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            for (iter = 0; iter < N; iter+=4)
            {
                regA = _mm_set_ps(A[row * N + iter], A[row * N + iter + 1], A[row * N + iter + 2], A[row * N + iter + 3]);
                regB = _mm_set_ps(B[iter * N + col], B[(iter + 1) * N + col], B[(iter +2) * N + col], B[(iter + 3) * N + col]);
                regTemp = _mm_mul_ps(regA, regB);
                _mm_storeu_ps(temp, regTemp);
                C[row * N + col] += temp[0] + temp[1] + temp[2] + temp[3];
            }
        }

    }
}

void matrixInit(float* A, bool randVal)
{
    for (int i = 0; i < N * N; i++)
    {
        float val = randVal ? (((float)(rand() % 10000)) / 100) : 0;
        A[i] = val;
    }
}

int matrixUnequalCount(float* A, float* B)
{
    int miss = 0;
    for (int i = 0; i < N * N; i++)
    {
        if (A[i] != B[i])
        {
            miss++;
        }
    }
    return miss;
}

void printMatrix(float* A)
{
    std::cout << "Printing Matrix";
    for (int i = 0; i < N; i++)
    {
        std::cout << std::endl;
        for (int j = 0; j < N; j++)
        {
            std::cout << A[i*N + j] << "\t";
        }
    }
    std::cout << std::endl << std::endl;
}

int main()
{
    std::cout << "Initializing Matrixes\r\n" << std::endl;

    float* A = (float*) _aligned_malloc(N * N * sizeof(float), 16);
    float* B = (float*) _aligned_malloc(N * N * sizeof(float), 16);
    float* C = (float*) _aligned_malloc(N * N * sizeof(float), 16);
    float* C_SSE = (float*)_aligned_malloc(N * N * sizeof(float), 16);
    matrixInit(A, true);
    matrixInit(B, true);
    matrixInit(C, false);
    matrixInit(C_SSE, false);

    std::cout << "Caculating A * B = C" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    matrixMult(A, B, C);
    auto end_time = std::chrono::high_resolution_clock::now();

    auto time = end_time - start_time;
    std::cout << "Regular Calculation took " << time/std::chrono::microseconds(1) << " microseconds" << "\r\n" << std::endl;

    std::cout << "SSE Caculating A * B = C" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    matrixMultSSE(A, B, C_SSE);
    end_time = std::chrono::high_resolution_clock::now();

    time = end_time - start_time;
    std::cout << "SSE Calculation took " << time / std::chrono::microseconds(1) << " microseconds" << "\r\n" << std::endl;

    std::cout << "Comparing C matrixes" << std::endl;
    int miss = matrixUnequalCount(C, C_SSE);
    if (miss)
    {
        std::cout << "Test Fail: Matrixes Unequal with " << miss << " misses" << std::endl;
    }
    else
    {
        std::cout << "Test Pass: Matrixes equal" << std::endl;
    }

    //std::cout << "Original" << std::endl;
    //printMatrix(C);
    //printMatrix(C_SSE);
    //std::cout << "SSE" << std::endl;
    _aligned_free(A);
    _aligned_free(B);
    _aligned_free(C);
    _aligned_free(C_SSE);

    //__m128 regA;
    //__m128 regB;
    //__m128 regC;

    //regA = _mm_set_ss(5.0);
    //regB = _mm_set_ss(6.0);
    //regC = _mm_mul_ss(regA, regB);
    //float* ansC = (float*) _aligned_malloc(sizeof(float), 16);
    //_mm_store_ss(ansC, regC);

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
