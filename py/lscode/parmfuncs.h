struct consvars_str {	/* constraints and the variables they constrain */
	int nconseq; /* constraint equation number */
	enum paramconstraints constraint; /* type of constraint */
	enum paramconstraints *vars;	/* pointer to constraint state
		           field of relevant parameter */
};

void initoutput(struct leastsquaresdata *lsdata);

int detCONS(int nvp, struct leastsquaresdata *lsdata, int *nc,
   double ***cA, double **cb, int *ncv, struct consvars_str **consvars);

void detDIAG(int mtype, int nh, int nnonh, struct parameters *parms, double *diag);

int countVP(int nh, int nnonh, struct parameters *parms);

void extractVP(int nh, int nnonh, struct parameters *parms, double *x);

void insertVP(int nh, int nnonh, struct parameters *parms, double *x);

void insertSE(int nh, int nnonh, struct parameters *parms, double *x);
