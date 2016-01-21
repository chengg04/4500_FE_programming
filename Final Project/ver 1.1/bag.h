#include <iostream> 
#include <windows.h> 
#include <process.h>
#include <stdio.h>
#include <stdlib.h> 

//Define the status of threads
#define RUNNING 1
#define WAITING 0
#define FINISHED 2
 

class bag{
public:
	//constructor
	bag(int Ninput,int Tinput,double alphaInput, double piInput, int nameInput, double P0input);
	//destructor
	~bag(){ printf("worker %d says goodbye\n", name); } 

	//algorithm computation
	int bagComputation();
	//mutex control
	/*void setConsoleMutex(HANDLE consoleMutexInput);*/
	void setMasterMutex(HANDLE consoleMutexInput);
    //get the running status of the thread
	int getStatus(){ return status; }
	//print the parameter
	void getPrintParameter(){printf("alpha = %g , pi = %g  , N = %d , T =%d\n", alpha, pi, N, T);}
	//get the optimal sresult
	double getOptimalResult(){return result; }

private:
	//definition of parameters
	int name;
	int N, T;
	double alpha, pi, P0;
	int status;
	double result;
  /*  HANDLE consoleMutex;*/
	HANDLE masterMutex;
};