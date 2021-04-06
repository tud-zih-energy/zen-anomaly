all:
	g++ -O0 -pthread workload.cpp -o compute
	gcc powerread.c -lsensors -lx86_adapt -o powerread
