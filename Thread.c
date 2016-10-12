#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define ThreadC 100
#define row 100
#define column 50

struct SubTotals
{
       int arraySub[row];
       pthread_mutex_t Ssub;
	   pthread_cond_t Fsub;
	   pthread_cond_t Esub;
	   int nextinSub, nextoutSub;
	   int occupiedS;
	 
};

struct Metrix_C
{		
        int arrayC[row][column];
        pthread_mutex_t SC;
		pthread_cond_t FC;
		pthread_cond_t EC;
		int nextinC, nextoutC;
		int occupiedC;
};

struct Metrix_A
{
        int arrayA[row][column];
        pthread_mutex_t SA;
		pthread_cond_t FA;
		pthread_cond_t EA;
		int nextinA, nextoutA;
		int occupiedA;
};

struct Metrix_B
{
        int arrayC[row][column];
        pthread_mutex_t SB;
		pthread_cond_t FB;
		pthread_cond_t EB;
		int nextinB, nextoutB;
		int occupiedB;
};

int K,M,N;
int metrix_A[row][column];
int metrix_B[row][column];
int metrix_C[row][column];

struct Metrix_C metrixC;
struct Metrix_A metrixA;
struct Metrix_B metrixB;
struct SubTotals subtotal;


void * producer(void *);
void * consumer1(void *);
void * consumer2(void *);
void * consumer3(void *);

pthread_t tid[ThreadC];
/* array of thread IDs */
main( int argc, char *argv[] )
{
	if(argc == 6)
	{

		//File Pointers
		FILE* pointerA=NULL;
		FILE* pointerB=NULL;

		//Variables Related to the 3 Metrix
		M = atoi(argv[3]);
		N = atoi(argv[4]);
		K = atoi(argv[5]);
		metrix_A[M][N];
		metrix_B[N][K];
		metrix_C[M][K];
		int r1,c1,r2,c2,i,j,z;
		
		//Variables Related to the Producer
		int size = row * column;
		
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
	
		pthread_cond_init(&(metrixC.FC), NULL);
		pthread_cond_init(&(metrixC.EC), NULL);

		pthread_create(&tid[0], NULL, producer, NULL);
		pthread_create(&tid[1], NULL, consumer1, NULL);
		pthread_create(&tid[2], NULL, consumer2, NULL);
		pthread_create(&tid[3], NULL, consumer3, NULL);


		for ( i = 0; i < M+1; i++)
		pthread_join(tid[i], NULL);
		printf("\nAll %d threads have terminated\n", i);
	}else{
		printf("Invalid Number of Arguments");
	}
exit(0);
}//Main End


void * producer(void * parm)
{	
	int size = row * column;
	int array1[M][K];
	int i,j;
	pthread_t id;
	id=pthread_self();
	for(i=0;i<M;i++){
		for(j=0;j<K;j++){
			array1[i][j] = metrix_C[i][j];
		}
	}
	printf("Producer Thread ID is : %u\n",id);
	for(i=0;i<M;i++){
		for(j=0;j<K;j++){ 
	pthread_mutex_lock(&(metrixC.SC));
	if (metrixC.occupiedC >= size)
	printf("producer waiting.\n");
	while (metrixC.occupiedC >= size)
	pthread_cond_wait(&(metrixC.EC),&(metrixC.SC) );
	printf("producer executing.\n");
	metrixC.arrayC[i][j] = array1[i][j];
	metrixC.nextinC++;
	metrixC.nextinC %= size;
	metrixC.occupiedC++;
	pthread_cond_signal(&(metrixC.FC));
	pthread_mutex_unlock(&(metrixC.SC));
	}
	}
	printf("Thread ID %u is exiting(Producer).\n",id);
	printf("\n");
	pthread_exit(0);
}


void * consumer1(void * parm)
{
	int size = row * column;
	int i,j,z,subtot;
	pthread_t id;
	id=pthread_self();
	int num[1][K];
	printf("Consumer1 Thread ID is : %u\n",id);
	for(z=0;z<1;z++){
		for(j=0;j<K;j++){
	pthread_mutex_lock(&(metrixC.SC) );
	if (metrixC.occupiedC <= 0) printf("consumer1 waiting.\n");
	while(metrixC.occupiedC <= 0)
	pthread_cond_wait(&(metrixC.FC),&(metrixC.SC) );
	printf("consumer1 executing.\n");
	num[z][j] = metrixC.arrayC[z][j];
	subtot = subtot + num[z][j];
	metrixC.nextoutC++;
	metrixC.nextoutC %= size;
	metrixC.occupiedC--;
	pthread_cond_signal(&(metrixC.EC));
	pthread_mutex_unlock(&(metrixC.SC));
	}
	}
	printf("Subtotal %d\n",subtot);
	printf("Thread ID %u is exiting(Consumer1).\n",id);
	printf("\n");
	pthread_exit(0);
}

void * consumer2(void * parm)
{
	int size = row * column;
	int i,j,z,subtot;
	pthread_t id;
	id=pthread_self();
	int num[1][K];
	printf("Consumer2 Thread ID is : %u\n",id);
	for(z=1;z<2;z++){
		for(j=0;j<K;j++){
	pthread_mutex_lock(&(metrixC.SC) );
	if (metrixC.occupiedC <= 0) printf("consumer2 waiting.\n");
	while(metrixC.occupiedC <= 0)
	pthread_cond_wait(&(metrixC.FC),&(metrixC.SC) );
	printf("consumer2 executing.\n");
	num[z][j] = metrixC.arrayC[z][j];
	subtot = subtot + num[z][j];
	metrixC.nextoutC++;
	metrixC.nextoutC %= size;
	metrixC.occupiedC--;
	pthread_cond_signal(&(metrixC.EC));
	pthread_mutex_unlock(&(metrixC.SC));
	}
	}
	printf("Subtotal %d\n",subtot);
	printf("Thread ID %u is exiting(Consumer2).\n",id);
	printf("\n");
	pthread_exit(0);
}

void * consumer3(void * parm)
{
	int size = row * column;
	int i,j,z,subtot;
	pthread_t id;
	id=pthread_self();
	int num[1][K];
	printf("Consumer3 Thread ID is : %u\n",id);
	for(z=2;z<3;z++){
		for(j=0;j<K;j++){
	pthread_mutex_lock(&(metrixC.SC) );
	if (metrixC.occupiedC <= 0) printf("consumer3 waiting.\n");
	while(metrixC.occupiedC <= 0)
	pthread_cond_wait(&(metrixC.FC),&(metrixC.SC) );
	printf("consumer3 executing.\n");
	num[z][j] = metrixC.arrayC[z][j];
	subtot = subtot + num[z][j];
	metrixC.nextoutC++;
	metrixC.nextoutC %= size;
	metrixC.occupiedC--;
	pthread_cond_signal(&(metrixC.EC));
	pthread_mutex_unlock(&(metrixC.SC));
	}
	}
	printf("Subtotal %d\n",subtot);
	printf("Thread ID %u is exiting(Consumer2).\n",id);
	printf("\n");
	pthread_exit(0);
}

