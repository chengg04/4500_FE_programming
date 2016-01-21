#include "qp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>

int read_data(char *filename, int *n, double *lambda, double *Q, double *mu, double *l, double *u);
double Min(int n, double lambda, double Q, double mu, double l, double u);

int main(int argc, char* argv[])//add argc argv to process command line arguments
/*when argc(argument count)=1, it will be stored in argv[0]. */
{
	int n;
	double lambda;
	double *Q;
	VectorXd mu, l, u;
	/*double optv;
	optv = Min();
	printf("The minimum value is %lf\n",optv);*/
	read_data(argc, argv[1], &n, &lambda, &Q, &mu, &l, &u);
	Min(n, lambda, Q, mu, l, u);

}
	

int read_data(int argc, char* argv[], int* n, double* lambda, MatrixXd* Q, VectorXd* mu, VectorXd* l, VectorXd* u)
{
	int i, j, k1, k2;
	int retcode = 0;
	FILE *in=NULL;//read in argv[1]
	char mybuffer[100];

	if (argc != 2){
		printf("Usage: qp.exe datafilename\n"); retcode = 100; goto BACK;
	}

	in = fopen(argv[1], "r");
	if (in == NULL){
		printf("could not read %s\n", argv[1]);
		retcode = 200; goto BACK;
	}

	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	*n = atoi(mybuffer);
	fscanf(in, "%s", mybuffer);

	for (i = 0; i <= (*n) - 1; i++){
		for (j = 0; j <= (*n) - 1; j++){
			fscanf(in, "%s", mybuffer);
			(*Q).block<1,1>(i,j) << atof(mybuffer);

		}
	}
	fscanf(in, "%s", mybuffer);
	for (k1 = 0; k1 <= (*n) - 1; k1++){
		fscanf(in, "%s", mybuffer);
		(*mu).segment<1>(k1) << atof(mybuffer);
	}
	for (k2 = 0; k2 <= 4 * (*n) - 1; k2++){
		fscanf(in, "%s", mybuffer);
		fscanf(in, "%s", mybuffer);
		(*l).segment<1>((k2 - 1) / 4) << atof(mybuffer);
		fscanf(in, "%s", mybuffer);
		fscanf(in, "%s", mybuffer);
		(*u).segment<1>((k2 - 3) / 4) << atof(mybuffer);
	}
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	*lambda = atof(mybuffer);
	fclose(in);

BACK:
	return retcode;

}

