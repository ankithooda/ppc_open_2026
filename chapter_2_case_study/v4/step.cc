#include <limits>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <ostream>
#include <cstdio>

#define FLOAT_PER_VECTOR 16

#define BLOCK_SIZE 4

// Intrinsic Headers

#include <immintrin.h>

void step(float* r, const float* d, int n);

void print_vector_grid(__m512 *grid, int rows, int cols);
void print_vector(__m512* vector);
void print_float_grid(float *grid, int rows, int cols);

void step(float* r, const float* d, int n) {

    int vector_cols = (n + FLOAT_PER_VECTOR - 1) / FLOAT_PER_VECTOR;
    constexpr float infinity = std::numeric_limits<float>::infinity();


    int avx_grid_cols = (vector_cols + BLOCK_SIZE - 1) / BLOCK_SIZE;
    //avx_grid_cols = avx_grid_cols * BLOCK_SIZE;

    int avx_grid_rows = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    avx_grid_rows = avx_grid_rows * BLOCK_SIZE;


    // Allocate memory for padded_transpose and padded_data
    std::vector<float> padded_data(vector_cols * FLOAT_PER_VECTOR * n, infinity);
    std::vector<float> padded_transpose(vector_cols * FLOAT_PER_VECTOR * n, infinity);

    //print_float_grid((float *)d, n, n);

    //printf("INFO %d %d %d %d %d %d\n", n, vector_cols, avx_grid_rows, avx_grid_cols, FLOAT_PER_VECTOR, BLOCK_SIZE);

    // Create transpose
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            //padded_data[i + j * vector_cols * FLOAT_PER_VECTOR] = d[i + j * n];
            padded_data[j + i * vector_cols * FLOAT_PER_VECTOR] = d[j + i * n];
            padded_transpose[i + j * vector_cols * FLOAT_PER_VECTOR] = d[j + i * n];
        }
    }


    float const *padded_data_memory = padded_data.data();
    float const *padded_transpose_memory = padded_transpose.data();

    //print_float_grid((float *)padded_data_memory, n, vector_cols * FLOAT_PER_VECTOR);
    //print_float_grid((float *)padded_transpose_memory, n, vector_cols * FLOAT_PER_VECTOR);


    //printf("INFO ALL RIGHT HERE\n");

    // VERY IMP. Memory for AVX datatypes like __m512 has to be properly aligned.

    //__m512 *data_avx = (__m512 *)malloc(avx_grid_rows * avx_grid_cols * FLOAT_PER_VECTOR * sizeof(float));
    //__m512 *transpose_avx = (__m512 *)malloc(avx_grid_rows * avx_grid_cols * FLOAT_PER_VECTOR * sizeof(float));

    __m512 *data_avx;
    __m512 *transpose_avx;

    if (posix_memalign((void**)&data_avx, 64, avx_grid_rows * avx_grid_cols * FLOAT_PER_VECTOR * sizeof(float)) != 0) {
        // Return from function, this will cause
        // address sanitizer issuer in the testing framework
        // as other memory has not been freed.
        // We dont care if we are not able to allocate memory for the __m512d
        // all is lost.
        return;
    }

    if (posix_memalign((void**)&transpose_avx, 64, avx_grid_rows * avx_grid_cols * FLOAT_PER_VECTOR * sizeof(float)) != 0) {
        // Return from function, this will cause
        // address sanitizer issuer in the testing framework
        // as other memory has not been freed.
        // We dont care if we are not able to allocate memory for the __m512d
        // all is lost.
        return;
    }

    // i iterates over the AVX rows
    // and n < avx_Grid_rows
    // Similarly j iterates over the columns but vector_cols < avx_grid_cols

    // Therefore we can iterate over the smaller n * vector_cols matrix first and then fill
    // the padding later.
    // Caution is to be taken in using the avx_grid_cols in index calculation.

    //printf("%d - %d - %d", avx_grid_cols, avx_grid_rows, vector_cols);

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < vector_cols; j++) {
            //printf("%d - %d ||| %d  | %d \n", i, j, j + i * avx_grid_cols,  (j * FLOAT_PER_VECTOR) +  (i * vector_cols * FLOAT_PER_VECTOR));
            data_avx[j + i * avx_grid_cols] = _mm512_loadu_ps((void const*)(padded_data_memory +  (j * FLOAT_PER_VECTOR) +  (i * vector_cols * FLOAT_PER_VECTOR)));

            transpose_avx[j + i * avx_grid_cols] = _mm512_loadu_ps((void const*)(padded_transpose_memory +  (j * FLOAT_PER_VECTOR) +  (i * vector_cols * FLOAT_PER_VECTOR)));
        }
    }

    // printf("---------------------------------------------\n");
    // print_vector_grid(data_avx, avx_grid_rows, avx_grid_cols);
    // printf("**************************************************************\n");
    // print_vector_grid(transpose_avx, avx_grid_rows, avx_grid_cols);


    // Fill the padding in avx grid

    for (int i = 0; i < n; i++) {

        for (int j = vector_cols; j < avx_grid_cols; j++) {

            data_avx[j + i * avx_grid_cols] = _mm512_set1_ps(infinity);
            transpose_avx[j + i * avx_grid_cols] = _mm512_set1_ps(infinity);
        }
    }

    for (int i = n; i < avx_grid_rows; i++) {

        for (int j = 0; j < avx_grid_cols; j++) {

            data_avx[j + i * avx_grid_cols] = _mm512_set1_ps(infinity);
            transpose_avx[j + i * avx_grid_cols] = _mm512_set1_ps(infinity);
        }
    }
    // printf("---------------------------------------------\n");
    // print_vector_grid(data_avx, avx_grid_rows, avx_grid_cols);
    // printf("**************************************************************\n");
    // print_vector_grid(transpose_avx, avx_grid_rows, avx_grid_cols);

    // All data has been loaded, Solve now

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            __m512 min_vector = _mm512_set1_ps(infinity);


            for (int k = 0; k < vector_cols; k++) {
                __m512 sum_vector = _mm512_add_ps(data_avx[k + i * vector_cols], transpose_avx[k + j * vector_cols]);

                // min_mask is 1 where sum_vector element is smaller
                __mmask16 min_mask = _mm512_cmplt_ps_mask(sum_vector, min_vector);


                // copy elements from sum_vector to min_vector using min_mask as write mask
                min_vector = _mm512_mask_mov_ps(min_vector, min_mask, sum_vector);

            }

            // Set values in result matrix
            r[j + i * n] = _mm512_reduce_min_ps(min_vector);
        }
    }
}


void print_vector(__m512 *vector) {
    float *single_vector_mem = (float *)malloc(sizeof(float) * FLOAT_PER_VECTOR);
    __mmask16 write_all= _cvtu32_mask16(65535);

    // Store a AVX-512 into FLOAT_PER_VECTOR floats
    _mm512_mask_compressstoreu_ps((void *)single_vector_mem, write_all, *vector);

    // Print all floats in the AVX-512 vector

    printf("[ ");
    for (int k = 0; k < FLOAT_PER_VECTOR; k++) {
        printf("%f ", single_vector_mem[k]);
    }
    printf(" ] ");
    free(single_vector_mem);
}

void print_vector_grid(__m512 *grid, int rows, int cols) {

    printf("Print AVX grid of %d rows and %d cols\n", rows, cols);

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {
            //printf("\nPrinting %d column of %d row.\n", j, i);
            print_vector(grid + j + i * cols);
        }
    }
}


void print_float_grid(float *grid, int rows, int cols) {

    printf("Printing float grid %d rows %d cols\n", rows, cols);

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            printf("(%f) | ", grid[j + i * cols]);

            //std::cout << grid[j + i * rows] << " (" << j + i * rows << ") ";
        }
        std::cout << std::endl;
    }
}
