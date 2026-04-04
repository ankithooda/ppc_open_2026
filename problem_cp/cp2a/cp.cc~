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

void print_double_grid(double *grid, int rows, int cols);
void correlate(int ny, int nx, const float *data, float *result) {

    double *norm_data = (double *)malloc(ny * nx * sizeof(double));

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

            norm_data[j + i * nx] = v;
            sq_sum = sq_sum + v * v;
        }

        sq_sum = std::sqrt(sq_sum);
        for (int j = 0; j < nx; j++) {

            norm_data[j + i * nx] = norm_data[j + i * nx] / sq_sum;
        }
    }

    // Multiply data and transpose

    for (int row1 = 0; row1 < ny; row1++) {

        for (int row2 = 0; row2 < ny; row2++) {

            if (row1 > row2) {
                continue;
            }

            double sum = 0;
            for (int k = 0; k < nx; k++) {

                sum = sum + norm_data[k + row1 * nx] * norm_data[k + row2 * nx];
            }
            result[row2 + row1 * ny] = (float)sum;
        }
    }

    free(norm_data);

}

void print_double_grid(double *grid, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            printf("%f ", grid[j + i * rows]);
        }
        printf("\n");
    }
}
