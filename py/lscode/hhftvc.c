/*
   This file contains:
     hhftvc - Hodgkin-Huxley model for the full traces of voltage clamp 
              data.
     boltz - Boltzmann function for the steady-state HH model parameters, 
             called by hhftvc
	 ss2 - steady-state function for MYTPE==2
	 time2 - time constant function for MTYPE==2
     traj - compute a single trajectory of full trace voltage clamp data

   The ORDER in which the parameters are processed in the function
   hhftvc (the jacobian calculations) is important and must match the 
   order used in the parmfuncs functions.
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "lsdata.h"
#include "parmfuncs.h"
#include "messages.h"

/*----------------------------------------------------------------------*/

double boltz(double v, double p1, double p2) {
/*
   Boltzmann sigmoid shaped function.
                1
   y = ----------------------
        1 + exp( (V-p1)/p2 )

   v - input, voltage
   p1 - input, halfway voltage
   p2 - input, slope parameter
   return value = y
*/

double temp;

temp = (v-p1)/p2;
if (temp < -38.0) return 1.0e0;
if (temp > 38.0) return exp(-temp);
return (1.0e0/( 1.0e0 + exp(temp)));
}

/*----------------------------------------------------------------------*/

double ss2(double v, double v2, double p1, double p2) {
/*
   Thermodynamic steady state function for MTYPE==2
                1
   y = -----------------------------------
        1 + exp( p1*(v-v2) + p2*(v-v2)^2 )

   v - input, voltage
   v2 - input, half potential
   p1 - input, linear factor
   p2 - input, quadratic factor
   return value = y
*/

double temp;

temp = v-v2;
return (1.0e0/( 1.0e0 + exp(p1*temp + p2*pow(temp, 2.0))));
}

/*----------------------------------------------------------------------*/

double time2(double v, double v2, double A0, double a1, double b1, 
			 double a2, double b2) {
/*
   Thermodynamic time constant function for MTYPE==2
                1
   y = --------------, where alpha = A0*exp(a1(v-v2)+a2*(v-v2)^2), and 
        alpha + beta         beta  = A0*exp(b1(v-v2)+b2*(v-v2)^2).

   v - input, voltage
   v2 - input, half potential
   A0 - input, constant factor
   a1 - input, linear factor for alpha
   b1 - input, linear factor for beta
   a2 - input, quadratic factor for alpha
   b2 - input, quadratic factor for beta
   return value = y
*/

double temp, temp2;

temp = v-v2;
temp2 = pow(temp, 2.0);
return (1.0e0/(A0*(exp(a1*temp + a2*temp2) + exp(b1*temp + b2*temp2))));
}

/*----------------------------------------------------------------------*/

void traj(int n, int nh, int nnonh, int pp, double mm[], 
		double *exptTm, double hh[], double **exptTh, double Vlev, double Erev, 
		double gmax, double gfrac[], double *f) {
/*
   Calculate a single trajectory at constant voltage.

   n - input, number of time points
   nh - input, number of inactivating channel types
   nnonh - input, number of noninactivating channel types
   pp - input, number of activation gates
   mm - input, initial and infinity values of activation
   exptTm - input, array of exp(-t/Tm)
   hh - input, initial and infinity values of inactivation
   exptTh - input, (nh) arrays of exp(-t/Th[k])
   Vlev - input, voltage level
   Erev - input, reversal potential
   gmax - input, maximal conductance
   gfrac - input, fraction of channels of each type
   f - output, trajectory
*/

int i, j;
double temp;

if (nnonh == 1)
	temp = gfrac[nh];
else
	temp = 0.0;
for (i=0; i<n; i++) f[i] = temp;
for (j=0; j<nh; j++)
	{
	for (i=0; i<n; i++)
		f[i] += gfrac[j] * (hh[1] + (hh[0] - hh[1]) * exptTh[j][i]);
	}
for (i=0; i<n; i++)
	{
	temp = mm[1] + (mm[0] - mm[1]) * exptTm[i];
	f[i] *= gmax * pow(temp, (double) pp) * (Vlev - Erev);
	}
}

/*----------------------------------------------------------------------*/

int hhftvc(int m, int n, double x[], double fvec[], double fjac[],
           int task, struct leastsquaresdata *lsdata) {
/*
 task=0:
   Print out x.
 task=1 or 11:
   Calculate the residues of the function to be minimized via least
   squares.  The function consists of a set of trajectories at different
   constant voltages as determined by the current parameter values from
   which are subtracted the observed data, and the result is scaled by 
   the NOISE.
   If task is 11 then the observed data is not subtracted off and the
   result is not scaled for the NOISE.
 task=2:
   Calculate the mxn jacobian of the residue function; stored
   columnwise.

   This function is called by LMDER and fitlsdata.

   m - input, number of residues
   n - input, number of varying parameters
   x - input, varying parameters
   fvec - if task=1, output, residues.  if task=2, input, residues.
   fjac - if task=2, output, mxn jacobian of fvec stored columnwise
   task- input, operation to perform.  
   return value - 0 if successful, negative otherwise
*/

#define MTYPE   (lsdata->modspecs.mtype)
#define NH      (lsdata->modspecs.nh)
#define NNONH   (lsdata->modspecs.nnonh)
#define PP      (lsdata->modspecs.pp)
#define NSETPARAM  (lsdata->parameters.nsetparam)
#define NVSLEV  (lsdata->parameters.nVslev)
#define EREV    (lsdata->parameters.param[i_Erev].value)
#define GMAX    (lsdata->parameters.param[i_gmax].value)
#define V2M     (lsdata->parameters.param[i_V2m].value)
#define SM      (lsdata->parameters.param[i_sm].value)
#define V2H     (lsdata->parameters.param[i_V2h].value)
#define SH      (lsdata->parameters.param[i_sh].value)
#define VHM     (lsdata->parameters.param[i_VHm].value)
#define A0M     (lsdata->parameters.param[i_A0m].value)
#define A1M     (lsdata->parameters.param[i_a1m].value)
#define B1M     (lsdata->parameters.param[i_b1m].value)
#define A2M     (lsdata->parameters.param[i_a2m].value)
#define B2M     (lsdata->parameters.param[i_b2m].value)
#define VHH     (lsdata->parameters.param[i_VHh].value)
#define A0H     (lsdata->parameters.param[i_A0h].value)
#define A1H     (lsdata->parameters.param[i_a1h].value)
#define B1H     (lsdata->parameters.param[i_b1h].value)
#define A2H     (lsdata->parameters.param[i_a2h].value)
#define B2H     (lsdata->parameters.param[i_b2h].value)
#define E0H2    (lsdata->parameters.param[i_e0h2].value)
#define E1H2    (lsdata->parameters.param[i_e1h2].value)
#define E2H2    (lsdata->parameters.param[i_e2h2].value)
#define E0H3    (lsdata->parameters.param[i_e0h3].value)
#define E1H3    (lsdata->parameters.param[i_e1h3].value)
#define E2H3    (lsdata->parameters.param[i_e2h3].value)
#define PRESTEP (lsdata->dataspecs.sweeps[i].Volt.prestep)
#define STEP    (lsdata->dataspecs.sweeps[i].Volt.step)
#define NOISE   (lsdata->dataspecs.sweeps[i].noise)
#define STR     (lsdata->dataspecs.sweeps[i].fitWin.str)
#define NSAMPLE (lsdata->dataspecs.sweeps[i].fitWin.nsample)
#define TIME    lsdata->dataspecs.sweeps[i].time

#define V_EREV  (lsdata->parameters.param[i_Erev].state == variable)
#define V_GMAX  (lsdata->parameters.param[i_gmax].state == variable)
#define V_V2M   (lsdata->parameters.param[i_V2m].state == variable)
#define V_SM    (lsdata->parameters.param[i_sm].state == variable)
#define V_V2H   (lsdata->parameters.param[i_V2h].state == variable)
#define V_SH    (lsdata->parameters.param[i_sh].state == variable)
#define V_VHM   (lsdata->parameters.param[i_VHm].state == variable)
#define V_A0M   (lsdata->parameters.param[i_A0m].state == variable)
#define V_A1M   (lsdata->parameters.param[i_a1m].state == variable)
#define V_B1M   (lsdata->parameters.param[i_b1m].state == variable)
#define V_A2M   (lsdata->parameters.param[i_a2m].state == variable)
#define V_B2M   (lsdata->parameters.param[i_b2m].state == variable)
#define V_VHH   (lsdata->parameters.param[i_VHh].state == variable)
#define V_A0H   (lsdata->parameters.param[i_A0h].state == variable)
#define V_A1H   (lsdata->parameters.param[i_a1h].state == variable)
#define V_B1H   (lsdata->parameters.param[i_b1h].state == variable)
#define V_A2H   (lsdata->parameters.param[i_a2h].state == variable)
#define V_B2H   (lsdata->parameters.param[i_b2h].state == variable)
#define V_E0H2  (lsdata->parameters.param[i_e0h2].state == variable)
#define V_E1H2  (lsdata->parameters.param[i_e1h2].state == variable)
#define V_E2H2  (lsdata->parameters.param[i_e2h2].state == variable)
#define V_E0H3  (lsdata->parameters.param[i_e0h3].state == variable)
#define V_E1H3  (lsdata->parameters.param[i_e1h3].state == variable)
#define V_E2H3  (lsdata->parameters.param[i_e2h3].state == variable)
	
int i, j, k, ii, jj, offset, jmp;
int varTm, varTh, varRatem, varRateh, varf;
double mm[2], hh[2], Tm;
double *gfrac, *Th;
double keep1, keep2, keep3, keep4;
double *keepa1, *keepa2, *keepa3, **keepa4;
double alpham, betam, (*abh)[5], *alphah, *betah;
double *exptTm, **exptTh;
double tmp1, tmp2, dmh0, dmh1;

if (task==0)
	{
	printf("x = ");
   for (i=0; i<n; i++) printf("%lg, ",x[i]);
   printf("\n");
   return 0; 
	}

insertVP(NH, NNONH, &(lsdata->parameters), x);  /* updating parameters */

/* Allocate some space. */
gfrac = (double *) malloc((NH + NNONH) * sizeof(double));
if (NH > 0)
	{
	Th = (double *) malloc(NH * sizeof(double));
	abh = (double (*)[5]) malloc(NH * 5 * sizeof(double));
	}

/* Calculate gfrac */
gfrac[NH+NNONH-1] = 1.0e0;
for (ii = 0; ii < NH+NNONH-1; ii++)
	{
	gfrac[ii] = lsdata->parameters.f[ii].value;
	gfrac[NH+NNONH-1] -= gfrac[ii];
	}

/* Set up the alpha, beta rate constants array. */
if (MTYPE == 2 && NH > 0)
	{
	abh[0][0] = A0H;
	abh[0][1] = A1H;
	abh[0][2] = B1H;
	abh[0][3] = A2H;
	abh[0][4] = B2H;
	if (NH > 1)
		{
		abh[1][0] = A0H + E0H2;
		abh[1][1] = A1H + E1H2;
		abh[1][2] = B1H + E1H2;
		abh[1][3] = A2H + E2H2;
		abh[1][4] = B2H + E2H2;
		if (NH > 2)
			{
			abh[2][0] = A0H + E0H3;
			abh[2][1] = A1H + E1H3;
			abh[2][2] = B1H + E1H3;
			abh[2][3] = A2H + E2H3;
			abh[2][4] = B2H + E2H3;
			}
		}
	}

/* If doing a Jacobian calc, determine what intermediate quantities are
	needed. */
if (task == 2)
	{
	if (MTYPE == 1)
		{
		varTm = 0;
		j = 0;
		while (j < NVSLEV &&
		    !(varTm = (lsdata->parameters.Tc[j].Tm.state == variable))) j++;
		varTh = 0;
		k = 0;
		while (k < NH && varTh == 0)
		   {
		   j = 0;
		   while (j < NVSLEV && !(varTh = (
					lsdata->parameters.Tc[j].Th[k].state == variable))) j++;
			k++;
			}
		}
	if (MTYPE == 2)
		{
		varRatem = (V_VHM || V_A0M || V_A1M || V_B1M || V_A2M || V_B2M);
		varRateh = (NH > 0 && (V_VHH || V_A0H || V_A1H || V_B1H || 
						V_A2H || V_B2H || V_E0H2 || V_E1H2 || V_E2H2 ||
						V_E0H3 || V_E1H3 || V_E2H3));
		}
	varf = 0;
	k = 0;
	while (k < NH + NNONH - 1 && 
			!(varf = (lsdata->parameters.f[k].state == variable))) k++;
	}

offset = 0;
/********************************************************************/
/* NOTE: DO NOT change the index variable name from "i", so that    */
/* the shortform define statements above will work.                 */
/********************************************************************/
for (i = 0; i < lsdata->dataspecs.nswp; i ++) { /* Loop through each sweep. */
   if (NSAMPLE > 0) 
		{
		/* Calculate m0, minf, h0, hinf, Tm, and Th[k] */
		if (MTYPE == 1)
			{
			mm[0] = boltz(PRESTEP, V2M,SM); 
			mm[1] = boltz(STEP, V2M, SM);
			if (NH > 0)
				{
				hh[0] = boltz(PRESTEP, V2H, SH);
				hh[1] = boltz(STEP, V2H, SH);
				}
			Tm = lsdata->dataspecs.sweeps[i].Volt.Tc->Tm.value;
			for (ii = 0; ii < NH; ii++)
				Th[ii] = lsdata->dataspecs.sweeps[i].Volt.Tc->Th[ii].value;
			}
		else if (MTYPE == 2)
			{
			mm[0] = ss2(PRESTEP, VHM, B1M-A1M, B2M-A2M);
			mm[1] = ss2(STEP, VHM, B1M-A1M, B2M-A2M);
			Tm = time2(STEP, VHM, A0M, A1M, B1M, A2M, B2M);
			if (NH > 0)
				{
				hh[0] = ss2(PRESTEP, VHH, B1H-A1H, B2H-A2H);
				hh[1] = ss2(STEP, VHH, B1H-A1H, B2H-A2H);
				for (k=0; k<NH; k++)
					Th[k] = time2(STEP, VHH, abh[k][0], abh[k][1], abh[k][2], 
									abh[k][3], abh[k][4]);
				}
			}
		/* Calculate exp(-t/Tm) and exp(-t/Th[k]) */
		exptTm = (double *) malloc(NSAMPLE * sizeof(double));
		exptTh = (double **) malloc(NH * sizeof(double *));
		for (k=0; k<NH; k++)
			exptTh[k] = (double *) malloc(NSAMPLE * sizeof(double));
		for (j = 0; j < NSAMPLE; j++)
			{
         tmp1 = -TIME[STR+j]/Tm;
         if (tmp1 < -701.0e0)
            exptTm[j] = 0.0;
         else
            exptTm[j] = exp(tmp1);
			}
		for (k=0; k<NH; k++)
			{
			for (j=0; j<NSAMPLE; j++)
				{
				tmp1 = -TIME[STR+j]/Th[k];
				if (tmp1 < -701.0e0)
					exptTh[k][j] = 0.0;
				else
					exptTh[k][j] = exp(tmp1);
				}
			}
    
      /**************************/
      /* CALCULATE THE RESIDUE. */
      /**************************/
      if (task%10 == 1)
			{
         traj(NSAMPLE, NH, NNONH, PP, mm, exptTm, hh, exptTh, STEP, EREV, 
				GMAX, gfrac, &fvec[offset]);
         if (task == 1)
			   {
			   for (j=0; j < NSAMPLE; j++)
				   fvec[offset+j] = (fvec[offset+j] - 
					   lsdata->dataspecs.sweeps[i].data[STR+j])/NOISE;
            }
			} 

		/***************************/
		/* CALCULATE THE JACOBIAN. */
		/***************************/
      else if (task == 2) 
			{
         jmp = 0;

         /* Compute some regularly used quantities. */
         keepa1 = (double *) malloc(NSAMPLE * sizeof(double));
			keepa2 = (double *) malloc(NSAMPLE * sizeof(double));
			keepa3 = (double *) malloc(NSAMPLE * sizeof(double));
			keepa4 = (double **) malloc(NH * sizeof(double *));
			if (MTYPE == 2)
				{
				alphah = (double *) malloc(NH * sizeof(double));
				betah = (double *) malloc(NH * sizeof(double));
				}
			for (k=0; k<NH; k++)
				keepa4[k] = (double *) malloc(NSAMPLE * sizeof(double));
			/* keepa1 = gmax*m^p*(V-Erev) / NOISE */
         tmp1 = 1.0;
         traj(NSAMPLE, 0, 1, PP, mm, exptTm, 
              hh, exptTh, STEP, EREV, GMAX / NOISE, &tmp1, keepa1);
         /* keepa2 = p*gmax*m^(p-1)*h*(V-Erev) / NOISE */
         traj(NSAMPLE, NH, NNONH, PP-1, mm, exptTm,
              hh, exptTh, STEP, EREV, GMAX * ((double) PP) / NOISE, gfrac, keepa2);
			if (MTYPE == 1 && (V_V2M || V_SM))
				{
				keep1 = mm[0] * boltz(PRESTEP, V2M, -SM) / SM;
				keep2 = mm[1] * boltz(STEP, V2M, -SM) / SM;
				}
			if (MTYPE == 1 && NH > 0 && (V_V2H || V_SH))
				{
				keep3 = hh[0] * boltz(PRESTEP, V2H, -SH) / SH;
				keep4 = hh[1] * boltz(STEP, V2H, -SH) / SH;
				}
			if ((MTYPE == 1 && varTm) || (MTYPE == 2 && varRatem))
				{
				for (j=0; j<NSAMPLE; j++)
					keepa3[j] = (mm[0] - mm[1]) * TIME[STR+j] * exptTm[j];
				}
			if ((MTYPE == 1 && varTh) || (MTYPE == 2 && varRateh))
				{
				for (k=0; k<NH; k++)
					{
					for (j=0; j<NSAMPLE; j++)
						keepa4[k][j] = gfrac[k] * (hh[0] - hh[1]) * TIME[STR+j] * exptTh[k][j];
					}
				}
			if (MTYPE == 2 && varRatem)
				{
				tmp1 = STEP - VHM;
				tmp2 = pow(tmp1, 2.0);
				alpham = A0M * exp(A1M * tmp1 + A2M * tmp2);
				betam = A0M * exp(B1M * tmp1 + B2M * tmp2);
				}
			if (MTYPE == 2 && varRateh)
				{
				tmp1 = STEP - VHH;
				tmp2 = pow(tmp1, 2.0);
				for (k=0; k<NH; k++)
					{
					alphah[k] = abh[k][0] * exp(abh[k][1] * tmp1 + abh[k][3] * tmp2);
					betah[k] = abh[k][0] * exp(abh[k][2] * tmp1 + abh[k][4] * tmp2);
					}
				}

         /* Erev */
         if (V_EREV) /* if Erev is a variable */
				{
            traj(NSAMPLE, NH, NNONH, PP, mm, exptTm,
                 hh, exptTh, 0.0, 1.0, GMAX/NOISE, gfrac, 
                 &(fjac[offset+jmp]));
            jmp += m;    /* next varying parameter */
				}

         /* gmax */
         if (V_GMAX) /* if gmax is a variable */
				{
            traj(NSAMPLE, NH, NNONH, PP, mm, exptTm,
                 hh, exptTh, STEP, EREV, 1.0/NOISE, gfrac, 
                 &(fjac[offset+jmp]));
            jmp += m;    /* next varying parameter */
				}

         /* V2m */
         if (MTYPE == 1 && V_V2M) /* if V2m is a variable */
				{
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = keepa2[j] * (keep1 * exptTm[j] + 
							(1 - exptTm[j]) * keep2);
					}
            jmp += m;      /* next varying parameter */
				}

			/* sm */
         if (MTYPE == 1 && V_SM) /* if sm is a variable */
				{
            tmp1 = (PRESTEP - V2M);
            dmh0 = keep1 * tmp1 / SM;
            tmp1 = (STEP - V2M);
            dmh1 = keep2 * tmp1 / SM;
            for (j = 0; j < NSAMPLE; j++) 
					{
               fjac[j+offset+jmp] = keepa2[j] * (dmh0 * exptTm[j] + 
							(1 - exptTm[j]) * dmh1);
					}
            jmp += m;        /* next varying parameter */
				}

		   /* V2h */
         if (MTYPE == 1 && NH > 0 && V_V2H) /* if V2h is a variable */
				{
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = 0.0;
               for (k = 0; k < NH; k++)
						{
                  fjac[j+offset+jmp] += gfrac[k] * (keep3 * exptTh[k][j] + 
								keep4 * (1 - exptTh[k][j]));
                  }
               fjac[j+offset+jmp] *= keepa1[j];
               }
            jmp += m;        /* next varying parameter */
				}

			/* sh */
         if (MTYPE == 1 && NH > 0 && V_SH) /* if sh is a variable */
				{
            tmp1 = (PRESTEP - V2H);
            dmh0 = keep3 * tmp1 / SH;
            tmp1 = (STEP - V2H);
            dmh1 = keep4 * tmp1 / SH;
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = 0.0;
               for (k = 0; k < NH; k++)
						{
                  fjac[j+offset+jmp] += gfrac[k] * (dmh0 * exptTh[k][j] + 
								dmh1 * (1 - exptTh[k][j]));
                  }
               fjac[j+offset+jmp] *= keepa1[j];
               }
            jmp += m;        /* next varying parameter */
				}

			/* VHm */
		   if (MTYPE == 2 && V_VHM) /* if VHm is a variable */
				{
				tmp1 = (B1M-A1M) + 2.0 * (B2M-A2M) * (PRESTEP - VHM);
            dmh0 = mm[0] * ss2(PRESTEP, VHM, A1M-B1M, A2M-B2M) * tmp1;
				tmp1 = A1M + 2.0 * A2M * (STEP - VHM);
				tmp2 = B1M + 2.0 * B2M * (STEP - VHM);
            dmh1 = mm[1] * ss2(STEP, VHM, A1M-B1M, A2M-B2M) * (tmp2 - tmp1);
				tmp1 = alpham * tmp1 + betam * tmp2;
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = (dmh0 * exptTm[j] + dmh1 * (1 - exptTm[j]) +
						keepa3[j] * tmp1) * keepa2[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* A0m */
		   if (MTYPE == 2 && V_A0M) /* if A0m is a variable */
				{
				tmp1 = (alpham + betam) / A0M;
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = -keepa3[j] * tmp1 * keepa2[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* a1m */
		   if (MTYPE == 2 && V_A1M) /* if a1m is a variable */
				{
            dmh0 = mm[0] * ss2(PRESTEP, VHM, A1M-B1M, A2M-B2M) * (PRESTEP - VHM);
            dmh1 = mm[1] * ss2(STEP, VHM, A1M-B1M, A2M-B2M) * (STEP - VHM);
				tmp1 = alpham * (VHM - STEP);
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = (dmh0 * exptTm[j] + dmh1 * (1 - exptTm[j]) +
						keepa3[j] * tmp1) * keepa2[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* b1m */
		   if (MTYPE == 2 && V_B1M) /* if b1m is a variable */
				{
            dmh0 = mm[0] * ss2(PRESTEP, VHM, A1M-B1M, A2M-B2M) * (VHM - PRESTEP);
            dmh1 = mm[1] * ss2(STEP, VHM, A1M-B1M, A2M-B2M) * (VHM - STEP);
				tmp1 = betam * (VHM - STEP);
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = (dmh0 * exptTm[j] + dmh1 * (1 - exptTm[j]) +
						keepa3[j] * tmp1) * keepa2[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* a2m */
		   if (MTYPE == 2 && V_A2M) /* if a2m is a variable */
				{
            dmh0 = mm[0] * ss2(PRESTEP, VHM, A1M-B1M, A2M-B2M) * pow(PRESTEP - VHM, 2.0);
				tmp1 = pow(STEP - VHM, 2.0);
            dmh1 = mm[1] * ss2(STEP, VHM, A1M-B1M, A2M-B2M) * tmp1;
				tmp1 = alpham * tmp1;
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = (dmh0 * exptTm[j] + dmh1 * (1 - exptTm[j]) -
						keepa3[j] * tmp1) * keepa2[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* b2m */
		   if (MTYPE == 2 && V_B2M) /* if b2m is a variable */
				{
            dmh0 = -mm[0] * ss2(PRESTEP, VHM, A1M-B1M, A2M-B2M) * pow(PRESTEP - VHM, 2.0);
				tmp1 = pow(STEP - VHM, 2.0);
            dmh1 = -mm[1] * ss2(STEP, VHM, A1M-B1M, A2M-B2M) * tmp1;
				tmp1 = betam * tmp1;
            for (j = 0; j < NSAMPLE; j++)
					{
               fjac[j+offset+jmp] = (dmh0 * exptTm[j] + dmh1 * (1 - exptTm[j]) -
						keepa3[j] * tmp1) * keepa2[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* VHh */
		   if (MTYPE == 2 && V_VHH) /* if VHh s a variable */
				{
				tmp1 = (B1H-A1H) + 2.0 * (B2H-A2H) * (PRESTEP - VHH);
            dmh0 = hh[0] * ss2(PRESTEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				tmp1 = (B1H-A1H) + 2.0 * (B2H-A2H) * (STEP - VHH);
            dmh1 = hh[1] * ss2(STEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] = 0.0;
				for (k=0; k<NH; k++)
					{
					tmp1 = alphah[k] * (abh[k][1] + 2.0 * abh[k][3] * (STEP - VHH)) +
							betah[k] * (abh[k][2] + 2.0 * abh[k][4] * (STEP - VHH));
					for (j = 0; j < NSAMPLE; j++)
						{
						fjac[j+offset+jmp] += gfrac[k] * (dmh0 * exptTh[k][j] + dmh1 * 
							(1 - exptTh[k][j])) + keepa4[k][j] * tmp1;
						}
					}
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] *= keepa1[j];
				jmp += m;      /* next varying parameter */
				}

			/* A0h */
		   if (MTYPE == 2 && V_A0H) /* if A0h s a variable */
				{
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] = 0.0;
				for (k=0; k<NH; k++)
					{
					tmp1 = (alphah[k] + betah[k]) / abh[k][0];
					for (j = 0; j < NSAMPLE; j++)
						{
						fjac[j+offset+jmp] -= keepa4[k][j] * tmp1;
						}
					}
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] *= keepa1[j];
				jmp += m;      /* next varying parameter */
				}

			/* a1h */
		   if (MTYPE == 2 && V_A1H) /* if a1h s a variable */
				{
            dmh0 = hh[0] * ss2(PRESTEP, VHH, A1H-B1H, A2H-B2H) * (PRESTEP - VHH);
				tmp1 = STEP - VHH;
            dmh1 = hh[1] * ss2(STEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] = 0.0;
				for (k=0; k<NH; k++)
					{
					tmp2 = alphah[k] * tmp1;
					for (j = 0; j < NSAMPLE; j++)
						{
						fjac[j+offset+jmp] += gfrac[k] * (dmh0 * exptTh[k][j] + dmh1 * 
							(1 - exptTh[k][j])) - keepa4[k][j] * tmp2;
						}
					}
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] *= keepa1[j];
				jmp += m;      /* next varying parameter */
				}

			/* b1h */
		   if (MTYPE == 2 && V_B1H) /* if b1h s a variable */
				{
            dmh0 = -hh[0] * ss2(PRESTEP, VHH, A1H-B1H, A2H-B2H) * (PRESTEP - VHH);
				tmp1 = STEP - VHH;
            dmh1 = -hh[1] * ss2(STEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] = 0.0;
				for (k=0; k<NH; k++)
					{
					tmp2 = betah[k] * tmp1;
					for (j = 0; j < NSAMPLE; j++)
						{
						fjac[j+offset+jmp] += gfrac[k] * (dmh0 * exptTh[k][j] + dmh1 * 
							(1 - exptTh[k][j])) - keepa4[k][j] * tmp2;
						}
					}
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] *= keepa1[j];
				jmp += m;      /* next varying parameter */
				}

			/* a2h */
		   if (MTYPE == 2 && V_A2H) /* if a2h s a variable */
				{
				tmp1 = pow(PRESTEP - VHH, 2.0);
            dmh0 = hh[0] * ss2(PRESTEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				tmp1 = pow(STEP - VHH, 2.0);
            dmh1 = hh[1] * ss2(STEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] = 0.0;
				for (k=0; k<NH; k++)
					{
					tmp2 = alphah[k] * tmp1;
					for (j = 0; j < NSAMPLE; j++)
						{
						fjac[j+offset+jmp] += gfrac[k] * (dmh0 * exptTh[k][j] + dmh1 * 
							(1 - exptTh[k][j])) - keepa4[k][j] * tmp2;
						}
					}
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] *= keepa1[j];
				jmp += m;      /* next varying parameter */
				}

			/* b2h */
		   if (MTYPE == 2 && V_B2H) /* if b2h s a variable */
				{
				tmp1 = pow(PRESTEP - VHH, 2.0);
            dmh0 = -hh[0] * ss2(PRESTEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				tmp1 = pow(STEP - VHH, 2.0);
            dmh1 = -hh[1] * ss2(STEP, VHH, A1H-B1H, A2H-B2H) * tmp1;
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] = 0.0;
				for (k=0; k<NH; k++)
					{
					tmp2 = betah[k] * tmp1;
					for (j = 0; j < NSAMPLE; j++)
						{
						fjac[j+offset+jmp] += gfrac[k] * (dmh0 * exptTh[k][j] + dmh1 * 
							(1 - exptTh[k][j])) - keepa4[k][j] * tmp2;
						}
					}
				for (j=0; j<NSAMPLE; j++) fjac[j+offset+jmp] *= keepa1[j];
				jmp += m;      /* next varying parameter */
				}

			/* e0h2 */
		   if (MTYPE == 2 && NH > 1 && V_E0H2) /* if e0h2 s a variable */
				{
				tmp1 = -(alphah[1] + betah[1]) / abh[1][0];
				for (j = 0; j < NSAMPLE; j++)
					{
					fjac[j+offset+jmp] = tmp1 * keepa4[1][j] * keepa1[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* e1h2 */
		   if (MTYPE == 2 && NH > 1 && V_E1H2) /* if e1h2 s a variable */
				{
				tmp1 = -(alphah[1] + betah[1]) * (STEP - VHH);
				for (j = 0; j < NSAMPLE; j++)
					{
					fjac[j+offset+jmp] = tmp1 * keepa4[1][j] * keepa1[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* e2h2 */
		   if (MTYPE == 2 && NH > 1 && V_E2H2) /* if e2h2 s a variable */
				{
				tmp1 = -(alphah[1] + betah[1]) * pow(STEP - VHH, 2.0);
				for (j = 0; j < NSAMPLE; j++)
					{
					fjac[j+offset+jmp] = tmp1 * keepa4[1][j] * keepa1[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* e0h3 */
		   if (MTYPE == 2 && NH > 2 && V_E0H3) /* if e0h3 s a variable */
				{
				tmp1 = -(alphah[2] + betah[2]) / abh[2][0];
				for (j = 0; j < NSAMPLE; j++)
					{
					fjac[j+offset+jmp] = tmp1 * keepa4[2][j] * keepa1[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* e1h3 */
		   if (MTYPE == 2 && NH > 2 && V_E1H3) /* if e1h3 s a variable */
				{
				tmp1 = -(alphah[2] + betah[2]) * (STEP - VHH);
				for (j = 0; j < NSAMPLE; j++)
					{
					fjac[j+offset+jmp] = tmp1 * keepa4[2][j] * keepa1[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* e2h3 */
		   if (MTYPE == 2 && NH > 2 && V_E2H3) /* if e2h3 s a variable */
				{
				tmp1 = -(alphah[2] + betah[2]) * pow(STEP - VHH, 2.0);
				for (j = 0; j < NSAMPLE; j++)
					{
					fjac[j+offset+jmp] = tmp1 * keepa4[2][j] * keepa1[j];
					}
				jmp += m;      /* next varying parameter */
				}

			/* loop through fn */
         for (ii = 0; ii < NH+NNONH-1; ii++)
				{
            if (lsdata->parameters.f[ii].state == variable) /* if fn is a variable */
					{
               for (j = 0; j < NSAMPLE; j++)
						{
                  tmp1 = hh[1] + (hh[0] - hh[1]) * exptTh[ii][j];
                  if (!NNONH) /* no noninactivating groups */
                     tmp2 = hh[1] + (hh[0] - hh[1]) * exptTh[NH-1][j];
						else
                     tmp2 = 1.0;
						fjac[j+offset+jmp] = keepa1[j] * (tmp1 - tmp2);
						}
               jmp += m;  /* next varying parameter */
					}
				}

			/* Time Constants */
         for (jj = 0; jj < NVSLEV; jj++) 
				{
			   /* Tm[] */
            if (lsdata->parameters.Tc[jj].Tm.state == variable) 
									/* if Tm[jj] is a variable */
					{
               if (lsdata->parameters.Tc[jj].Volt == STEP)
                  /* if this sweep uses Tm[jj] */
						{
                  for (j = 0; j < NSAMPLE; j++)
							{
                     fjac[j+offset+jmp] = keepa2[j] * keepa3[j] / (Tm * Tm);
                     }
						}
					else /* this sweep is independent of Tm[jj] */
						{
                  for (j = 0; j < NSAMPLE; j++)
                     fjac[j+offset+jmp] = 0.0;
                  }
               jmp += m;
               }
            /* Th */
            for (k = 0; k < NH; k++)
					{
               if (lsdata->parameters.Tc[jj].Th[k].state == variable)
	               /* if Th[jj][k] is a variable */
						{
                  if (lsdata->parameters.Tc[jj].Volt == STEP)
                     /* if this sweep uses Th[jj][k] */
							{
                     for (j = 0; j < NSAMPLE; j++)
								{
                        fjac[j+offset+jmp] = keepa1[j] * keepa4[k][j] / (Th[k] * Th[k]);
                        }
							}
						else /* this sweep is independent of Th[jj][k] */
							{
                     for (j = 0; j < NSAMPLE; j++)
                        fjac[j+offset+jmp] = 0.0;
                     }
                  jmp += m;
						}
					}
				}
			free(keepa1);
			free(keepa2);
			free(keepa3);
			for (k=0; k<NH; k++) free(keepa4[k]);
			free(keepa4);
			if (MTYPE == 2)
				{
				free(alphah);
				free(betah);
				}
			} /* end if task = 2 */
		for (k=0; k<NH; k++) free(exptTh[k]);
		free(exptTh);
		free(exptTm);
		offset += NSAMPLE;
		} /* end if nsample>0 */
	} /* end for i=1 to nswp */
free(gfrac);
if (NH > 0)
	{
	free(abh);
	free(Th);
	}
return 0;
}
