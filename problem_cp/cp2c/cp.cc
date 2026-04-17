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
#define BLOCK_SIZE 8

void print_double_grid(double *grid, int rows, int cols);
void correlate(int ny, int nx, const float *data, float *result) {

    int avx_cols = (nx + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int pad_nx = avx_cols * BLOCK_SIZE;

    double *norm_data = (double *)malloc(ny * pad_nx * sizeof(double));

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

        // padding

        for (int j = nx; j < pad_nx; j++) {
            norm_data[j + i * pad_nx] = 0;
        }
    }

    print_double_grid(norm_data, ny, pad_nx);

    __m512d *avx_grid;

    if (posix_memalign((void**)&avx_grid, 64, ny * avx_cols * BLOCK_SIZE * sizeof(double)) != 0) {
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

            avx_grid[j + i * avx_cols] = _mm512_loadu_pd((void const*)(norm_data + (j * BLOCK_SIZE) + i * pad_nx));
        }
    }

    // Multiply data and transpose

    for (int row1 = 0; row1 < ny; row1++) {

        for (int row2 = 0; row2 < ny; row2++) {

            if (row1 > row2) {
                continue;
            }

            __m512d sum_vector = _mm512_set1_pd(0);

            for (int k = 0; k < avx_cols; k++) {

                sum_vector = _mm512_fmadd_pd(avx_grid[k + row1 * avx_cols], avx_grid[k + row2 * avx_cols], sum_vector);
            }

            result[row2 + row1 * ny] = (float)_mm512_reduce_add_pd(sum_vector);
        }
    }

    free(norm_data);
    free(avx_grid);

}

void print_double_grid(double *grid, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            printf("%f ", grid[j + i * cols]);
        }
        printf("\n");
    }
}
