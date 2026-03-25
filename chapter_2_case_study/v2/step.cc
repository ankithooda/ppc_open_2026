#include <limits>
#include <cstdlib>
#include <vector>
#include <ostream>
#include <iostream>

void step(float* r, const float* d, int n);
void print_grid(std::vector<float> grid, int rows, int cols);

void step(float* r, const float* d, int n) {

    //print_grid(d, n, n);

    constexpr float infinity = std::numeric_limits<float>::infinity();
    constexpr int pad = 4;

    int padded_cols = (n + pad - 1) / pad;
    padded_cols = padded_cols * pad;

    // std::cout << infinity << std::endl;
    // std::cout << n << std::endl;
    // std::cout << padded_cols << "  ----"<< std::endl;

    // Allocate memory for transpose
    std::vector<float> padded_data(padded_cols * n, infinity);

    std::vector<float> padded_transpose(padded_cols * n, infinity);

    // print_grid(padded_data, n, padded_cols);

    // print_grid(padded_transpose, n, padded_cols);


    // Update Padded data and transpose
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            padded_data[i + j * padded_cols] = d[i + j * n];
            padded_transpose[j + i * padded_cols] = d[i + j * n];
        }
    }

    //print_grid(padded_data, n, padded_cols);

    //print_grid(padded_transpose, n, padded_cols);

    // Main computation loop
    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            // We maintain 4(pad) min values
            float min_values[pad];

            // set infinity guard value
            for (int m = 0; m < pad; m++) {
                min_values[m] = infinity;
            }

            for (int k = 0; k < padded_cols; k = k+4) {

                for (int m = 0; m < pad; m++) {
                    float value = padded_data[k + m + i * padded_cols] + padded_transpose[k + m + j * padded_cols];
                    min_values[m] = std::min(min_values[m], value);
                }
            }

            // Set in result data for i, j
            float min_ij = infinity;

            for (int m = 0; m < pad; m++) {
                min_ij = std::min(min_ij, min_values[m]);
            }

            r[j + i * n] = min_ij;
        }
    }
}

void print_grid(std::vector<float> grid, int rows, int cols) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << grid[i + j * cols] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "##################" << std::endl;
}
