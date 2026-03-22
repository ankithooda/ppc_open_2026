#!/bin/bash

rm ./bin/benchmark_test
g++ -o bin/benchmark_test benchmark.cc $1/step.cc
./bin/benchmark_test

