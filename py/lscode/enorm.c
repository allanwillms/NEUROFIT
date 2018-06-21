#include <math.h>
#include "enorm.h"

double enorm(int n, double x[]) {
/*
*     **********
*
*     function enorm
*
*     given an n-vector x, this function calculates the
*     euclidean norm of x.
*
*     the euclidean norm is computed by accumulating the sum of
*     squares in three different sums. the sums of squares for the
*     small and large components are scaled so that no overflows
*     occur. non-destructive underflows are permitted. underflows
*     and overflows do not occur in the computation of the unscaled
*     sum of squares for the intermediate components.
*     the definitions of small, intermediate and large components
*     depend on two constants, rdwarf and rgiant. the main
*     restrictions on these constants are that rdwarf**2 not
*     underflow and rgiant**2 not overflow. the constants
*     given here are suitable for every known computer.
*
*  Parameters:
*
*	n is a positive integer input variable.
*
*	x is an input array of length n.
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
int i;
double agiant,floatn,s1,s2,s3,xabs,x1max,x3max;
double ans, temp;
static double rdwarf = 3.834e-20;
static double rgiant = 1.304e19;
static double zero = 0.0;
static double one = 1.0;

s1 = zero;
s2 = zero;
s3 = zero;
x1max = zero;
x3max = zero;
floatn = n;
agiant = rgiant/floatn;

for( i=0; i<n; i++ ) 
	{
	xabs = fabs(x[i]);
   if( (xabs > rdwarf) && (xabs < agiant) ) 
		{
/*
*	    sum for intermediate components.
*/
		s2 += xabs*xabs;
		continue;
		}

if(xabs > rdwarf) 
	{
/*
*	       sum for large components.
*/
	if(xabs > x1max) 
		{
		temp = x1max/xabs;
		s1 = one + s1*temp*temp;
		x1max = xabs;
		}
	else 
		{
		temp = xabs/x1max;
		s1 += temp*temp;
		}
	continue;
	}
/*
*	       sum for small components.
*/
	if(xabs > x3max)
		{
		temp = x3max/xabs;
		s3 = one + s3*temp*temp;
		x3max = xabs;
		}
	else	
		{
		if(xabs != zero)
			{
			temp = xabs/x3max;
			s3 += temp*temp;
			}
		}
	}
/*
*     calculation of norm.
*/
if(s1 != zero)
	{
	temp = s1 + (s2/x1max)/x1max;
	ans = x1max*sqrt(temp);
	return(ans);
	}
if(s2 != zero)
	{
	if(s2 >= x3max)
		temp = s2*(one+(x3max/s2)*(x3max*s3));
	else
		temp = x3max*((s2/x3max)+(x3max*s3));
	ans = sqrt(temp);
	}
else
	{
	ans = x3max*sqrt(s3);
	}
return(ans);
/*
*     last card of function enorm.
*/
}
