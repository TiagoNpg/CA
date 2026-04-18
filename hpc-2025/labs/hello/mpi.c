#include <stdio.h>
#include <mpi.h> 

int main (int argc, char *argv[]) {

    int myrank, size;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get my rank
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // Get the total number of processors
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    printf("Processor %d of %d: Hello World!\n", myrank, size);

    // Terminate MPI
    MPI_Finalize();
}