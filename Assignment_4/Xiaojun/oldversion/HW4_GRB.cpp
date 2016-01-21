
#include <stdio.h>
#include <stdlib.h>


#include <gurobi_c++.h>




int GurobiSolve(int n, int T, int pcNum, double *V, double *F, double *D, double *mu, double lambda, int nameLimit, double ubLimit, int constType)
{
  int retcode = 0;
  GRBenv   *env = NULL;
  GRBmodel *model = NULL;
  int j,m;
  double *obj      = NULL;
  double *lb  = NULL;
  double *ub       = NULL;
  double *x;
  int *qrow, *qcol, Nq;
  double *qval;
  int *cind;
  double rhs;
  char sense;
  double *cval;
  int numnonz;

  char **names, *vartype, bigname[100];

  int numVariable = n*2  + pcNum;

  retcode = GRBloadenv(&env, "factormodel.log");
  if (retcode) goto BACK;

 /* Create initial model */
  retcode = GRBnewmodel(env, &model, "HW4", numVariable, 
                      NULL, NULL, NULL, NULL, NULL);
  if (retcode) goto BACK;

  names = (char **) calloc(numVariable, sizeof(char *));

  /** next we create the remaining attributes for the n columns **/
  obj     = (double *) calloc (numVariable, sizeof(double));
  ub     = (double *) calloc (numVariable, sizeof(double));
  lb     = (double *) calloc (numVariable, sizeof(double));
  x     = (double *) calloc (numVariable, sizeof(double));
  vartype = (char *)calloc(numVariable, sizeof(char));

  for(j = 0; j < n; j++){
    names[j] = (char *)calloc(5, sizeof(char));
    if(names[j] == NULL){
      retcode = 1; goto BACK;
    }
	obj[j] = -mu[j];
	ub[j] = ubLimit;
    sprintf(names[j],"x%d", j);
  }

  for(j = n; j < n + pcNum; j++){
    names[j] = (char *)calloc(5, sizeof(char));
    if(names[j] == NULL){
		  retcode = 1; goto BACK;
    }
	ub[j] = 100;
	lb[j] = -100;
    sprintf(names[j],"y%d", j - n);
  }
  

  /** finally, do the binary variables **/
  for (j = n + pcNum; j < numVariable; j++){
	  names[j] = (char *)calloc(5, sizeof(char));
	  if (names[j] == NULL){
		  retcode = 1; goto BACK;
	  }
	  sprintf(names[j], "z%d", j -(n + pcNum));
	  lb[j] = 0; /* redundant because of calloc */
	  ub[j] = 1.0;
	  obj[j] = 0.0; /* redundant, again */
  }



  /* initialize variables */
  for(j = 0; j < numVariable; j++){
    retcode = GRBsetstrattrelement(model, "VarName", j, names[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "Obj", j, obj[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "LB", j, lb[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "UB", j, ub[j]);
    if (retcode) goto BACK;

	if (j < n + pcNum) vartype[j] = GRB_CONTINUOUS;
	else vartype[j] = GRB_BINARY;
	
	retcode = GRBsetcharattrelement (model, "VTYPE", j, vartype[j]);
	if (retcode) goto BACK;
  }

  /** next, the quadratic -- there are 11 nonzeroes: 7 residual variances plus the 2x2
									factor covariance matrix**/

  Nq = n + pcNum*pcNum; 
  qrow = (int *) calloc(Nq, sizeof(int));  /** row indices **/
  qcol = (int *) calloc(Nq, sizeof(int));  /** column indices **/
  qval = (double *) calloc(Nq, sizeof(double));  /** values **/

  if( ( qrow == NULL) || ( qcol == NULL) || (qval == NULL) ){
    printf("could not create quadratic\n");
    retcode = 1; goto BACK;
  }


  for (int k = 0 ; k < n ; k++){
	qval[k] = 2*lambda*D[k];
	qrow[k] = k;
	qcol[k] = k;
  
  }

 // for (int i = n; i < n + pcNum; i++){
	//qval[i] = 2*lambda* F[i*(pcNum+1)];
	//qrow[i] = i;
	//qcol[i] = i;
 // 
 // }

  for (int i = 0; i < pcNum; i++){
    for (j = 0; j < pcNum; j++){
      m = i*pcNum + j;
      qval[m + n] = 2*lambda*F[m];
      qrow[m + n] = n + i;
      qcol[m + n] = n + j;
    }
  }

  retcode = GRBaddqpterms(model, Nq, qrow, qcol, qval);
  if (retcode) goto BACK;

  /** now we will add one constraint at a time **/
  /** we need to have a couple of auxiliary arrays **/

  cind = (int *)calloc(n + 1, sizeof(int));  /** n is over the top since no constraint is totally dense;
					     but it's not too bad here **/
  cval= (double *)calloc(n + 1, sizeof(double));

  /** two factor constraints, first one is next**/
  for(int i = 0 ; i < pcNum; i++){
	  for (j = 0; j < n; j++){
		  cval[j] = V[i + j*pcNum];
	  }
	  cval[n] = -1;

	  for(int k = 0 ; k < n ; k++) {
		  cind[k] = k;
	  }
	  cind[j] = n + i;

	  numnonz = n + 1;
	  rhs = 0;
	  sense = GRB_EQUAL;

	  sprintf(bigname,"Constraint%d",i);
	  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, bigname);
      if (retcode) goto BACK;
  }
  

  /** sum of x variables = 1 **/
  for (j = 0; j < n; j++){
	   cval[j] = 1.0;
	   cind[j] = j;
    }

  numnonz = n;
  rhs = 1.0;
  sense = GRB_EQUAL;

  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, "convexity");
  if (retcode) goto BACK;

  /** now say xj = 0 unless zj = 1 **/

  for (j = 0; j < n; j++){
	  cval[0] = 1.0;  cind[0] = j;
	  cval[1] = -ub[j];  cind[1] = n + pcNum + j;

	  numnonz = 2;
	  rhs = 0.0;
	  sense = GRB_LESS_EQUAL;

	  /* let's reuse some space */
	  sprintf(names[0], "control%d", j);

	  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, names[0]);
	  if (retcode) goto BACK;
  }

  /** sum of z variables <= 3, i.e. not more than 3 names in portfolio **/

  for (j = 0 ; j < n ;j++){
	cval[j] = 1.0;
  }
  

  for (j = 0; j < n; j++) {
	  cind[j] = j + n + pcNum;
  }

  numnonz = n;
  rhs = nameLimit;
  if (constType == 0 ){
	  sense = GRB_GREATER_EQUAL;
  }
  else if (constType == 1 ){
	  sense = GRB_LESS_EQUAL;
  }

  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, "namesbound");
  if (retcode) goto BACK;

  retcode = GRBupdatemodel(model);
  if (retcode) goto BACK;

  /** optional: write the problem **/

  retcode = GRBwrite(model, "factorwithnames.lp");
  if (retcode) goto BACK;


  retcode = GRBoptimize(model);
  if (retcode) goto BACK;


  /** get solution **/


  retcode = GRBgetdblattrarray(model,
                               GRB_DBL_ATTR_X, 0, numVariable,
                               x);
  if(retcode) goto BACK;

  /** now let's see the values **/

  for(j = 0; j < numVariable; j++){
    printf("%s = %g\n", names[j], x[j]);
  }

  GRBfreeenv(env);


  free(cind);
  free(cval);
  free(qval);
  free(qrow);
  free(qcol);
  free(lb);
  free(ub);
  free(obj);
  free(vartype);
  free(x);
  free(names);

 BACK:
  printf("\nexiting with retcode %d\n", retcode);
  return retcode;
}