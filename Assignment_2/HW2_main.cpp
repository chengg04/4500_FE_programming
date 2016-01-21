//Homework 2 main
//Name: Xiaojun Dong(xd2156) Yanwei Deng(yd2343) Julei Cao(jc4469) Zimeng Ji(zj2194) Cheng Guo(cg2917)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HW2_numAlgo.h"


int readData(char *filename, int *n, double **Q, double **mu, double **upperbound, double **lowerbound, double *lambda);
int numAlgo(int n, double *Q, double *mu, double *upperbound, double *lowerbound, double lambda, double *result);


int main(int argc, char* argv[]){

	int retcode = 0;
	int n;
	double *Q = nullptr;
	double *mu = nullptr;
	double *upperbound = nullptr;
	double *lowerbound = nullptr;
	double *result;
	double lambda = 1;
	if(argc != 2){
	  printf("Usage: HW2.exe datafilename\n"); retcode = 100; goto BACK;
	 } 
	readData(argv[1], &n, &Q, &mu, &upperbound, &lowerbound, &lambda);

	result = (double *)calloc(n,sizeof(double));

	numAlgo(n, Q, mu, upperbound, lowerbound, lambda, result);
	
	
BACK:
	return retcode;
}

int readData(char *filename, int *n, double **Q, double **mu, double **upperbound, double **lowerbound, double *lambda){
	int retcode = 0;
	FILE *in = NULL;
	in = fopen(filename, "r");
	char mybuffer[100];

	if (in == NULL){
	  printf("could not open %s for reading\n", filename);
	  retcode = 200; goto BACK;
    }


	fscanf(in, "%s", mybuffer);
    fscanf(in, "%s", mybuffer);
    *n = atoi(mybuffer);
	

	
	(*Q) = (double *)calloc( (*n)*(*n), sizeof(double));
    if ((*Q) == NULL){
	  printf("no memory\n"); retcode = 400; goto BACK;
    }

	fscanf(in, "%s", mybuffer);
	for (int k = 0 ; k < (*n)*(*n) ;  k++){
		 fscanf(in, "%s", mybuffer);
		 (*Q)[k] = atof(mybuffer);
	}

	
	(*mu) = (double *)calloc( (*n), sizeof(double));
    if ((*mu) == NULL){
	  printf("no memory\n"); retcode = 400; goto BACK;
    }

	fscanf(in, "%s", mybuffer);
	for (int k = 0 ; k < (*n) ;  k++){
		 fscanf(in, "%s", mybuffer);
		 (*mu)[k] = atof(mybuffer);
	}



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
	fscanf(in, "%s", mybuffer);
    fscanf(in, "%s", mybuffer);
    *lambda = atof(mybuffer);


BACK:
	return retcode;
}




