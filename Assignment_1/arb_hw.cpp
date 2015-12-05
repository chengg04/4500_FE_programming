#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>

char does_it_exist(char *filename);

int main(int argc, char* argv[])
{
	int retcode = 0;
	FILE *in = NULL, *out = NULL;
	char mybuffer[100];
	int numsec, numscen, j, k, numnonz;
	double *r;
	double *p, optimalvalue, xvalue;
	FILE *results = NULL;

  if(argc != 3){
	  printf("Usage:  arb1.exe datafilename lpfilename\n"); retcode = 100; goto BACK;
  }


  in = fopen(argv[1],"r");
  if (in == NULL){
	  printf("could not open %s for reading\n", argv[1]);
	  retcode = 200; goto BACK;
  }

  fscanf(in, "%s", mybuffer);
  fscanf(in, "%s", mybuffer);
  numsec = atoi(mybuffer);//convert string to integer
  fscanf(in, "%s", mybuffer);
  fscanf(in, "%s", mybuffer);
  numscen = atoi(mybuffer);
  fscanf(in, "%s", mybuffer);

  printf("securities: %d, scenarios: %d\n",
	  numsec, numscen); //;;  r = %g

  r = (double*)calloc(numscen+1, sizeof(double));
  if (r == NULL){
	  printf("no memory\n"); retcode = 300; goto BACK;
  }
  for (k = 0; k <= numscen; k++){
	  fscanf(in, "%s", mybuffer);
	  r[k] = atof(mybuffer);
  }


  p = (double *)calloc((1 + numscen)*(1 + numsec), sizeof(double));//allocate the initial zero arrays
  if (p == NULL){
	  printf("no memory\n"); retcode = 400; goto BACK;
  }
  for (k = 0; k <= numscen; k++){
	  fscanf(in, "%s", mybuffer);
	  p[k*(1 + numsec)] = 1 + r[k]*(k!=0);//p is a number vector, not matrix
	  for (j = 1; j <= numsec; j++){
		  fscanf(in, "%s", mybuffer);
		  p[k*(1 + numsec) + j] = atof(mybuffer);
	  }
  }

  fscanf(in, "%s", mybuffer);

  fclose(in);

  out = fopen(argv[2], "w");
  if (out == NULL){
	  printf("can't open %s\n", argv[2]); retcode = 500; goto BACK;
  }
  printf("printing LP to file %s\n", argv[2]);

  fprintf(out, "Maximize z");
  
  fprintf(out, "\nSubject to\n");
  fprintf(out, "today: ");
  for (j = 0; j <= numsec; j++){
	  if (p[j] >= 1) fprintf(out, "+ "); fprintf(out, "%g x%d ", p[j], j);
  }
  fprintf(out, "- v0 = 0\n");
  for (k = 1; k <= numscen; k++){
	  fprintf(out, "scen%d: ", k);

	  for (j = 0; j <= numsec; j++){
		  if (p[k*(1 + numsec) + j] > 0) fprintf(out, "+ ");
		  fprintf(out, "%g x%d ", p[k*(1 + numsec) + j], j);
	  }
	  fprintf(out, "-");
	  fprintf(out, "%g", p[k*(1 + numsec)]);
	  fprintf(out, " v0 - z >= 0\n");
  }
  fprintf(out, "v0 >= 1\n");

  fprintf(out, "Bounds\n");
  for (j = 0; j <= numsec; j++){
	  fprintf(out, "-1 <= x%d <= 1\n", j);
  }
  fprintf(out, "End\n");

  fclose(out);

  free(p);

// system("mygurobi.py arb.lp");
//  exit(3);

  out = fopen("hidden.dat", "w");
  fclose(out);

  sprintf(mybuffer, "python script.py %s hidden.dat nothidden.dat", argv[2]);

  printf("mybuffer: %s\n", mybuffer);

  if (does_it_exist("nothidden.dat")){
	  remove("nothidden.dat");
  }

  system(mybuffer);

  /** sleep-wake cycle **/

  for (;;){
	  if (does_it_exist("nothidden.dat")){ 
		  printf("\ngurobi done!\n");
		  Sleep(1000);
		  break;
	  }
	  else{
		  Sleep(100);
	  }
  }

  /** next, read mygurobi.log **/

  results = fopen("mygurobi.log", "r");
  if (!results){
	  printf("cannot open mygurobi.log\n"); retcode = 300; 
	  goto BACK;
  }
  /* read until finding Optimal */

  for (;;){
	  fscanf(results, "%s", mybuffer);
	  /* compare mybuffer to 'Optimal'*/
	  if (strcmp(mybuffer, "Optimal") == 0){
		  /* now read three more*/
		  fscanf(results, "%s", mybuffer);
		  fscanf(results, "%s", mybuffer);
		  fscanf(results, "%s", mybuffer);
		  optimalvalue = atof(mybuffer);
		  printf(" value = %g\n", optimalvalue);
		  if (optimalvalue < -.0001){
			  printf("type A arbitrage exists!\n");
			  /* read again to get the number of nonzeros*/
			  fscanf(results, "%s", mybuffer);
			  numnonz = atoi(mybuffer);
			  fscanf(results, "%s", mybuffer); fscanf(results, "%s", mybuffer); fscanf(results, "%s", mybuffer);
			  fscanf(results, "%s", mybuffer);
			  for (k = 0; k < numnonz; k++){
				  fscanf(results, "%s", mybuffer);
				  j = atoi(mybuffer + 1);
				  fscanf(results, "%s", mybuffer); fscanf(results, "%s", mybuffer);
				  xvalue = atof(mybuffer);
				  printf("%d -> %g\n", j, xvalue);
			  }
		  }
		  else{
			  printf("no type A\n"); break;
		  }
	  }
	  else if (strcmp(mybuffer, "bye.") == 0){
		  break;
	  }
  }
   
BACK:
  return retcode;
}


char does_it_exist(char *filename)
{
	struct stat buf;

	// the function stat returns 0 if the file exists

	if (0 == stat(filename, &buf)){
		return 1;
	}
	else return 0;
}