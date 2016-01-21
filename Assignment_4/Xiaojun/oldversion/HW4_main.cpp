
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int readData(char *filename, int *assetNum, int *dayNum, double **price);
double factDecomp(int n, int T, double *price, double *D, double *V, double *F, double *mu, int pcNum) ;
int arrayPrint(double *array, int row, int col);
int GurobiSolve(int n, int T, int pcNum, double *V, double *F, double *D, double *mu, double lambda,int nameLimit, double ubLimit, int constType);

int main(int argc, char **argv){

	int retcode = 0;
	int assetNum,dayNum;
	double *D = nullptr;
	double *F = nullptr;
	double *V = nullptr; 
	double *price = nullptr; 
	double *mu = nullptr;
	int pcNum = 10;
	double lambda = 1;
	int nameLimit = 10;
	double ubLimit = 0.2;
	int constType = 1;

	if(argc < 2){
    printf("usage: port filename\n"); retcode = 1; goto BACK;
	}


	readData(argv[1], &assetNum, &dayNum, &price);
	
	V = (double *)calloc(pcNum*assetNum,sizeof(double));
	F = (double *)calloc(pcNum*pcNum,sizeof(double));
	D = (double *)calloc(assetNum,sizeof(double));
	mu = (double *)calloc(assetNum,sizeof(double));

	factDecomp(assetNum, dayNum, price, D,  V,  F, mu, pcNum);

	////Print result
	//printf("V:\n");
	//arrayPrint(V, assetNum, pcNum);
	//printf("F:\n");
	//arrayPrint(F ,pcNum,pcNum);
	//printf("D:\n");
	//arrayPrint(D, assetNum, 1);


	printf("lambda = %g\n", lambda);
	GurobiSolve(assetNum, dayNum, pcNum, V, F, D, mu, lambda, nameLimit, ubLimit, constType );

	

	free(price);
	free(D);
	free(V);
	free(F);

	BACK:
		return retcode;
}


int readData(char *filename, int *assetNum, int *dayNum, double **price){

	int retcode = 0;
	char bufferRow[10000];
	char *buffer = NULL;
	int n = 0, T = 0;

	FILE *input = fopen(filename, "r");
	if(input == NULL) {
		 printf("cannot read file %s\n", filename); retcode = 1; goto BACK;
	}

	fscanf(input,"%s", bufferRow);
	fscanf(input,"%s", bufferRow);
	n = n + 1;
	buffer = strtok(bufferRow, ",");
	while (strtok(NULL, ",") != NULL){
			T =  T + 1;
	}

	while (fscanf(input,"%s", bufferRow)> 0){
		n = n + 1;
	}

	fclose(input);


	*assetNum = n;
	*dayNum = T;

	*price = (double *)calloc(n*T,sizeof(double));

	fopen(filename, "r");
	if(input == NULL) {
		 printf("cannot read file %s\n", filename); retcode = 1; goto BACK;
	}
	
	
	fscanf(input,"%s", bufferRow);
	

	for (int i = 0; i < n; i++){
		fscanf(input,"%s", bufferRow);
		strtok(bufferRow, ",");
		for (int j = 0; j < T; j++){
			buffer = strtok(NULL, ",");
			if (buffer != NULL)
				(*price)[j + i * T]  = atof(buffer);
		
		}
	}

	fclose(input);
	
	BACK:
		return retcode;

}