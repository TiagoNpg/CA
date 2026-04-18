all: mpi openmp

mpi: mpi.c
	mpicc mpi.c -o mpi

openmp: openmp.c
	gcc -fopenmp openmp.c -o openmp

clean:
	rm -f mpi openmp
