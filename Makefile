./main:
	gcc main.c matrix.c -o main

run_standard: ./main
	./main standard

run_recursive: ./main
	./main recursive