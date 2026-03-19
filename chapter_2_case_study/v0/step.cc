#include <limits>

void step(float* r, const float* d, int n);

void step(float* r, const float* d, int n) {

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            float min_ij = std::numeric_limits<double>::infinity();

            for (int k = 0; k < n; k++) {
                float value = d[i + k * n] + d[k + j * n];

                if (value < min_ij) {
                    min_ij = value;
                }
            }

            // Set in result data for i, j
            r[i + j * n] = min_ij;
        }
    }
}
