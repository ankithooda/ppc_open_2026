#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNT 16000000
//#define COUNT 10

int main() {
    // Seed the random number generator once at the start
    srand(time(NULL));

    // Dynamically allocate memory for 16 million floats
    float* random_floats = (float*)malloc(COUNT * sizeof(float));
    if (random_floats == NULL) {
        printf("Memory allocation failed\\n");
        return 1;
    }

    // Generate 16 million random floats in the range [0.0, 1.0)
    for (int i = 0; i < COUNT; i++) {
        // Generate a random integer and divide by RAND_MAX to get a float between 0.0 and 1.0
        random_floats[i] = (float)rand() / (float)RAND_MAX;
    }

    // Optional: Print a few values to verify
    for (int i = 0; i < COUNT; i++) {
         printf("%f\n", random_floats[i]);
    }

    // Free the dynamically allocated memory
    free(random_floats);

    return 0;
}
