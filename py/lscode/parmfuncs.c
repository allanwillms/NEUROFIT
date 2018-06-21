/*
	This file contains functions that rely on the specific ordering
	of the parameters in the extracted array of varying parameters.
	The order of the parameters is:
	  - the set parameters as ordered in lsdata.h
	  - the f parameters (if any)
	  - the time constant parameters for each voltage level:
	      - Tm then Th1, Th2, ..., Th[nh]

    initoutput - initialize the output fields in the data structure
	detCONS - determine the relevant constraint inequalities
		for the given varying parameters and test the original 
		parameter values
	detDIAG - determine the diagonal scaling matrix for lmder
	countVP - count the number of varying parameters
	extractVP - extract the varying parameters to an array
	insertVP - insert the varying parameters into the structure
	insertSE - insert the standard errors into the structure
*/

#include <stdlib.h>
#include <stdio.h>
#include "lsdata.h"
#include "parmfuncs.h"
#include "messages.h"

#define MAX(a,b)  ((a) >= (b) ? (a) : (b))
#define MIN(a,b)  ((a) <= (b) ? (a) : (b))

/* ------------------------------------------------------------------- */

void initoutput(struct leastsquaresdata *lsdata)
{
/*
	This function takes the least squares data structure as passed by 
	the user interface module and initializes all the output fields
	except the values of the parameters.

	lsdata - input/output, pointer to least squares data structure
*/

int i,j;

lsdata->fitresults.nfev = 0;
lsdata->fitresults.njev = 0;
lsdata->fitresults.npts = 0;
lsdata->fitresults.nfitp = 0;
lsdata->fitresults.variance = 0.0;
lsdata->fitresults.nmsg = 0;
for (i=0; i<lsdata->dataspecs.nswp; i++)
	{
	lsdata->dataspecs.sweeps[i].noise = 0.0;
	lsdata->dataspecs.sweeps[i].varcont = 0.0;
	}
for (i=0; i<lsdata->parameters.nsetparam; i++)
	{
	lsdata->parameters.param[i].stderror = 0.0;
	lsdata->parameters.param[i].constraint = none;
	}
for (i=0; i<lsdata->modspecs.nh + lsdata->modspecs.nnonh - 1; i++)
	{
	lsdata->parameters.f[i].stderror = 0.0;
	lsdata->parameters.f[i].constraint = none;
	}
for (i=0; i<lsdata->parameters.nVslev; i++)
	{
	lsdata->parameters.Tc[i].Tm.stderror = 0.0;
	lsdata->parameters.Tc[i].Tm.constraint = none;
	for (j=0; j<lsdata->modspecs.nh; j++)
		{
		lsdata->parameters.Tc[i].Th[j].stderror = 0.0;
		lsdata->parameters.Tc[i].Th[j].constraint = none;
		}
	}
}

/*----------------------------------------------------------------------*/
int detCONS(int nvp, struct leastsquaresdata *lsdata, int *nc, 
	double ***cA, double **cb, int *ncv, struct consvars_str **consvars)
{
/*
   Determine the constraint inequalities for the given set of varying
	parameters and test the original parameter values to see that the
   constraints are met initially.

	The constraints are given as
			cA * x <= cb
	where x is the varying parameters, cA is an nc x n matrix, nc is the
	number of constraint inequalities, n is the number of varying 
	parameters, and cb is a vector of lenght nc.

	For each of the following constraints, if any of the varying parameters
	are involved in it, then that constraint will be extracted.  The
	original parameter values will be tested on all of the following
	constraints to ensure that they are met initially.

   Model type = 1:
		gmax >= 0
		sm < 0
		sh > 0   (only if nh>0)
		f[i] >= 0,     i=0,...,nh+nnonh-2
		f[i] <= 1,     i=0,...,nh+nnonh-2
		Sum f[i] <= 1
		Tm >= 0   (all sweeps)
		Th >= 0 or Th of previous channel type  (all sweeps)
   Model type = 2:
		gmax >= 0
		A0m > 0
		a1m >= 0
		b1m <= 0
		a2m <= 0
		b2m <= 0
		A0h > 0   (only if nh>0)
		a1h <= 0        "
		b1h >= 0        "
		a2h <= 0        "
		b2h <= 0        "
		e0h2 + A0h > 0   (only if nh>1)  
		e1h2 + a1h <= 0        "
		e1h2 + b1h >= 0        "
		e2h2 + a2h <= 0        "
		e2h2 + b2h <= 0        " 
		e0h3 + A0h > 0   (only if nh>2)  
		e1h3 + a1h <= 0        "
		e1h3 + b1h >= 0        "
		e2h3 + a2h <= 0        "
		e2h3 + b2h <= 0        "
		f[i] >= 0,     i=0,...,nh+nnonh-2
		f[i] <= 1,     i=0,...,nh+nnonh-2
		Sum f[i] <= 1

	Parameters:

	nvp - input, number of varying parameters
   lsdata - input, leastsquares data structure
   nc - output, number of constraint inequalities
	cA - output, coefficient matrix for constraint inequalities
	cb - output, right hand side vector for constraint inequalities
	ncv - output, number of elements in consvars structure
	consvars - output, structure with ncv elements with fields for the
		constraint number, the constraint type, and a pointer to the
		relevant parameter constraint field

	Return value:
	0 if parameter values meet all hard constraints, positive otherwise 
	  giving relevant error message number
*/
int i,j,k,nf,totcons,totconsvars,nh,nnonh,mtype;
int *ord,**loc;
double Tau_small,V_small,temp,A0_small;

extern char *messages[];
struct parameters *parms;

mtype = lsdata->modspecs.mtype;
nh = lsdata->modspecs.nh;
nnonh = lsdata->modspecs.nnonh;
parms = &(lsdata->parameters);

/* Determine the SMALL values for voltage and time parameters based on
	the input data.  These values are used as limits for the respective
	parameters which cannot be zero. */
temp = lsdata->dataspecs.sweeps[0].Volt.prestep;
V_small = lsdata->dataspecs.sweeps[0].Volt.step;
Tau_small = 0.0;
A0_small = 0.0;
for (i=0; i<lsdata->dataspecs.nswp; i++)
	{
	temp = MIN(temp,lsdata->dataspecs.sweeps[i].Volt.prestep);
	V_small = MAX(V_small,lsdata->dataspecs.sweeps[i].Volt.step);
	k = lsdata->dataspecs.sweeps[i].ntime;
	A0_small = MAX(A0_small, lsdata->dataspecs.sweeps[i].time[k-1]);
	if (k > 0)
		{
		if (Tau_small == 0.0)
			Tau_small = MIN(lsdata->dataspecs.sweeps[i].time[1] -
				lsdata->dataspecs.sweeps[i].time[0],
			   lsdata->dataspecs.sweeps[i].time[k-1] -
				lsdata->dataspecs.sweeps[i].time[k-2]);
		else
			Tau_small = MIN(Tau_small,lsdata->dataspecs.sweeps[i].time[1] -
				lsdata->dataspecs.sweeps[i].time[0]);
			Tau_small = MIN(Tau_small,lsdata->dataspecs.sweeps[i].time[k-1] -
				lsdata->dataspecs.sweeps[i].time[k-2]);
		}
	}
V_small = (V_small - temp)/500.0;
Tau_small = Tau_small/2.0;
A0_small = 0.5/A0_small;

/* Determine the number of f's. */
nf = nh + nnonh - 1;
/* Count all but the sum f[i] constraint. */
if (mtype == 1)
	{
	totcons = 2 + MIN(nh,1) + 2 * nf + parms->nVslev * (nh+1);
	totconsvars = 2 + MIN(nh,1) + 2 * nf + parms->nVslev * (1 + MAX(0,2*nh-1));
	}
else if (mtype == 2)
	{
	totcons = 6 + 5 * nh + 2 * nf;
	totconsvars = 6 + 5 * nh + 5 * MAX(0,nh-1) + 2 * nf;
	}
/* If there is more than one f, add the sum f[i] constraint too. */
if (nf > 1) 
	{
	totcons++;
	totconsvars += nf;
	}

/* Allocate space for all these possible constraints. */
*cA = (double **) malloc(totcons * sizeof(double *));
for (k=0; k<totcons; k++)
	(*cA)[k] = (double *) calloc(nvp,sizeof(double));
*cb = (double *) calloc(totcons,sizeof(double));
*consvars = (struct consvars_str *) malloc(totconsvars * 
	sizeof(struct consvars_str));

/* Set up the total constraint system. */
i = 0;
*nc = 0;
*ncv = 0;

/* Erev */
if (parms->param[i_Erev].state == variable) i++;

/* gmax */
if (parms->param[i_gmax].value < 0.0) return GMAXINVALID;
if (parms->param[i_gmax].state == variable) 
	{
	(*cA)[*nc][i++] = -1.0;
	(*consvars)[*ncv].nconseq = *nc;
	(*consvars)[*ncv].constraint = lowerbnd;
	(*consvars)[*ncv].vars = &(parms->param[i_gmax].constraint);
	(*nc)++;
	(*ncv)++;
	}

if (mtype == 1)
	{
	/* V2m */
	if (parms->param[i_V2m].state == variable) i++;
	
	/* sm */
	if (parms->param[i_sm].value >= 0.0) return SMINVALID;
	if (parms->param[i_sm].value > -V_small)
		{
		parms->param[i_sm].value = -V_small;
		if (parms->param[i_sm].state == constant)
			lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[SMMOD];
		}
	if (parms->param[i_sm].state == variable)
		{
		(*cA)[*nc][i++] = 1.0;
		(*cb)[*nc] = -V_small;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = upperbnd;
		(*consvars)[*ncv].vars = &(parms->param[i_sm].constraint);
		(*nc)++;
		(*ncv)++;
		}
	
	/* V2h */
	if (nh>0)
		{
		if (parms->param[i_V2h].state == variable) i++;
		}
	
	/* sh */
	if (nh>0)
		{
		if (parms->param[i_sh].value <= 0.0 ) return SHINVALID;
		if (parms->param[i_sh].value < V_small) 
			{
			parms->param[i_sh].value = V_small;
			if (parms->param[i_sh].state == constant)
				lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = 
					messages[SHMOD];
			}
		if (parms->param[i_sh].state == variable)
			{
			(*cA)[*nc][i++] = -1.0;
			(*cb)[*nc] = -V_small;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_sh].constraint);
			(*nc)++;
			(*ncv)++;
			}
		}
	}
else if (mtype == 2)
	{
	/* VHm */
	if (parms->param[i_VHm].state == variable) i++;
	
	/* A0m */
	if (parms->param[i_A0m].value <= 0.0) return A0MINVALID;
	if (parms->param[i_A0m].state == variable)
		{
		(*cA)[*nc][i++] = -1.0;
		(*cb)[*nc] = -A0_small;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = lowerbnd;
		(*consvars)[*ncv].vars = &(parms->param[i_A0m].constraint);
		(*nc)++;
		(*ncv)++;
		}

	/* a1m */
	if (parms->param[i_a1m].value < 0.0) return A1MINVALID;
	if (parms->param[i_a1m].state == variable)
		{
		(*cA)[*nc][i++] = -1.0;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = lowerbnd;
		(*consvars)[*ncv].vars = &(parms->param[i_a1m].constraint);
		(*nc)++;
		(*ncv)++;
		}

	/* b1m */
	if (parms->param[i_b1m].value > 0.0) return B1MINVALID;
	if (parms->param[i_b1m].state == variable)
		{
		(*cA)[*nc][i++] = 1.0;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = upperbnd;
		(*consvars)[*ncv].vars = &(parms->param[i_b1m].constraint);
		(*nc)++;
		(*ncv)++;
		}

	/* a2m */
	if (parms->param[i_a2m].value > 0.0) return A2MINVALID;
	if (parms->param[i_a2m].state == variable)
		{
		(*cA)[*nc][i++] = 1.0;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = upperbnd;
		(*consvars)[*ncv].vars = &(parms->param[i_a2m].constraint);
		(*nc)++;
		(*ncv)++;
		}

	/* b2m */
	if (parms->param[i_b2m].value > 0.0) return B2MINVALID;
	if (parms->param[i_b2m].state == variable)
		{
		(*cA)[*nc][i++] = 1.0;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = upperbnd;
		(*consvars)[*ncv].vars = &(parms->param[i_b2m].constraint);
		(*nc)++;
		(*ncv)++;
		}
	if (nh > 0)
		{
		ord = (int *) malloc(5*sizeof(int));
		/* VHh */
		if (parms->param[i_VHh].state == variable) i++;
		
		/* A0h */
		if (parms->param[i_A0h].value <= 0.0) return A0HINVALID;
		if (parms->param[i_A0h].state == variable)
			{
			ord[0] = i;
			(*cA)[*nc][i++] = -1.0;
			(*cb)[*nc] = -A0_small;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_A0h].constraint);
			(*nc)++;
			(*ncv)++;
			}
	
		/* a1h */
		if (parms->param[i_a1h].value > 0.0) return A1HINVALID;
		if (parms->param[i_a1h].state == variable)
			{
			ord[1] = i;
			(*cA)[*nc][i++] = 1.0;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_a1h].constraint);
			(*nc)++;
			(*ncv)++;
			}
	
		/* b1h */
		if (parms->param[i_b1h].value < 0.0) return B1HINVALID;
		if (parms->param[i_b1h].state == variable)
			{
			ord[2] = i;
			(*cA)[*nc][i++] = -1.0;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_b1h].constraint);
			(*nc)++;
			(*ncv)++;
			}
	
		/* a2h */
		if (parms->param[i_a2h].value > 0.0) return A2HINVALID;
		if (parms->param[i_a2h].state == variable)
			{
			ord[3] = i;
			(*cA)[*nc][i++] = 1.0;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_a2h].constraint);
			(*nc)++;
			(*ncv)++;
			}
	
		/* b2h */
		if (parms->param[i_b2h].value > 0.0) return B2HINVALID;
		if (parms->param[i_b2h].state == variable)
			{
			ord[4] = i;
			(*cA)[*nc][i++] = 1.0;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_b2h].constraint);
			(*nc)++;
			(*ncv)++;
			}
		}
	if (nh > 1)
		{
		/* e0h2 */
		if (parms->param[i_e0h2].value + parms->param[i_A0h].value <= 0.0) 
				return E0H2INVALID;
		if (parms->param[i_e0h2].state == variable)
			{
			(*cA)[*nc][i++] = -1.0;
			if (parms->param[i_A0h].state == variable)
				(*cA)[*nc][ord[0]] = -1.0;
			else
				(*cb)[*nc] = parms->param[i_A0h].value;
			(*cb)[*nc] -= A0_small;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e0h2].constraint);
			(*nc)++;
			(*ncv)++;
			}
	
		/* e1h2 */
		if (parms->param[i_e1h2].value + parms->param[i_a1h].value > 0.0 ||
			 parms->param[i_e1h2].value + parms->param[i_b1h].value < 0.0)
				return E1H2INVALID;
		if (parms->param[i_e1h2].state == variable)
			{
			(*cA)[*nc][i] = 1.0;
			if (parms->param[i_a1h].state == variable)
				(*cA)[*nc][ord[1]] = 1.0;
			else
				(*cb)[*nc] = -parms->param[i_a1h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e1h2].constraint);
			(*nc)++;
			(*ncv)++;
			(*cA)[*nc][i++] = -1.0;
			if (parms->param[i_b1h].state == variable)
				(*cA)[*nc][ord[2]] = -1.0;
			else
				(*cb)[*nc] = parms->param[i_b1h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e1h2].constraint);
			(*nc)++;
			(*ncv)++;
			}

		/* e2h2 */
		if (parms->param[i_e2h2].value + parms->param[i_a2h].value > 0.0 ||
			 parms->param[i_e2h2].value + parms->param[i_b2h].value > 0.0)
				return E2H2INVALID;
		if (parms->param[i_e2h2].state == variable)
			{
			(*cA)[*nc][i] = 1.0;
			if (parms->param[i_a2h].state == variable)
				(*cA)[*nc][ord[3]] = 1.0;
			else
				(*cb)[*nc] = -parms->param[i_a2h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e2h2].constraint);
			(*nc)++;
			(*ncv)++;
			(*cA)[*nc][i++] = 1.0;
			if (parms->param[i_b2h].state == variable)
				(*cA)[*nc][ord[4]] = 1.0;
			else
				(*cb)[*nc] = -parms->param[i_b2h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e2h2].constraint);
			(*nc)++;
			(*ncv)++;
			}
		}
	if (nh > 2)
		{
		/* e0h3 */
		if (parms->param[i_e0h3].value + parms->param[i_A0h].value <= 0.0) 
				return E0H2INVALID;
		if (parms->param[i_e0h3].state == variable)
			{
			(*cA)[*nc][i++] = -1.0;
			if (parms->param[i_A0h].state == variable)
				(*cA)[*nc][ord[0]] = -1.0;
			else
				(*cb)[*nc] = parms->param[i_A0h].value;
			(*cb)[*nc] -= A0_small;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e0h3].constraint);
			(*nc)++;
			(*ncv)++;
			}
	
		/* e1h3 */
		if (parms->param[i_e1h3].value + parms->param[i_a1h].value > 0.0 ||
			 parms->param[i_e1h3].value + parms->param[i_b1h].value < 0.0)
				return E1H2INVALID;
		if (parms->param[i_e1h3].state == variable)
			{
			(*cA)[*nc][i] = 1.0;
			if (parms->param[i_a1h].state == variable)
				(*cA)[*nc][ord[1]] = 1.0;
			else
				(*cb)[*nc] = -parms->param[i_a1h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e1h3].constraint);
			(*nc)++;
			(*ncv)++;
			(*cA)[*nc][i++] = -1.0;
			if (parms->param[i_b1h].state == variable)
				(*cA)[*nc][ord[2]] = -1.0;
			else
				(*cb)[*nc] = parms->param[i_b1h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e1h3].constraint);
			(*nc)++;
			(*ncv)++;
			}

		/* e2h3 */
		if (parms->param[i_e2h3].value + parms->param[i_a2h].value > 0.0 ||
			 parms->param[i_e2h3].value + parms->param[i_b2h].value > 0.0)
				return E2H2INVALID;
		if (parms->param[i_e2h3].state == variable)
			{
			(*cA)[*nc][i] = 1.0;
			if (parms->param[i_a2h].state == variable)
				(*cA)[*nc][ord[3]] = 1.0;
			else
				(*cb)[*nc] = -parms->param[i_a2h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e2h3].constraint);
			(*nc)++;
			(*ncv)++;
			(*cA)[*nc][i++] = 1.0;
			if (parms->param[i_b2h].state == variable)
				(*cA)[*nc][ord[4]] = 1.0;
			else
				(*cb)[*nc] = -parms->param[i_b2h].value;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = upperbnd;
			(*consvars)[*ncv].vars = &(parms->param[i_e2h3].constraint);
			(*nc)++;
			(*ncv)++;
			}
		}
	if (nh > 0) free(ord);
	}

/* f[i] */
k = i;
for (j=0; j<nf; j++)
	{
	if (parms->f[j].value < 0.0 || parms->f[j].value > 1.0) return FINVALID;
	if (parms->f[j].state == variable)
		{
		(*cA)[*nc][i] = -1.0;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = lowerbnd;
		(*consvars)[*ncv].vars = &(parms->f[j].constraint);
		(*nc)++;
		(*ncv)++;
		(*cA)[*nc][i++] = 1.0;
		(*cb)[*nc] = 1.0;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = upperbnd;
		(*consvars)[*ncv].vars = &(parms->f[j].constraint);
		(*nc)++;
		(*ncv)++;
		}
	}
if (nf > 1)
	{
	(*cb)[*nc] = 1.0;
	for (j=0; j<nh+nnonh-1; j++)
		{
		if (parms->f[j].state == variable) 
			{
			(*cA)[*nc][k++] = 1.0;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = sumfbnd;
			(*consvars)[*ncv].vars = &(parms->f[j].constraint);
			(*ncv)++;
			}
		else (*cb)[*nc] -= parms->f[j].value;
		}
	if ((*cb)[*nc] <= 0.0) return FCSUMINVALID;
	(*nc)++;
	}

/* Time constants: Tm and Th */
/* Allocate some space for temporary arrays. */
ord = (int *) malloc(nh*sizeof(int));
loc = (int **) malloc((nh+1)*sizeof(int *));
for (k=0; k<nh+1; k++) loc[k] = (int *) malloc((parms->nVslev)*sizeof(int));

/* Determine the locations of all of the Tm and Th parameters, that is, the
   columns of cA which correspond to them.  If a parm is not variable,
   then set its location to -1. */
for (j=0; j<parms->nVslev; j++)
	{
	if (parms->Tc[j].Tm.state == variable) loc[0][j] = i++;
	else loc[0][j] = -1;
	for (k=1; k<nh+1; k++)
		{
		if (parms->Tc[j].Th[k-1].state == variable) loc[k][j] = i++;
		else loc[k][j] = -1;
		}
	}
if (nh > 0 && parms->nVslev >0)
	{	
	/* For each voltage step level, the ordering of the Th's must be
		the same.  First determine the order from the first voltage 
		step level. Channel type ord[j] is the jth in the ordering. */
	ord[0] = 0;
	for (k=1; k<nh; k++)
		{
		j = k-1;
		while (j >= 0 &&
			parms->Tc[0].Th[k].value < parms->Tc[0].Th[ord[j]].value) 
			{
			ord[j+1] = ord[j];
			j--;
			}
		ord[j+1] = k;
		}
	}
for (j=0; j<parms->nVslev; j++)
	{
	/* Tm */
	if (parms->Tc[j].Tm.value < 0.0) return TMINVALID;
	if (parms->Tc[j].Tm.value < Tau_small)
		{
		parms->Tc[j].Tm.value = Tau_small;
		if (parms->Tc[j].Tm.state == constant)
			lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = 
				messages[TAUMMOD];
		}
	if (parms->Tc[j].Tm.state == variable)
		{
		(*cA)[*nc][loc[0][j]] = -1.0;
		(*cb)[*nc] = -Tau_small;
		(*consvars)[*ncv].nconseq = *nc;
		(*consvars)[*ncv].constraint = lowerbnd;
		(*consvars)[*ncv].vars = &(parms->Tc[j].Tm.constraint);
		(*nc)++;
		(*ncv)++;
		}

    if (nh>0)
		{
		/* Th */
		if (parms->Tc[j].Th[ord[0]].value < 0.0) return THINVALID;
		if (parms->Tc[j].Th[ord[0]].value < Tau_small)
			{
			parms->Tc[j].Th[ord[0]].value = Tau_small;
			if (parms->Tc[j].Th[ord[0]].state == constant)
				lsdata->fitresults.msg[lsdata->fitresults.nmsg++] =
					messages[TAUHMOD];
			}
		if (parms->Tc[j].Th[ord[0]].state == variable)
			{
			(*cA)[*nc][loc[ord[0]+1][j]] = -1.0;
			(*cb)[*nc] = -Tau_small;
			(*consvars)[*ncv].nconseq = *nc;
			(*consvars)[*ncv].constraint = lowerbnd;
			(*consvars)[*ncv].vars = &(parms->Tc[j].Th[ord[0]].constraint);
			(*nc)++;
			(*ncv)++;
			}
		for (k=1; k<nh; k++)
			{
			if (parms->Tc[j].Th[ord[k]].value < parms->Tc[j].Th[ord[k-1]].value)
				return THORDINVALID;
			if (parms->Tc[j].Th[ord[k]].state == variable)
				{
				(*cA)[*nc][loc[ord[k]+1][j]] = -1.0;
				(*consvars)[*ncv].nconseq = *nc;
				(*consvars)[*ncv].constraint = tauhbnd;
				(*consvars)[*ncv].vars = &(parms->Tc[j].Th[ord[k]].constraint);
				(*ncv)++;
				if (loc[ord[k-1]+1][j] >= 0)
					{
					(*cA)[*nc][loc[ord[k-1]+1][j]] = 1.0;
					(*consvars)[*ncv].nconseq = *nc;
					(*consvars)[*ncv].constraint = tauhbnd;
					(*consvars)[*ncv].vars = &(parms->Tc[j].Th[ord[k-1]].constraint);
					(*ncv)++;
					}
				else
					{
					(*cb)[*nc] = -parms->Tc[j].Th[ord[k-1]].value;
					}
				(*nc)++;
				}
			}
		}
	}
	
/* Free temporary space. */
free(ord);
for (k=0; k<nh+1; k++) free(loc[k]);
free(loc);

/* Free unused space. */
for (j=*nc; j<totcons; j++) free((*cA)[j]);
(*cA) = (double **) realloc((*cA),*nc * sizeof(double *));
(*cb) = (double *) realloc((*cb),*nc * sizeof(double));
(*consvars) = (struct consvars_str *) realloc((*consvars),*ncv * 
	sizeof(struct consvars_str));

return 0;
}

/*----------------------------------------------------------------------*/

void detDIAG(int mtype, int nh, int nnonh, struct parameters *parms, double *diag) {
/*
	Determine the scaling for the varying parameters.  The scaling is
	used by lmder to define the trust region in which it takes steps.
	The larger diag[i] is, the smaller the step in the ith variable will
	be.
*/

int i,j,k;
double temp;

if (mtype == 1)
	{
	temp = -4.0/parms->param[i_sm].value;
	if (nh > 0) temp += 4.0/parms->param[i_sh].value;
	}
else if (mtype ==2)
	{
	temp = 4.0*(parms->param[i_a1m].value - parms->param[i_b1m].value);
	if (nh > 0) temp += 4.0*(parms->param[i_b1h].value - parms->param[i_a1h].value);
	}
i = 0;
if (parms->param[i_Erev].state == variable) diag[i++]=temp;
if (parms->param[i_gmax].state == variable) diag[i++]=2.0/parms->param[i_gmax].value;
if (mtype == 1)
	{
	if (parms->param[i_V2m].state == variable) diag[i++]=-2.0/parms->param[i_sm].value;
	if (parms->param[i_sm].state == variable)  diag[i++]=-4.0/parms->param[i_sm].value;
	if (nh > 0)
		{
		if (parms->param[i_V2h].state == variable) diag[i++]=2.0/parms->param[i_sh].value;
		if (parms->param[i_sh].state == variable)  diag[i++]=4.0/parms->param[i_sh].value;
		}
	}
else if (mtype == 2)
	{
	if (parms->param[i_VHm].state  == variable)
		diag[i++] = 2.0*(parms->param[i_a1m].value - parms->param[i_b1m].value);
	if (parms->param[i_A0m].state  == variable)
		diag[i++] = 5.0/parms->param[i_A0m].value;
	if (parms->param[i_a1m].state  == variable)
		diag[i++] = 4.0/parms->param[i_a1m].value;
	if (parms->param[i_b1m].state  == variable)
		diag[i++] = -4.0/parms->param[i_b1m].value;
	if (parms->param[i_a2m].state  == variable)
		diag[i++] = 16.0/parms->param[i_a1m].value;
	if (parms->param[i_b2m].state  == variable)
		diag[i++] = -16.0/parms->param[i_b1m].value;
	if (nh > 0)
		{
		if (parms->param[i_VHh].state  == variable)
			diag[i++] = 2.0*(parms->param[i_b1h].value - 
									parms->param[i_a1h].value);
		if (parms->param[i_A0h].state  == variable)
			diag[i++] = 5.0/parms->param[i_A0h].value;
		if (parms->param[i_a1h].state  == variable)
			diag[i++] = -4.0/parms->param[i_a1h].value;
		if (parms->param[i_b1h].state  == variable)
			diag[i++] = 4.0/parms->param[i_b1h].value;
		if (parms->param[i_a2h].state  == variable)
			diag[i++] = -16.0/parms->param[i_a1h].value;
		if (parms->param[i_b2h].state  == variable)
			diag[i++] = 16.0/parms->param[i_b1h].value;
		}
	if (nh > 1)
		{
		if (parms->param[i_e0h2].state  == variable)
			diag[i++] = 5.0/parms->param[i_A0h].value;
		if (parms->param[i_e1h2].state  == variable)
			diag[i++] = 4.0*(parms->param[i_b1h].value - 
									parms->param[i_a1h].value);
		if (parms->param[i_e2h2].state  == variable)
			diag[i++] = 16.0*(parms->param[i_b1h].value - 
									parms->param[i_a1h].value);
		}
	if (nh > 2)
		{
		if (parms->param[i_e0h3].state  == variable)
			diag[i++] = 5.0/parms->param[i_A0h].value;
		if (parms->param[i_e1h3].state  == variable)
			diag[i++] = 4.0*(parms->param[i_b1h].value - 
									parms->param[i_a1h].value);
		if (parms->param[i_e2h3].state  == variable)
			diag[i++] = 16.0*(parms->param[i_b1h].value - 
									parms->param[i_a1h].value);
		}
	}
for (j=0; j<nh+nnonh-1; j++)
	if(parms->f[j].state == variable) diag[i++]=0.2;
if (mtype == 1)
	{
	for (j=0; j<parms->nVslev; j++)
		{
		if (parms->Tc[j].Tm.state == variable)  diag[i++]=5.0/parms->Tc[j].Tm.value;
		for (k=0; k<nh; k++)
			if (parms->Tc[j].Th[k].state == variable)  
								diag[i++]=5.0/parms->Tc[j].Th[k].value;
		}
	}
}

/*----------------------------------------------------------------------*/

int countVP(int nh, int nnonh, struct parameters *parms) {
/* 
   Count the number of varying parameters in the full parameter 
   storage structure.

   nh - input, number of inactivating channel types
   nnonh - input, number of noninactivating channel types
   parms - input, full parameter structure
   return value - number of varying parameters
*/

int i,j,k;

i = 0;
for (j=0; j<parms->nsetparam; j++)
    if (parms->param[j].state == variable)  i++;
for (j=0; j<nh+nnonh-1; j++)
	if(parms->f[j].state == variable) i++;
for (j=0; j<parms->nVslev; j++)
	{
	if (parms->Tc[j].Tm.state == variable)  i++;
    for (k=0; k<nh; k++)
		if (parms->Tc[j].Th[k].state == variable)  i++;
	}
return i;
}

/*----------------------------------------------------------------------*/

void extractVP(int nh, int nnonh, struct parameters *parms, double *x) {
/* 
   Extract the varying parameters from the full parameter storage 
   structure into an array as used by LMDER.

   nh - input, number of inactivating channel types
   nnonh - input, number of noninactivating channel types
   parms - input, full parameter structure
   x - output, extracted array
*/

int i,j,k;

i = 0;
for (j=0; j<parms->nsetparam; j++)
    if (parms->param[j].state == variable)  x[i++]=parms->param[j].value;
for (j=0; j<nh+nnonh-1; j++)
	if(parms->f[j].state == variable) x[i++]=parms->f[j].value;
for (j=0; j<parms->nVslev; j++)
	{
	if (parms->Tc[j].Tm.state == variable)  x[i++]=parms->Tc[j].Tm.value;
    for (k=0; k<nh; k++)
		if (parms->Tc[j].Th[k].state == variable)  
								x[i++]=parms->Tc[j].Th[k].value;
	}
}

/*----------------------------------------------------------------------*/

void insertVP(int nh, int nnonh, struct parameters *parms, double *x) {
/* 
   Insert the varying parameters from the array used by LMDER into the 
   full parameter storage structure.

   nh - input, number of inactivating channel types
   nnonh - input, number of noninactivating channel types
   parms - output, full parameter structure
   x - input, compacted array
*/

int i,j,k;

i = 0;
for (j=0; j<parms->nsetparam; j++)
    if (parms->param[j].state == variable)  parms->param[j].value=x[i++];
for (j=0; j<nh+nnonh-1; j++)
	if(parms->f[j].state == variable) parms->f[j].value=x[i++];
for (j=0; j<parms->nVslev; j++)
	{
	if (parms->Tc[j].Tm.state == variable)  parms->Tc[j].Tm.value=x[i++];
    for (k=0; k<nh; k++)
		if (parms->Tc[j].Th[k].state == variable)  
								parms->Tc[j].Th[k].value=x[i++];
	}
}

/*----------------------------------------------------------------------*/

void insertSE(int nh, int nnonh, struct parameters *parms, double *x) {
/* 
   Insert the standard errors for the varying parameters from the given
   array into the full parameter storage structure.
   Also, set the standard error to zero for constant parameters.

   nh - input, number of inactivating channel types
   nnonh - input, number of noninactivating channel types
   parms - output, full parameter structure
   x - input, array of standard errors
*/

int i,j,k;

i = 0;
for (j=0; j<parms->nsetparam; j++)
    if (parms->param[j].state == variable)  parms->param[j].stderror=x[i++];
	else                                    parms->param[j].stderror=0.0;
for (j=0; j<nh+nnonh-1; j++)
	if(parms->f[j].state == variable) parms->f[j].stderror=x[i++];
	else                              parms->f[j].stderror=0.0;
for (j=0; j<parms->nVslev; j++)
	{
	if (parms->Tc[j].Tm.state == variable)  parms->Tc[j].Tm.stderror=x[i++];
	else                                    parms->Tc[j].Tm.stderror=0.0;
    for (k=0; k<nh; k++)
		if (parms->Tc[j].Th[k].state == variable)  
								parms->Tc[j].Th[k].stderror=x[i++];
		else					parms->Tc[j].Th[k].stderror=0.0;
	}
}
