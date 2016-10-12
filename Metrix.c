#include<stdio.h>
int main(){

FILE* pointerA=NULL;
FILE* pointerB=NULL;

	int metrix_A[3][2];
	int metrix_B[2][4];
	int metrix_C[3][4];
	int r1,c1,r2,c2,i,j;

//First Metric
pointerA = fopen("matrix_A.txt", "r");

for(r1=0;r1<3;r1++){
	for(c1=0;c1<2;c1++){
		fscanf(pointerA, "%d", &metrix_A[r1][c1]);
	}
}
//Printing methods Start
printf("Elements of the First Metric (Metric A)\n");
for(r1=0;r1<3;r1++){
	for(c1=0;c1<2;c1++){
		printf("\t%d", metrix_A[r1][c1]);
	}
	printf("\n");
}
printf("\n");
//prnting method close
fclose(pointerA);



//Second Metric
pointerB = fopen("matrix_B.txt", "r");

for(r2=0;r2<2;r2++){
	for(c2=0;c2<4;c2++){
		fscanf(pointerB, "%d", &metrix_B[r2][c2]);
	}
}
//Printing methods Start
printf("Elements of the Second Metric (Metric B)\n");
for(r2=0;r2<2;r2++){
	for(c2=0;c2<4;c2++){
		printf("\t%d", metrix_B[r2][c2]);
	}
	printf("\n");
}
printf("\n");
//prnting method close
fclose(pointerB);


pid = fork();
printf("%d",pid);


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
//Printing methods Start
printf("Elements of the Result Metric after the Multiplication(Metric C)\n");
for(i=0;i<r1;i++){
	for(j=0;j<c2;j++){
		printf("\t%d", metrix_C[i][j]);
	}
	printf("\n");
}
printf("\n");
//Printing methods End







	return 0;
}