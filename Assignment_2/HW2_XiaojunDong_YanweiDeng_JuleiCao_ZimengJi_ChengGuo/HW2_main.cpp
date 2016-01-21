//Homework 2 main
//Name: Xiaojun Dong(xd2156) Yanwei Deng(yd2343) Julei Cao(jc4469) Zimeng Ji(zj2194) Cheng Guo(cg2917)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HW2_numAlgo.h"


int readData(char *filename, int *n, double **Q, double **mu, double **upperbound, double **lowerbound, double *lambda);
int numAlgo(int n, double *Q, double *mu, double *upperbound, double *lowerbound, double lambda, double *result);
double objectValue ( int n, double *Q, double *mu, double lambda, double *result, int type);

int main(int argc, char* argv[]){
	/*----Initialization---*/
	int retcode = 0;
	int n,r;
	double *Q = nullptr;
	double *mu = nullptr;
	double *upperbound = nullptr;
	double *lowerbound = nullptr;
	double *result;
	double lambda = 1;
	double objValue = 0, variance = 0;

	/*----Judge whether the form of input is right----*/
	if(argc != 2){
	  printf("Usage: HW2.exe datafilename\n"); retcode = 100; goto BACK;
	 }

	/*----Read the data from txt file----*/
	readData(argv[1], &n, &Q, &mu, &upperbound, &lowerbound, &lambda);


	/*----Initialize the result vector and run the algorithm----*/
	result = (double *)calloc(n,sizeof(double));
	r = numAlgo(n, Q, mu, upperbound, lowerbound, lambda, result);
	

	/*----Output the result----*/
	if (r == 0){ //r = 0 means that it calculated successfully
	
		variance = objectValue(n, Q, mu, lambda, result,2);
		objValue = objectValue(n, Q, mu, lambda, result,1);

		printf("Final Result:\n");
		for ( int x = 0 ;x<n; x++){
	
		printf("x%d:  %g\n", x+1 , result[x]);
	
		}

		printf("Final Object Value: %g \n",objValue);
		printf("Final Portfolio Variance: %g \n",variance);
	
	}else if (r == -1){  //r = -1 means that the problem is infeasible
		printf("The problem is infeasible.\n");
	}
	

BACK:
	return retcode;
}

int readData(char *filename, int *n, double **Q, double **mu, double **upperbound, double **lowerbound, double *lambda){
	/*----Initialization---*/
	int retcode = 0;
	FILE *in = NULL;
	in = fopen(filename, "r");
	char mybuffer[100];


	/*----Check whether the file has been opened successfully----*/
	if (in == NULL){
	  printf("could not open %s for reading\n", filename);
	  retcode = 200; goto BACK;
    }


	/*----Read data----*/
	fscanf(in, "%s", mybuffer);
    fscanf(in, "%s", mybuffer);
    *n = atoi(mybuffer); // read n
	

	 // read Q
	(*Q) = (double *)calloc( (*n)*(*n), sizeof(double));
    if ((*Q) == NULL){
	  printf("no memory\n"); retcode = 400; goto BACK;
    }

	fscanf(in, "%s", mybuffer);
	for (int k = 0 ; k < (*n)*(*n) ;  k++){
		 fscanf(in, "%s", mybuffer);
		 (*Q)[k] = atof(mybuffer);
	}

	
	// read mu
	(*mu) = (double *)calloc( (*n), sizeof(double));
    if ((*mu) == NULL){
	  printf("no memory\n"); retcode = 400; goto BACK;
    }

	fscanf(in, "%s", mybuffer);
	for (int k = 0 ; k < (*n) ;  k++){
		 fscanf(in, "%s", mybuffer);
		 (*mu)[k] = atof(mybuffer); 
	}


	// read upperbound and lowerbound
	(*upperbound) = (double *)calloc( (*n), sizeof(double));
	(*lowerbound) = (double *)calloc( (*n), sizeof(double));

	for (int k = 0 ; k < (*n) ;  k++){
		fscanf(in, "%s", mybuffer);
		fscanf(in, "%s", mybuffer);
		(*lowerbound)[k] = atof(mybuffer);
		fscanf(in, "%s", mybuffer);
		fscanf(in, "%s", mybuffer);
		(*upperbound)[k] = atof(mybuffer);
	}
	
	// read lambda
	fscanf(in, "%s", mybuffer);
    fscanf(in, "%s", mybuffer);
    *lambda = atof(mybuffer);


BACK:
	return retcode;
}




