#include <windows.h> 
#include <process.h>
#include "bag.h"

bag::bag(int Ninput,int Tinput,double alphaInput, double piInput, int nameInput)
	:N(Ninput),T(Tinput),alpha(alphaInput),pi(piInput),name(nameInput){//constructor initializer list, more efficient
	status = WAITING;
}

void bag::setConsoleMutex(HANDLE consoleMutexInput)
{
	consoleMutex = consoleMutexInput;
}

void bag::setMasterMutex(HANDLE consoleMutexInput)
{
	masterMutex = consoleMutexInput;
}

int bag::bagComputation(){
	//initialization
	status = RUNNING;
	int retcode = 0;
	double *optimal, *shiftP;//, *bestSales;
	double newPrice, bestValue = 0, candidate;
	optimal = (double *)calloc((N + 1)*T, sizeof(double));
	if (!optimal){
	  printf("cannot allocate large matrix for optimal\n"); retcode = 200; goto BACK;
	}

	shiftP = (double *)calloc(N + 1, sizeof(double));
	if (!shiftP){
	  printf("cannot allocate large matrix for shift price\n"); retcode = 200; goto BACK;
	}

	/*bestSales = (double *)calloc(T, sizeof(double));
	if (!bestSales){
	  printf("cannot allocate large matrix for number of best sales\n"); retcode = 200; goto BACK;
	}*/

	//compute the shifted price after selling assets
	for (int i = 0 ; i < N + 1; i++){
		shiftP[i] = 1 - alpha * pow((double)i,pi);
	}

	//compute the last stage: if we have j shares left, we should sell all of them
	for (int j = 0 ; j < N + 1; j++){
		newPrice = shiftP[j];
		optimal[(T - 1)*(N + 1) + j] = newPrice*j;
	}

	//compute the remaining T - 1 stages
	for (int t = T - 2 ; t >= 0 ; t--){//starting from the stage before last stage (go back to the previous stage)
		for (int j = 0 ; j <= N ; j++){// j = number of shares that can be sold in period t

			bestValue = 0;
			for (int h = 0; h <= j ; h++){// h = number of shares we choose to sell in period t
				newPrice = shiftP[h];
				//value of cash flow that we can obtain by selling h shares in this period t and j - h shares on stages t+1,..., T -1 
				candidate = newPrice*h + newPrice*optimal[(t + 1)*(N + 1) + j - h];
				if (candidate > bestValue)//compare between values and find the optimal value
					bestValue = candidate;

			}

			optimal[t *(N + 1) + j] = bestValue;
		}
		 printf("Worker %d : done with stage t = %d\n", name, t);
	}


BACK:
	result  = optimal[N];
	status = FINISHED;
	return retcode;

}



