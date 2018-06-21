/*********************************************************
 *
 *   LSInter.c - Python interface module to least squares
 *   =========   code for Fulltrace
 *
 *               Greg Ewing, May 2001
 *
 *********************************************************/

static int debug = 0;
static int debug_dump = 0;
static int debug_limit_arrays = 1;

#include <stdio.h>
#include <stdlib.h>
#include "Python.h"
#include "lsdata.h"
#include "fitlsdata.h"

static PyObject *LSInter_Error;

/*
 *   Utilities
 */

static PyObject *get_item(PyObject *py_container, int index) {
  /* Return a borrowed reference to a sequence item. */
  PyObject *py_result = PySequence_GetItem(py_container, index);
  Py_XDECREF(py_result);
  return py_result;
}

static int check_length(PyObject *obj, int required_length, char *what) {
  int len = PyObject_Length(obj);
  if (len < 0)
    return 0;
  if (len != required_length) {
    PyErr_Format(PyExc_ValueError, 
		 "Wrong length object for %s (expected %d, got %d)",
		 what, required_length, len);
    return 0;
  }
  return 1;
}

static int check_tuple(PyObject *obj, int required_length, char *what) {
  if (!PyTuple_Check(obj)) {
    PyErr_Format(PyExc_TypeError, "Object for %s not a tuple", what);
    return 0;
  }
  return check_length(obj, required_length, what);
}

static int check_list(PyObject *obj, char *what) {
  if (!PyList_Check(obj)) {
    PyErr_Format(PyExc_TypeError, "Object for %s not a list", what);
    return 0;
  }
  return 1;
}

static int check_list_length(PyObject *obj, int required_length, char *what) {
  return
    check_list(obj, what) &&
    check_length(obj, required_length, what);
}

static int get_int(PyObject *py_container, int index, int *value, char *what) {
  PyObject *item = get_item(py_container, index);
  if (!PyInt_Check(item)) {
    PyErr_Format(PyExc_TypeError, "Value for %s not an integer", what);
    return 0;
  }
  *value = PyInt_AsLong(item);
  return 1;
}

static int get_enum(PyObject *py_container, int index, int min, int max,
		    int *value, char *what)
{
  if (!get_int(py_container, index, value, what))
    return 0;
  if (*value < min || *value > max) {
    PyErr_Format(PyExc_ValueError, 
		 "Value for %s (%d) not in range %d..%d",
		 what, *value, min, max);
    return 0;
  }
  return 1;
}
 
static int get_double(PyObject *py_container, int index, 
		      double *value, char *what) {
  PyObject *item = get_item(py_container, index);
  if (!PyFloat_Check(item)) {
    PyErr_Format(PyExc_TypeError, "Value for %s not a float", what);
    return 0;
  }
  *value = PyFloat_AsDouble(item);
  return 1;
}

static int get_double_array(PyObject *py_container, int index, int *size,
			    double **parray, char *what) {
  PyObject *py_array;
  int i, n;
  char element_what[48];
  double *array;

  py_array = get_item(py_container, index);
  if (!check_list(py_array, what))
    return 0;
  n = PyObject_Length(py_array);
  if (*size == -1)
    *size = n;
  else if (*size != n) {
    PyErr_Format(PyExc_ValueError,
		 "Wrong length object for %s (expected %d, got %d)",
		 what, *size, n);
    return 0;
  }
  array = (double *)calloc(n, sizeof(double));
  if (!array) {
    PyErr_NoMemory();
    return 0;
  }
  *parray = array;
  sprintf(element_what, "element of %s", what);
  for (i = 0; i < n; i++) {
    if (!get_double(py_array, i, &array[i], element_what))
      return 0;
  }
  return 1;
}

/*
 *   Freeing
 */

static void free_sweep(struct sweep *sw) {
  if (sw->time)
    free(sw->time);
  if (sw->data)
    free(sw->data);
}

static void free_dataspecs(struct dataspecs *ds) {
  int i;
  if (ds->sweeps) {
    for (i = 0; i < ds->nswp; i++)
      free_sweep(&ds->sweeps[i]);
    free(ds->sweeps);
  }
}

static void free_timeconsts(struct timeconsts *tc) {
  if (tc->Th)
    free(tc->Th);
}

static void free_parameters(struct parameters *p) {
  int i;
  if (p->param)
	  free(p->param);
  if (p->f)
	  free(p->f);
  if (p->Tc) {
    for (i = 0; i < p->nVslev; i++)
      free_timeconsts(&p->Tc[i]);
    free(p->Tc);
  }
}

static void free_fitresults(struct fitresults *fr) {
  if (fr->msg) {
    /*int i;
    for (i = 0; i < fr->nmsg; i++)
      free(fr->msg[i]);*/
    free((char **)fr->msg); /* cast to suppress const warning */
  }
}

static void free_lsdata(struct leastsquaresdata *ls) {
  if (ls) {
    free_dataspecs(&ls->dataspecs);
    free_parameters(&ls->parameters);
    free_fitresults(&ls->fitresults);
    free(ls);
  }
}

/*
 *   Windows
 */

static int get_window(PyObject *py_sweep, int index, 
		      int ntime, double time[], struct DataWin *win,
		      char *what)
{
  PyObject *py_win;

  py_win = PyTuple_GetItem(py_sweep, index);
  if (!(check_tuple(py_win, 2, what) &&
	get_int(py_win, 0, &win->str, "window start index") &&
	get_int(py_win, 1, &win->nsample, "number of samples in window")))
    return 0;
  return 1;
}

/*
 *   Sweeps
 */

static int get_voltage_pro(PyObject *py_sweep, 
			   struct timeconsts tc[], 
			   struct VoltagePro *vp) 
{
  PyObject *py_vp;
  int tc_index, VPtype;

  py_vp = PyTuple_GetItem(py_sweep, 0);
  if (!(check_tuple(py_vp, 4, "struct VoltagePro") &&
	get_enum(py_vp, 0, 0, 1, &VPtype, "voltage protocol type") &&
	get_double(py_vp, 1, &vp->prestep, "prestep voltage") &&
	get_double(py_vp, 2, &vp->step, "step voltage") &&
	get_int(py_vp, 3, &tc_index, "timeconst index in VoltagePro")))
    return 0;
  vp->VPtype = (enum VoltageProType) VPtype;
  vp->Tc = &tc[tc_index];
  return 1;
}

static int get_sweep(PyObject *py_sweep, 
		     struct timeconsts tc[], 
		     struct sweep *sweep) 
{
  sweep->ntime = -1;
  return
    check_tuple(py_sweep, 8, "struct sweep") &&
    get_voltage_pro(py_sweep, tc, &sweep->Volt) &&
    get_double_array(py_sweep, 1, 
		     &sweep->ntime, &sweep->time, "times array") &&
    get_double_array(py_sweep, 2, 
		     &sweep->ntime, &sweep->data, "data array") &&
    get_window(py_sweep, 3, 
	       sweep->ntime, sweep->time, &sweep->fitWin, "fitting window") &&
    get_window(py_sweep, 4, 
	       sweep->ntime, sweep->time, &sweep->noiseWin, "noise window");
  /* 5 == noise OUTPUT */
  /* 6 == varcont OUTPUT */
  /* 7 == fitted function values OUTPUT */
}

static int get_dataspecs(PyObject *py_lsdata,
			 struct timeconsts Tc[], 
			 struct dataspecs *ds) {
  PyObject *py_sweeps;
  struct sweep *sweeps;
  int i;

  py_sweeps = PyTuple_GetItem(py_lsdata, 2);
  if (!check_list(py_sweeps, "sweeps"))
    return 0;
  ds->nswp = PyObject_Length(py_sweeps);
  sweeps = (struct sweep *)calloc(ds->nswp, sizeof(struct sweep));
  if (!sweeps) {
    PyErr_NoMemory();
    return 0;
  }
  ds->sweeps = sweeps;
  for (i = 0; i < ds->nswp; i++) {
    PyObject *py_sweep = PyList_GetItem(py_sweeps, i);
    if (!get_sweep(py_sweep, Tc, &sweeps[i]))
      return 0;
	sweeps[i].fitvals = (double *) calloc(sweeps[i].ntime, sizeof(double));
    if (!sweeps[i].fitvals) {
      PyErr_NoMemory();
      return 0;
	}
  }
  /* printf("Successful return from get_sweep\n"); */
  return 1;
}

/*
 *   Parameters
 */

static int get_param(PyObject *py_params, int index, struct param *param,
		     char *what) {
  PyObject *py_param;
  int state;

  py_param = get_item(py_params, index);
  if (!(check_tuple(py_param, 5, what) &&
	get_double(py_param, 0, &param->value, "value of parameter") &&
	get_enum(py_param, 1, 0, 1, &state, "state of parameter")))
    /* 2 == set standard_error */
    /* 3 == set constraint */
	/* 4 = set value */
    return 0;
  param->state = (enum paramstate)state;
  return 1;
}

static int get_param_array(PyObject *py_container, int index, int size, 
			   struct param **pparams, char *what) 
{
  PyObject *py_param_array;
  int i;
  struct param *params;
  char element_what[48];

  py_param_array = PyTuple_GetItem(py_container, index);
  if (!check_list_length(py_param_array, size, what))
    return 0;
  params = (struct param *)calloc(size, sizeof(struct param));
  if (!params) {
    PyErr_NoMemory();
    return 0;
  }
  *pparams = params;
  sprintf(element_what, "element of %s", what);
  for (i = 0; i < size; i++) {
    if (!get_param(py_param_array, i, &params[i], element_what))
      return 0;
  }
  return 1;
}

static int get_timeconsts(PyObject *py_timeconsts, int nh, 
			  struct timeconsts *timeconsts)
{
  return
    check_tuple(py_timeconsts, 3, "struct timeconsts") &&
    get_param(py_timeconsts, 0, &timeconsts->Tm, "Tm") &&
    get_param_array(py_timeconsts, 1, nh, &timeconsts->Th, "Th array") &&
    get_double(py_timeconsts, 2, &timeconsts->Volt, "voltage of timeconsts");
}

static int get_timeconsts_array(PyObject *py_params, int index, int nh, 
				int *pnVslev, struct timeconsts **pTc)
{
  PyObject *py_timeconsts_array;
  int nVslev, i;
  struct timeconsts *Tc;

  py_timeconsts_array = PyTuple_GetItem(py_params, index);
  if (!check_list(py_timeconsts_array, "Tc array"))
    return 0;
  nVslev = PyObject_Length(py_timeconsts_array);
  *pnVslev = nVslev;
  Tc = (struct timeconsts *)calloc(nVslev, sizeof(struct timeconsts));
  if (!Tc) {
    PyErr_NoMemory();
    return 0;
  }
  *pTc = Tc;
  for (i = 0; i < nVslev; i++) {
    PyObject *py_timeconsts = PyList_GetItem(py_timeconsts_array, i);
    if (!get_timeconsts(py_timeconsts, nh, &Tc[i]))
      return 0;
  }
  return 1;
}

static int get_parameters(PyObject *py_lsdata, struct modspecs *ms,
			  struct parameters *parms) {
  PyObject *py_parms = PyTuple_GetItem(py_lsdata, 3);
  int nf = ms->nh + ms->nnonh - 1;
  if (ms->mtype == 1)
	  parms->nsetparam = 6;
  else if (ms->mtype == 2)
	  {
	  if (ms->nh > 0) parms->nsetparam = 14 + 3 * (ms->nh-1);
	  else parms->nsetparam = 8;
	  }
  return
    check_tuple(py_parms, 3, "struct parameters") &&
	get_param_array(py_parms, 0, parms->nsetparam, &parms->param, "set parameters") &&
    get_param_array(py_parms, 1, nf, &parms->f, "f parameters") &&
    get_timeconsts_array(py_parms, 2, ms->nh, &parms->nVslev, &parms->Tc);
}

/*
 *   Struct leastsquaresdata and its substructures
 */

static int get_modspecs(PyObject *py_lsdata, struct modspecs *ms) {
  PyObject *py_ms = PyTuple_GetItem(py_lsdata, 0);
  return
    check_tuple(py_ms, 4, "struct modspecs") &&
	get_int(py_ms, 0, &ms->mtype, "mtype") &&
    get_int(py_ms, 1, &ms->nh, "nh") &&
    get_int(py_ms, 2, &ms->nnonh, "nnonh") &&
    get_int(py_ms, 3, &ms->pp, "pp");
}

static int get_algspecs(PyObject *py_lsdata, struct algspecs *as) {
  PyObject *py_as = PyTuple_GetItem(py_lsdata, 1);
  return
    check_tuple(py_as, 5, "struct algspecs") &&
    get_int(py_as, 0, &as->noiseDeg, "noiseDeg") &&
    get_double(py_as, 1, &as->ftol, "ftol") &&
    get_double(py_as, 2, &as->xtol, "ftol") &&
    get_double(py_as, 3, &as->gtol, "ftol") &&
    get_int(py_as, 4, &as->maxfeval, "maxfeval");
}

static int get_lsdata(PyObject *py_lsdata, struct leastsquaresdata **plsdata) {
  struct leastsquaresdata *lsdata;

  if (debug)
    printf("LSInter: converting input data to C structures\n");
  lsdata = (struct leastsquaresdata *)
    calloc(1, sizeof(struct leastsquaresdata));
  if (!lsdata) {
    PyErr_NoMemory();
    return 0;
  }
  *plsdata = lsdata;
  return
    check_tuple(py_lsdata, 5, "struct leastsquaresdata") &&
    get_modspecs(py_lsdata, &lsdata->modspecs) &&
    get_algspecs(py_lsdata, &lsdata->algspecs) &&
    get_parameters(py_lsdata, &lsdata->modspecs, &lsdata->parameters) &&
    get_dataspecs(py_lsdata, lsdata->parameters.Tc, &lsdata->dataspecs);
}

/*--------------------------- Debugging ---------------------------*/

static void dump_modspecs(struct modspecs *ms) {
  printf("modspecs:\n");
  printf("  mtype = %d\n", ms->mtype);
  printf("  nh = %d\n", ms->nh);
  printf("  nnonh = %d\n", ms->nnonh);
  printf("  pp = %d\n", ms->pp);
}

static void dump_algspecs(struct algspecs *as) {
  printf("algspecs:\n");
  printf("noiseDeg = %d\n", as->noiseDeg);
  printf("ftol = %lg\n", as->ftol);
  printf("xtol = %lg\n", as->xtol);
  printf("gtol = %lg\n", as->gtol);
  printf("maxfeval = %d\n", as->maxfeval);
}

static void dump_voltagepro(struct VoltagePro *vp, struct timeconsts Tc[]) {
  printf("        prestep = %lg\n", vp->prestep);
  printf("        step    = %lg\n", vp->step);
  printf("        Tc      = %d\n", vp->Tc - Tc);
}

static void dump_double_array(int indent, char *lbl, int n, double x[],
			      int max) 
{
  int i;

  if (!debug_limit_arrays)
    max = n;
  printf("%*s%s:", indent, "", lbl);
  for (i = 0; i < n && i < max; i++) {
    if (i % 6 == 0) {
      if (i > 0)
	printf("\n");
      printf("%*s", indent, "");
    }
    printf("%12lg", x[i]);
  }
  if ( i < n)
    printf(" ...");
  printf("\n");
}

static void dump_window(int indent, char *lbl, struct DataWin *win) {
  printf("%*s%s: %10d %10d\n", indent, "", lbl, win->str, win->nsample);
}

static void dump_sweep(struct sweep *sw, struct timeconsts Tc[], int output) {
  if (!output) {
    dump_voltagepro(&sw->Volt, Tc);
    printf(", ntime = %d\n", sw->ntime);
    dump_double_array(4, "time", sw->ntime, sw->time, 5);
    dump_double_array(4, "data", sw->ntime, sw->data, 5);
    dump_window(6, "fitWin", &sw->fitWin);
    dump_window(6, "noiseWin", &sw->noiseWin);
  }
  if (output) {
    printf("    noise = %lg, varcont = %lg\n", sw->noise, sw->varcont);
  }
}

static void dump_dataspecs(struct dataspecs *ds, struct timeconsts Tc[],
			   int output) 
{
  int i;
  printf("dataspecs:\n");
  printf("  nswp = %d\n", ds->nswp);
  for (i = 0; i < ds->nswp; i++) {
    printf("  sweep %d:\n", i);
    dump_sweep(&ds->sweeps[i], Tc, output);
  }
}

static char *enum_to_str(int x, int range, char *names[]) {
  if (x >= 0 && x < range)
    return names[x];
  else
    return "<ENUM OUT OF RANGE>";
}

static char *state_names[] = {"constant", "variable"};
static char *constraint_names[] = {"none", "upperbnd", "lowerbnd", "sumfbnd"};

static void dump_param(char *lbl, struct param *param, int output) {
  printf("  %s: value = %lg", lbl, param->value);
  if (!output)
    printf(", state = %s", enum_to_str(param->state, 2, state_names));
  if (output)
    printf(", stderror = %lg, constraint = %s",
	   param->stderror, 
	   enum_to_str(param->constraint, 4, constraint_names));
  printf("\n");
}

static void dump_timeconsts(int nVslev, int nh, struct timeconsts Tc[],
			    int output) 
{
  int i, j;
  char lbl[48];
  struct timeconsts *tc;

  for (i = 0; i < nVslev; i++) {
    tc = &Tc[i];
    printf("[%d] Volt = %lg\n", i, tc->Volt);
    sprintf(lbl, "Tm(%lg)", tc->Volt);
    dump_param(lbl, &tc->Tm, output);
    for (j = 0; j < nh; j++) {
      sprintf(lbl, "Th%d(%lg)", j, tc->Volt);
      dump_param(lbl, &tc->Th[j], output);
    }
  }
}

static void dump_parameters(struct parameters *par, struct modspecs *ms,
			    int output) 
{
  int nf, i;
  char lbl[16];

  printf("parameters:\n");
  printf("  nVslev = %d\n", par->nVslev);
  printf("  nsetparam = %d\n", par->nsetparam);
  for (i=0; i<par->nsetparam; i++)
	  {
	  sprintf(lbl, "param[%d]", i);
	  dump_param(lbl, &par->param[i], output);
	  }
  nf = ms->nh + ms->nnonh - 1;
  for (i = 0; i < nf; i++) {
    sprintf(lbl, "f[%d]", i);
    dump_param(lbl, &par->f[i], output);
  }
  dump_timeconsts(par->nVslev, ms->nh, par->Tc, output);
}

static void dump_fitresults(struct fitresults *fr) {
  int i;

  printf("fitresults:\n");
  printf("  nfev = %d\n", fr->nfev);
  printf("  njev = %d\n", fr->njev);
  printf("  npts = %d\n", fr->npts);
  printf("  variance = %lg\n", fr->variance);
  printf("  nmsg = %d\n", fr->nmsg);
  printf("  msg:\n");
  for (i = 0; i < fr->nmsg; i++)
    printf("    \"%s\"\n", fr->msg[i]);
}

static void dump_lsdata(struct leastsquaresdata *lsdata, char *heading,
			int output) 
{
  printf("===== LSInter: %s =====\n", heading);
  if (!output) {
    dump_modspecs(&lsdata->modspecs);
    dump_algspecs(&lsdata->algspecs);
  }
  dump_dataspecs(&lsdata->dataspecs, lsdata->parameters.Tc, output);
  dump_parameters(&lsdata->parameters, &lsdata->modspecs, output);
  if (output)
    dump_fitresults(&lsdata->fitresults);
}

/*---------------------- Storing output values --------------------*/

static PyObject *get_function(PyObject *py_container, int index, char *what) {
  PyObject *py_function = get_item(py_container, index);
  if (!PyCallable_Check(py_function)) {
    PyErr_Format(PyExc_TypeError, "Object for setting %s not callable", what);
    return 0;
  }
  return py_function;
}

static int check_result(PyObject *py_result, char *what) {
  if (!py_result) {
    fprintf(stderr, "LSInter: Error concerning %s\n", what);
    return 0;
  }
  Py_DECREF(py_result);
  return 1;
}

static int put_int(int value, 
		   PyObject *py_container, int index, 
		   char *what) 
{
  PyObject *py_setter = get_function(py_container, index, what);
  return check_result(PyObject_CallFunction(py_setter, "i", value), what);
}

static int put_double(double value, 
		      PyObject *py_container, int index, 
		      char *what) 
{
  PyObject *py_setter = get_function(py_container, index, what);
  return check_result(PyObject_CallFunction(py_setter, "f", value), what);
}

static int put_double_array(int size, double values[], 
			    PyObject *py_container, int index, 
			    char *what)
{
  PyObject *py_values;
  int i;

  if (debug)
    printf("LSInter: putting double array\n");
  py_values = get_item(py_container, index);
  if (!check_list(py_values, what))
    return 0;
  PySequence_DelSlice(py_values, 0, PyInt_GetMax());
  for (i = 0; i < size; i++) {
    double x = values[i];
    PyObject *py_value;
    /* Convert infinities into something Python can deal with better */
    if (x < -1e99)
      x = -1e99;
    else if (x > 1e99)
      x = 1e99;
    py_value = PyFloat_FromDouble(x);
    if (!py_value)
      return 0;
    PyList_Append(py_values, py_value);
    Py_DECREF(py_value);
  }
  return 1;
}

static int put_sweep_results(struct sweep *sweep, PyObject *py_sweep) {
  if (debug)
    printf("LSInter: putting sweep results\n");
  return
    put_double(sweep->noise, py_sweep, 5, "noise of sweep") &&
    put_double(sweep->varcont, py_sweep, 6, "varcont of sweep") &&
    put_double_array(sweep->ntime, sweep->fitvals, py_sweep, 7, "fitted data");
}

static int put_dataspecs_results(struct dataspecs *ds, 
				 PyObject *py_dataspecs) 
{
  int i;
  struct sweep *sweep;
  PyObject *py_sweep;
  
  if (debug)
    printf("LSInter: putting dataspecs results\n");
  for (i = 0; i < ds->nswp; i++) {
    sweep = &ds->sweeps[i];
    py_sweep = get_item(py_dataspecs, i);
    if (!put_sweep_results(sweep, py_sweep))
      return 0;
  }
  return 1;
}  

static int put_param_results(struct param *param, 
			     PyObject *py_container, int index) 
{
  PyObject *py_param;
  
  if (debug)
    printf("LSInter: putting param results %d\n", index);
  py_param = get_item(py_container, index);
  return
    put_double(param->stderror, py_param, 2, "standard error of parameter") &&
    put_int(param->constraint, py_param, 3, "constraint of parameter") &&
    put_double(param->value, py_param, 4, "fitted value of parameter");
}

static int put_param_array_results(struct param params[],
				   PyObject *py_container, int index)
{
  int i, size;
  PyObject *py_params;

  if (debug)
    printf("LSInter: putting param array results\n");
  py_params = get_item(py_container, index);
  size = PySequence_Length(py_params);
  for (i = 0; i < size; i++) {
    if (!put_param_results(&params[i], py_params, i))
      return 0;
  }
  return 1;
}

static int put_timeconsts_results(struct timeconsts *tc, 
				  PyObject *py_container, int index) 
{
  PyObject *py_tc;
  
  if (debug)
    printf("LSInter: putting timeconsts results %d\n", index);
  py_tc = get_item(py_container, index);
  return
    put_param_results(&tc->Tm, py_tc, 0) &&
    put_param_array_results(tc->Th, py_tc, 1);
}

static int put_timeconsts_array_results(struct timeconsts Tc[],
					PyObject *py_container, int index)
{
  int i, size;
  PyObject *py_Tc;

  if (debug)
    printf("LSInter: putting timeconsts array results\n");
  py_Tc = get_item(py_container, index);
  size = PySequence_Length(py_Tc);
  for (i = 0; i < size; i++) {
    if (!put_timeconsts_results(&Tc[i], py_Tc, i))
      return 0;
  }
  return 1;
}

static int put_parameters_results(struct parameters *params,
				  PyObject *py_params)
{
  if (debug)
    printf("LSInter: putting param results\n");
  return
	put_param_array_results(params->param, py_params, 0) &&
    put_param_array_results(params->f, py_params, 1) &&
    put_timeconsts_array_results(params->Tc, py_params, 2);
}

static int put_messages(int nmsg, const char *msgs[], 
			PyObject *py_container, int index, 
			char *what)
{
  PyObject *py_msgs;
  int i;
  
  if (debug)
    printf("LSInter: putting messages\n");
  py_msgs = get_item(py_container, index);
  if (!check_list(py_msgs, what))
    return 0;
  for (i = 0; i < nmsg; i++) {
    PyObject *py_msg = PyString_FromString(msgs[i]);
    if (!py_msg)
      return 0;
    if (PyList_Append(py_msgs, py_msg) < 0)
      return 0;
    Py_DECREF(py_msg);
  }
  return 1;
}

static int put_fitresults(struct fitresults *fr, PyObject *py_fr) {
  if (debug)
    printf("LSInter: putting fit results\n");
  return
    check_tuple(py_fr, 6, "struct fitresults") &&
    put_int(fr->nfev, py_fr, 0, "nfev result") &&
    put_int(fr->njev, py_fr, 1, "njev result") &&
    put_int(fr->npts, py_fr, 2, "npts result") &&
    put_int(fr->nfitp, py_fr, 3, "nfitp result") &&
    put_double(fr->variance, py_fr, 4, "variance result") &&
    put_messages(fr->nmsg, fr->msg, py_fr, 5, "messages");
}

static int put_lsdata_results(struct leastsquaresdata *lsdata,
			      PyObject *py_lsdata) 
{
  PyObject *py_dataspecs;
  PyObject *py_parameters;
  PyObject *py_fitresults;

  if (debug)
    printf("LSInter: storing return values\n");
  py_dataspecs = get_item(py_lsdata, 2);
  py_parameters = get_item(py_lsdata, 3);
  py_fitresults = get_item(py_lsdata, 4);
  return
    put_dataspecs_results(&lsdata->dataspecs, py_dataspecs) &&
    put_parameters_results(&lsdata->parameters, py_parameters) &&
    put_fitresults(&lsdata->fitresults, py_fitresults);
}

/*---------------------- Exported Functions -----------------------*/

static PyObject *LSInter_fit(PyObject *self, PyObject *args) {
  PyObject *py_lsdata = 0;
  struct leastsquaresdata *lsdata = 0;

  if (!PyArg_ParseTuple(args, "O", &py_lsdata))
    goto bad;
  if (!get_lsdata(py_lsdata, &lsdata))
    goto bad;
  /* printf("Successful return from get_lsdata, about to call fitlsdata\n"); */
  if (debug_dump)
    dump_lsdata(lsdata, "Before Fitting", 0);
  if (debug) {
    printf("LSInter: Calling Full Fitting code\n");
    fflush(stdout);
  }
  fitlsdata(lsdata);
  if (debug)
    printf("LSInter: Returned from Full Fitting code\n");
  if (debug_dump)
    dump_lsdata(lsdata, "After Fitting", 1);
  if (!put_lsdata_results(lsdata, py_lsdata))
    goto bad;
  free_lsdata(lsdata);
  if (debug)
    printf("LSInter: normal return\n");
  Py_INCREF(Py_None);
  return Py_None;

bad:
  free_lsdata(lsdata);
  if (debug)
    printf("LSInter: error return\n");
  return 0;
}

static PyObject *LSInter_guess(PyObject *self, PyObject *args) {
  PyObject *py_lsdata = 0;
  struct leastsquaresdata *lsdata = 0;

  if (!PyArg_ParseTuple(args, "O", &py_lsdata))
    goto bad;
  if (!get_lsdata(py_lsdata, &lsdata))
    goto bad;
  if (debug_dump)
    dump_lsdata(lsdata, "Before Quick Fitting", 0);
  if (debug) {
    printf("LSInter: Calling Quick Fitting code\n");
    fflush(stdout);
  }
  getinitval(lsdata);
  if (debug)
    printf("LSInter: Returned from Quick Fitting code\n");
  if (debug_dump)
    dump_lsdata(lsdata, "After Quick Fitting", 1);
  if (!put_lsdata_results(lsdata, py_lsdata))
    goto bad;
  free_lsdata(lsdata);
  if (debug)
    printf("LSInter: normal return\n");
  Py_INCREF(Py_None);
  return Py_None;

bad:
  free_lsdata(lsdata);
  if (debug)
    printf("LSInter: error return\n");
  return 0;
}

/*---------------------- Function Table -----------------------*/

static PyMethodDef LSInter_methods[] = {
  {"fit",	  LSInter_fit,   METH_VARARGS},
  {"guess",	LSInter_guess, METH_VARARGS},
  {NULL,	NULL}
};

/*---------------------- Initialisation -----------------------*/

DL_EXPORT(void)
initLSInter()
{
  PyObject *m, *d;
  /*** printf("Entering initLSInter\n"); */
  m = Py_InitModule("LSInter", LSInter_methods);
  d = PyModule_GetDict(m);
  /*LSInter_Error = PyErr_NewException("LSInter.Error", NULL, NULL);*/
  /*PyDict_SetItemString(d, "error", ErrorObject);*/
  /*** printf("Leaving initLSInter\n"); */
}