
/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <immintrin.h>
#define DOUBLES_PER_VECTOR 8

#define CACHE_BLOCK 4

void print_double_grid(double *grid, int rows, int cols);
void print_float_grid(const float* grid,int rows,int cols);
void print_vector_grid(__m512d* grid, int avx_rows, int avx_cols);
void print_vector(__m512d* vector);


void correlate(int ny, int nx, const float *data, float *result) {
    int avx_cols = (nx + DOUBLES_PER_VECTOR - 1) / DOUBLES_PER_VECTOR;

    if (avx_cols < CACHE_BLOCK) {
        avx_cols = CACHE_BLOCK;
    }

    int pad_nx = avx_cols * DOUBLES_PER_VECTOR;

    int avx_rows = (ny + CACHE_BLOCK - 1) / CACHE_BLOCK;
    avx_rows = avx_rows * CACHE_BLOCK;

    double *norm_data = (double *)malloc(ny * pad_nx * sizeof(double));

    for (int i = 0; i < ny; i++) {

        for (int j = 0; j < pad_nx; j++) {

            norm_data[j + i * pad_nx] = 0;
        }
    }

    // Row wise normalization

    for (int i = 0; i < ny; i++) {

        double sum = 0.0;
        double sq_sum = 0.0;

        for (int j = 0; j < nx; j++) {
            sum = sum + data[j + i * nx];
        }

        sum = sum / nx;

        for (int j = 0; j < nx; j++) {
            double v = (double)data[j + i * nx] - sum;

            norm_data[j + i * pad_nx] = v;
            sq_sum = sq_sum + v * v;
        }

        sq_sum = std::sqrt(sq_sum);
        for (int j = 0; j < nx; j++) {

            norm_data[j + i * pad_nx] = norm_data[j + i * pad_nx] / sq_sum;
        }

    }

    //print_double_grid(norm_data, ny, pad_nx);

    __m512d *avx_grid;

    if (posix_memalign((void**)&avx_grid, 64, avx_rows * avx_cols * DOUBLES_PER_VECTOR * sizeof(double)) != 0) {
        // Return from function, this will cause
        // address sanitizer issuer in the testing framework
        // as other memory has not been freed.
        // We dont care if we are not able to allocate memory for the __m512d
        // all is lost.
        return;
    }

    // Convert to vector
    for (int i = 0; i < ny; i++) {

        for (int j = 0; j < avx_cols; j++) {

            avx_grid[j + i * avx_cols] = _mm512_loadu_pd((void const*)(norm_data + (j * DOUBLES_PER_VECTOR) + i * pad_nx));
        }
    }
    // padded rows in avx grid
    for (int i = ny; i < avx_rows; i++) {
        for (int j = 0; j < avx_cols; j++) {

            avx_grid[j + i * avx_cols] = _mm512_set1_pd(0);
        }
    }

    //print_vector_grid(avx_grid, avx_rows, avx_cols);

    // Multiply data and transpose

    //printf("%d - %d - %d - %d - %d\n\n\n", avx_rows, avx_cols, ny, pad_nx, nx);



    __m512d *cache_grid;

    if (posix_memalign((void**)&cache_grid, 64, CACHE_BLOCK * CACHE_BLOCK * DOUBLES_PER_VECTOR * sizeof(double)) != 0) {
        // Return from function, this will cause
        // address sanitizer issuer in the testing framework
        // as other memory has not been freed.
        // We dont care if we are not able to allocate memory for the __m512d
        // all is lost.
        return;
    }

    for (int row1 = 0; row1 < avx_rows; row1 = row1 + CACHE_BLOCK) {

        for (int row2 = 0; row2 < avx_rows; row2 = row2 + CACHE_BLOCK) {

            if (row1 > row2) {
                continue;
            }

            // Cache grid acts a accumulator
            for (int ci = 0; ci < CACHE_BLOCK; ci++) {

                for (int cj = 0; cj < CACHE_BLOCK; cj++) {
                    cache_grid[cj + ci * CACHE_BLOCK] =  _mm512_set1_pd(0);
                }
            }

            for (int k = 0; k < avx_cols; k++) {

                for (int ci = 0; ci < CACHE_BLOCK; ci++) {

                    for (int cj = 0; cj < CACHE_BLOCK; cj++) {

                        cache_grid[cj + ci * CACHE_BLOCK] = _mm512_fmadd_pd(avx_grid[k + (row1 + ci) * avx_cols], avx_grid[k + (row2 + cj) * avx_cols], cache_grid[cj + ci * CACHE_BLOCK]);
                    }
                }
            }

            for (int ci = 0; ci < CACHE_BLOCK; ci++) {

                for (int cj = 0; cj < CACHE_BLOCK; cj++) {

                    int res_row = row1 + ci;
                    int res_col = row2 + cj;

                    if (res_row < ny && res_col < ny) {

                        result[res_col + res_row * ny] = (float)_mm512_reduce_add_pd(cache_grid[cj + ci * CACHE_BLOCK]);
                    }
                }
            }

            //print_vector_grid(cache_grid, CACHE_BLOCK, CACHE_BLOCK);

        }
    }

    free(norm_data);
    free(avx_grid);
    free(cache_grid);

}

void print_vector_grid(__m512d *grid, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {
            print_vector(grid + j + i * cols);
        }
        printf("\n");
    }
}

void print_vector(__m512d *vector) {
    double *single_vector_mem = (double *)malloc(sizeof(double) * DOUBLES_PER_VECTOR);
    __mmask8 write_all= _cvtu32_mask8(255);

    // Store a AVX-512 into FLOAT_PER_VECTOR floats
    _mm512_mask_compressstoreu_pd((void *)single_vector_mem, write_all, *vector);

    // Print all floats in the AVX-512 vector

    printf("[ ");
    for (int k = 0; k < DOUBLES_PER_VECTOR; k++) {
        printf("%f ", single_vector_mem[k]);
    }
    printf(" ] ");
    free(single_vector_mem);
}

void print_double_grid(double *grid, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            printf("%f ", grid[j + i * cols]);
        }
        printf("\n");
    }
}

void print_float_grid(const float *grid, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            printf("%f ", grid[j + i * cols]);
        }
        printf("\n");
    }
}
