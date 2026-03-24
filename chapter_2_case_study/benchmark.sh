#!/bin/bash

rm ./bin/$1_benchmark_test
g++ -o bin/$1_benchmark_test -fopenmp -g -O3 -march=native -std=c++17 benchmark.cc $1/step.cc
./bin/$1_benchmark_test $2

