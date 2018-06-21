#include <stdlib.h>
#include <math.h>

void qrsolv(int n, double r[], int ldr, int ipvt[], double diag[], 
            double qtb[], double x[], double sdiag[])
{
/*
*     **********
*
*     function qrsolv
*
*     given an m by n matrix a, an n by n diagonal matrix d,
*     and an m-vector b, the problem is to determine an x which
*     solves the system
*
*	    a*x = b ,	  d*x = 0 ,
*
*     in the least squares sense.
*
*     this subroutine completes the solution of the problem
*     if it is provided with the necessary information from the
*     qr factorization, with column pivoting, of a. that is, if
*     a*p = q*r, where p is a permutation matrix, q has orthogonal
*     columns, and r is an upper triangular matrix with diagonal
*     elements of nonincreasing magnitude, then qrsolv expects
*     the full upper triangle of r, the permutation matrix p,
*     and the first n components of (q transpose)*b. the system
*     a*x = b, d*x = 0, is then equivalent to
*
*		   t	   t
*	    r*z = q *b ,  p *d*p*z = 0 ,
*
*     where x = p*z. if this system does not have full rank,
*     then a least squares solution is obtained. on output qrsolv
*     also provides an upper triangular matrix s such that
*
*	     t	 t		 t
*	    p *(a *a + d*d)*p = s *s .
*
*     s is computed within qrsolv and may be of separate interest.
*
*  Parameters:
*
*	n is a positive integer input variable set to the order of r.
*
*	r is an n by n array. on input the full upper triangle
*	  must contain the full upper triangle of the matrix r.
*	  on output the full upper triangle is unaltered, and the
*	  strict lower triangle contains the strict upper triangle
*	  (transposed) of the upper triangular matrix s.
*
*	ldr is a positive integer input variable not less than n
*	  which specifies the leading dimension of the array r.
*
*	ipvt is an integer input array of length n which defines the
*	  permutation matrix p such that a*p = q*r. column j of p
*	  is column ipvt(j) of the identity matrix.
*
*	diag is an input array of length n which must contain the
*	  diagonal elements of the matrix d.
*
*	qtb is an input array of length n which must contain the first
*	  n elements of the vector (q transpose)*b.
*
*	x is an output array of length n which contains the least
*	  squares solution of the system a*x = b, d*x = 0.
*
*	sdiag is an output array of length n which contains the
*	  diagonal elements of the upper triangular matrix s.
*
*     subprograms called
*
*	C math library ... fabs,sqrt
*
*     argonne national laboratory. minpack project. march 1980.
*     burton s. garbow, kenneth e. hillstrom, jorge j. more
*
*     **********
*/
int i,ij,ik,kk,j,jp1,k,kp1,l,nsing;
double cos,cotan,qtbpj,sin,sum,tan,temp;
double *wa;
static double zero = 0.0;
static double p25 = 0.25;
static double p5 = 0.5;

/* Allocate space. */
wa = (double *) malloc(n*sizeof(double));

/*
*     copy r and (q transpose)*b to preserve input and initialize s.
*     in particular, save the diagonal elements of r in x.
*/
kk = 0;
for( j=0; j<n; j++ )
	{
	ij = kk;
	ik = kk;
	for( i=j; i<n; i++ )
		{
		r[ij] = r[ik];
		ij += 1;   /* [i+ldr*j] */
		ik += ldr; /* [j+ldr*i] */
		}
	x[j] = r[kk];
	wa[j] = qtb[j];
	kk += ldr+1; /* j+ldr*j */
	}
/*
*     eliminate the diagonal matrix d using a givens rotation.
*/
for( j=0; j<n; j++ )
	{
/*
*	 prepare the row of d to be eliminated, locating the
*	 diagonal element using p from the qr factorization.
*/
	l = ipvt[j];
	if(diag[l] == zero)
		goto L90;
	for( k=j; k<n; k++ )
		sdiag[k] = zero;
	sdiag[j] = diag[l];
/*
*	 the transformations to eliminate the row of d
*	 modify only a single element of (q transpose)*b
*	 beyond the first n, which is initially zero.
*/
	qtbpj = zero;
	for( k=j; k<n; k++ )
		{
/*
*	    determine a givens rotation which eliminates the
*	    appropriate element in the current row of d.
*/
		if(sdiag[k] == zero)
			continue;
		kk = k + ldr * k;
		if(fabs(r[kk]) < fabs(sdiag[k]))
			{
			cotan = r[kk]/sdiag[k];
			sin = p5/sqrt(p25+p25*cotan*cotan);
			cos = sin*cotan;
			}
		else
			{
			tan = sdiag[k]/r[kk];
			cos = p5/sqrt(p25+p25*tan*tan);
			sin = cos*tan;
			}
/*
*	    compute the modified diagonal element of r and
*	    the modified element of ((q transpose)*b,0).
*/
		r[kk] = cos*r[kk] + sin*sdiag[k];
		temp = cos*wa[k] + sin*qtbpj;
		qtbpj = -sin*wa[k] + cos*qtbpj;
		wa[k] = temp;
/*
*	    accumulate the tranformation in the row of s.
*/
		kp1 = k + 1;
		if( n > kp1 )
			{
			ik = kk + 1;
			for( i=kp1; i<n; i++ )
				{
				temp = cos*r[ik] + sin*sdiag[i];
				sdiag[i] = -sin*r[ik] + cos*sdiag[i];
				r[ik] = temp;
				ik += 1; /* [i+ldr*k] */
				}
			}
		}
L90:
/*
*	 store the diagonal element of s and restore
*	 the corresponding diagonal element of r.
*/
	kk = j + ldr*j;
	sdiag[j] = r[kk];
	r[kk] = x[j];
	}
/*
*     solve the triangular system for z. if the system is
*     singular, then obtain a least squares solution.
*/
nsing = n;
for( j=0; j<n; j++ )
	{
	if( (sdiag[j] == zero) && (nsing == n) )
		nsing = j;
	if(nsing < n)
		wa[j] = zero;
	}
if(nsing < 1)
	goto L150;

for( k=0; k<nsing; k++ )
	{
	j = nsing - k - 1;
	sum = zero;
	jp1 = j + 1;
	if(nsing > jp1)
		{
		ij = jp1 + ldr * j;
		for( i=jp1; i<nsing; i++ )
			{
			sum += r[ij]*wa[i];
			ij += 1; /* [i+ldr*j] */
			}
		}
	wa[j] = (wa[j] - sum)/sdiag[j];
	}
L150:
/*
*     permute the components of z back to components of x.
*/
for( j=0; j<n; j++ )
	{
	l = ipvt[j];
	x[l] = wa[j];
	}

free(wa);
/*
*     last card of subroutine qrsolv.
*/
}
