#include <stdlib.h>
#include <math.h>
#include "dpmach.h"
#include "enorm.h"

#define MAX(a,b)  ((a) >= (b) ? (a) : (b))
#define MIN(a,b)  ((a) <= (b) ? (a) : (b))

void qrfac(int m, int n, double a[], int *ipvt, double rdiag[], 
           double acnorm[])
{

/*
*     **********
*
*     function qrfac
*
*     this subroutine uses householder transformations with column
*     pivoting (optional) to compute a qr factorization of the
*     m by n matrix a. that is, qrfac determines an orthogonal
*     matrix q, a permutation matrix p, and an upper trapezoidal
*     matrix r with diagonal elements of nonincreasing magnitude,
*     such that a*p = q*r. the householder transformation for
*     column k, k = 1,2,...,min(m,n), is of the form
*
*			    t
*	    i - (1/u(k))*u*u
*
*     where u has zeros in the first k-1 positions. the form of
*     this transformation and the method of pivoting first
*     appeared in the corresponding linpack subroutine.
*
*  Parameters:
*
*	m is a positive integer input variable set to the number
*	  of rows of a.
*
*	n is a positive integer input variable set to the number
*	  of columns of a.
*
*	a is an m by n array. on input a contains the matrix for
*	  which the qr factorization is to be computed. on output
*	  the strict upper trapezoidal part of a contains the strict
*	  upper trapezoidal part of r, and the lower trapezoidal
*	  part of a contains a factored form of q (the non-trivial
*	  elements of the u vectors described above).
*
*	ipvt is a pointer.  If the pointer is NULL, then no column
*    pivotting is done.  If the pointer is not null, then it
*    should point to an area large enough for an array of n 
*    integers.  This output array defines the permutation 
*    matrix p such that a*p = q*r.
*	  column j of p is column ipvt(j) of the identity matrix.
*
*	rdiag is an output array of length n which contains the
*	  diagonal elements of r.
*
*	acnorm is an output array of length n which contains the
*	  norms of the corresponding columns of the input matrix a.
*	  if this information is not needed, then acnorm can coincide
*	  with rdiag.
*
*     subprograms called
*
*	minpack-supplied ... enorm
*
*	C math library ... sqrt
*
*     argonne national laboratory. minpack project. march 1980.
*     burton s. garbow, kenneth e. hillstrom, jorge j. more
*
*     **********
*/
int i,ij,jj,j,jp1,k,kmax,minmn;
short pivot;
double ajnorm,sum,temp;
double *wa;
static double zero = 0.0;
static double one = 1.0;
static double p05 = 0.05;

/* Allocate space. */
wa = (double *) malloc(n*sizeof(double));

/* Determine if column pivotting is to take place. */
if (ipvt != NULL)
	pivot = 1;
else
	pivot = 0;

/*
*     compute the initial column norms and initialize several arrays.
*/
ij = 0;
for( j=0; j<n; j++ )
	{
	acnorm[j] = enorm(m,&a[ij]);
	rdiag[j] = acnorm[j];
	wa[j] = rdiag[j];
	if(pivot)
		ipvt[j] = j;
	ij += m; /* m*j */
	}
/*
*     reduce a to r with householder transformations.
*/
minmn = MIN(m,n);
for( j=0; j<minmn; j++ )
	{
	if (!pivot)
		goto L40;
/*
*	 bring the column of largest norm into the pivot position.
*/
	kmax = j;
	for( k=j; k<n; k++ )
		{
		if(rdiag[k] > rdiag[kmax])
			kmax = k;
		}
	if(kmax == j)
		goto L40;
	
	ij = m * j;
	jj = m * kmax;
	for( i=0; i<m; i++ )
		{
		temp = a[ij]; /* [i+m*j] */
		a[ij] = a[jj]; /* [i+m*kmax] */
		a[jj] = temp;
		ij += 1;
		jj += 1;
		}
	rdiag[kmax] = rdiag[j];
	wa[kmax] = wa[j];
	k = ipvt[j];
	ipvt[j] = ipvt[kmax];
	ipvt[kmax] = k;
	
	L40:
/*
*	 compute the householder transformation to reduce the
*	 j-th column of a to a multiple of the j-th unit vector.
*/
	jj = j + m*j;
	ajnorm = enorm(m-j,&a[jj]);
	if(ajnorm == zero)
		goto L100;
	if(a[jj] < zero)
		ajnorm = -ajnorm;
	ij = jj;
	for( i=j; i<m; i++ )
		{
		a[ij] /= ajnorm;
		ij += 1; /* [i+m*j] */
		}
	a[jj] += one;
/*
*	 apply the transformation to the remaining columns
*	 and update the norms.
*/
	jp1 = j + 1;
	if(jp1 < n )
		{
		for( k=jp1; k<n; k++ )
			{
			sum = zero;
			ij = j + m*k;
			jj = j + m*j;
			for( i=j; i<m; i++ )
				{
				sum += a[jj]*a[ij];
				ij += 1; /* [i+m*k] */
				jj += 1; /* [i+m*j] */
				}
			temp = sum/a[j+m*j];
			ij = j + m*k;
			jj = j + m*j;
			for( i=j; i<m; i++ )
				{
				a[ij] -= temp*a[jj];
				ij += 1; /* [i+m*k] */
				jj += 1; /* [i+m*j] */
				}
			if( (pivot) && (rdiag[k] != zero) )
				{
				temp = a[j+m*k]/rdiag[k];
				temp = MAX( zero, one-temp*temp );
				rdiag[k] *= sqrt(temp);
				temp = rdiag[k]/wa[k];
				if( (p05*temp*temp) <= MACHEP)
					{
					rdiag[k] = enorm(m-j-1,&a[jp1+m*k]);
					wa[k] = rdiag[k];
					}
				}
			}
		}
	
	L100:
		rdiag[j] = -ajnorm;
	}

free(wa);
/*
*     last card of subroutine qrfac.
*/
}
