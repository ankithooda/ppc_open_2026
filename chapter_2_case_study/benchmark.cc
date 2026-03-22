#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <chrono>

#define N 4000

void step(float* r, const float* d, int n);

int main() {
    std::string filename = "16million";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        return 1;
    }

    // Pre-allocate memory to avoid multiple reallocations
    std::vector<float> data;
    data.reserve(N * N);

    float temp;

    // Use >> operator to parse text into floats
    while (file >> temp) {
        data.push_back(temp);
    }


    file.close();
    float *data_ptr = data.data();
    // std::cout << data_ptr[0] << std::endl;
    // std::cout << data_ptr[1] << std::endl;
    // std::cout << data_ptr[2] << std::endl;
    // std::cout << data_ptr[3] << std::endl;
    // std::cout << data_ptr[4] << std::endl;
    // std::cout << data_ptr[5] << std::endl;

    std::cout << "Loaded " << data.size() << " floats." << std::endl;


    // allocate memory for results

    float *result_ptr = (float *)calloc(N * N, sizeof(float));


    auto start = std::chrono::steady_clock::now();

    step(result_ptr, data_ptr, N);

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> duration = end - start;


    std::cout << "Time elapsed: " << duration.count() << " s" << std::endl;

    return 0;
}
