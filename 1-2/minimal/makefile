minimal: minimal.c
	gcc -c minimal.c
	ld minimal.o -o minimal.out
	objdump -d minimal.out > minimal_out-disas.txt
	./minimal.out

minimal_hello: minimal_hello.s
	as -o minimal_hello.o minimal_hello.s
	ld -o minimal_hello.out minimal_hello.o
	./minimal_hello.out; echo "status: $$?" 

hello: hello.c
	gcc -static hello.c -o hello.out
	./hello.out; echo "status: $$?"