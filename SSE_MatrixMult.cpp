// SSE_MatrixMult.cpp : This program calculates a matrix multiplication using standard instructions and SSE instructions seperatly and then compares their timing results. It first makes this calculation normally through serial instructions and then again using SSE instructions. Both functions compute the matrix equation A * B = C and are timed separately.
//

#include <iostream>
#include <immintrin.h>
#include <chrono>

#define N (1 << 10)
/// <summary>
/// Calculates an A * B = C matrix multiplication using standard instructions.
/// </summary>
/// <param name="A">The first input matrix</param>
/// <param name="B">The second input matrix</param>
/// <param name="C">The output matrix</param>
void matrixMult(float* A, float* B, float* C)
{
    int row, col, iter;

    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            float tempC = 0;
            for (iter = 0; iter < N; iter++)
            {
                tempC += A[row * N + iter] * B[iter * N + col];
            }
            C[row * N + col] = tempC;
        }
    }
}
/// <summary>
/// Calculates an A * B = C matrix multiplication using SSE operations.
/// </summary>
/// <param name="A">The first input matrix</param>
/// <param name="B">The second input matrix</param>
/// <param name="C">The output matrix</param>
void matrixMultSSE(float* A, float* B, float* C)
{
    __m128 regA;
    __m128 regB;

    int row, col, iter;
    float *temp = (float*)_aligned_malloc(sizeof(float), 16);

    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            float tempC = 0;
            for (iter = 0; iter < N; iter+=4)
            {
                regA = _mm_set_ps(A[row * N + iter], A[row * N + iter + 1], A[row * N + iter + 2], A[row * N + iter + 3]);
                regB = _mm_set_ps(B[iter * N + col], B[(iter + 1) * N + col], B[(iter +2) * N + col], B[(iter + 3) * N + col]);
                regA = _mm_dp_ps(regA, regB, 0xff);
                _mm_store_ps1(temp, regA);
                tempC += *temp;
            }
            C[row * N + col] = tempC;
        }

    }

    _aligned_free(temp);
}

/// <summary>
/// Initializes values into a float matrix of N by N size. 
/// </summary>
/// <param name="A">
/// Pointer to the maxtrix to be initialized
/// </param>
/// <param name="randVal">If true, sets values to random values 0.00 to 99.99, else sets values
/// to 0.
/// </param>
void matrixInit(float* A, bool randVal)
{
    for (int i = 0; i < N * N; i++)
    {
        float val = randVal ? (((float)(rand() % 10000)) / 100) : 0;    /* Random float 0.00 to 99.99 if randVal is True, else 0. */
        A[i] = val;
    }
}
/// <summary>
/// Verifies if the floats of both matrixes all equal
/// </summary>
/// <param name="A"></param>
/// <param name="B"></param>
/// <returns>The amount of unequal indexes</returns>
int matrixUnequalCount(float* A, float* B)
{
    int miss = 0;
    for (int i = 0; i < N * N; i++)
    {
        float tempA = A[i];
        float tempB = B[i];
        if (fabs(tempA - tempB) > 0.1f)
        {
            //std::cout << tempA << " does not equal " << tempB << std::endl;
            miss++;
        }
    }
    return miss;
}
/// <summary>
/// Prints all indexes of the given matrix.
/// </summary>
/// <param name="A"></param>
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

    /* Algining data into 16-byte to improve register performance. */
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

    //std::cout << "Comparing C matrixes" << std::endl;
    //int miss = matrixUnequalCount(C, C_SSE);
    //if (miss)
    //{
    //    std::cout << "Test Fail: Matrixes Unequal with " << miss << " misses" << std::endl;
    //}
    //else
    //{
    //    std::cout << "Test Pass: Matrixes equal" << std::endl;
    //}

    //std::cout << "Original" << std::endl;
    //printMatrix(C);
    //printMatrix(C_SSE);
    //std::cout << "SSE" << std::endl;
    _aligned_free(A);
    _aligned_free(B);
    _aligned_free(C);
    _aligned_free(C_SSE);

    return 0;
}
