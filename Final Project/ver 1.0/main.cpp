#include <windows.h> 
#include <process.h>
#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "bag.h"


unsigned _stdcall comp_wrapper(void *foo);
int readPara(char *filename, int *threadNum, int **totalShare, int **periodT, double **alpha, double **pi);

int main(int argc, char *argv[])
{
	/*-------------initialization---------------*/
	int retcode =  0, flag = 0;
	int threadNum;
	int *totalShareN = nullptr, *periodT = nullptr, *threadFinishChecked;
	double *alpha = nullptr, *pi = nullptr;
	HANDLE *pThread;
	unsigned *pthreadID;
	bag **pbags;
	HANDLE consolemutex;
	HANDLE *mastermutexes;
	char jstatus = RUNNING;
	int finishedThreads = 0;
	
	//check input
	if(argc != 2){
	  printf("Usage: Final.exe datafilename\n"); retcode = 100; goto BACK;
	 }

	/*-------------Read parametmer from data file---------------*/
	readPara(argv[1], &threadNum, &totalShareN, &periodT, &alpha, &pi);


	/*-------------Initialize multi threads---------------*/
	pbags = (bag **)calloc(threadNum, sizeof(bag *));
	pThread = (HANDLE *)calloc(threadNum, sizeof(HANDLE));
	pthreadID= (unsigned *)calloc(threadNum, sizeof(unsigned));
	mastermutexes = (HANDLE *)calloc(threadNum, sizeof(HANDLE));
	threadFinishChecked = (int *)calloc(threadNum, sizeof(int));
	if(pthreadID == NULL || pThread == NULL || pbags == NULL || mastermutexes == NULL){
		printf("cannot allocate %d threads and their bags", threadNum);
		retcode = 1; goto BACK;
	}


	consolemutex = CreateMutex(NULL, 0, NULL);
	for(int j = 0; j < threadNum; j++){
		pbags[j] = new bag(totalShareN[j],periodT[j],alpha[j], pi[j],j+1); //create new bags(like argument list) for threads
		pbags[j]->setConsoleMutex(consolemutex);
		threadFinishChecked[j] = 0;
	}

	for (int j = 0; j < threadNum; j++){
		mastermutexes[j] = CreateMutex(NULL, 0, NULL);
		pbags[j]->setMasterMutex(mastermutexes[j]);
	}

	/*-------------Threads work begin---------------*/
	for(int j = 0; j < threadNum; j++){
		pThread[j] = (HANDLE)_beginthreadex( NULL, 0, &comp_wrapper, (void *) pbags[j], 
			0, 	&pthreadID[j] );
	}

	//master program
	while (true){
		Sleep(1000);//check the workers at every 1000 milliseconds
		printf("\n----master will now check on workers----\n"); fflush(stdout);

		for(int j = 0; j < threadNum; j++){
			WaitForSingleObject(mastermutexes[j], INFINITE);//thread j wait for the master to get status
			jstatus = pbags[j]->getStatus();
			ReleaseMutex(mastermutexes[j]);//release thread j

			
			if(jstatus == FINISHED){//if the status is FINISHED, we print out to tell user that the thread is finished.
				printf("Worker %d finished work.\n", j + 1);
				if (threadFinishChecked[j] == 0){//if it is the first time we find it finished, change value of variables
					finishedThreads += 1;
					threadFinishChecked[j] = 1;
				}

				/*if the number of finished threads equals to the total number of threads, 
				we consider all the work finished and get out of the loop*/
				if (finishedThreads == threadNum) {
					printf("All workers finished work.\n");
					flag = 1;
					break;
				}
			}
			else{
				printf("Worker %d is still working\n", j + 1);
			}
		}
		

		if (flag == 1) 
			break;
	}

	/*---------------Threads exit and output result----------------*/
	for(int j = 0; j < threadNum; j++){
		WaitForSingleObject(pThread[j], INFINITE);
		printf("\n==============Result Output================\n"); 
		printf("--> thread %d done\n", j); 
		pbags[j] -> getPrintParameter();
		printf("optimal value for trade sequencing = %g\n", pbags[j] -> getOptimalResult());
	}

	printf("\n");

	//free the space 
	for(int j = 0; j < threadNum; j++){
		delete pbags[j]; 
	}
	free(pthreadID);
	free(pThread);
	free(alpha);
	free(pi);
	free(pbags);
	free(totalShareN);
	free(periodT);
	free(threadFinishChecked);

BACK:
	return retcode;
}

//function used to call the algorithm 
unsigned _stdcall comp_wrapper(void *genericaddress)
{
	bag *pbag = (bag *) genericaddress;
	int retcode = 0;

	retcode = pbag->bagComputation();

	return retcode;
}

//function used to read parameter
int readPara(char *filename, int *threadNum, int **totalShare, int **periodT, double **alpha, double **pi){
	//initialize
	int retcode = 0;
	char buffer[100];
	
	FILE *input = fopen(filename, "r");
	if(input == NULL) {
		 printf("cannot read file %s\n", filename); retcode = 1; goto BACK;
	}

	//get the number of quadruple 
	fscanf(input,"%s", buffer);
	fscanf(input,"%s", buffer);
	(*threadNum) = atoi(buffer);

	//get the number of total shares we should sell in the period
	(*totalShare) = (int *)calloc( (*threadNum) ,sizeof(int));
	if (!(*totalShare)){
	  printf("cannot allocate space\n"); retcode = 2; goto BACK;
	}
	fscanf(input,"%s", buffer);
	for (int i = 0 ; i < (*threadNum); i++){
		fscanf(input,"%s", buffer);
		(*totalShare)[i] = atoi(buffer);
	}

	//get the number of period 
	(*periodT) = (int *)calloc( (*threadNum) ,sizeof(int));
	if (!(*periodT)){
	  printf("cannot allocate space\n"); retcode = 2; goto BACK;
	}
	fscanf(input,"%s", buffer);
	for (int i = 0 ; i < (*threadNum); i++){
		fscanf(input,"%s", buffer);
		(*periodT)[i] = atoi(buffer);
	}

	//get alpha
	(*alpha) = (double *)calloc( (*threadNum) ,sizeof(double));
	if (!(*alpha)){
	  printf("cannot allocate space\n"); retcode = 2; goto BACK;
	}
	fscanf(input,"%s", buffer);
	for (int i = 0 ; i < (*threadNum); i++){
		fscanf(input,"%s", buffer);
		(*alpha)[i] = atof(buffer);
	}

	//get pi
	(*pi) = (double *)calloc( (*threadNum) ,sizeof(double));
	if (!(*pi)){
	  printf("cannot allocate space\n"); retcode = 2; goto BACK;
	}
	fscanf(input,"%s", buffer);
	for (int i = 0 ; i < (*threadNum); i++){
		fscanf(input,"%s", buffer);
		(*pi)[i] = atof(buffer);
	}


	BACK:
		return retcode;

}