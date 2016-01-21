//Homework 2 numerical algorithm
//Name: Xiaojun Dong(xd2156) Yanwei Deng(yd2343) Julei Cao(jc4469) Zimeng Ji(zj2194) Cheng Guo(cg2917)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define min(x,y)  ( x < y ? x :y )

double sumArray(double *array, int n);
double coefB ( int n, double *Q, double *mu, double lambda, double *result, int k, int h);
double objectValue ( int n, double *Q, double *mu, double lambda, double *result, int type);

int numAlgo(int n, double *Q, double *mu, double *upperbound, double *lowerbound, double lambda, double *result){
	/*--------Initialization--------*/
	double feaSum = 0, tempVal = 0, diff = 0;
	double scale = 5.0;
	double epsilon = 0 , a = 0, b = 0, epsilonMax = 0, epsilonMin = 0, objValue = 0,  variance = 0;
	int count = 0, ind = 0, count_loop = 0, testCount = 0;


	/*---------Achieve feasibility--------*/

	//Check upper bound and lower bound and judge whether the problem is feasible or not 
	if (sumArray(lowerbound,n) > 1 || sumArray(upperbound,n) < 1){
		return -1; //if it is not feasible, return -1
	}
	

	//Find one feasibile solution
	while (feaSum < 1){
		ind = count % n;
		diff = 1 - sumArray(result,n);
		tempVal = upperbound[ind] -  lowerbound[ind];
		//start from the lowerbound, then add a percentage of the difference between upperbound and lowerbound each time
		//if count/n is bigger than scale, which means it reaches the upperbound, this result stops adding any more
		result[ind] = lowerbound[ind] + tempVal/scale*(count/n) * (count/n  <= scale); 
		//when count< n, which means it is the first loop, we only want the lowerbound
		//when count>= n, we only add the temporary part to the feaSum(the lowerbound part will not be added again) 
		feaSum = feaSum + lowerbound[ind]* (count < n) + tempVal/scale * (count >= n && count/n  <= scale);
		count = count + 1;
		}
	//subtract the redundant part of the last result
	result[ind] = 1 - sumArray(result,n) + result[ind] ;

	//print the feasible solution and initial objective value
	printf("One feasible solution to the problem:\n");
	for ( int x = 0 ;x<n; x++){
	
	printf("x%d:  %g\n", x+1 , result[x]);   // %g-double?
	
	}
	objValue = objectValue(n, Q, mu,lambda, result,1);
	variance = objectValue(n, Q, mu,lambda, result,2);
	printf("Object Value: %g \n",objValue);
	printf("Portfolio Variance: %g \n\n",variance);

	/*--------- Improvement phase --------*/
	//set 1000 to control the times of improvement
	while(count_loop < 1000){
		//result h - epsilon, result k + epsilon, choose epsilon to minimize the objective value
		for (int h = 0 ; h <n - 1 ; h++ ){
			for (int k = h +1  ; k < n ; k ++){
				//the computation of b is so complex that we build a function for it
				b = coefB (n, Q, mu,lambda, result, k, h);
				a = lambda*(Q[h*(n+1)] + Q[k*(n+1)] - 2*Q[k -h +h*(n+1)]);
				
				//calculate the upperbound and lowerbound for epsilon to ensure the results stay in their limit interval
				epsilonMax = min( result[h] - lowerbound[h], upperbound[k] - result[k])*(result[h] >= lowerbound[h])*(upperbound[k] >= result[k]) ;
				epsilonMin = -min( upperbound[h] - result[h], result[k] - lowerbound[k])*(upperbound[h] >= result[h])*(result[k] >= lowerbound[k]);
				
				//if a = 0, we can't divide it directly. To minimize the objective value, we choose epsilon as its lowerbound. 
				if (a == 0)
					epsilon = epsilonMin;
				else
					epsilon = -b/(2*a);
				
				
				//if epsilon is not in the legal interval, just take it as the boundary.
				if (epsilon < epsilonMin)
					epsilon = epsilonMin;
				else if (epsilon > epsilonMax)
					epsilon = epsilonMax;

			//codes to print the result during the improvement	
			/*	printf("+epsilon:x%i   -epsilon:x%i\n",k+1,h+1);
				printf("a: %g   b:%g \n",a,b);
				printf("epsilon:%g\n",epsilon);
				printf("epsilonMax:%g   epsilonMin:%g    \n",epsilonMax,epsilonMin);*/

				//adjust the result
				result[h] = result[h] - epsilon;
				result[k] = result[k] + epsilon;


			/*	printf("Result:\n");
				for ( int x = 0 ;x<n; x++){
	
				printf("x%d:  %g\n", x+1 , result[x]);
	
				}*/

				
			}

		}
		count_loop = count_loop + 1;
	}

	

	return 0;
}


//function used for adding the elements of array
double sumArray(double *array, int n){
	double sum = 0;

	for (int k =0 ; k < n ;k++){
		sum = sum + array[k];
	} 

	return sum;
}


//calculate coefficient b
double coefB ( int n, double *Q, double *mu, double lambda, double *result, int k, int h){
	double coefficient = 0, tempPart = 0;

	for (int i = 0; i < n; i++){
		if  ( i == k || i == h)
			continue;
		else{
			tempPart = tempPart + (Q[ k*n + i ] - Q[h*n + i ])*result[i];
		
		}
	}

	coefficient = 2*lambda*(-Q[h*(n+1)]*result[h] + Q[k*(n+1)]*result[k] + Q[k -h +h*(n+1)]*(result[h] - result[k]) + tempPart ) + mu[h] - mu[k]; 

	return coefficient;

}

//function used for calculating the objective value
double objectValue ( int n, double *Q, double *mu, double lambda, double *result, int type){
	double objValue = 0, iVar = 0 , iMu = 0, ijTemp = 0 ;
	for (int i = 0; i < n ; i++){
		for (int j = i + 1; j <n ; j++){	
			ijTemp = ijTemp + Q[i*n + j]*result[i]*result[j];
		}
		iVar = iVar + pow(result[i],2) * Q[i*(n+1)];
		iMu  = iMu  + result[i]*mu[i];
	}

	//if type = 1, it returns the objective value
	//if type = 2, it returns the variance of the portfolio
	if (type == 1)
	objValue = lambda *(iVar + 2*ijTemp ) - iMu;
	else if (type == 2)
	objValue = (iVar + 2*ijTemp );


	return objValue;
}