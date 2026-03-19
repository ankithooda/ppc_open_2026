#!/bin/bash

g++ -o bin/$1_test driver.cc $1/step.cc
./bin/$1_test > bin/test_output
cat bin/test_output

echo "Showing difference with expected output"
diff -q bin/test_output expected_output
