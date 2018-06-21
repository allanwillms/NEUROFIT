/* 
   Data structures for passing of information between the user interface
   module and the number crunching module.

   Version 15
*/

/* Define what indicies of the parameter array the set parameter values are located in */

/* MODEL TYPES 1 and 2 */
#define i_Erev 0    /* Reversal potential */
#define i_gmax 1    /* maximal conductance */

/* MODEL TYPE 1 */
#define i_V2m 2     /* half activation potential */
#define i_sm 3      /* activation slope factor */
#define i_V2h 4     /* half inactivation potential */
#define i_sh 5      /* inactivation slope factor */

/* MODEL TYPE 2 */
#define i_VHm 2     /* half activation potential */
#define i_A0m 3     /* activation rate constant factor */
#define i_a1m 4     /* activation rate linear factor for alpha */
#define i_b1m 5     /* activation rate linear factor for beta */
#define i_a2m 6     /* activation rate quadratic factor for alpha */
#define i_b2m 7     /* activation rate quadratic factor for beta */
#define i_VHh 8     /* half inactivation potential */
#define i_A0h 9     /* inactivation rate constant factor */
#define i_a1h 10    /* inactivation rate linear factor for alpha */
#define i_b1h 11    /* inactivation rate linear factor for beta */
#define i_a2h 12    /* inactivation rate quadratic factor for alpha */
#define i_b2h 13    /* inactivation rate quadratic factor for beta */
#define i_e0h2 14   /* inactivation rate extra constant factor for h2 */
#define i_e1h2 15   /* inactivation rate extra linear factor for h2 */
#define i_e2h2 16   /* inactivation rate extra quadratic factor for h2 */
#define i_e0h3 17   /* inactivation rate extra constant factor for h3 */
#define i_e1h3 18   /* inactivation rate extra linear factor for h3 */
#define i_e2h3 19   /* inactivation rate extra quadratic factor for h3 */


enum VoltageProType { /* type of voltage protocol */
	varyingstep,      /* varying step potential experiment */
	varyingprestep    /* varying prestep potential experiment */
};

enum paramstate { /* state of model parameter */
	constant,      /* to be held constant */
	variable       /* to be varied */
};

enum paramconstraints { /* constraints on best fit parameter value */
	none,                /* not constrained */
	upperbnd,            /* constrained at an upper boundary */
	lowerbnd,            /* constrained at a lower boundary */
	sumfbnd,             /* sum of the f's is constrained */
	tauhbnd              /* Tauh(i) is equal to Tauh(i+1) */
};

struct param {             /* model parameter information */
	double value;           /* input and OUTPUT, value of parameter */
	double stderror;        /* OUTPUT, standard error */
	enum paramstate state;  /* state of model parameter */
	enum paramconstraints constraint;  /* OUTPUT, constraints on best 
	                                      fit parameter value */
};

struct DataWin {  /* Data window */
	int str;       /* index of start time in time array */
	int nsample;   /* number of samples within window */
};

struct timeconsts {   /* time constant information */
	struct param Tm;   /* activation time constant */
	struct param *Th;  /* size nh, inactivation time constants */
	double Volt;       /* Voltage level for these time constants */
};

struct VoltagePro {       /* Voltage protocol */
	enum VoltageProType VPtype;  /* Voltage Protocol type */
	double prestep;        /* prestep potential */
	double step;           /* step potential */
	struct timeconsts *Tc; /* time constants, This should point to 
                             one of the same structures as pointed 
                             to by one of the Tc pointers in the 
                             "parameters" structure. */
};

struct sweep {              /* sweep information */
	struct VoltagePro Volt;  /* voltage protocol */
	int ntime;               /* number of time points */
	double *time;            /* size ntime */
	double *data;            /* size ntime, observed data */
	struct DataWin fitWin;   /* fitting window */
	struct DataWin noiseWin; /* window for noise calculation */
	double *fitvals;         /* OUTPUT, size ntime, fitted function values */
	double noise;            /* OUTPUT, noise level */
	double varcont;          /* OUTPUT, percent contribution to variance */
};

struct modspecs { /* model specifications */
	int mtype;     /* model type */
	int nh;        /* number of inactivating channel types */
	int nnonh;     /* number of noninactivating channel types */
	int pp;        /* number of independent activation gates */
};

struct algspecs {  /* algorithm specifications */
	int noiseDeg;   /* degree of polynomial for noise calc */
	double ftol;    /* function tolerance for LM code */
	double xtol;    /* solution tolerance for LM code */
	double gtol;    /* gradient tolerance for LM code */
	int maxfeval;   /* maximum number of function evaluations */
};

struct dataspecs {       /* data specifications */
	int nswp;             /* total number of sweeps */
	struct sweep *sweeps; /* size nswp, info for each sweep */
};

struct parameters {       /* model parameters */
	int nVslev;            /* number of distinct voltage step levels */
	int nsetparam;         /* number of parameters in param array */
	struct param *param;   /* set parameters, size nsetparam */
	struct param *f;       /* size nh+nnonh-1, fractions */
	struct timeconsts *Tc; /* size nVslev, time constants */
};

struct fitresults {  /* OUTPUT, results from the fitting process */
	int nfev;         /* number of function evaluations */
	int njev;         /* number of jacobian evaluations */
	int npts;         /* total number of points fitted */
	int nfitp;        /* number of fitted parameters */
	double variance;  /* noise adjusted variance of fit */
	int nmsg;         /* Number of messages */
	const char **msg; /* messages */
};

struct leastsquaresdata {   /* all of the above together */
	struct modspecs modspecs;
	struct algspecs algspecs;
	struct dataspecs dataspecs;
	struct parameters parameters;
	struct fitresults fitresults;
};


