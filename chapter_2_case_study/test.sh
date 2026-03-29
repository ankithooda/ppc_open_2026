#!/bin/bash

g++ -o bin/$1_test -march=native driver.cc $1/step.cc
./bin/$1_test $2 > bin/test_output_$2

echo "Your Output"
cat bin/test_output_$2

echo "Showing difference with expected output"
diff -q bin/test_output_$2 expected_output_$2
