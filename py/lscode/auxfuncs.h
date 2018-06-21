int binsearch(double x, int n, double *y);

void getprottype(double Erev, struct dataspecs *dataspecs, int *prottype, 
	int count[3]);

double getIres(int fact, int nh, int tinfl, struct sweep *sweep);

double expsolve(double b, double tol);

void linfit(int m, int n, double *A, double *b, double *x);

void peakform(int st, int en, double *x, double *y, int *n,
   double *A, double *b);

void polyform(int st, int en, int deg, double *x, double *y, int *n,
   double *A, double *b);

void getpeak(int fact, struct sweep *sweep, double *Ipeak, 
	double *tpeak, int *tinfl);

int noisecalc(int deg, int ndata, double *time, double *data, double *noise);

int secalc(int m, int n, double *R, int *ipvt, double *stderror);
