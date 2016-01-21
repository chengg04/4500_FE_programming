
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



#define min(x, y)(x<y?x:y)
double sumArray(double *array, int n);
double mtplMtrx(double * array1, double* array2, int n1, int n2);

//use qp to minimize
double Min(int n, double lambda, double* Q, double* mu, double* l, double* u)
{
	double* x;
	double epsilon,epsilonp,epsilonm, A, B;
	double objValue;

	/*----Achieving feasibility----*/
	if (sumArray(l,n)>1 || sumArray(u,n) < 1){
		printf("The problem is infeasible/n");
		return 0;
	}

	//initialize x
	x = l;
	//get the feasible x
	for (int k=0; k <= n - 1; k++){
		double difValue = 1 - sumArray(x,n);
		double difBound = u[k]-l[k];
		x[k] = l[k]+min(difValue, difBound);
	}
	/*-----Improvement Phase------*/
	for (int k=0; k < 10; k++){
		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++){
				if (i != j){
					A = coefA(Q, i, j, n, lambda);
					B = coefB(Q, lambda, i, j, n, x, mu);
					epsilon = -B / (2 * A);
					epsilonp = min(x[i - 1] - l[i - 1], u[j - 1] - x[j - 1]);
					epsilonm = min(u[i - 1] - x[i - 1], x[j - 1] - l[j - 1]);
					if (epsilon < -epsilonm)
						epsilon = -epsilonm;
					else if (epsilon>epsilonp)
						epsilon = epsilonp;
					objValue = A*epsilon*epsilon + B*epsilon;
					x[i - 1] = x[i - 1] - epsilon;
					x[j - 1] = x[j - 1] + epsilon;
				}
				
			}
		}
		
	}
	return 0;
}

double sumArray(double *array, int n){
	double sum = 0;
	for (int i = 0; i < n; i++){
		sum = sum + array[i];
	}
	return sum;
}

double mtplMtrx(double * array1, double* array2, int n1, int n2){
	//n1 is the number of rows in matA, n2 is the number of columns of matB
	double *result;
	result = (double*)calloc(n1*n1, sizeof(double));
	for (int k = 0; k < n1; k++){
		for (int i= 0; i < n1; i++){
			double sum = 0;
			for (int j = 0; j < n2; j++){
				sum = sum+array1[k*n2+j]*array2[j*n1+i];
			}
			result[k*n1 + i] = sum;
		}	
	}
	return *result;
}

double coefA(double *Q, int h, int k, int n, double lambda){
	double result;
	result = lambda*(Q[(n+1)*(h-2)] + Q[(n+1)*(k-2)] - 2 * Q[(k-2)*n+h-2]);
	return result;
}

double coefB(double *Q, double lambda, int h, int k, int n,double *x, double *mu){
	double result, sum=0;
	for (int i = 0; i < n*n; i++){
		if (i = h | k)
			sum = sum + 0;
		else{
			sum = sum + x[i-1] * (Q[n*(k-2) + i-1] - Q[n*(h-2) + i-1]);
		}
	}
	result = 2 * lambda*(-Q[(n + 1)*(h - 2)] * x[h - 1] + Q[(n + 1)*(k - 2)] * x[k - 1] + Q[(k - 2)*n + h - 2] * x[h - 1] * x[k - 1] + sum) + mu[h - 1] - mu[k - 1];
	return result;
}
