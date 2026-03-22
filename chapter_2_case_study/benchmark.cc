#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define N 4000

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
    std::cout << data[0] << std::endl;
    std::cout << data[1] << std::endl;
    std::cout << data[2] << std::endl;
    std::cout << data[3] << std::endl;
    std::cout << data[4] << std::endl;
    std::cout << data[5] << std::endl;

    std::cout << "Loaded " << data.size() << " floats." << std::endl;
}
