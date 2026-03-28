#include <limits>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <ostream>
#include <cstdio>

#define FLOAT_PER_VECTOR 16
// Intrinsic Headers

#include <immintrin.h>

void step(float* r, const float* d, int n);

void print_vector_grid(__m512 *grid, int rows, int cols);
void print_float_grid(float *grid, int rows, int cols);

void step(float* r, const float* d, int n) {

    int vector_cols = (n + FLOAT_PER_VECTOR - 1) / FLOAT_PER_VECTOR;
    constexpr float infinity = std::numeric_limits<float>::infinity();


    // Allocate memory for padded_transpose and padded_data
    std::vector<float> padded_data(vector_cols * FLOAT_PER_VECTOR * n, infinity);
    std::vector<float> padded_transpose(vector_cols * FLOAT_PER_VECTOR * n, infinity);

    print_float_grid((float *)d, n, n);

    std::cout << n << vector_cols << FLOAT_PER_VECTOR << std::endl;

    // Create transpose
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            //padded_data[i + j * vector_cols * FLOAT_PER_VECTOR] = d[i + j * n];
            padded_data[j + i * vector_cols * FLOAT_PER_VECTOR] = d[j + i * n];
            padded_transpose[i + j * vector_cols * FLOAT_PER_VECTOR] = d[j + i * n];
        }
    }




    // VERY IMP. Memory for AVX datatypes like __m512 has to be properly aligned.

    __m512 *data_avx = (__m512 *)malloc(n * vector_cols * FLOAT_PER_VECTOR * sizeof(float));
    __m512 *transpose_avx = (__m512 *)malloc(n * vector_cols * FLOAT_PER_VECTOR * sizeof(float));

    if (posix_memalign((void**)&data_avx, 64, n * vector_cols * FLOAT_PER_VECTOR * sizeof(float)) != 0) {
        // Return from function, this will cause
        // address sanitizer issuer in the testing framework
        // as other memory has not been freed.
        // We dont care if we are not able to allocate memory for the __m512d
        // all is lost.
        return;
    }

    if (posix_memalign((void**)&transpose_avx, 64, n * vector_cols * FLOAT_PER_VECTOR * sizeof(float)) != 0) {
        // Return from function, this will cause
        // address sanitizer issuer in the testing framework
        // as other memory has not been freed.
        // We dont care if we are not able to allocate memory for the __m512d
        // all is lost.
        return;
    }

    // Both AVX data arrays have n rows and vector_cols columns.
    // Both float arrays have n rows and vector_cols * FLOAT_PER_VECTOR columns;
    // We will load FLOAT_PER_VECTOR floats into 1 vector.


    float const *padded_data_memory = padded_data.data();
    float const *padded_transpose_memory = padded_transpose.data();

    print_float_grid((float *)padded_data_memory, n, vector_cols * FLOAT_PER_VECTOR);
    print_float_grid((float *)padded_transpose_memory, n, vector_cols * FLOAT_PER_VECTOR);

    printf("Going in the loop\n");

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < vector_cols; j++) {

            printf("AVX-%d <- MEM-%d  <-  %f\n", j + i * vector_cols, (j * FLOAT_PER_VECTOR) +  (i * vector_cols * FLOAT_PER_VECTOR),
                   *(padded_data_memory +  (j * FLOAT_PER_VECTOR) +  (i * vector_cols * FLOAT_PER_VECTOR)));


            data_avx[j + i * vector_cols] = _mm512_loadu_ps((void const*)(padded_data_memory +  (j * FLOAT_PER_VECTOR) +  (i * vector_cols * FLOAT_PER_VECTOR)));

            transpose_avx[j + i * vector_cols] = _mm512_loadu_ps((void const*)(padded_transpose_memory +  (j * FLOAT_PER_VECTOR) +  (i * vector_cols * FLOAT_PER_VECTOR)));
        }
    }

    print_vector_grid(data_avx, n, vector_cols);
    print_vector_grid(transpose_avx, n, vector_cols);



}

void print_vector_grid(__m512 *grid, int rows, int cols) {

    float *single_vector = (float *)malloc(sizeof(float) * FLOAT_PER_VECTOR);

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {
            __mmask16 write_all= _cvtu32_mask16(65535);

            //grid[j + i * cols] = _mm512_set4_ps(6, 7, 8, 9);
            // Store a AVX-512 into FLOAT_PER_VECTOR floats
            _mm512_mask_compressstoreu_ps((void *)single_vector, write_all, grid[j + i * cols]);

            // Print all floats in the single vector

            for (int k = 0; k < FLOAT_PER_VECTOR; k++) {
                printf("%f ", single_vector[k]);
            }
        }
        std::cout << std::endl;
    }
}


void print_float_grid(float *grid, int rows, int cols) {

    printf("Printing grid %d %d \n", rows, cols);

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            printf("(%f) | ", grid[j + i * cols]);

            //std::cout << grid[j + i * rows] << " (" << j + i * rows << ") ";
        }
        std::cout << std::endl;
    }
}
