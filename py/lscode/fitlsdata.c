/*
	 This file contains:
		fitlsdata - Starting with a data set, model specifications, and 
			 initial parameter guesses, this function performs the numerical 
			 optimization to find the nonlinear least squares fit to the data.
			 This function calls LMDER to do the Levenberg-Marquardt 
			 algorithm.
		getinitval - function to estimate initial values of parameters
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lsdata.h"
#include "lm.h"
#include "hhftvc.h"
#include "parmfuncs.h"
#include "auxfuncs.h"
#include "dpmach.h"
#include "messages.h"

#define MAX(a,b)  ((a) >= (b) ? (a) : (b))
#define MIN(a,b)  ((a) <= (b) ? (a) : (b))

/* #define JACTEST */
/*
#define LEAKSUB 
#define GLEAK  0.170613
#define ELEAK  -47.1657 */
/* #define DEBUG */

/* ------------------------------------------------------------------- */

void fitlsdata(struct leastsquaresdata *lsdata)
{
/*
	This function takes the least squares data structure as passed by 
	the user interface module and computes the least squares fit as
	instructed.  It calls LMDER to do the fitting.

	lsdata - input/output, pointer to least squares data structure
*/

int npts, ntotal;
double *fvec, *fjac, *x, temp;
int *ipvt ,i,j,k,noiseadj;
int np, info, nc, ncv, *appc;
double *qtf, *wa, **cA, *cb, *diag;
double fnorm;

extern char *messages[];
struct consvars_str *consvars;

#ifdef JACTEST
double *fjac_fd;
FILE *fptr;
#endif

#ifdef DEBUG
printf("fitlsdata: initializing output fields.\n");
#endif
/* Initialize the output fields. */
initoutput(lsdata);

/* Allocate space for the messages array. */
lsdata->fitresults.msg = (const char **) malloc(MAXMSG * sizeof(char *));

#ifdef LEAKSUB
for (i=0; i<lsdata->dataspecs.nswp; i++)
	{
	for (j=0; j<lsdata->dataspecs.sweeps[i].ntime; j++)
		lsdata->dataspecs.sweeps[i].data[j] -= GLEAK*(
			lsdata->dataspecs.sweeps[i].Volt.step - ELEAK);
	}
#endif
/* In nh is input as zero then make sure the Vh2 and sh parameters
	are marked as constants. */
if (lsdata->modspecs.nh == 0 && lsdata->modspecs.mtype == 1)
	{
	lsdata->parameters.param[i_V2h].state = constant;
	lsdata->parameters.param[i_sh].state = constant;
	}

/* Count the number of varying parameters, allocate space for
	an array of them and extract them to the array. */
np = countVP(lsdata->modspecs.nh, lsdata->modspecs.nnonh,
		        &(lsdata->parameters));
lsdata->fitresults.nfitp = np;
x = (double *) malloc(np * sizeof(double));
extractVP(lsdata->modspecs.nh, lsdata->modspecs.nnonh, 
		  &(lsdata->parameters), x);

#ifdef DEBUG
printf("fitlsdata: determining constraint inequalities.\n");
#endif
/* Set up the constraint inequalities for these varying parameters,
   and check the validity of the passed in parameter values. */
if ((i = detCONS(np,lsdata,&nc,&cA,&cb,&ncv,&consvars)) != 0)
	{
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[i];
	return;
	}

/* Count the total number of fitted points, npts, and total number of
   points for which to construct the fitted curves, ntotal.. */
npts = 0;
ntotal = 0;
for (i=0; i < lsdata->dataspecs.nswp; i++) {
	npts += lsdata->dataspecs.sweeps[i].fitWin.nsample;
	ntotal += lsdata->dataspecs.sweeps[i].ntime;
}
lsdata->fitresults.npts = npts;
#ifdef DEBUG
	printf("fitlsdata: calculating noise levels\n");
#endif

/* Calculate the noise level for each sweep. */
k = 0;
for (i=0; i<lsdata->dataspecs.nswp; i++)
	{
	j = lsdata->dataspecs.sweeps[i].noiseWin.str;
	k += noisecalc(lsdata->algspecs.noiseDeg,
		       lsdata->dataspecs.sweeps[i].noiseWin.nsample,
		       &(lsdata->dataspecs.sweeps[i].time[j]),
		       &(lsdata->dataspecs.sweeps[i].data[j]),
		       &(lsdata->dataspecs.sweeps[i].noise));
	}
if (k == 0)  /* Noise levels for all sweeps calculated successfully. */
	noiseadj = 1;
else /* Either all the noise windows were empty or warn user. */
	{
	noiseadj = 0;
	if (k < lsdata->dataspecs.nswp) /* Some sweeps had valid windows. */
		{
		lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[NOISELEV];
		/* Re-set all noise levels to 1.0. */
		for (i=0; i<lsdata->dataspecs.nswp; i++)
			lsdata->dataspecs.sweeps[i].noise = 1.0;
		}
	}

/* Allocate some space */
appc = (int *) malloc(nc*sizeof(int));
fvec = (double *) malloc(ntotal * sizeof(double));
fjac = (double *) malloc(npts*np * sizeof(double));
qtf = (double *) malloc(np * sizeof(double));
wa = (double *) malloc(np * sizeof(double));
ipvt = (int *) malloc(np * sizeof(int));
diag = (double *) malloc(np*sizeof(double));

#ifdef DEBUG
printf("fitlsdata: determining scaling of paramters.\n");
#endif
/* Determine the scaling of the parameters to pass to lmder. */
detDIAG(lsdata->modspecs.mtype, lsdata->modspecs.nh, lsdata->modspecs.nnonh, 
		  &(lsdata->parameters), diag);

info=0;

#ifdef DEBUG
printf("fitlsdata: about to call lmder.\n");
#endif

#ifdef JACTEST
printf("Doing JACTEST...\n  calculating and printing function...");
fflush(stdout);
fjac_fd = (double *) malloc(npts*np * sizeof(double));
hhftvc(npts, np, x, fvec, fjac, 1, lsdata);
fnorm = enorm(npts,fvec);
fptr = fopen("func","w");
for (j=0; j<npts; j++) fprintf(fptr,"%13e\n",fvec[j]);
fclose(fptr);
printf("done\n  calculating and printing analytic jacobian...");
fflush(stdout);
hhftvc(npts, np, x, fvec, fjac, 2, lsdata);
fptr = fopen("jac_anal","w");
for (j=0; j<npts; j++)
	{
	for (i=0; i<np; i++) fprintf(fptr,"%13e ",fjac[i*npts+j]);
	fprintf(fptr,"\n");
	}
fclose(fptr);
printf("done\n  calculating and printing finite difference jacobian...");
fflush(stdout);
for (i=0; i<np; i++)
	{
	temp = x[i];
	x[i] *= 1.001;
	hhftvc(npts, np, x, &fjac_fd[i*npts], NULL, 1, lsdata);
	for (j=0; j<npts; j++)
		fjac_fd[i*npts+j] = (fjac_fd[i*npts+j] - fvec[j])/(x[i]-temp);
	x[i] = temp;
	}
fptr = fopen("jac_fd","w");
for (j=0; j<npts; j++)
	{
	for (i=0; i<np; i++) fprintf(fptr,"%13e ",fjac_fd[i*npts+j]);
	fprintf(fptr,"\n");
	}
fclose(fptr);
free(fjac_fd);
printf("done\nCompleted JACTEST\n");
fflush(stdout);
info = 10000;
#else

/* call lmder to do the fitting */
if (lsdata->modspecs.mtype == 2)
	{
	diag = NULL;
	temp = 100.0;
	}
else
	{
	temp = -1.0;
	}
info = lmder(&hhftvc, lsdata, npts, np, x, diag, temp, 0,
      lsdata->algspecs.ftol, lsdata->algspecs.xtol, lsdata->algspecs.gtol,
		lsdata->algspecs.maxfeval, nc, cA, cb, appc, fvec, &fnorm, fjac, 
		ipvt, qtf, &(lsdata->fitresults.nfev), &(lsdata->fitresults.njev));
#endif

#ifdef DEBUG
printf("fitlsdata: successful return from lmder.\n");
#endif

if (info == 0)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMINVALID];
else if (info == 1)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMFTOL];
else if (info == 2)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMXTOL];
else if (info == 3)
	{
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMFTOL];
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMXTOL];
	}
else if (info == 4)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMGTOL];
else if (info == 5)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMNFEV];
else if (info == 6)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMFTOL2SMALL];
else if (info == 7)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMXTOL2SMALL];
else if (info == 8)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[LMGTOL2SMALL];
else if (info == 9)
	lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[INVALIDCONS];

if (info > 0) {
	/* Insert the final solution x into the parameters structure. */
	insertVP(lsdata->modspecs.nh, lsdata->modspecs.nnonh,
	         &(lsdata->parameters), x);

	/* calculate the variance and the percentage contribution
		of each sweep to the variance */
	k = 0;
	for (i=0; i<lsdata->dataspecs.nswp; i++) {
		if ((j = lsdata->dataspecs.sweeps[i].fitWin.nsample) > 0) {
		   wa[0] = enorm(j,&fvec[k])/fnorm;
		   lsdata->dataspecs.sweeps[i].varcont = 100.0*wa[0]*wa[0];
		   k += j;
		}
	}
	lsdata->fitresults.variance = fnorm*fnorm/(npts-np);

#ifndef JACTEST
	/* calculate the standard errors */
	if (secalc(npts,np,fjac,ipvt,wa) != np)
		lsdata->fitresults.msg[lsdata->fitresults.nmsg++] = messages[JACSING];
	/* If the noise levels were not used for adjustment, then adjust the
		standard errors by the standard deviation of the fit. */
	temp = sqrt(lsdata->fitresults.variance);
	if (!noiseadj) {
		for (i=0; i<np; i++) wa[i] *= temp;
	}
	/* Insert the standard errors into the data structure. */
	insertSE(lsdata->modspecs.nh, lsdata->modspecs.nnonh, 
		     &(lsdata->parameters), wa);

	/* Set the output constraint states for the fitted parameters. */
	for (i=0; i<ncv; i++) {
		if (appc[consvars[i].nconseq] && *(consvars[i].vars) == none)
			*(consvars[i].vars) = consvars[i].constraint;
	}
#endif

	/* Compute the fitted curves over the entire time set. */
	for (i=0; i<lsdata->dataspecs.nswp; i++) {
		lsdata->dataspecs.sweeps[i].fitWin.str = 0;
		lsdata->dataspecs.sweeps[i].fitWin.nsample = lsdata->dataspecs.sweeps[i].ntime;
	}
    hhftvc(ntotal, np, x, fvec, NULL, 11, lsdata);
	k = 0;
    for (i=0; i<lsdata->dataspecs.nswp; i++) {
		for (j=0; j<lsdata->dataspecs.sweeps[i].ntime; j++) {
			lsdata->dataspecs.sweeps[i].fitvals[j] = fvec[j+k];
		}
		k += lsdata->dataspecs.sweeps[i].ntime;
	}
}

#ifdef LEAKSUB
for (i=0; i<lsdata->dataspecs.nswp; i++)
	{
	for (j=0; j<lsdata->dataspecs.sweeps[i].ntime; j++)
		lsdata->dataspecs.sweeps[i].fitvals[j] += GLEAK*(
			lsdata->dataspecs.sweeps[i].Volt.step - ELEAK);
	}
#endif
free(appc);
free(x);
free(fvec);
free(fjac);
free(qtf);
free(wa);
free(ipvt);
free(diag);
for (i=0; i<nc; i++) free(cA[i]);
free(cA);
free(cb);
free(consvars);
}

/* ------------------------------------------------------------------- */

void getinitval(struct leastsquaresdata *lsdata)
{
/*
	This function takes the least squares data structure as passed 
	by the user interface module and computes estimates of the model
	parameter values to be used as initial values for the fitting
	routine.
	It assumes that Erev has been specified.

	lsdata - input/output, pointer to least squares data structure
*/

int i,j,k,ii,jj,ij,m,n,si,last;
int nf,nh,nswp;
int npts,np;
int count[3],*prottype,*swind,*vlsort;
int *tinfl,*fact;
double temp,temp2;
double *Ipeak,*tpeak,Imax,*Ires,**I0,*gfrac;
double *A,*b,*Thcoef,*h0,*hinf;
double coef[2],Erev;
struct parameters *parms;
struct dataspecs *data;

/* Initialize output fields. */
initoutput(lsdata);

/* Define some short forms. */
nswp = lsdata->dataspecs.nswp;
nh = lsdata->modspecs.nh;
nf = nh + lsdata->modspecs.nnonh - 1;
parms = &lsdata->parameters;
data = &lsdata->dataspecs;
Erev = parms->param[i_Erev].value;

/* Count the number of varying parameters. */
np = countVP(nh, lsdata->modspecs.nnonh, parms);
lsdata->fitresults.nfitp = np;

/* Count the total number of fitted points, npts, and total number of
   points for which to construct the fitted curves, ntotal.. */
npts = 0;
for (i=0; i < nswp; i++)
	{
	npts += data->sweeps[i].fitWin.nsample;
	}
lsdata->fitresults.npts = npts;

/* Allocate some space. */
Ipeak = (double *) calloc(nswp,sizeof(double)); /* must be calloc */
tpeak = (double *) malloc(nswp*sizeof(double));
tinfl = (int *) malloc(nswp*sizeof(int));
Ires = (double *) calloc(nswp,sizeof(double)); /* must be calloc */
I0 = (double **) malloc((nh+1)*sizeof(double *));
for (i=0; i<nh+1; i++)    /* must be calloc */
	I0[i] = (double *) calloc(parms->nVslev,sizeof(double));
prottype = (int *) malloc(nswp*sizeof(int));
gfrac = (double *) malloc((nf+1)*sizeof(double));
h0 = (double *) malloc(nswp*sizeof(double));
hinf = (double *) malloc(nswp*sizeof(double));
swind = (int *) malloc(parms->nVslev*sizeof(int));
vlsort = (int *) malloc(parms->nVslev*sizeof(int));
fact = (int *) malloc(nswp*sizeof(int));

/* Classify sweeps into types: 0 - empty fit window, 1 - varying prestep
	experiment, 2 - varying step experiment */
getprottype(Erev, data, prottype, count);

/* Set the factor depending on whether this is a postive or
	negative current. */
for (i=0; i<nswp; i++)
	{
	if (data->sweeps[i].Volt.step > Erev) fact[i] = 1;
	else fact[i] = -1;
	}

/* For each voltage level, choose a sweep to use.  Choose a prottype
	2 if possible. */
for (i=0; i<parms->nVslev; i++)
	{
	j = 0;
	while (j<nswp && (parms->Tc[i].Volt != 
		    data->sweeps[j].Volt.step || prottype[j] != 2)) j++;
	if (j==nswp)
		{
		j = 0;
		while ( j<nswp && (parms->Tc[i].Volt != 
		    data->sweeps[j].Volt.step ||
			 prottype[j] == 0)) j++;
		}
	if (j==nswp)
		swind[i] = -1;
	else
		swind[i] = j;
	}

/* If nh > 0 estimate the peaks. This info possibly needed for noise 
	calc, so do it now. If nh = 0 then use tpeak to record the last 
    time point of the rising exponential. */
for (i=0; i<nswp; i++)
	{
	if (prottype[i] != 0)
		{
		if (nh > 0)
			{
			/* Find the peaks of each sweep and the inflection point 
				following the peak.  Peaks are always positive. */
			getpeak(fact[i],&(data->sweeps[i]),Ipeak+i,tpeak+i,
				     tinfl+i);
			}
		else
			{
			tpeak[i] = data->sweeps[i].fitWin.str + 
				data->sweeps[i].fitWin.nsample - 1;
			}
		}
	}

/* Find the residual current for each trace if nnonh==1.  If nh=0 then
   copy abs(Ires) to Ipeak, so that voltage step sorting works. */
if (lsdata->modspecs.nnonh == 1)
	{
	for (i=0; i<nswp; i++)
		{
		if (prottype[i] != 0)
			{
			Ires[i] = getIres(fact[i],nh,tinfl[i],
					 &(data->sweeps[i]));
			if (nh == 0) Ipeak[i] = fact[i] * Ires[i];
			}
		}
	}

/* For the voltage step levels associated with sweeps (swind != -1), sort
   them from largest magnitude peak to smallest. If swind[i] == -1 then
   place that level anywhere in the sorted order. */
vlsort[0] = 0;
for (i=1; i<parms->nVslev; i++)
	{
	if (swind[i] == -1) vlsort[i] = i;
	else
		{
	    j = 0;
	    while (j<i && (swind[vlsort[j]] == -1 || 
			Ipeak[swind[i]] < Ipeak[swind[vlsort[j]]])) j++;
	    for (k=i-1; k>=j; k--) vlsort[k+1] = vlsort[k];
        vlsort[j] = i;
		}
	}

/* Find the maximum current for the prestep experiments. */
if (nh > 0)
	{
	Imax = 0.0;
	for (i=0; i<nswp; i++)
		{
		if (prottype[i] == 1)
			Imax = MAX(Imax,Ipeak[i]-fact[i]*Ires[i]);
		}
	}

/* Calculate the noise level for each sweep; if there weren't enough
	points then estimate the noise as one one hundredth of the peak. */
for (i=0; i<nswp; i++)
	{
	if (prottype[i] != 0)
		{
		j = data->sweeps[i].noiseWin.str;
		if (noisecalc(lsdata->algspecs.noiseDeg,
		       data->sweeps[i].noiseWin.nsample,
		       &(data->sweeps[i].time[j]),
		       &(data->sweeps[i].data[j]),
		       &(data->sweeps[i].noise)))
			{
			if (nh > 0)
				data->sweeps[i].noise = 0.01*Ipeak[i];
			else
				data->sweeps[i].noise = 0.01*Ires[i];
			}
		}
	}

/* If nh>0 then estimate all of the inactivation related parameters. */
if (nh > 0)
	{
	/* Determine the steady-state inactivation parameters. */
	if (count[1] > 0) 
		{
		A = (double *) malloc(2*count[1]*sizeof(double));
		b = (double *) malloc(count[1]*sizeof(double));
		j=0;
		for (i=0; i<nswp; i++)
			{
			if (prottype[i] == 1)
				{
				A[j] = 1.0;
				A[count[1]+j] = data->sweeps[i].Volt.prestep;
				b[j] = log(1.01*Imax/(Ipeak[i]-fact[i]*Ires[i]) - 1.0);
				j++;
				}
			}
		linfit(count[1],2,A,b,coef);
		free(A);
		free(b);
		}
	else   /* Since there are no varying prestep data, we need to do something
		      mildly intelligent with the varying step data to estimate V2h and sh. 
			  We'll assume that at the prestep level, h is 0.999, and at the lowest
			  step level it is at 0.001.  The equations are then directly solvable
			  for coef[0] and coef[1]. */
		{
		j = 0;
		temp = 0.0;
		for (i=0; i<nswp; i++)
			{
			if (prottype[i] == 2)
				{
				if (j == 0) temp2 = data->sweeps[i].Volt.prestep;
				if (j == 0 || temp > data->sweeps[i].Volt.step)
					temp = data->sweeps[i].Volt.step;
				j = 1;
				}
			}
		coef[0] = 6.9067548*(temp2 + temp) / (temp2 - temp);
		coef[1] = -2*6.9067548 / (temp2 - temp);
		}
	if (parms->param[i_V2h].state == variable)
		parms->param[i_V2h].value = -coef[0]/coef[1];
	if (parms->param[i_sh].state == variable)
		parms->param[i_sh].value = 1.0/coef[1];
	for (i=0; i<nswp; i++)
		{
		h0[i] = boltz(data->sweeps[i].Volt.prestep,
				      parms->param[i_V2h].value, parms->param[i_sh].value);
		hinf[i] = boltz(data->sweeps[i].Volt.step,
					    parms->param[i_V2h].value, parms->param[i_sh].value);
		}

	/* Fit the inactivation time constants. */
	Thcoef = (double *) malloc(nh*2*sizeof(double));
	n = 0;
	temp = 0.0;
	for (i=0; i<parms->nVslev; i++)
		{
		si = vlsort[i];   /* Process the voltage step levels in sorted order. */
		/* Skip fit if no sweep with non empty fitting window was found. */
		if (swind[si]==-1) continue;  /* Go to end of for loop */

		/* Count all the points from the inflection point to the end */
		ii = data->sweeps[swind[si]].fitWin.str + 
			  data->sweeps[swind[si]].fitWin.nsample -
			  tinfl[swind[si]];
		A = (double *) malloc(2*ii*sizeof(double));
		b = (double *) malloc(ii*sizeof(double));
		ii = ii/nh;
		for (k=nh-1; k>=0; k--)
			{
			m = tinfl[swind[si]] + k*ii;
			j = 0;
			jj = 0;
			while (jj<ii && (j<5 || jj<20))
				{
				A[jj] = 1.0;
				A[ii+jj]= data->sweeps[swind[si]].time[m+jj];
				b[jj] = (data->sweeps[swind[si]].data[m+jj] - 
					     Ires[swind[si]])*fact[swind[si]];
				for (ij=nh-1; ij>k; ij--) 
					b[jj] -= exp(Thcoef[2*ij] + Thcoef[2*ij+1]*A[ii+jj]);
				if (b[jj] < 3.0*(data->sweeps[swind[si]].noise)) j++;
				b[jj] = log(MAX(b[jj],
						3.0 * (data->sweeps[swind[si]].noise)));
				jj++;
				}
			if (jj<ii)
				for (ij=0; ij<jj; ij++) A[jj+ij] = A[ii+ij];
			linfit(jj,2,A,b,Thcoef+2*k);
			ij = data->sweeps[swind[si]].fitWin.str + 
				  data->sweeps[swind[si]].fitWin.nsample - 1;
			/* Make sure Th will be positive and smaller than any previous Th's
			   at this voltage level, and no more than 10 times the last time value. */
			if ((Thcoef[2*k+1] > 0.0) || 
				(k<nh-1 && Thcoef[2*k+1] > Thcoef[2*(k+1)+1]) ||
				(Thcoef[2*k+1] > -0.1/data->sweeps[swind[si]].time[ij]))
				{
				/* If there is a previous Th, make this one 0.95 times it. */
				if (k<nh-1) Thcoef[2*k+1] = Thcoef[2*(k+1)+1]/0.95;
				else
					{
					if (n>0)  /* Use the Th from previous voltage step. */
						Thcoef[2*k+1] = -1.0/parms->Tc[last].Th[k].value;
					else   /* No previous step; use ten times ending time. */
						Thcoef[2*k+1] = -0.1/data->sweeps[swind[si]].time[ij];
					}
				}
			I0[k][si] = exp(Thcoef[2*k]);
			I0[nh][si] += I0[k][si];
			if (parms->Tc[si].Th[k].state == variable)
				parms->Tc[si].Th[k].value = -1.0/Thcoef[2*k+1];
			}
		if (lsdata->modspecs.nnonh == 1)
			temp += (h0[swind[si]] - hinf[swind[si]])*fact[swind[si]]*
					  Ires[swind[si]]/I0[nh][si] - hinf[swind[si]];
		free(A);
		free(b);
		n++;
		last = si;
		}
	free(Thcoef);

	/* Fit the channel fractions. */
	if (lsdata->modspecs.nnonh == 1)
		{
		temp /= ((double) n);
		temp = temp/(1.0 + temp);
		}
	for (k=0; k<nh; k++)
		{
		gfrac[k] = 0.0;
		for (i=0; i<parms->nVslev; i++)
			if (swind[i] != -1)	gfrac[k] += I0[k][i]/I0[nh][i];
		gfrac[k] *= (1.0 - temp)/((double) n);
		}
	if (lsdata->modspecs.nnonh == 1) gfrac[nf] = temp;
	temp = 0.0;
	temp2 = 0.0;
	j = 0;
	for (i=0; i<nf; i++)
		{
		if (parms->f[i].state == variable)
			{
			parms->f[i].value = gfrac[i];
			temp += gfrac[i];
			j++;
			}
		else
			{
			gfrac[i] = parms->f[i].value;
			temp2 += gfrac[i];
			}
		}
	if (temp + temp2 > 1.0)
		{
		temp = (1.0 - temp2)/temp;
		for (i=0; i<nf; i++)
			{
			if (parms->f[i].state == variable)
				{
				gfrac[i] *= temp;
				parms->f[i].value = gfrac[i];
				}
			}
		gfrac[nf] = 0.0;
		}
	else
		gfrac[nf] = 1.0 - temp - temp2;
	}
else  /* nh==0, nnonh==1, and nf==0 */
	{
	gfrac[0] = 1.0;
	}

/* Estimate the activation time constants. */
if (nh == 0)  /* No inactivaton, just fit a rising exponential. */
	{
	last = 0;
	for (i=0; i<parms->nVslev; i++)
		{
		si = vlsort[i];   /* Process the voltage step levels in sorted order. */
		if (swind[si] != -1)
			{
			m = data->sweeps[swind[si]].fitWin.str;
			k = data->sweeps[swind[si]].fitWin.nsample;
			A = (double *) malloc(2*k*sizeof(double));
			b = (double *) malloc(k*sizeof(double));
			for (j=0; j<k; j++)
				{
				A[j] = 1.0;
				A[k+j] = data->sweeps[swind[si]].time[m+j];
				b[j] = fact[swind[si]]*(Ires[swind[si]] -
						 data->sweeps[swind[si]].data[m+j]);
				if (j>20 && b[j] < 3.0*data->sweeps[swind[si]].noise) 
					break;
				b[j] = log(MAX(b[j],
					3.0*data->sweeps[swind[si]].noise));
				}
			if (j<k)
            for (jj=0; jj<j; jj++) A[j+jj] = A[k+jj];
			linfit(j,2,A,b,coef);
			if (parms->Tc[si].Tm.state == variable)
				{
				/* Make sure Tm will be positive and no more than 10 times the 
				   last time value. */
				if (coef[1] >= 0.0)
					{
					if (last > 0) parms->Tc[si].Tm.value = parms->Tc[last].Tm.value;
					else parms->Tc[si].Tm.value = 0.1 * tpeak[si]; /* tenth of total time */
					}
				else
					{
					if (last > 0) coef[1] = MIN(coef[1], -0.1/parms->Tc[last].Tm.value);
					parms->Tc[si].Tm.value = -1.0/coef[1];
					}
				}
			free(A);
			free(b);
			last = si;
			}
		}
	}
else  /* Use the peak current and Tau_h's to estimate Tau_m. */
	{
	for (i=0; i<parms->nVslev; i++)
		{
		if (swind[i] != -1)
			{
			coef[0] = 1.0/(h0[swind[i]] - hinf[swind[i]]);
			if (lsdata->modspecs.nnonh == 1)
				coef[0] *= ((1.0-gfrac[nf])*hinf[swind[i]] + gfrac[nf]);
			else
				coef[0] *= hinf[swind[i]];
			coef[1] = 0.0;
			for (k=0; k<nh; k++)
				{
				temp = gfrac[k]*exp(-tpeak[swind[i]] /
						    parms->Tc[i].Th[k].value);
				coef[0] += temp;
				coef[1] += temp/parms->Tc[i].Th[k].value;
				}
			coef[0] *= ((double) lsdata->modspecs.pp) /
				        (coef[1]*tpeak[swind[i]]);
			coef[1] = expsolve(coef[0],1.0e-2);
			if (coef[1] <= 0.0)
				coef[1] = 0.5*tpeak[swind[i]];
			else
				coef[1] = tpeak[swind[i]]/coef[1];
			if (parms->Tc[i].Tm.state == variable)
				parms->Tc[i].Tm.value = coef[1];
			}
		}
	}

/* Find the maximal conductance and the steady-state activation
	parameters. */
Imax = 0.0;    /* Use this now as max of g*m^p */
j = 0;
ii = parms->nVslev;
A = (double *) malloc(2*ii*sizeof(double));
b = (double *) malloc(ii*sizeof(double));
for (i=0, n=0; i<ii; i++)
	{
	if (swind[i]!=-1)
		{
		if (lsdata->modspecs.nnonh == 1) temp = gfrac[nf];
		else temp = 0.0;
		for (k=0; k<nh; k++)
			temp += gfrac[k]*(hinf[swind[i]] + (h0[swind[i]] - hinf[swind[i]]) *
					exp(-tpeak[swind[i]]/parms->Tc[i].Th[k].value));
		temp *= pow(1.0 - exp(-tpeak[swind[i]]/parms->Tc[i].Tm.value),
			        (double) lsdata->modspecs.pp) * 
			    (data->sweeps[swind[i]].Volt.step - Erev);
		if (fabs(temp) < (1.0e-9)*fabs(Ipeak[swind[i]]))  /* Effectively zero. */
			continue;
		else if (nh > 0 && Ipeak[swind[i]] != 0.0)
			temp /= (fact[swind[i]]*Ipeak[swind[i]]);
		else if (nh == 0 && Ires[swind[i]] != 0.0)
			temp /= Ires[swind[i]];
		else
			continue;
		A[n] = 1.0;
		A[ii+n] = data->sweeps[swind[i]].Volt.step;
		b[n] = temp;
		n++;
		temp = 1.0/temp;
		if (temp > Imax)
			{
			Imax = temp;
			j = swind[i];  /* Use this below in estimation of gmax. */
			}
		}
	}

for (i=0; i<n; i++)
	{
	A[n+i] = A[ii+i];  /* shift things over */
	b[i] = log(pow(1.01*Imax*b[i],1.0/((double) lsdata->modspecs.pp)) - 1.0);
	}
linfit(n,2,A,b,coef);
if (coef[1] > 0.0)  /* One twentieth of a reasonable range. */
    coef[1] = 20.0/(data->sweeps[0].Volt.step -
	          data->sweeps[0].Volt.prestep);
free(A);
free(b);

if (parms->param[i_V2m].state == variable)
	parms->param[i_V2m].value = -coef[0]/coef[1];

if (parms->param[i_sm].state == variable)
	parms->param[i_sm].value = 1.0/coef[1];

if (parms->param[i_gmax].state == variable)
	parms->param[i_gmax].value = Imax/
	    pow(boltz(data->sweeps[j].Volt.step,
		       parms->param[i_V2m].value, 
			   parms->param[i_sm].value), 
		    (double) lsdata->modspecs.pp);

/* Compute the variance of the fit in the fitting region and the percent
   contribution of each sweep to the variance. */
b = (double *) calloc(np, sizeof(double));
extractVP(nh, lsdata->modspecs.nnonh, &lsdata->parameters, b);

A = (double *) calloc(npts, sizeof(double));
hhftvc(npts, np, b, A, NULL, 1, lsdata);
temp = enorm(npts, A);

k = 0;
for (i=0; i<nswp; i++) {
	if ((j = data->sweeps[i].fitWin.nsample) > 0) {
		 temp2 = enorm(j,&A[k])/temp;
		 data->sweeps[i].varcont = 100.0*temp2*temp2;
		 k += j;
	}
}
lsdata->fitresults.variance = temp*temp/(npts-np);
free(A);

/* Compute the fitted curves over the entire time set. */
n = 0;
for (i=0; i<nswp; i++) {
	data->sweeps[i].fitWin.str = 0;
	data->sweeps[i].fitWin.nsample = data->sweeps[i].ntime;
	n += data->sweeps[i].ntime;
}
A = (double *) calloc(n, sizeof(double));
hhftvc(n, np, b, A, NULL, 11, lsdata);
k = 0;
for (i=0; i<nswp; i++) {
	for (j=0; j<data->sweeps[i].ntime; j++) {
		data->sweeps[i].fitvals[j] = A[j+k];
	}
	k += data->sweeps[i].ntime;
}
free(A);
free(b);

/* Free space. */
free(Ipeak);
free(tpeak);
free(tinfl);
free(Ires);
for (i=0; i<nh+1; i++) free(I0[i]);
free(I0);
free(prottype);
free(gfrac);
free(h0);
free(hinf);
free(swind);
free(vlsort);
free(fact);
}
