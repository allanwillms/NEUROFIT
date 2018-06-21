int lmder(int (*fcn)(), void *lsdata, int m, int n, double x[], 
			double *diag, double factor, int nprint,
			double ftol, double xtol, double gtol, int maxfev, 
			int nc, double **cA, double *cb, int appc[],
			double fvec[], double *fnorm, double fjac[], int ipvt[], 
			double qtf[], int *nfev, int *njev);

enum cons_state {inactive, active, deactive, applied};

int getjac(int m, int n, double x[], double fvec[], double fnorm, 
           int (*fcn)(), void *lsdata, double fjac[], int ipvt[], 
           double qtf[], double jaccnorm[], double *gnorm);

void calcscons(int n, double *x, int nc, double **cA, double *cb, 
               double *cbs);

int updatecons(int n, int nc, int *apporder, double **cA, double *cbs, 
      double **Acons, double *bcons, enum cons_state *cstate, 
      int (*selcons)[2], int *freeind, int *nact, int *napp);

void genconssys(int m, int n, int napp, double *fjac, double *qtf,
      int *ipvt, double **Acons, double *bcons, double *R, double *b,
      int *perm);

int testcons(int n, int napp, double *x, int *freeind, int nc, 
		double **cA, double *cbs, enum cons_state *cstate, int *apporder);

