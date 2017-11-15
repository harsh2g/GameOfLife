//http://beige.ucs.indiana.edu/I590/node115.html



#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BORN 1
#define DIES 0

/* The Life function */
double life(int matrix_size, int ntimes, MPI_Comm comm) {

	int rank, size; //rank is the node which is working on the program, the size is the total nodes in the system
	int next, prev;
	int i, j, k;
	int mysize, sum;
	int **matrix, **temp, **addr;
	double slavetime, totaltime, starttime;
//  int      my_offset;

	/* Determine size and my rank in communicator */
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	/* Set neighbors */

	/*BUSHAN SHA
	 * Maybe I should just set this to wrap, then I will just use prev and next
	 * to coordinate which process gets what
	 */

	if (rank == 0)
		prev = size -1;
		//prev = MPI_PROC_NULL; // means there is no previous process
	else
		prev = rank - 1;
	if (rank == size - 1)
		next = 0;
		//next = MPI_PROC_NULL; // means there is no next process
	else
		next = rank + 1;

	/* Determine my part of the matrix */
	mysize = matrix_size / size; //+ ((rank < (matrix_size % size)) ? 1 : 0 ) ;
	/*
	 * rows
	 */
	/*my_offset = rank * (matrix_size/size);
	 if (rank > (matrix_size % size))
	 my_offset += (matrix_size % size);
	 else
	 my_offset += rank;*/

	/* allocate the memory dynamically for the matrix */
	matrix = (int **) malloc(sizeof(int *) * (mysize + 2));
	temp = (int **) malloc(sizeof(int *) * (mysize + 2));
	addr = (int **) malloc(sizeof(int *) * (mysize + 2));

	for (i = 0; i < mysize + 2; i++) {
		matrix[i] = (int *) malloc(sizeof(int) * (matrix_size + 2));
		temp[i] = (int *) malloc(sizeof(int) * (matrix_size + 2)); //columns
		addr[i] = (int *) malloc(sizeof(int) * (matrix_size + 2));

	}

	/* Initialize the boundaries of the life matrix *//*
	 for (j = 0; j < matrix_size+2; j++)
	 matrix[0][j] = matrix[mysize+1][j] = temp[0][j] = temp[mysize+1][j] = DIES ;
	 for (i = 0; i < mysize+2; i++)
	 matrix[i][0] = matrix[i][matrix_size+1] = temp[i][0] = temp[i][matrix_size+1] = DIES ;*/

	/* Initialize the life matrix */

	srand(time(NULL));

	for (i = 0; i < mysize; i++) { //rows
		for (j = 0; j < matrix_size; j++) { //collumns
			if ((rand() % 2) == 1)
				matrix[i][j] = BORN;
			else
				matrix[i][j] = DIES;
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	for (i = 0; i < 2; i++) {
		for (j = 0; j < matrix_size; j++) {
			addr[i][j] = DIES;
			printf("%d ", addr[i][j]);
		}
		printf("\n");

	}

	printf("\n");

	/* Play the game of life for given number of iterations */
	starttime = MPI_Wtime() ;
	int mSize = matrix_size - 1;

	for (k = 0; k < ntimes; k++) {

		MPI_Request      req[4];
		MPI_Status       status[4];

		 /*Send and receive boundary information
		 MPI_Isend(&matrix[1][0],matrix_size+2,MPI_INT,prev,0,comm,req);
		 MPI_Irecv(&matrix[0][0],matrix_size+2,MPI_INT,prev,0,comm,req+1);
		 MPI_Isend(&matrix[mysize][0],matrix_size+2,MPI_INT,next,0,comm,req+2);
		 MPI_Irecv(&matrix[mysize+1][0],matrix_size+2,MPI_INT,next,0,comm,req+3);
		 MPI_Waitall(4, req, status);*/



		/*
		 * THIS ISNT RIGHT YET....NEED TO MODIFY COE ABOVE (NEXT AND PREVIOUS)
		 */
		for (int loop = 0; loop < matrix_size; loop++) {
			 /*Send and receive boundary information*/
					 MPI_Isend(&matrix[0][loop],1,MPI_INT,prev,0,comm,req);
					 MPI_Irecv(&addr[0][loop],1,MPI_INT,prev,0,comm,req+1);
					 MPI_Isend(&matrix[matrix_size -1][loop],1,MPI_INT,next,0,comm,req+2);
					 MPI_Irecv(&matrix[1][loop],1,MPI_INT,next,0,comm,req+3);
					 MPI_Waitall(4, req, status);

/*
			//MPI_Send(&matrix[loop][0], 1, MPI_INT, rank -1, 0, MPI_COMM_WORLD);//left side
			MPI_Send(&matrix[0][loop], 1, MPI_INT, prev, 0, MPI_COMM_WORLD); //top of array to array above

			MPI_Recv(&addr[0][loop], 1, MPI_INT, next, 0, MPI_COMM_WORLD,
								MPI_STATUS_IGNORE);    //bottom of the array before it. IE the shared top

			//MPI_Send(&matrix[0][loop], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);//top of array to array below
			//MPI_Send(&matrix[matrix_size -1][loop], 1, MPI_INT, rank -1, 0, MPI_COMM_WORLD);//bottom
			MPI_Send(&matrix[matrix_size - 1][loop], 1, MPI_INT, next, 0,
					MPI_COMM_WORLD);    //bottom
			//MPI_Send(&matrix[loop][matrix_size -1], 1, MPI_INT, rank -1, 0, MPI_COMM_WORLD);//right


			MPI_Recv(&addr[1][loop], 1, MPI_INT, prev, 0, MPI_COMM_WORLD,
					MPI_STATUS_IGNORE);    //top of the array after it IE the bottom shared

			//MPI_Recv(&addr[2][loop], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//MPI_Recv(&addr[3][loop], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/

		}

		for (int loop = 0; loop < matrix_size; loop++){
			printf("%d ",addr[0][loop]);
		}
		printf("\n");


		/* For each element of the matrix ... */
		for (i = 0; i < mysize; i++) {
			for (j = 0; j < matrix_size; j++) {

				/* find out the value of the current cell */
				if (i == 0) {
					if (j == 0 || j == matrix_size - 1) {
						//corner info
						if (j == 0)
							sum += (addr[0][mSize] + matrix[i][mSize]
									+ matrix[i + 1][j] + addr[1][mSize]
									+ addr[0][0] + addr[0][j + 1]
									+ matrix[i][j + 1] + matrix[i +1][j + 1]);
						/*
						 * top left, leftWrap + bottom + bot left + top + top right + right + bottom right
						 */
						else
							sum += (addr[0][j] + matrix[i][0] + addr[0][0]
									+ addr[1][0] + addr[1][j -1] + addr[0][j - 1]
									+ addr[1][mSize] + matrix[i][j - 1]);
						// top + neighbor(side right) + cornerT + cornerB
						// bottomL + top L + bot + left
					} else {
						//for the rest of top row
						//top + top left + top right + left and right + bot + botL + botR
						sum += addr[i][j] + addr[i][j - 1] + addr[i][j + 1]
								+ matrix[i][j - 1] + matrix[i][j + 1]
								+ matrix[i + 1][j] + matrix[i + 1][j - 1]
								+ matrix[i + 1][j + 1];

						/*
						 * This will not work for all cases, but for the assignment, 1000 by 1000 it will
						 * (1 row per node is out of bounds)
						 */
					}

				} else if (i == matrix_size - 1) {
					//bottom border info
					if (j == 0 || j == matrix_size - 1) {
						if(j == 0){
						sum += matrix[i][mSize] + addr[0][mSize]
								+ addr[1][mSize] + addr[1][j]
								+ addr[1][j+1] + matrix[i][j+1]
							    + matrix[i-1][j+1] + matrix[i-1][j];
						/*
						 * LeftW + topL + botL + bot + botR + right + topR + top
						 */
						}
					} else {
						sum += addr[1][j] + addr[1][0]
							+ addr[1][j - 1] + matrix[i-1][0]
							+ matrix[i-1][j] + matrix[i-1][j-1]
							+ matrix[i][j-1] + matrix[i][0];
						/*
						 * bot + botR + botL + topR + top + topL + left + rightW
						 */
					}
				} else if (j == 0 || j == matrix_size - 1) {
					if (j == matrix_size - 1) {
						//left and right side info
						sum += matrix[i][0] + matrix[i-1][0]
								+ matrix[i+1][0] + matrix[i - 1][j]
								+ matrix[i - 1][j - 1] + matrix[i + 1][j - 1]
								+ matrix[i][j - 1] + matrix[i + 1][j];
						/*
						 * RightW + topR + botR + top +  topL + botL + left + bot
						 */
					} else {
						sum += matrix[i][mSize] + addr[0][mSize]
								+ addr[1][mSize] + matrix[i-1][j]
								+ matrix[i-1][j+1] + matrix[i][j+1]
								+matrix[i+1][j+1] + matrix[i+1][j];
						/*
						 * LeftW + topL + botL + top + topR + right + botR + bot
						 */
					}
				} else {
					//inner matrix
					sum += matrix[i][j+1] + matrix[i][j-1]
						+ matrix[i-1][j+1] + matrix[i-1][j-1]
						+ matrix[i-1][j] + matrix[i+1][j+1]
						+ matrix[i+1][j-1] + matrix[i+1][j];
					/*
					 * right + left + topR + topL + top + BotR + botL + bot
					 */
				}

				/*
				 sum = matrix[i-1][j-1] + matrix[i-1][j] + matrix[i-1][j+1]
				 + matrix[i][j-1] + matrix[i][j+1]
				 + matrix[i+1][j-1] + matrix[i+1][j] + matrix[i+1][j+1] ;*/

				/* check if the cell dies or life is born */
				if (sum < 2 || sum > 3)
					matrix[i][j] = DIES;
				else if (sum == 3)
					matrix[i][j] = BORN;
				//else
				//	matrix[i][j] = matrix[i][j];
				printf("%d|%d ", matrix[i][j], sum);
				sum = 0;
			}
			printf("\n");
		}
		printf("\n");

		/* Swap the matrices */
		/*addr = matrix ;
		 matrix = temp ;
		 temp = addr ;*/
	}

	/* Return the average time taken/processor */
	slavetime = MPI_Wtime() - starttime;
	MPI_Reduce(&slavetime, &totaltime, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

	return (totaltime / (double) size);
}

int main(argc, argv)
	int argc;char *argv[]; {
	int rank, N, iters;
	double time;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* If I'm process 0, determine the matrix size and # of iterations */
	/* This relies on the MPI implementation properly flushing output
	 that does not end in a newline.  MPI does not require this, though
	 high-quality implementations will do this.
	 */
	if (rank == 0) {
		printf("Matrix Size : ");
		scanf("%d", &N);
		printf("Iterations : ");
		scanf("%d", &iters);
	}

	/* Broadcast the size and # of iterations to all processes */
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&iters, 1, MPI_INT, 0, MPI_COMM_WORLD);

	/* Call the life routine */
	time = life(N, iters, MPI_COMM_WORLD);

	/* Print the total time taken */
	if (rank == 0)
		printf("[%d] Life finished in %lf seconds\n", rank, time / 100);

	//MPE_Close_graphics(&graph);
	MPI_Finalize();
}
