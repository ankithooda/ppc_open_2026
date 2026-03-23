#!/bin/bash

rm ./bin/benchmark_test
g++ -o bin/benchmark_test -g -O3 -march=native -std=c++17 benchmark.cc $1/step.cc
./bin/benchmark_test

