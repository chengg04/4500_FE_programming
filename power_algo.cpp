#include <stdlib.h>
#include <stdio.h>

double power(int* n, int* T, double** p)
{
	double* Q;
	double* F;
	double* V;
	double* D;
	double* mean_r;
	double* r;
	double sumr=0, sumco=0;
	(*p) = (double*)calloc((*n)*(*T), sizeof(double));
	r = (double*)calloc((*n)*(*T - 1), sizeof(double));
	Q = (double*)calloc((*n)*(*n), sizeof(double));
	F = (double*)calloc(10 * 10, sizeof(double));
	V = (double*)calloc((*n) * 10, sizeof(double));
	D = (double*)calloc(10 * 10, sizeof(double));
	mean_r =(double*)calloc((*n), sizeof(double));

	for (int i = 0; i < (*n); i++){
		for (int j = i*(*T); j < i*(*T + 1); j++){
			r[j - i] = (*p[j + 1] - *p[j]) / (*p[j]);
		}
	}

	for (int k = 0; k < (*n); k++){
		mean_r[k] = average_array(*T-1,r[;
	}

	for (int j = 0; j < (*n); j++){	
		for (int i = 0; i <(*n); i++){	
			for (int k = j*(*T - 1); k < (j + 1)*(*T - 1); k++){
				sumco = 0;
				sumco = sumco + (r[i] - mean_r[j])*(r[k*(*T - 1) + i % (*T - 1)] - mean_r[k]);
				Q[i] = sumco / (*T - 2);
			}
		}
	}

}

//define a function for the average of array a's t items, where a is double array
double average_array(int t, double *a){
	double sum_a, mean_a;
	sum_a = 0;
	a = (double*)calloc(t, sizeof(double));
	for (int k = 0; k < t;k++){
		sum_a = sum_a + a[k];
	}
	mean_a = sum_a / t;
	return mean_a;
}
