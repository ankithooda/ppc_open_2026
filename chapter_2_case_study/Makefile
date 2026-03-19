clean:
	rm bin/*

gen_16million:
	gcc -o bin/gen generate_large_array.c && ./bin/gen > 16million
bin:
	mkdir -p bin

test: test.sh
	./test.sh $(T)
