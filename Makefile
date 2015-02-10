all:
	./build.sh

run:
	LD_LIBRARY_PATH=. ./main

gdb:
	LD_LIBRARY_PATH=. gdb ./main
