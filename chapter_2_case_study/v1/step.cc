#include <limits>
#include <cstdlib>

void step(float* r, const float* d, int n);

void step(float* r, const float* d, int n) {

    // Allocate memory for transpose
    float *t = (float *)calloc(n * n, sizeof(float));

    // Create transpose
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            t[i + j * n] = d[j + i * n];
        }
    }

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            float min_ij = std::numeric_limits<double>::infinity();

            for (int k = 0; k < n; k++) {
                float value = d[k + i * n] + t[k + j * n];

                if (value < min_ij) {
                    min_ij = value;
                }
            }

            // Set in result data for i, j
            r[j + i * n] = min_ij;
        }
    }
}
