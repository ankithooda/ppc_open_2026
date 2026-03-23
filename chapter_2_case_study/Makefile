clean:
	rm bin/*

gen_16million:
	gcc -o bin/gen generate_large_array.c && ./bin/gen > 16million
bin:
	mkdir -p bin

test: test.sh
	./test.sh $(T)

benchmark:
	./benchmark.sh $(T) $(N)

asm:
	g++ -S -g -O3 -march=native -std=c++17 benchmark.cc $(T)/step.cc
