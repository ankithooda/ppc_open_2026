#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <chrono>


void step(float* r, const float* d, int n);

int main(int argc, char **argv) {

    int num;

    if (argc < 2) {
        num = 4000;
    } else {
        num = std::stoi(argv[1]);
    }

    std::cout << "Input size " << num << std::endl;


    std::string filename = "16million";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        return 1;
    }

    // Pre-allocate memory to avoid multiple reallocations
    std::vector<float> data;
    data.reserve(num * num);

    float temp;

    // Use >> operator to parse text into floats
    for (int l = 0; l < num * num; l++) {
        file >> temp;
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

    float *result_ptr = (float *)calloc(num * num, sizeof(float));


    auto start = std::chrono::steady_clock::now();

    // Call the step function which we want to benchmark
    step(result_ptr, data_ptr, num);

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> duration = end - start;

    float useful_ops = (num/1000.0 * num/1000.0 * num/1000.0)  * 2.0;

    std::cout << "Useful ops in Billion: " << useful_ops << std::endl;

    std::cout << "Time elapsed: " << duration.count() << " s" << std::endl;

    float ops_per_sec = useful_ops / duration.count();

    std::cout << "Useful ops (B) per sec: " << ops_per_sec << std::endl;

    return 0;
}
