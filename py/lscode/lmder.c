#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "dpmach.h"
#include "lmpar.h"
#include "qrfac.h"
#include "enorm.h"
#include "lmder.h"

#define MAX(a,b)  ((a) >= (b) ? (a) : (b))
#define MIN(a,b)  ((a) <= (b) ? (a) : (b))

#define ZERO 0.0
#define ONE 1.0
/******* lmder ************************************/

int lmder(int (*fcn)(), void *lsdata, int m, int n, double x[], 
			double *diag, double factor, int nprint,
			double ftol, double xtol, double gtol, int maxfev, 
			int nc, double **cA, double *cb, int appc[],
			double fvec[], double *fnorm, double fjac[], int ipvt[], 
			double qtf[], int *nfev, int *njev)
{
/*
*     **********
*
*     function lmder
*
*     the purpose of lmder is to minimize the sum of the squares of
*     m nonlinear functions in n variables by a modification of
*     the levenberg-marquardt algorithm. the user must provide a
*     subroutine which calculates the functions and the jacobian 
*     The minimization is performed subject to a set of linear
*     inequality constraints on x defined by the input parameters
*     nc, cA and cb.
*
*  Parameters:
*
*	fcn is a pointer to the user-supplied function which
*	  calculates the functions and jacobian
*    fcn must be declared as
*
*	  int fcn(int m, int n, double x[], double fvec[],
*            double fjac[], int task, void *lsdata) {
*    (The "void" may be replaced with any data type, likely a structure.)
*	  ----------
*       If task is 0: Print x or any other values of interest; do not
*                     alter any arguments.
*       If task is 1: Calculate the functions at x and return this 
*                     vector in fvec; do not alter any other arguments.
*       If task is 2: Calculate the jacobian functions at x and return 
*                     this (mxn) array in the vector in fjac stored 
*                     columnwise; do not alter any other arguments.
*	  ----------
*	  return value;
*	  }
*
*	  If the return value of fcn is negative, lmder terminates with
*    that value for info.
*
*  lsdata is a pointer to some data type (likely a structure)
*    which contains any other information the function fcn
*    needs to compute fvec and fjac.  This pointer is simply passed
*    on to fcn, its contents are not accessed in this function.
*
*	m is a positive integer input variable set to the number
*	  of functions.
*
*	n is a positive integer input variable set to the number
*	  of variables. n must not exceed m.
*
*	x is an array of length n. on input x must contain
*	  an initial estimate of the solution vector. on output x
*	  contains the final estimate of the solution vector.
*
*	diag is either a NULL pointer, in which case scaling of
*    the variables will be done internally, or diag should
*    point to an array of length n which must contain positive
*	  entries that serve as multiplicative scale factors for 
*    the variables.
*
*	factor is a nonzero input variable used in determining the
*	  initial step bound.  If factor is positive, this bound is 
*    set to the product of factor and the euclidean norm of 
*    diag*x if nonzero, or else to factor itself.  In most cases 
*    factor should lie in the interval (.1,100.). 100. is a 
*    generally recommended value.  If factor is negative then
*    the initial step bound is set to -factor and is NOT 
*    multiplied by the euclidean norm of diag*x.
*
*	nprint is an integer input variable that enables controlled
*	  printing of iterates if it is positive. in this case,
*	  fcn is called with task = 0 at the beginning of the first
*	  iteration and every nprint iterations thereafter and
*	  immediately prior to return, with x and fvec available
*	  for printing. if nprint is not positive, no special calls
*	  of fcn with task = 0 are made.
*
*	ftol is a nonnegative input variable. termination
*	  occurs when both the actual and predicted relative
*	  reductions in the sum of squares are at most ftol.
*	  therefore, ftol measures the relative error desired
*	  in the sum of squares.
*
*	xtol is a nonnegative input variable. termination
*	  occurs when the relative error between two consecutive
*	  iterates is at most xtol. therefore, xtol measures the
*	  relative error desired in the approximate solution.
*
*	gtol is a nonnegative input variable. termination
*	  occurs when the cosine of the angle between fvec and
*	  any column of the jacobian is at most gtol in absolute
*	  value. therefore, gtol measures the orthogonality
*	  desired between the function vector and the columns
*	  of the jacobian.
*
*	maxfev is a positive integer input variable. termination
*	  occurs when the number of calls to fcn is at least
*	  maxfev by the end of an iteration.
*
*  nc is an input integer giving the number of linear constraints on x
*
*  cA is an (nc x n) input matrix that defines the left hand side of 
*    the linear constraints on x:  cA * x <= cb. 
*    cA[i][j] is the ith row, jth column of cA.
*
*  cb is a vector of nc entries that defines the right hand side of
*    the linear constraints on x:  cA * x <= cb.
*
*  appc is an output array of length nc which indicates whether each
*    constraint is active at the solution x. 0=inactive, 1=active.
*
*	fvec is an output array of length m which contains
*	  the functions evaluated at the output x.
*
*  fnorm is an output variable giving the Euclidean norm of fvec
*
*	fjac is an output m by n array. the upper n by n submatrix
*	  of fjac contains an upper triangular matrix r with
*	  diagonal elements of nonincreasing magnitude such that
*
*		 t     t	   t
*		p *(jac *jac)*p = r *r,
*
*	  where p is a permutation matrix and jac is the jacobian at
*	  the output x.. column j of p is column ipvt(j) (see below) 
*    of the identity matrix. the lower trapezoidal part of fjac 
*    contains information generated during the computation of r.
*
*	ipvt is an integer output array of length n. ipvt
*	  defines a permutation matrix p such that jac*p = q*r,
*	  where jac is the jacobian at the output x, q is
*	  orthogonal (not stored), and r is upper triangular
*	  with diagonal elements of nonincreasing magnitude.
*	  column j of p is column ipvt(j) of the identity matrix.
*
*	qtf is an output array of length n which contains
*	  the first n elements of the vector (q transpose)*fvec.
*
*	nfev is an integer output variable set to the number of
*	  calls to fcn with task=1.
*
*	njev is an integer output variable set to the number of
*	  calls to fcn with task=2.
*
*  Return value:
*
*	info is an integer return value. If the user has terminated 
*    execution via returning a negative value from fcn (see descripion 
*    of fcn above) then info is this value, otherwise info is set as 
*    follows.
*
*	  info = 0  improper input parameters.
*
*	  info = 1  both actual and predicted relative reductions
*		    in the sum of squares are at most ftol.
*
*	  info = 2  relative error between two consecutive iterates
*		    is at most xtol.
*
*	  info = 3  conditions for info = 1 and info = 2 both hold.
*
*	  info = 4  the cosine of the angle between fvec and any
*		    column of the jacobian is at most gtol in
*		    absolute value.
*
*	  info = 5  number of calls to fcn has reached or
*		    exceeded maxfev.
*
*	  info = 6  ftol is too small. no further reduction in
*		    the sum of squares is possible.
*
*	  info = 7  xtol is too small. no further improvement in
*		    the approximate solution x is possible.
*
*	  info = 8  gtol is too small. fvec is orthogonal to the
*		    columns of the jacobian to machine precision.
*
*    info = 9  constraints are inconsistent
*
*     subprograms called
*
*	user-supplied ...... fcn,
*
*	minpack-supplied ... enorm,lmpar,qrfac
*
*	C math library ... fabs,sqrt
*
*     argonne national laboratory. minpack project. march 1980.
*     burton s. garbow, kenneth e. hillstrom, jorge j. more
*
*     **********
*/
int i,ij,j,info,mode;
double actred,delta,dirder,fnorm1,gnorm;
double par,stepnorm,prered,ratio;
double temp,temp1,temp2,xnorm;
double *step,*wa1,*wa2,*fvec1;
double **cAw,*cbs,*cbsw,**Acons,*bcons;
int (*selcons)[2], *freeind;
int *actiter, release, *apporder;
double *R,*b,*diagw;
int *perm;
int nact, napp;
short reapply;
enum cons_state *cstate;
static double p1 = 0.1;
static double p5 = 0.5;
static double p25 = 0.25;
static double p75 = 0.75;
static double p0001 = 1.0e-4;

#ifdef DEBUG
char ch;
printf("Entering LMDER with DEBUG on\n");
printf("n = %i,  m = %i,  factor = %f,  maxfev = %i\n",n,m,factor,maxfev);
#endif
info = 0;
*nfev = 0;
*njev = 0;
for (i=0; i<nc; i++) appc[i] = 0;

/* Check the input parameters for errors.  */
if( (n <= 0) || (m < n) || (ftol < ZERO)
	|| (xtol < ZERO) || (gtol < ZERO) || (maxfev <= 0)
	|| (factor == ZERO) )
	return info;

#ifdef DEBUG
printf("First check in LMDER OK\n");
#endif
/* Check the diag entries if user-imposed scaling. Set the mode. */
if (diag != NULL)
	{ 
	for (j=0; j<n; j++)
		if (diag[j] <= 0.0) return info;
	mode = 2;
	}
else
	mode = 1;

/* Evaluate the function at the starting point and calculate its norm. */
*nfev = 1;
if ((i = fcn(m,n,x,fvec,fjac,1,lsdata)) < 0) return i;
*fnorm = enorm(m,fvec);

/* Allocate space for various arrays. */
step = (double *) malloc(n*sizeof(double));
wa1 = (double *) malloc(n*sizeof(double));
wa2 = (double *) malloc(n*sizeof(double));
fvec1 = (double *) malloc(m*sizeof(double));
freeind = (int *) malloc(n*sizeof(int));
if (mode == 1) diag = (double *) malloc(n*sizeof(double));
if (nc > 0)
	{
	Acons = (double **) malloc(n*sizeof(double *));
	for (i=0; i<n; i++) Acons[i] = (double *) malloc(n*sizeof(double));
	bcons = (double *) malloc(n*sizeof(double));
	actiter = (int *) calloc(nc,sizeof(int));  /* calloc to initialize to 0 */
	cstate = (enum cons_state *) malloc(nc*sizeof(enum cons_state));
	cAw = (double **) malloc(nc*sizeof(double *));
	for (i=0; i<nc; i++) cAw[i] = (double *) malloc(n*sizeof(double));
	cbs = (double *) malloc(nc*sizeof(double));
	cbsw = (double *) malloc(nc*sizeof(double));
	selcons = (int (*)[2]) malloc(nc*2*sizeof(int));
	apporder = (int *) malloc(nc*sizeof(int));
	R = (double *) malloc(n*n*sizeof(double));
	b = (double *) malloc(n*sizeof(double));
	perm = (int *) malloc(n*sizeof(int));
	diagw = (double *) malloc(n*sizeof(double));
	}

/* Initialize the number of applied and active constraints and the
	constraint states.  Each constraint has one of four states:
   	inactive - not currently in effect
   	active - in effect, but yet to be applied
   	applied - in effect, has been applied
   	deactive - was in effect last iteration, but is being
			temporarily inactivated to see if this iteration 
			will move the solution off the constraint.
	If a constraint becomes active on the current interation when it was
	not active on the previous iteration then actiter is set to the
	iteration number, njev.  Thus njev-actiter+1 is the number of 
	consecutive successful steps for which this constraint has been 
	applied.  When njev-actiter+1 matches a value of 2, 5, 9, 14, 20, 
	27, 35, 45, 55, ..., then that constraint is deactivated.  If a 
	deactive state becomes active on the current iteration then this is 
	considered a failed deactivation attempt. If a deactive state does 
	not become active on the current iteration then actiter is re-set to 0.
*/
napp = 0;
nact = 0;
for (i=0; i<nc; i++)
	cstate[i] = inactive;

/* Initialize freeind.  freeind is an index of the variables that are free, 
	that is, have not been specified by the application of a constraint. */
for (i=0; i<n; i++) freeind[i] = i;

/* Get a QR factorization of the jacobian, and related values. */
*njev = 1;
if ((i = getjac(m,n,x,fvec,*fnorm,fcn,lsdata,fjac,ipvt,qtf,wa1,&gnorm)) < 0)
	{
	info = i;
	}

/* If mode is 1, scale according to the norms of the columns 
   of the initial jacobian. */
if (mode == 1)
	for (j=0; j<n; j++) diag[j] = (wa1[j] == ZERO) ? ONE : wa1[j];

/* Calculate the norm of the scaled x and initialize the step 
   bound delta. */
for (j=0; j<n; j++) wa2[j] = diag[j] * x[j];
xnorm = enorm(n,wa2);
if (factor > ZERO)
	delta = (xnorm == ZERO) ? factor : (factor*xnorm);
else
	delta = -factor;
#ifdef DEBUG
printf("delta = %f\n",delta);
#endif

/* Calculate the constraints on the next step. */
calcscons(n,x,nc,cA,cb,cbs);

/* if requested, call fcn to enable printing of iterates. */
if (nprint > 0)
	{
	if ((i = fcn(m,n,x,fvec,fjac,0,lsdata)) < 0) 
		{
		info = i;
		}
	printf("fnorm %.15e\n",*fnorm);
	}

/* Initialize loop logicals, levenberg-marquardt parameter and 
   iteration counter. */
reapply = 1;
release = 0;
par = ZERO;

/* Test to see if the maximum number of function evaluations has been
	exceeded. (In case maxfev was input as 1.) */
if (*nfev >= maxfev) info = 5;

/* Continue calculating putative steps while info is zero, or if this is an
   attempt at releasing all of the active constraints after convergence. */
while ((info == 0) || release)
	{
	/* If we need to re-apply the constraints, re-initialize the free 
		index array, reset all applied constraints to active, reset 
		nact and napp, reset the apporder array, and reset the constraint 
		system. */
	if (reapply == 1) 
		{
		for (i=0; i<n; i++) freeind[i] = i;
		napp = 0;
		nact = 0;
		for (i=0; i<nc; i++)
			if (cstate[i] == applied)
				{
				cstate[i] = active;
				nact++;
				}
		i = 0;
		j = 0;
		while (j < nact)
			if (cstate[apporder[i++]] == active) apporder[j++] = apporder[i-1];
		for (i=0; i<nc; i++)
			{
			for (j=0; j<n; j++) cAw[i][j] = cA[i][j];
			cbsw[i] = cbs[i];
			}
		reapply = 0;
		}

	/* Continue calculating new steps until one is found which meets
	   all of the constraints.  (This is a do-while loop since we want
		to do at at least once.) */
	do		/* while first time or nact > 0 */
		{
		/* If this is the first time we are about to apply constraints
			then initialize the constraint modification matrix and 
			vector, the working constraint system, and diagw. */
		if (napp == 0 && nact > 0)
			{
			for (i=0; i<n; i++) 
				{
				for (j=0; j<n; j++) Acons[i][j] = ZERO;
				Acons[i][i] = ONE;
				bcons[i] = ZERO;
				}
			}

		/* Add the active constraints by updating the constraint 
			modification matrix and vector, and updating the constraint 
			system.  Also re-create the diagonal matrix D selecting
			only the free variables. */
		if (nact > 0)
			{
			if (updatecons(n,nc,apporder,cAw,cbsw,Acons,bcons,
				cstate,selcons,freeind,&nact,&napp) != 0)
				{
				info = 9;
				goto TERMINATE;
				}
			for (i=0; i<n-napp; i++) diagw[i] = diag[freeind[i]];
			}

		/* Generate the QR factorized constrained system if any
			constraints have been applied. 
			Determine the levenberg-marquardt parameter and new step
			for the free variables. */
		if (napp > 0)
			{
#ifdef DEBUG
printf("About to call genconssys and lmpar\n");
#endif
			genconssys(m,n,napp,fjac,qtf,ipvt,Acons,bcons,R,b,perm);
			lmpar(n-napp,R,n,perm,diagw,b,delta,&par,wa1,wa2);
			}
		else
			{
#ifdef DEBUG
printf("About to call lmpar\n");
#endif
			lmpar(n,fjac,m,ipvt,diag,qtf,delta,&par,wa1,wa2);
			/* Change the sign of the computed new step, since the right
				hand side passed to lmpar was qtf rather than -qtf. */
			for (i=0; i<n-napp; i++) wa1[i] = -wa1[i];
			}

		/* Test the new free step to see if it meets the inactive and
		   deactive constraints.  If not, then activate those 
		   constraints which were not met and re-calculate the step. */
		nact = testcons(n,napp,wa1,freeind,nc,cAw,cbsw,cstate,apporder);

		} while (nact > 0);    /* End of do-while loop. */

	/* Compute the step for all variables, unconstrained and constrained. */
	for (i=0; i<n-napp; i++) step[freeind[i]] = wa1[i];
	for (i=napp-1; i>=0; i--)  /* Must be done in backwards order. */
		{
		temp1 = ZERO;
		/* cAw[*][j] will be zero for step[j] not yet defined. */
		for (j=0; j<n; j++) temp1 += cAw[selcons[i][1]][j]*step[j];
		step[selcons[i][0]] = cbsw[selcons[i][1]] - temp1;
		}

	/* If this was a release attempt, re-set the release flag to zero,
	   and check to see if any deactivated states still exist.  If so, 
		then deactivation of constraints resulted in a better solution
	   being found so re-set info to zero and continue, otherwise exit. */
	if (release)
		{
		release = 0;  /* This is necessary. */
		i = 0;
		while (i<nc && cstate[i] != deactive) i++;
		if (i < nc) info = 0;
		else break; /* Breaks from inner loop; release=0 causes exit
		               from outer loop. */
		}

	/* store the direction x + step. calculate the norm of step. */
	for ( j=0; j<n; j++ ) 
		{
	  	wa1[j] = x[j] + step[j]; 
	   wa2[j] = diag[j]*step[j];
		}
	stepnorm = enorm(n,wa2);

	/* on the first iteration, adjust the initial step bound. */
	if (*njev == 1) delta = MIN(delta,stepnorm);
#ifdef DEBUG
if (*njev ==1) printf("delta adjusted first iter to delta = %f\n",delta);
#endif

	/* evaluate the function at x + step and calculate its norm. */
	*nfev += 1;
	if ((i = fcn(m,n,wa1,fvec1,fjac,1,lsdata)) < 0) 
		{
		info = i;
		break;
		}
	fnorm1 = enorm(m,fvec1);

	/* compute the scaled actual reduction. */
	actred = -ONE;
	if( (p1*fnorm1) < *fnorm)
		{
		temp = fnorm1/(*fnorm);
		actred = ONE - temp * temp;
		}

	/* compute the scaled predicted reduction and the scaled 
	   directional derivative. */
	for (j=0; j<n; j++)  /* compute R*P^T*step */
		{
		wa2[j] = ZERO;
		temp = step[ipvt[j]];
		ij = j*m;
		for (i=0; i<=j; i++) wa2[i] += fjac[ij+i]*temp;
		}
	temp1 = enorm(n,wa2)/(*fnorm);
	temp2 = (sqrt(par)*stepnorm)/(*fnorm);
	prered = temp1*temp1 + (temp2*temp2)/p5;
	dirder = -(temp1*temp1 + temp2*temp2);

	/* compute the ratio of the actual to the predicted reduction. */
	if (prered != ZERO) 
		ratio = actred/prered;
	else
		ratio = ZERO;
	
	/* update the step bound. */
	if (ratio <= p25)
		{
		if (actred >= ZERO)
			temp = p5;
		else
			temp = p5*dirder/(dirder + p5*actred);
		if ( ((p1*fnorm1) >= *fnorm) || (temp < p1) ) temp = p1;
		delta = temp*MIN(delta,stepnorm/p1);
		par = par/temp;
		}
	else if ( (par == ZERO) || (ratio >= p75) )
		{
		delta = stepnorm/p5;
		par = p5*par;
		}

	/* If the step is successful, update x, fvec, and their norms,
	   and get a new jacobian and related values.  Also get constraints
		on the next step. */
	if (ratio >= p0001)
		{
#ifdef DEBUG
printf("     Step WAS successful, napp =  %d\n",napp);
#endif
		for( j=0; j<n; j++ )
			{
			x[j] = wa1[j];
			wa1[j] = diag[j]*x[j];
			}
		for ( i=0; i<m; i++ ) fvec[i] = fvec1[i];
		xnorm = enorm(n,wa1);
		*fnorm = fnorm1;
		*njev += 1;
		if ((i = getjac(m,n,x,fvec,*fnorm,fcn,lsdata,fjac,ipvt,qtf,
			             wa1,&gnorm)) < 0)
			{
			info = i;
			break;
			}

		/* If scaling is internal (mode == 1) then re-scale diag if 
		   necessary by the norms of the columns of jac. */
		if (mode == 1)
			for (j=0; j<n; j++) diag[j] = MAX(diag[j],wa1[j]);

		/* Calculate the constraints on the next step. */
		calcscons(n,x,nc,cA,cb,cbs);

		/* if requested, call fcn to enable printing of iterates. */
		if ((nprint > 0) && ((*njev-1) % nprint) == 0)
			{
			if ((i = fcn(m,n,x,fvec,fjac,0,lsdata)) < 0) 
				{
				info = i;
				break;
				}
			printf("fnorm %.15e  stepnorm %.15e\n",*fnorm,stepnorm);
			}

		/* Set the re-apply flag. */
		reapply = 1;

		/* Re-set all deactive constraints to inactive and re-set their
		   activation counters.  Deactivate any applied constraints that 
			have been applied for the specified number of successful 
			iterations.  */
		for (i=0; i<nc; i++)
			{
			if (cstate[i] == deactive)
				{
				cstate[i] = inactive;
				actiter[i] = 0;
				}
			else if (cstate[i] == applied)
				{
				if (actiter[i] == 0)
					actiter[i] = *njev;
				else
					{
					j = *njev - actiter[i] + 1;
					if (j == 2 || j == 5 || j == 9 || j == 14 || 
							j == 20 || j == 27 || (j > 27 && j%10 == 5))
						cstate[i] = deactive;
					}
				}
			}
		}

	/* If the step was not successful, inactivate all constraints
		that were activated for the first time this last round.  If
		there were any, then we will need to re-apply all other applied
		constraints. */
	else   
		{
#ifdef DEBUG
printf("     Step was NOT successful, napp = %d\n",napp);
#endif
		for (i=0; i<nc; i++)
			{
			if (cstate[i] == applied && actiter[i] == 0) 
				{
				cstate[i] = inactive;
				reapply = 1;
				}
			}
		}
#ifdef DEBUG
printf("delta changed to %f\n",delta);
printf("Hit any key to continue...\n");
scanf("%c",&ch);
#endif

	/* test for termination due to too many fcn evaluations */
	if ( *nfev >= maxfev)
		info = 5;
	else
		{
		/* tests for convergence.  */
		if ( (fabs(actred) <= ftol) && (prered <= ftol) && (p5*ratio <= ONE) )
			info = 1;
		if (delta <= xtol*xnorm)
			{
			if (info == 1) info = 3;
			else info = 2;
			}
		else if (gnorm <= gtol)
			info = 4;
		/* tests for stringent tolerances.  */
		else if ( (fabs(actred) <= MACHEP) && (prered <= MACHEP) && 
				    (p5*ratio <= ONE) )
			info = 6;
		else if (delta <= MACHEP*xnorm)
			info = 7;
		else if (gnorm <= MACHEP)
			info = 8;
		}
	
	/* If convergence but some constraints were applied, then set the
	   release flag and force another iteration to see if a better 
		solution can be obtained.  Increase delta and decrease par. */
	if (info > 0 && info < 5)
		{
		if (napp > 0)
			{
			release = 1;
			delta = delta/p5;
			par = p5*par;
			for (i=0; i<nc; i++)
				{
				if (cstate[i] == applied && actiter[i] == 0) 
					{
					cstate[i] = inactive;
					reapply = 1;
					}
				}
			}
		}

	}  /* end of the loop */

/* termination, either normal or user imposed.  */

TERMINATE:

for (i=0; i<nc; i++)
	if ((cstate[i] == applied) || (cstate[i] == active)) appc[i] = 1;
free(step);
free(wa1);
free(wa2);
free(fvec1);
free(freeind);
if (mode == 1) free(diag);
if (nc > 0)
	{
	for (i=0; i<n; i++) free(Acons[i]);
	free(Acons);
	free(bcons);
	free(actiter);
	free(cstate);
	for (i=0; i<nc; i++) free(cAw[i]);
	free(cAw);
	free(cbs);
	free(cbsw);
	free(selcons);
	free(apporder);
	free(R);
	free(b);
	free(perm);
	free(diagw);
	}
if (nprint > 0)
	{
	fcn(m,n,x,fvec,fjac,0,lsdata);
	printf("fnorm %.15e\n",*fnorm);
	}
return info;
}


/*--------------------------------------------------------------------*/
int getjac(int m, int n, double x[], double fvec[], double fnorm, 
           int (*fcn)(), void *lsdata, double fjac[], int ipvt[], 
           double qtf[], double jaccnorm[], double *gnorm)
{
/*
	getjac - get the jacobian and related values

	Parameters:
		m - input, number of rows of fjac
		n - input, number of cols of fjac
		x - input, current solution
		fvec - input, values of f at x
		fnorm - input, norm of fvec
		fcn - input, pointer to function to evaluate fjac
		lsdata - input, pointer to structure to pass to fcn
		fjac - output, m x n QR factorization of jacobian of f, stored
		       columnwise.  The upper triangle of fjac contains R, and
		       the strict lower trapezoid contains values used in the
		       QR factorization of fjac.  
		             jac * P = Q * R
		ipvt - output, n array giving the permutation matrix P above,
		       column j of P is column ipvt(j) of the identity.
		qtf - output, n array giving the first n components of Q^T * fvec
		jaccnorm - output, n array giving the norms of the columns of the
		           jacobian
		gnorm - output, the maximum cosine of the angle between fvec and
		        any column of the jacobian

	Return value:
		If fcn returns a negative number then this is the return value,
		otherwise the return value is zero.
*/
int i,j,ij,jj,l;
double *rdiag, *wa, sum, temp1, temp2;

/* calculate the jacobian matrix. */
if ((i = fcn(m,n,x,fvec,fjac,2,lsdata)) < 0) return i;

/* Allocate some space. */
rdiag = (double *) malloc(n*sizeof(double));
wa = (double *) malloc(m*sizeof(double));

/* compute the qr factorization of the jacobian. */
qrfac(m,n,fjac,ipvt,rdiag,jaccnorm);

/* form (q transpose)*fvec and store the first n components in qtf. */
for (i=0; i<m; i++) wa[i] = fvec[i];
for (j=0; j<n; j++)
	{
	ij = j*m;
	jj = ij+j;
	temp2 = fjac[jj];
	if (temp2 != ZERO)
		{
		sum = ZERO;
		for (i=j; i<m; i++) sum += fjac[ij+i] * wa[i];
		temp1 = -sum / temp2;
		for (i=j; i<m; i++) wa[i] += fjac[ij+i] * temp1;
		}
	fjac[jj] = rdiag[j];
	qtf[j] = wa[j];
	}

/* compute the norm of the scaled gradient. */
*gnorm = ZERO;
if (fnorm != ZERO)
	{
	for (j=0; j<n; j++)
		{
		l = ipvt[j];
		if (jaccnorm[l] != ZERO)
			{
			sum = ZERO;
			ij = j*m;
			for (i=0; i<=j; i++)
				{
				sum += fjac[ij+i]*(qtf[i]/fnorm);
				}
			*gnorm = MAX(*gnorm,fabs(sum/jaccnorm[l]));
			}
		}
	}

free(rdiag);
free(wa);
return 0;
}

/*--------------------------------------------------------------------*/
void calcscons(int n, double *x, int nc, double **cA, double *cb, 
               double *cbs)
{
/*
	calcscons - calculate the right hand side vector cbs of the
               constraint equations on the step

	The constraints are given by   cA * y <= cb.  y = x+s where s is the 
	next step.  Thus the constraint on s is  cA * s = cb - cA * x.  We 
	define this new right hand side as cbs, and this function calculates it.

	Parameters:
		n - input, number of variables
		x - input, current variable values
		nc - input, number of constraints
		cA - input, coefficient matrix of constraint eqns
		cb - input, right hand side vector of constraint eqns
		cbs - output, right hand side vector of constraint eqns on s
*/
int i,j;
double temp;

for (i=0; i<nc; i++)
	{
	temp = 0.0;
	for (j=0; j<n; j++) temp += cA[i][j] * x[j];
	cbs[i] = cb[i] - temp;
	}
}

/*--------------------------------------------------------------------*/
int updatecons(int n, int nc, int *apporder, double **cA, double *cbs, 
		double **Acons, double *bcons, enum cons_state *cstate, 
		int (*selcons)[2], int *freeind, int *nact, int *napp)
{
/*
	updatecons - Update the constraint modification matrix and vector and 
		the constraint equations for each active constraint.

   Parameters:
      n - input, number of variables
		nc - input, total number of constraints
		apporder - input/output, order to apply active constraints
		cA - input/output, constraint matrix, the constraints are 
           given by   cA * s = cbs.
		cbs - input/output, right hand side of the constraint equations
		Acons - input/output, the constraint modification matrix
			columnwise
		bcons - input/output, the constraint modification vector
		cstate - input/output, state of each constraint equation
		selcons - input/output, the selected variables ([0]) and equation
			numbers ([1]) for each applied constraint
		freeind - input/output, index of remaining free variables
		nact - input/output, number of active constraints
		napp - input/output, number of applied constraints

   Return value:
		0 if successful, -1 if constraints could not be applied

		This routine updates the constraint modification matrix and vector,
		and the constraint system.  For each constraint equation, i, to be 
		applied, one variable is selected to be solved for, say s[j], and
		that constraint equation is scaled to 
			[E_1, 1, E_2] s = cbs[i], where the 1 is in the jth position.
		The remaining constraint equations are modified by adding appropriate
		multiples of equation i to zero out the jth column of cA.  The 1
		in the ith equation is then set to zero for ease of computation of
		the variable s[j] later.
		The constraint modification vector, bcons, is updated to
			Acons * [0 cbs[i] 0]^T + bcons,
		and the constraint modification matrix is then updated to
			        [  I     0  ]
			Acons * [-E_1  -E_2 ]  <- jth row
			        [  0     I  ]
*/
int i,j;
double temp,largest;

while (*nact > 0)
	{
	/* Find the largest coefficient in the current active constraint. */
	largest = ZERO;
	for (j=0; j<n; j++)
		{
		if (fabs(cA[apporder[*napp]][j]) > largest)
			{
			largest = fabs(cA[apporder[*napp]][j]);
			selcons[*napp][0] = j;
			selcons[*napp][1] = apporder[*napp];
			}
		}
	if (largest == ZERO) 
		/* This constraint should now be satisfied as a strict inequality,
			so we can inactivate it.  If not, then something is wrong. */
		{
		if (cbs[apporder[*napp]] < ZERO) return -1;
		cstate[apporder[*napp]] = inactive;
		/* Shift the apporder array to eliminate this active constraint. */
		for (i=*napp; i<*napp+*nact-1; i++) apporder[i] = apporder[i+1];
		}
	else
		{
		/* Update the index of variables that are free. */
		i = 0;
		while (freeind[i] != selcons[*napp][0]) i++;
		for (j=i+1; j<n-*napp; j++) freeind[j-1] = freeind[j];
		
		/* Update the selected constraint equation. */
		temp = cA[selcons[*napp][1]][selcons[*napp][0]];
		for (j=0; j<selcons[*napp][0]; j++) 
			cA[selcons[*napp][1]][j] /= temp;
		/* Set selected column to zero rather than 1 for ease of future comp. */
		cA[selcons[*napp][1]][selcons[*napp][0]] = ZERO;
		for (j=selcons[*napp][0]+1; j<n; j++) 
			cA[selcons[*napp][1]][j] /= temp;
		cbs[selcons[*napp][1]] /= temp;
	
		/* Mark the selected constraint as applied. */
	   cstate[selcons[*napp][1]] = applied;
	
		/* Update the unapplied constraint equations. */
		for (i=0; i<nc; i++)
			{
			if (cstate[i] != applied)
				{
				temp = cA[i][selcons[*napp][0]];
				for (j=0; j<selcons[*napp][0]; j++)
					cA[i][j] -= temp*cA[selcons[*napp][1]][j];
				/* Set selected column to zero directly instead of subtracting 
					same value to avoid numerical errors. */
				cA[i][selcons[*napp][0]] = ZERO;
				for (j=selcons[*napp][0]+1; j<n; j++)
					cA[i][j] -= temp*cA[selcons[*napp][1]][j];
				cbs[i] -= temp*cbs[selcons[*napp][1]];
				}
			}
	
		/* Update the constraint modification vector. */
		temp = cbs[selcons[*napp][1]];
		for (i=0; i<n; i++)
			bcons[i] += temp*Acons[i][selcons[*napp][0]];
	
		/* Update the constraint modification matrix. */
		for (i=0; i<n; i++)
			{
			temp = Acons[i][selcons[*napp][0]];
			for (j=0; j<selcons[*napp][0]; j++)
				Acons[i][j] -= temp*cA[selcons[*napp][1]][freeind[j]];
			for (j=selcons[*napp][0]; j<n-*napp-1; j++)
				Acons[i][j] = Acons[i][j+1] - 
					temp*cA[selcons[*napp][1]][freeind[j]];
			}
		(*napp)++;
		}

	/* This constraint has been dealt with so reduce the number of 
		active constraints by one. */
	(*nact)--;
	}

return 0;
}

/*--------------------------------------------------------------------*/
void genconssys(int m, int n, int napp, double *fjac, double *qtf,
		int *ipvt, double **Acons, double *bcons, double *R, double *b,
		int *perm)
{
/*
	genconssys - generate a QR factorized constrained system to be
		solved by lmpar.

	Parameters:
		m - input, rows in fjac
		n - input, columns in fjac
		napp - input, number of applied constraints
		fjac - input, QR factorized unconstrained system
		qtf - input, Q^T * f for unconstrained system
		ipvt - input, permutation matrix for QR factorization of 
			unconstrained system
		Acons - input, constraint modification matrix
		bcons - input, constraint modification vector
		R - output, upper triangular R of the QR factorized constrained
			system
		b - output, Q^T * right hand side vector for constrained system
		perm - output, permutation matrix for QR factorization of
			constrained system

	This routine modifies the original QR factorized system to one
	which incorporates the constraints as specified by the constraint
	modification matrix and vector.  The original system is
		jacobian * s = -f.
	jacobian is QR factorized so that  jacobian*P = (fjac_Q)*(fjac_R) 
	where P is given by ipvt.  The constrained system is
		(fjac_R)*(P^T)*Acons * s = -qtf - (fjac_R)*(P^T)*bcons
	where qtf is (fjac_Q)*f.  This routine calculates this new system
	and then QR factorizes it so that it is Q*R*perm^T * s = b0.  It 
	returns R, b=Q^T * b0, and perm.
*/
int i,j,k,ij,jj;
double temp1,temp2,sum;
double *rdiag;

/* Allocate some space. */
rdiag = (double *) malloc(n*sizeof(double));

/* Create the new system matrix R = (fjac_R)*(P^T)*Acons, and 
	b = -qtf - (fjac_R)*(P^T)*bcons. */
for (j=0; j<n-napp; j++)
	{
	jj = j*n;
	for (i=0; i<n; i++) R[jj+i] = ZERO;
	for (k=0; k<n; k++)
		{
		ij = k*m;
		for (i=0; i<=k; i++) 
			R[jj+i] += fjac[ij+i]*Acons[ipvt[k]][j];
		}
	}
for (i=0; i<n; i++) b[i] = -qtf[i];
for (k=0; k<n; k++)
	{
	ij = k*m;
	for (i=0; i<=k; i++)
		b[i] -= fjac[ij+i]*bcons[ipvt[k]];
	}

/* QR factorize R. */
qrfac(n,n-napp,R,perm,rdiag,rdiag);

/* Compute Q^T * b, store in b, and then put rdiag into R. */
for (j=0; j<n-napp; j++)
   {
   ij = j*n;
   jj = ij+j;
   temp2 = R[jj];
   if (temp2 != ZERO)
      {
      sum = ZERO;
      for (i=j; i<n; i++) sum += R[ij+i] * b[i];
      temp1 = -sum / temp2;
      for (i=j; i<n; i++) b[i] += R[ij+i] * temp1;
      }
   R[jj] = rdiag[j];
   }

/* Free the space. */
free(rdiag);
}

/*--------------------------------------------------------------------*/
int testcons(int n, int napp, double *x, int *freeind, int nc, 
		double **cA, double *cbs, enum cons_state *cstate, int *apporder)
{
/*
	testcons - test the inactive and deactive constraints to see if they 
		are met, if not, activate them.  The activated constraints are 
		ordered for application according to how severely they were 
		violated.  The activation counters are updated if the constraint
		was deactive.

	Parameters:
		n - input, total number of variables
		napp - input, number of applied constraints
		x - input, current variable values
		freeind - input, index of free variables
		nc - input, total number of constraint eqns
		cA - input, coefficient matrix of constraint eqns
		cbs - input, right hand side vector of constraint eqns
		cstate - input/output, activation state of constraints
		apporder - output, order to apply constraints

	Return value:
		number of newly activated constraints (i.e. number of inactive
		or deactive constraints which failed to be met)
*/
int i,j,k,new,hi,lo;
double temp,*viol;

if (nc < 1) return 0;

viol = (double *) malloc(nc*sizeof(double));

new = 0;
for (i=0; i<nc; i++)
	{
	if (cstate[i] == inactive || cstate[i] == deactive)
		{
		temp = ZERO;
		for (j=0; j<n-napp; j++) temp += cA[i][freeind[j]] * x[j];
		if (temp > cbs[i])   /* The constraint is not met; activate it. */
			{
			cstate[i] = active;
			/* Order the activated constraints according to the severity of 
				the failure.  cbs[i]/temp will never be negative since if 
				temp>cbs[i] and the ratio is negative then this implies that 
				this constraint was violated last step (temp=0) but not 
				applied.  The smaller cbs[i]/temp is, the more severe was 
				the failure. */
			temp = cbs[i]/temp;
			if (new == 0 || temp < viol[0]) hi = 0;
			else if (temp >= viol[new-1]) hi = new;
			else
				{
				lo = 0;
				hi = new-1;
				while (hi-lo>1)
					{
					k = (hi+lo)/2;
					if (viol[k] <= temp) lo = k;
					else hi = k;
					}
				}
			for (j=new; j>hi; j--)
				{
				viol[j]= viol[j-1];
				apporder[napp+j] = apporder[napp+j-1];
				}
			viol[hi] = temp;
			apporder[napp+hi] = i;
			new++;
			}
		}
	}
free(viol);

return new;
}
