
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int readData(char *filename, int *assetNum, int *dayNum, double **price);
double factDecomp(int n, int T, double *price, double *D, double *V, double *F, double *mu, int pcNum) ;
int GurobiSolve(int n, int T, int pcNum, double *V, double *F, double *D, double *mu, double lambda,int nameLimit, double ubLimit, int constType);

int main(int argc, char **argv){
	/*--------------Initialization--------------*/
	int retcode = 0;
	int assetNum,dayNum;
	double *D = nullptr;
	double *F = nullptr;
	double *V = nullptr; 
	double *price = nullptr; 
	double *mu = nullptr;
	int pcNum = 10; //numbers of principal components
	double ubLimit = 0.02; //upperbound for each asset
	double lambda;
	int nameLimit=200;
	int constType=0; //constType = 0 means GRB_GREATER_EQUAL; constType = 1 means GRB_LESS_EQUAL in the contraints for name limits

	//Judge whether the input form is correct
	if(argc < 2){
    printf("usage: port filename\n"); retcode = 1; goto BACK;
	}

	printf("\nPlease enter the limit on the number of names: ");
	scanf("%d",&nameLimit);
	printf("Please enter the limit type for the number of names  \n (0 - GREATER_EQUAL, 1 - LESS_EQUAL): ");
	scanf("%d",&constType);
	while (constType != 0 && constType != 1){
		printf("Please enter 0 or 1: ");
		scanf("%d",&constType);
		if (constType == 0 || constType == 1)
			break;
	}
	/*--------------Read data from csv file--------------*/
	printf("Reading data...\n");
	readData(argv[1], &assetNum, &dayNum, &price);
	
	
	/*--------------Principal Component Decomposition --------------*/
	//Initialize V,F,D,mu
	V = (double *)calloc(pcNum*assetNum,sizeof(double));
	F = (double *)calloc(pcNum*pcNum,sizeof(double));
	D = (double *)calloc(assetNum,sizeof(double));
	mu = (double *)calloc(assetNum,sizeof(double));
	
	//Conduct principal component decomposition 
	printf("Principal Component Decomposition...\n");
	factDecomp(assetNum, dayNum, price, D,  V,  F, mu, pcNum);

	

	/*--------------Solve the problem using Gurobi--------------*/
	printf("Optimaztion...\n");
	//test with different lambda
	for (lambda = 1 ; lambda < 5; lambda = lambda + 1){
		printf("\n---------lambda = %g---------\n", lambda);
		GurobiSolve(assetNum, dayNum, pcNum, V, F, D, mu, lambda, nameLimit, ubLimit, constType );
	}
	

	//free the calloc space
	free(price);
	free(D);
	free(V);
	free(F);

	BACK:
		return retcode;
}


int readData(char *filename, int *assetNum, int *dayNum, double **price){
	/*--------------Initialization--------------*/
	int retcode = 0;
	char bufferRow[10000];
	char *buffer = NULL;
	int n = 0, T = 0;

	/*--------------Open file--------------*/
	FILE *input = fopen(filename, "r");
	if(input == NULL) {
		 printf("cannot read file %s\n", filename); retcode = 1; goto BACK;
	}

	/*--------------Get the asset number and day number--------------*/
	//Since csv file is a comma separated value file, we will have the whole row when we scan the file each time
	fscanf(input,"%s", bufferRow);//skip the first row 

	//get the asset number and day number
	fscanf(input,"%s", bufferRow);
	n = n + 1;
	//strtok function is used to get each value between comma(use bufferRow to get the first value and use NULL to get the rest values)
	buffer = strtok(bufferRow, ",");
	while (strtok(NULL, ",") != NULL){
			T =  T + 1;//count the day number when the value we can get is not NULL
	}

	while (fscanf(input,"%s", bufferRow)> 0){
		n = n + 1;//count the asset number when the value we can get is not NULL(if NULL fscanf() will return -1)
	}

	fclose(input);


	//record the asset number and day number
	*assetNum = n;
	*dayNum = T;
	//initialize the price array
	*price = (double *)calloc(n*T,sizeof(double));

	/*--------------Scan and store the price data--------------*/
	//start from the beginning
	input = fopen(filename, "r");
	if(input == NULL) {
		 printf("cannot read file %s\n", filename); retcode = 1; goto BACK;
	}
	
	
	fscanf(input,"%s", bufferRow);
	
	//scan and store the price data
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