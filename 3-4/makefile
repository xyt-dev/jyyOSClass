test2: test2.cpp
	clang++ test2.cpp -o test.out && ./test.out

test2-strace: test2.cpp
	clang++ test2.cpp -o test.out && strace ./test.out

test3: test3.cpp
	clang++ -O2 test3.cpp -o test.out
	./test.out