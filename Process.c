#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/stat.h>
#define row 100
#define column 50

struct SubTotals
{
       int arraySub[row];
        sem_t S, E, F;
};

struct Metrix_C
{
        int arrayC[row][column];
        sem_t SC, EC, FC;
};

struct Metrix_A
{
        int arrayA[row][column];
        sem_t SA, EA, FA;
};

struct Metrix_B
{
        int arrayB[row][column];
        sem_t SB, EB, FB;
};


int main(int argc, char *argv[])
{
	if(argc == 6)
	{
		//Define Structure Variables
		struct SubTotals subtotal;
		struct Metrix_C metrixC;
		struct Metrix_A metrixA;
		struct Metrix_B metrixB;
		
		//File Pointers
		FILE* pointerA=NULL;
		FILE* pointerB=NULL;
			
		//Variables Related to the 3 Metrix
		int M = atoi(argv[3]);
		int N = atoi(argv[4]);
		int K = atoi(argv[5]);
		int metrix_A[M][N];
		int metrix_B[N][K];
		int metrix_C[M][K];
		int r1,c1,r2,c2,i;

		//Variables Related to 4 Shared Memories
		int shmidA,shmidB,shmidC,shmidsub;
		key_t key;
		key_t keym;
		size_t SHMSZ = sizeof(subtotal);
		size_t SHMSZA = sizeof(metrixA);
		size_t SHMSZB = sizeof(metrixB);
		size_t SHMSZC = sizeof(metrixC);
		int sizeA  = row*column;
		int sizeB = row*column;
		int sizeC = row*column;
		int sizeSub = row;
			
		int sema1,sema2,sema3;
			
		//Variables Related to Child Processes
		int x=1;
		int w=0;
		int child_id;
		int Fulltotal=0;
		int j,z;
			
		//First Metric
		pointerA = fopen(argv[1], "r");

		for(r1=0;r1<M;r1++){
			for(c1=0;c1<N;c1++){
				fscanf(pointerA, "%d", &metrix_A[r1][c1]);
			}
		}

		//Printing methods Start (For the A  metrix)
		printf("Elements of the First Metric (Metric A)\n");
		for(r1=0;r1<M;r1++){
			for(c1=0;c1<N;c1++){
				printf("\t%d", metrix_A[r1][c1]);
			}
			printf("\n");
		}
		printf("\n");
		//prnting method close
		fclose(pointerA);



		//Second Metric
		pointerB = fopen(argv[2], "r");

		for(r2=0;r2<N;r2++){
			for(c2=0;c2<K;c2++){
				fscanf(pointerB, "%d", &metrix_B[r2][c2]);
			}
		}

		//Printing methods Start (For the B  metrix)
		printf("Elements of the Second Metric (Metric B)\n");
		for(r2=0;r2<N;r2++){
			for(c2=0;c2<K;c2++){
				printf("\t%d", metrix_B[r2][c2]);
			}
			printf("\n");
		}
		printf("\n");
		//prnting method close
		fclose(pointerB);

		//Multiply the above two Metrices and find the values of the third metrix
		for (i=0;i<r1;i++)
		{
			for (j=0;j<c2;j++)
			{
				metrix_C[i][j] = 0;
				for (int k=0;k<c1;k++)
				{
					metrix_C[i][j] = metrix_C[i][j] + metrix_A[i][k] * metrix_B[k][j];
				}

			}
			
		}

		//Printing methods Start(for the C metrix)
		printf("Elements of the Result Metric after the Multiplication(Metric C)\n");
		for(i=0;i<r1;i++){
			for(j=0;j<c2;j++){
				printf("\t%d", metrix_C[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		//Printing methods End

		
		//Create a Shared memory to Store the A Metrix
		if ((shmidA = shmget(key, SHMSZA, IPC_CREAT | 0666)) < 0) {
		perror("Shared Memory Creation error");
		exit(1);
		}
		//Link the Datastructure of Subtotal with the shared Memory
		struct Metrix_A *SharedMemA = shmat(shmidA, NULL, 0);
		//Create a Semaphore Value to Maintain the critical section and store the address in the shared Memory
		if((sema1=sem_init(&SharedMemA->SA,22,1)) == -1){
		perror("Semaphore Value for Critical Section is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Empty and store the address in the shared Memory
		if((sema2=sem_init(&SharedMemA->EA,22,sizeA)) == -1){
		perror("Semaphore Value to Check the Buffer is Empty is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Full and store the address in the shared Memory 
		if((sema3=sem_init(&SharedMemA->FA,22,0)) == -1){
		perror("Semaphore Value to Check the Buffer is Full is not Created");
		}
		
		
		//Create a Shared memory to Store the B Metrix
		if ((shmidB = shmget(key, SHMSZB, IPC_CREAT | 0666)) < 0) {
			perror("Shared Memory Creation error");
			exit(1);
		}
		//Link the Datastructure of Subtotal with the shared Memory 
		struct Metrix_B *SharedMemB = shmat(shmidB, NULL, 0);
		//Create a Semaphore Value to Maintain the critical section and store the address in the shared Memory
		if((sema1=sem_init(&SharedMemB->SB,22,1)) == -1){
		perror("Semaphore Value for Critical Section is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Empty and store the address in the shared Memory
		if((sema2=sem_init(&SharedMemB->EB,22,sizeB)) == -1){
		perror("Semaphore Value to Check the Buffer is Empty is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Full and store the address in the shared Memory
		if((sema3=sem_init(&SharedMemB->FB,22,0)) == -1){
		perror("Semaphore Value to Check the Buffer is Full is not Created");
		}
		
		
		//Create a Shared memory to Store the Sub Total
		if ((shmidsub = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
			perror("Shared Memory Creation error");
			exit(1);
		}
		//Link the Datastructure of Subtotal with the shared Memory
		struct SubTotals *SharedMemSub = shmat(shmidsub, NULL, 0);
		//Create a Semaphore Value to Maintain the critical section and store the address in the shared Memory
		if((sema1=sem_init(&SharedMemSub->S,22,1)) == -1){
		perror("Semaphore Value for Critical Section is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Empty and store the address in the shared Memory
		if((sema2=sem_init(&SharedMemSub->E,22,sizeC)) == -1){
		perror("Semaphore Value to Check the Buffer is Empty is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Full and store the address in the shared Memory
		if((sema3=sem_init(&SharedMemSub->F,22,0)) == -1){
		perror("Semaphore Value to Check the Buffer is Full is not Created");
		}


		//Create a Shared memory to Store the C Metrix
		if ((shmidC = shmget(keym, SHMSZC, IPC_CREAT | 0666)) < 0) {
			perror("Shared Memory Creation error");
			exit(1);
		}
		//Link the Datastructure of C Metrix with the shared Memory 
		struct Metrix_C *SharedMemC = shmat(shmidC, NULL, 0);
		//Create a Semaphore Value to Maintain the critical section and store the address in the shared Memory
		if((sema1=sem_init(&SharedMemC->SC,22,1)) == -1){
		perror("Semaphore Value for Critical Section is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Empty and store the address in the shared Memory
		if((sem_init(&SharedMemC->EC,22,sizeSub)) == -1){
		perror("Semaphore Value to Check the Buffer is Empty is not Created");
		}
		//Create a Semaphore Value to Identify the Buffer is Full and store the address in the shared Memory	
		if((sem_init(&SharedMemC->FC,22,0)) == -1){
		perror("Semaphore Value to Check the Buffer is Full is not Created");
		}

		//Put The Values of the A Metrix in to the Shared Memory
		for(i=0;i<M;i++){
			for(j=0;j<N;j++){
				sem_wait(&SharedMemA->EA);
				sem_wait(&SharedMemA->SA);
				SharedMemA->arrayA[i][j] = metrix_A[i][j];
				sem_post(&SharedMemA->SA);
				sem_post(&SharedMemA->FA);
			}
		}
		
		//Put The Values of the B Metrix in to the Shared Memory
		for(i=0;i<N;i++){
			for(j=0;j<K;j++){
				sem_wait(&SharedMemB->EB);
				sem_wait(&SharedMemB->SB);
				SharedMemB->arrayB[i][j] = metrix_B[i][j];
				sem_post(&SharedMemB->SB);
				sem_post(&SharedMemB->FB);
			}
		}
		
		//Put The Values of the C Metrix in to the Shared Memory
		for(i=0;i<M;i++){
			for(j=0;j<K;j++){
				sem_wait(&SharedMemC->EC);
				sem_wait(&SharedMemC->SC);
				SharedMemC->arrayC[i][j] = metrix_C[i][j];
				sem_post(&SharedMemC->SC);
				sem_post(&SharedMemC->FC);
			}
		}

		//Create New Child Processes
		for(i=0;i<M;i++){	
			if((child_id=fork())==0)
			{
				int stotal=0;
				int num[1][K];
				for(z=w;z<x;z++){
					for(j=0;j<K;j++){
						sem_wait(&SharedMemC->FC);
						sem_wait(&SharedMemC->SC);
						num[z][j] = SharedMemC->arrayC[z][j];
						sem_post(&SharedMemC->SC);
						sem_post(&SharedMemC->EC);
						stotal = stotal + num[z][j];
					}
				}
				int pid = getpid();
				sem_wait(&SharedMemSub->E);
				sem_wait(&SharedMemSub->S);
				SharedMemSub->arraySub[0] = pid;
				SharedMemSub->arraySub[1] = stotal;
				sem_post(&SharedMemSub->S);
				sem_post(&SharedMemSub->F);

				exit(0);
			}
			if(child_id>0)
			{
				int status;
				wait(&status);
				sem_wait(&SharedMemSub->F);
				sem_wait(&SharedMemSub->S);
				int pid = SharedMemSub->arraySub[0];
				int subTota = SharedMemSub->arraySub[1];
				sem_post(&SharedMemSub->S);
				sem_post(&SharedMemSub->E);
				printf("process ID %d\n",pid);
				printf("Sub Total %d\n",subTota);
				printf("\n");
				Fulltotal = Fulltotal+subTota;
			}
			x=x+1;
			w=w+1;
		}
		printf("Total of all the Sub Totals: %d\n",Fulltotal);   
	}
	else{
		printf("Invalid Number of Arguments");
	}
exit(0);
}//Main End

