#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <direct.h>

#define Run  _stdcall

int arrayMultiply(double *array1, double *array2 ,int row1, int col1, int col2, double **result);
bool diffJudge(double *array1, double *array2 ,int row, int col);
int arrayTranspose(double *array1, int row1, int col1, double **result);
int arrayCopy(double *array, int n, double **result);
int arrayPrint(double *array, int row, int col);
int arrayNormalize(double **array, int n);

double Run factDecomp(int n, int T, double *price, double*D, double *V, double *F) {
	/*Initialization*/
	srand((unsigned)time(NULL));//Set seed for random
	
	int count =0 , count_limit = 1000, pcNum = 10;
	double *r,*avgR,*Q,*w,*w0, *wOld, *eigValue,  *Qcal;
	double *tempW = nullptr;
	double *tempT1 = nullptr,*tempT2 = nullptr,*tempWW = nullptr, *tempVF = nullptr, *Vtrans = nullptr, *tempVFV =nullptr;
	double tempSum = 0, tempQ  = 0 , temp = 0 , tempNorm = 0;
	r = (double *)calloc(n*(T-1),sizeof(double));
	avgR = (double *)calloc(n,sizeof(double));
	Q = (double *)calloc(n*n,sizeof(double));
	bool diff;

	/*Compute return of each stock r = (P_n - P_n-1)/P_n-1 */
	for (int h = 0 ; h < n; h++){
		tempSum = 0;
		for (int k = 0 ; k < T-1 ; k++){
			r[k + h * (T-1)] = (price[k + h * T +1] - price[k + h * T])/price[k + h * T];
			tempSum = tempSum + r[k + h * (T-1)];
		}
		avgR[h] = tempSum/(T - 1);
	}


	/*Compute the covariance matrix Q = E[(x-Ex)(y-Ey)]*/
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			tempQ  = 0;
			for ( int k = 0; k < T - 1; ++k){
				tempQ = tempQ + (r[k + i * (T-1)]  - avgR[i])*(r[k + j * (T-1)]  - avgR[j]);
			}
			Q[i*n + j] = tempQ/(T-2);
		}

	}

		

	/*Use power method to get the principal component*/
	//Initialization
	w = (double *)calloc(n,sizeof(double));
	w0 = (double *)calloc(n,sizeof(double));

	eigValue = (double *)calloc(pcNum,sizeof(double));
	Vtrans = (double *)calloc(n*pcNum,sizeof(double));
	
	
	Qcal = (double *)calloc(n*n,sizeof(double));
	tempT1 = (double *)calloc(1,sizeof(double));
	tempT2 = (double *)calloc(n,sizeof(double));

	wOld = (double *)calloc(n,sizeof(double));
	tempW = (double *)calloc(n,sizeof(double));
	tempWW = (double *)calloc(n,sizeof(double));

	tempVF = (double *)calloc(n*pcNum,sizeof(double));
	tempVFV = (double *)calloc(n*n,sizeof(double));
	

	//Generate a random array w0
	for (int i = 0 ; i < n; i++){
		w0[i] = rand()/(RAND_MAX+1.0);
		w[i] = w0[i];
	}


	//Copy Q to Qcal
	arrayCopy(Q, n*n,&Qcal);


	//Compute eigenvalue, eigenvector
	for (int h = 0; h < pcNum ; h++){
		count = 0;

		//Get a new vector that is orthogonal to all previous w as a start after the first iteration
		if (h != 0){
			//Generate new random w0


				//Compute wTw0w 
				arrayMultiply(w, w0 ,1, n, 1,&tempT1);
				arrayMultiply(tempT1, w ,1, 1, n, &tempT2);
				
				//Subtract wTw0w for each previous w from w0
				for (int ind = 0; ind < n; ind++){
					w0[ind] =  w0[ind] - tempT2[ind];
				}
			
			
			
			arrayCopy(w0, n, &w);

			//Q' = Q - lambda*w*wT
			for (int l = 0 ; l < n ; l++ ){
				for (int g = 0 ; g < n ; g++)
					Qcal[l*n + g] =  Qcal[l*n + g] - tempW[l]*tempW[g]*eigValue[h-1];
			}
			

		}


		//iteration begin
		while (count < count_limit){
			arrayCopy(w , n, &wOld);
			arrayMultiply(Qcal, w ,n, n, 1, &tempW);
			arrayNormalize(&tempW, n);
			arrayCopy(tempW, n, &w);
			 
			count = count + 1;
			
			//Break if the difference between the new w and the old w is very small
			diff = diffJudge( w,wOld,n,1);
			if (diff == true) {
				//printf("%d\n",count);
				break; }
			
		}
	   

		//Compute eigenvalue and store the eigenvector
		temp = 0;
		for (int j = 0 ; j <n ; j++){
			temp = temp + Q[j] * w[j];
			V[h + j*pcNum] = w[j];
		}
		eigValue[h] = temp/w[0];
			 
	}

	//Compute F
	for (int ind = 0 ; ind < pcNum; ind++){
		F[ind*(pcNum+1)] = eigValue[ind];
	}

	//Compute principal part of Q :VFV'
	arrayTranspose(V, n, pcNum,&Vtrans);
	arrayMultiply(V, F ,n, pcNum, pcNum, &tempVF);
	arrayMultiply(tempVF, Vtrans ,n, pcNum, n, &tempVFV);

	//Compute D and take the main diagonal of R
	/*for (int ind2 = 0 ; ind2 < n; ind2++){
		D[ind2*(n+1)] = Q[ind2*(n+1)] - tempVFV[ind2*(n+1)];
	}*/

	//Compute D (not take the main diagonal)
	for (int ind2 = 0 ; ind2 < n*n; ind2++){
		D[ind2] = Q[ind2] - tempVFV[ind2];
	}



	return 0;

}


//Function used to multiply two arrays(matrix)
int arrayMultiply(double *array1, double *array2 ,int row1, int col1, int col2, double **result){
	double temp = 0;


	for (int i = 0 ; i< row1 ; i++){
		for (int k = 0 ; k < col2 ; k++){
			temp = 0;
			for (int j = 0 ; j <col1 ; j++){
				temp = temp + array1[i*col1 + j] * array2[k + j*col2];
			}

			(*result)[i*col2 + k] = temp;
			
		}
		
	}
	return 0;
}






//Function used to calculate the transpose of a matrix
int arrayTranspose(double *array1, int row1, int col1, double **result){

	for (int i = 0; i < col1  ; i++){
		for ( int j = 0 ; j < row1 ; j++){
			(*result)[i*row1 + j] = array1[i + col1*j];
		}
	
	}

	return 0;

}

//Function used to print the result(for check purpose)
int arrayPrint(double *array, int row, int col){

	for (int i = 0 ; i < row* col; i++){
		if (i != 0 && i % col == 0)
			printf("\n");
		printf("%g  ",array[i]);
	
	}
	printf("\n\n");

	return 0;
}

//Function used to normalize the array(matrix)
int arrayNormalize(double **array, int n){
	double temp = 0;

	for (int i = 0 ; i < n ; i ++){
		temp = temp + (*array)[i]*(*array)[i];
	}

	for (int j = 0 ; j < n ; j ++){
		(*array)[j] = (*array)[j]/sqrt(temp);
	}

	return 0 ;

}

//Function used to copy the array
int arrayCopy(double *array, int n, double **result){

	
	for (int i = 0 ; i < n ; i++){
		(*result)[i] = array[i];
	}

	return 0;

}

//Function used to judge whether the difference between two arrays is small
bool diffJudge(double *array1, double *array2 ,int row, int col){
	double temp = 0;
	double *result;
	result = (double *)calloc(row* col, sizeof(double));

	for (int i = 0 ; i< row*col ; i++){
			result[i] =array1[i] - array2[i];
			
	}
	
	for (int i = 0 ; i < row* col ; i ++){
		temp = temp + abs(result[i]);
	}

	if (temp < 0.0001)
		return true;
	else
		return false;
}
