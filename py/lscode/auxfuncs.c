/*
	 This file contains:
		binsearch - binary search in a sorted array
		getprottype - classify sweeps according to experiment type
		getIres - fit residual current
		expsolve - Newton's method for solving exp(u)=1+b*u.
		linfit - do a linear fitting
		polyform - define a polynomial functional form
		getpeak - get peak currents and times as part of the initial 
			parameter value estimation
		noisecalc - calculate the noise level in the data by fitting a 
			low order polynomial to a portion of the data
		secalc - calculate the standard errors of the best fit parameter 
			values using the Jacobian evaluated at the optimal solution
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lsdata.h"
#include "dpmach.h"
#include "auxfuncs.h"
#include "qrfac.h"
#include "enorm.h"

#define MAX(a,b)  ((a) >= (b) ? (a) : (b))
#define MIN(a,b)  ((a) <= (b) ? (a) : (b))
#define ROUND(a) ((a)-floor((a)) < ceil((a))-(a) ? floor((a)) : ceil((a)))


/* ------------------------------------------------------------------- */

int binsearch(double x, int n, double *y)
{
/*
	Do a binary search in the non-decreasing array y for the index of
	the element closest to x.

	x - input, value to search for
	n - input, length of array y
	y - input, array of length n, monotonic increasing

	Return value: index of element in y which is closest to x.
	If a number of elements of y are identical to x, then the index
	of the first element is returned.

*/
int lo,hi,i;

lo = 0;
hi = n-1;
if (hi <= 0) return 0;
if (x <= y[lo]) return 0;
if (x > y[hi]) return hi; 
while (x==y[hi]) hi--;
if (hi<n-1) return ++hi;
while (hi-lo>1)
	{
	i = (hi+lo)/2;
	if (y[i] < x) lo = i;
	else hi = i;
	}
if (x-y[lo] < y[hi]-x) return lo;
else return hi;
}

/* ------------------------------------------------------------------- */

void getprottype(double Erev, struct dataspecs *dataspecs, int *prottype, 
	int count[3])
{
/*
	This function will classify each sweep into one of four	categories:
		0 - fit window is empty or step = reverasal potential
		1 - from the set of varying prestep experiments
		2 - from the set of varying step experiments
	   -1 - none of the above (due to having multiple files of either
			 type with different prestep or step potentials)
	It will also count the number of sweeps of each type 0, 1, and 2.

	Erev - input, reversal potential
	dataspecs - input, pointer to dataspecs structure
	prottype - output, size nswp, classification array
	count - output, number in each classification

	The classification is done by finding set of varying step potential 
	experiments with the most different prestep potential sweeps with non-
	empty fit regions, and the set of varying prestep potential experiments 
	with the most different step potential sweeps with nonempty fit regions.
	Any sweeps not in these sets are classified as -1.
	If the fit window for a sweep is empty it is classified as type 0.
*/
int i,j,k,nset[2],*set[2],selset[2],*ct[2],identical,haveset;

/* Allocate space. */
for (j=0; j<2; j++) {
	set[j] = (int *) malloc(dataspecs->nswp*sizeof(int));
	ct[j] = (int *) calloc(dataspecs->nswp, sizeof(int));
}

/* Determine the different sets of experiments and the number in each set. */
nset[0] = 0;
nset[1] = 0;
for (i=0; i<dataspecs->nswp; i++) {
	if (dataspecs->sweeps[i].Volt.VPtype == varyingstep) {
		if (dataspecs->sweeps[i].fitWin.nsample < 1 ||
			dataspecs->sweeps[i].Volt.step == Erev) {
			set[0][i] = -1;
		}
		else {
	     	/* Classify this varying step potential experiment. */
	        haveset = -1;
	  	    identical = 0;
		    for (j=0; j<i; j++) {
		    	if (set[0][j] != -1 && 
					dataspecs->sweeps[j].Volt.VPtype == varyingstep) {
			    	if (dataspecs->sweeps[j].Volt.prestep ==
			    		dataspecs->sweeps[i].Volt.prestep) {
			    		haveset = j;
			    		if (dataspecs->sweeps[j].Volt.step ==
			    			dataspecs->sweeps[i].Volt.step) identical = 1;
					}
				}
			}
	    	if (haveset == -1) 
	    		set[0][i] = nset[0]++;
	    	else
	    		set[0][i] = set[0][haveset];
	    	if (!identical) ct[0][set[0][i]]++;
		}
		set[1][i] = -1;  /* this sweep not a varying prestep */
	}
	else { 
        if (dataspecs->sweeps[i].fitWin.nsample < 1 ||
			dataspecs->sweeps[i].Volt.step == Erev) {
			set[1][i] = -1;
		}
		else {
		    /* Classify this varying prestep potential experiment. */
		    haveset = -1;
	    	identical = 0;
		    for (j=0; j<i; j++) {
		    	if (set[1][j] != -1 && 
					dataspecs->sweeps[j].Volt.VPtype == varyingprestep) {
		    		if (dataspecs->sweeps[j].Volt.step ==
			    		dataspecs->sweeps[i].Volt.step) {
			    		haveset = j;
			    		if (dataspecs->sweeps[j].Volt.prestep ==
			    			dataspecs->sweeps[i].Volt.prestep) identical = 1;
					}
				}
			}
	    	if (haveset == -1) 
		    	set[1][i] = nset[1]++;
		    else
			    set[1][i] = set[1][haveset];
	    	if (!identical)	ct[1][set[1][i]]++;
		}
	    set[0][i] = -1;  /* this sweep not a varying step */
	}
}

/* Select the sets with the highest counts. */
for (k=0; k<2; k++) {
	i = 0;
	selset[k] = -2;
	for (j=0; j<nset[k]; j++) {
		if (ct[k][j] > i) {
			selset[k] = j;
			i = ct[k][j];
		}
	}
}

/* Initialize the counts. */
for (i=0; i<3; i++) count[i] = 0;

/* Classify each sweep. */
for (i=0; i<dataspecs->nswp; i++)
	{
	if (set[0][i] == -1 && set[1][i] == -1)
		{
		prottype[i] = 0;
		count[0]++;
		}
	else if (set[0][i] == selset[0])
		{
		prottype[i] = 2;
		count[2]++;
		}
	else if (set[1][i] == selset[1])
		{
		prottype[i] = 1;
		count[1]++;
		}
	else
		prottype[i] = -1;
	}
	
/* Free space. */
for (j=0; j<2; j++) {
	free(set[j]);
	free(ct[j]);
}
}

/* ------------------------------------------------------------------- */

double getIres(int fact, int nh, int tinfl, struct sweep *sweep)
{
/*
	This function will fit the last few points of the fit window to a
	parabola and will use this info to estimate the residual current.  
	It is assumed that the fit window has at least three points.
	
	fact - input, whether a sweep is negative or positive current
	nh - input, number of inactivation time constants
	tinfl - input, location of inflection point if nh>0
	sweep - input, sweep structure
	Return value
	Ires, residual current
*/
int ii,i,j,k;
double *A,*b,coef[3],Ires;

ii = sweep->fitWin.str + sweep->fitWin.nsample;
if (nh > 0) 
	i = ii-(ii-tinfl)/5;  /* take last fifth of points from tinfl */
else
	i = sweep->fitWin.str + 4*sweep->fitWin.nsample/5;
j = ii-i;
A = (double *) malloc(3*j*sizeof(double));
b = (double *) malloc(j*sizeof(double));
for (k=0; k<j; k++)
	{
	A[k] = 1.0;
	A[j+k] = sweep->time[i+k] - sweep->time[ii-1];
	A[2*j+k] = pow(A[j+k],2.0);
	b[k] = sweep->data[i+k];
	}
linfit(j,3,A,b,coef);
/* If the 2nd derivative is opposite to expected then this is due to noise,
	re-fit the data to a straight line and set coef[2] to be zero. */
if ((nh > 0 && fact*coef[2] < 0.0) || (nh == 0 && fact*coef[2] > 0.0))
	{
	linfit(j,2,A,b,coef);
	coef[2] = 0.0;
	}
/* If the slope is opposite to expected then we can assume the slope
	is due only to noise, re-fit the data to a constant. */
if ((nh > 0 && fact*coef[1] > 0.0) || (nh ==0 && fact*coef[1] < 0.0))
	{
	coef[0] = b[0];
	for (k=1; k<j; k++) coef[0] += b[k];
	coef[0] /= ((double) j);
	for (k=1; k<3; k++) coef[k] = 0.0;
	}
/* If the constant is opposite to expected, then set it to zero. */
if (fact*coef[0] < 0 )
	{
	for (k=0; k<3; k++) coef[k] = 0.0;
	}
/* Ires is coef[0] - coef[1]^2/(2*coef[2]) */
if (coef[2] != 0.0)
	Ires = coef[0] - pow(coef[1],2.0)/(2.0*coef[2]);
else if (coef[1] == 0.0)
	Ires = coef[0];
else  /* Estimate Tau_h to be about 0.5*tmax, or Tau_m to be 0.2*tmax */
	{
	if (nh > 0)
		Ires = coef[0] + coef[1]*0.5*sweep->time[ii-1];
	else
		Ires = coef[0] + coef[1]*0.2*sweep->time[ii-1];
	}

if (fact == 1) Ires = MAX(0.0,Ires);
else Ires = MIN(0.0,Ires);
free(A);
free(b);
return Ires;
}

/* ------------------------------------------------------------------- */

double expsolve(double b, double tol)
{
/*
	This function will use Newton's method to solve the equation 
	exp(u) = 1+b*u. 
	The nonzero solution is found if it exists.
	
	b - input, constant
	tol - input, tolerance level
	Return value: u
*/

double u,f;

if (b == 1.0 || b <= 0.0) return 0.0;
u = 1.5*log(b);
while (fabs(f = exp(u) - b*u - 1.0) > tol) u = u - f/(exp(u)-b);
return u;
}

/* ------------------------------------------------------------------- */

void linfit(int m, int n, double *A, double *b, double *x)
{
/*
	This function will find the least squares solution to A*x=b.
	It calls the routine qrfac to QR factorize A.

	m - input, number of rows of A
	n - input, number of elements of x
	A - input/output, mxn matrix stored columnwise in a vector
	    on output the upper triangle of A has the R part of the
	    QR factorization of A.
	b - input, vector with m entries
	x - output, solution x
*/

int i,j,ij,jj;
int *ipvt;
double *diag,*qtb;
double sum,temp;

/* Allocate space. */
ipvt = (int *) malloc(n*sizeof(int));
diag = (double *) malloc(n*sizeof(double));
qtb = (double *) malloc(m*sizeof(double));

qrfac(m,n,A,ipvt,diag,diag);

/* compute Q (transpose) * b. */
for (i=0; i<m; i++) qtb[i] = b[i];
for (j=0; j<n; j++)
   {
   ij = j*m;
   jj = ij+j;
   temp = A[jj];
   if (temp != 0.0)
      {
      sum = 0.0;
      for (i=j; i<m; i++) sum += A[ij+i] * qtb[i];
      temp = -sum / temp;
      for (i=j; i<m; i++) qtb[i] += A[ij+i] * temp;
      }
	A[jj] = diag[j];
   }

/* Use back substitution to find the solution of R*P^T*x=Q^T * b */
jj = n-1;
while (diag[jj] == 0.0) x[ipvt[jj--]] = 0.0;
for (j=jj; j>=0; j--)
	{
	x[ipvt[j]] = qtb[j] / diag[j];
	ij = j*m;
	for (i=0; i<j; i++)
		{
		qtb[i] -= A[ij+i] * x[ipvt[j]];
		}
	}

/* Free space. */
free(ipvt);
free(diag);
free(qtb);
}

/* ------------------------------------------------------------------- */

void polyform(int st, int en, int deg, double *x, double *y, int *n,
	double *A, double *b)
{
/*
	This function will generate the design matrix A and b for fitting a
	portion of the data (x,y) to a linear combination of the functions, 
	1, x, x^2, ..., x^deg

	st - input, start index for data
	en - input, end index for data
	deg - input, degree for polynomial, deg>=0.  If deg<0, 0 is used.
	x,y - input, data
	n - input/output, number of points from data to use/used
	A - output, design matrix stored columnwise (size: deg*n)
	b - output, right hand vector (size: n)

	The points selected from (x,y) are determined by dividing (en-st)
	evenly into n-1 pieces and taking the closest point to each division.
	If n does not satisfy n<=en-st+1, then n is set to en-st+1.
*/

int i,j;
double inc;

/* Determine n and the increment. */
*n = MIN(*n,en-st+1);
inc = ((double) (en-st)) / ((double) (*n-1));

/* Construct the design matrix and right hand vector. */
for (i=0; i<*n; i++) A[i] = 1.0;
if (deg>=1) 
	for (i=0; i<*n; i++) A[*n+i] = x[st + (int) ROUND(i*inc)];
for (j=2; j<=deg; j++)
	{
	for (i=0; i<*n; i++)
		A[j*(*n)+i] = A[*n+i] * A[(j-1)*(*n)+i];
	}
for (i=0; i<*n; i++) b[i] = y[st + (int) ROUND(i*inc)];
}

/* ------------------------------------------------------------------- */

void getpeak(int fact, struct sweep *sweep, double *Ipeak, 
	double *tpeak, int *tinfl)
{
#define MAXFITP 100   /* Max and min number of points for fitting. */
#define MINFITP 40    
#define WINSIZE 5

/*
	This function will compute the peak current and the peak time, and 
	the index of the time of the inflection point after the peak time.

	fact - input, whether a sweep is positive or negative current.
	sweep - input, pointer to sweep structure
	Ipeak - output, peak current, always a positive number
	tpeak - output, peak time
	tinfl - output, index of time of inflection point
*/

int i,tloc;
int st,en,n;
double tmax,temp,Icrit,tcrit,t1,I1;
double coef[4];
double *A,*b;

/* Allocate space. */
A = (double *) malloc(4*MAXFITP*sizeof(double));
b = (double *) malloc(MAXFITP*sizeof(double));

/* Set the start and end indices according to the fitting window.
	We do not want time=0.0 so st must be 1 at the smallest. */
st = MAX(1,sweep->fitWin.str);   
en = sweep->fitWin.str + sweep->fitWin.nsample - 1;

/* Do a sliding window average to estimate the peak. */
if (en-st+1 < WINSIZE)
	{
	tloc = (en+st)/2;
	Icrit = fact * sweep->data[tloc];
	}
else
	{
	temp = 0.0;
	for (i=st; i<st+WINSIZE; i++) temp += fact * sweep->data[i];
	tloc = st + WINSIZE/2;
	Icrit = temp;
	for (i=st+WINSIZE; i<=en; i++)
		{
		temp = temp + fact * (sweep->data[i] - sweep->data[i-WINSIZE]);
		if (temp > Icrit) 
			{
			tloc = i-WINSIZE/2;
			Icrit = temp;
			}
		}
	}
Icrit /= ((double) WINSIZE);
tcrit = sweep->time[tloc];
/*
*/

/* Discard data at ends of trace that are below 75% of the estimated peak 
	or more than MINFITP away from the estimated peak location. */
/*
while (((fact*(sweep->data[st]) < 0.75*Icrit) || (tloc-st > MINFITP)) 
		  && (en-st+1 > MINFITP)) st++;
while (((fact*(sweep->data[en]) < 0.75*Icrit) || (en-tloc > MINFITP))
		  && (en-st+1 > MINFITP)) en--;

printf("st = %i en = %i\n",st,en);
n = MAXFITP;
polyform(st,en,3,sweep->time,sweep->data,&n,A,b);
printf("Calling linfit, n=%i\n",n);
linfit(n,4,A,b,coef);
if (fact == -1)
	for (i=0; i<4; i++) coef[i] *= fact;
printf("REturned from linfit, coef=%8.3g %8.3g %8.3g %8.3g\n",coef[0],
coef[1],coef[2],coef[3]);
Icrit = 0.0;
for (i=st; i<=en; i+=en-st)
	{
	t1 = sweep->time[i];
	I1 = coef[0] + coef[1]*t1 + coef[2]*pow(t1,2.0) + coef[3]*pow(t1,3.0);
	if (I1 > Icrit)
		{
		tcrit = t1;
		Icrit = I1;
		tloc = i;
		}
	}
temp = coef[2]*coef[2] - 3.0*coef[1]*coef[3];
if (temp > 0.0)
	{
	for (i=0; i<2; i++)
		{
		t1 = -coef[2] + (2*i-1) * sqrt(temp);
		if (t1 > sweep->time[st] && t1 < sweep->time[en])
			{
			I1 = coef[0] + coef[1]*t1 + coef[2]*pow(t1,2.0) + 
				  coef[3]*pow(t1,3.0);
			if (I1 > Icrit)
				{
				tcrit = t1;
				Icrit = I1;
				tloc = st + binsearch(tcrit,en-st+1,&(sweep->time[st]));
				}
			}
		}
	}

printf("Second estimate: t=%8.3g I=%8.3g\n",tcrit,Icrit);
*/

/* Set the peak time and current. */
*tpeak = tcrit;
*Ipeak = Icrit;

/* Now find the inflection point time by fitting the region from the
	peak time to 3*tpeak to a cubic polynomial. */
st = tloc;
tmax = sweep->time[sweep->fitWin.str + sweep->fitWin.nsample - 1];
i = sweep->fitWin.str + sweep->fitWin.nsample - st;
en = st + binsearch(MIN(tmax,3.0*tcrit),i,&(sweep->time[st]));
n = MAXFITP;
polyform(st,en,3,sweep->time,sweep->data,&n,A,b);
linfit(n,4,A,b,coef);
temp = -coef[2]/(3.0*coef[3]);
if (sweep->time[st] < temp && temp < sweep->time[en]) 
	*tinfl = st + binsearch(temp,en-st+1,&(sweep->time[st]));
else
	*tinfl = st + binsearch(tcrit+MIN(tcrit,(tmax-tcrit)/2.0),
				     en-st+1,&(sweep->time[st]));

/* Free space. */
free(A);
free(b);
}

/* ------------------------------------------------------------------- */

int noisecalc(int deg, int ndata, double *time, double *data, 
	double *noise)
{
/*
	This function will compute the noise level for a given set of
	data by fitting the data to a polynomial of degree deg.

	deg - input, degree of polynomial
	ndata - input, number of data points
	time - input, time array
	data - input, data array
	noise - output, noise level (standard error of fit)
	return value - 1 if insufficient data, in this case noise is set
		to 1.0
*/

int i,j,n;
int *ipvt;
double *a, *wa1, *wa2, sum, temp;

*noise = 1.0;
if (deg < 0) deg = 0;
n = deg + 1;
if (ndata <= n) return 1;  /* Not enough data in the window */

a = (double *) malloc(ndata*n*sizeof(double));
wa1 = (double *) malloc(n * sizeof(double));
wa2 = (double *) malloc(ndata * sizeof(double));
ipvt = (int *) malloc(n* sizeof(int));

/* set up the design matrix, stored columnwise */
for (i=0; i < ndata; i++) a[i] = 1.0;
if (deg>0) for (i=0; i < ndata; i++) a[ndata+i] = *(time+i);
for (j=2; j < n; j++)
	for (i=0; i < ndata; i++)
		a[j*ndata+i] = *(time+i) * a[(j-1)*ndata+i];

/* get the qr factorisation of the design matrix */
qrfac(ndata, n, a, ipvt, wa1, wa1);

/* Copy the data to a work array. */
for (i = 0; i < ndata; i++) wa2[i] = *(data+i);

/* form q'*b where q is (ndata x n) and b is (ndata x 1) the data */
/* store this in the first n entries of wa2 */
for (j = 0; j < n; j++)
	{
	sum = 0.0;
	for (i = j; i < ndata; i++)
		sum += a[j*ndata+i] * wa2[i];
	temp = -sum/a[j*ndata+j];
	for (i = j; i < ndata; i++)
		wa2[i] += a[j*ndata+i] * temp;
	}

/* calculate the RMS noise.
   ||Ax-b||^2 = ||b||^2 - ||q'*b||^2, where A=q*r. */
wa1[0] = enorm(n, wa2);
wa2[0] = enorm(ndata, data);
*noise = sqrt((wa2[0]*wa2[0] - wa1[0]*wa1[0])/((double) (ndata-deg-1)));

free(ipvt);
free(wa2);
free(wa1);
free(a);
return 0;
}

/* ------------------------------------------------------------------- */

int secalc(int m, int n, double *R, int *ipvt, double *stderror)
{
/*
	This function will compute the standard errors for a set of n
	parameters that have been fit via least squares.
	The standard errors are the square roots of the diagonal entries of 
	(J^T J)^{-1}, where J is the mxn Jacobian matrix, dF/da.  It is 
	assumed that J has been QR factorized so that JP = QR where P is a 
	permutation matrix and R is upper triangular with diagonal entries 
	of nonincreasing magnitude.  It is also assumed that F has been 
	scaled by the noise, that is F = (f_model - f_obs)/noise.  If this
	is not the case, then the standard errors output from this routine
	need to be multiplied by the noise.  If the noise level is constant
	for all observation points, then the noise is the Euclidian norm of
	(f_model - f_obs).
	If any of the diagonal entries of R are zero (singular matrix) then
	the corresponding parameters do not affect the function.  In this
	case we only look at the nonsingular columns of R and assign
	0 for the standard error for the singular parameters.

	m - input, number of rows in R
	n - input, number of columns in R
	R - input, mxn matrix stored columnwise.  Only the full upper 
		 triangle is used.
	ipvt - input, permutation array defining the permutation matrix P, 
			 the ith column of P is the ipvt(i)th column of the identity
			 matrix
	stderror - output, standard errors for each column of R
	return value - rank of R
*/

int ncol, i, j, k;
double temp;

/* Determine how many independent columns in R. */
i = n-1;
while (i>=0 && fabs(R[i*m+i]) < MACHEP) i--;
ncol = i+1;

/* R is upper triangular. Calculate (R inverse) transpose (which is
	lower triangular) and store in the lower triangle of R. */
/* start with last column and work backwards, 
	i is the column label, columns are labelled between 0 to n-1 */
for (i=ncol-1; i>=0; i--)
	{ 
	/* compute the diagonal entry */
	R[i*m+i] = 1.0/R[i*m+i];
	/* compute the entries left of the diagonal, 
	   j is the row label, rows are labelled between 0 and n-1 */
	for (j=i-1; j>=0; j--)
		{
		temp = 0.0;
		for (k=0; k<i-j; k++)
			{
			temp += R[(i-k)*m+j]*R[(i-k)*m+i];
			}
		R[j*m+i] = -temp/R[j*m+j];
		}
	}

/* Compute the square root of the diagonal entries of R^{-1} R^{-1}^T 
	and store in the first column of R */
for (k=0; k<ncol; k++)
	R[k] = enorm(ncol-k,&R[k*m+k]);
	 
/* unapply the permutation */
for (k = 0; k < ncol; k++)
	stderror[ipvt[k]] = R[k];
for (k = ncol; k < n; k++)
	stderror[ipvt[k]] = 0.0;
return ncol;
}
