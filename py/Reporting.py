############################################################
#
#  Reporting.py - NEUROFIT, report generation module
#  ============   Greg Ewing, Jun 2001
#
############################################################

import os

from types import *
from string import digits, find, count
import FtProject
import time

def report(project, filename):
  """Produce a report of the results of the most recent
  fitting operation, written to the specified file."""
  f = open(filename, 'w')
  try:
    project_report(f, project)
  finally:
    f.close()

def project_report(f, project):
  f.write("NEUROFIT Report\n")
  f.write("----------------\n\n")
  datetime = time.ctime(time.time())
  f.write("Date:    %s\n" % datetime)
  path = project.get_pathname()
  if path is not None:
    path = os.path.normpath(path)
  f.write(format_values("Project: %s\n\n", (path,)))
  f.write("Model Specifications\n\n")
  model_type = FtProject.model_type_names[project.mtype-1]
  fmtstr = "  Model Type: %d\n" +\
           "  nh          = %d\n" +\
           "  nnonh       = %d\n" +\
           "  p           = %d\n\n"
  f.write(format_values(fmtstr, (model_type, project.nh,
                                 project.nnonh, project.p.value)))
  f.write("Algorithm Specifications\n\n")
  fmtstr = "  noise_deg   = %d\n" +\
           "  ftol        = %.7g\n" +\
           "  xtol        = %.7g\n" +\
           "  gtol        = %.7g\n" +\
           "  nevalfac    = %d\n\n"
  f.write(format_values(fmtstr, (project.noise_deg.value, project.tol_f.value,
                                 project.tol_x.value, project.tol_g.value,
                                 project.nevalfac.value)))
  f.write("Data Specifications\n\n")
  report_experiment(f, project.experiments[0])
  report_experiment(f, project.experiments[1])
  f.write("Fitting Results\n\n")
  fmtstr = "  Number of function evaluations = %d\n" +\
           "  Number of jacobian evaluations = %d\n" +\
           "  Total number of points fitted  = %d\n" +\
           "  Number of fitted parameters    = %d\n" +\
           "  Noise adjusted variance of fit = %g\n\n"
  f.write(format_values(fmtstr,(project.nfev.value, project.njev.value,
                                project.npts.value, project.nfitp.value,
                                project.variance.value)))
  f.write("Model Parameters\n\n")
  report_param_header(f)
  f.write("\n")
  report_param(f, project.Erev)
  report_param(f, project.gmax)
  report_param(f, project.V2m)
  report_param(f, project.sm)
  report_param(f, project.V2h)
  report_param(f, project.sh)
  for fi in project.f:
    report_param(f, fi)
  f.write("  Time Constants:\n")
  report_time_constants(f, project.T)

def report_experiment(f, exp):
  names = ("Step", "Prestep")
  i = exp.experiment_number
  f.write(" Varying %s Potential Experiments\n" % names[i])
  f.write("\n")
  for sweep_set in exp.sweep_sets:
    f.write("  Data File: %s\n" % sweep_set.path)
    f.write("\n")
    f.write("   %s Voltage = %s\n\n" % (names[i-1], sweep_set.Vhold))
    f.write("   %5s  %7s  %21s  %21s  %10s  %9s\n" %
            ("Sweep", names[i], "---Fitting Region----", "----Noise Region-----", "percent of", "noise"))
    f.write("   %5s  %7s  %21s  %21s  %10s  %9s\n" %
            ("Num.", "Voltage", "start             end", "start             end", "  variance", "level"))
    f.write("\n")
    sweeps = sweep_set.sweeps
    for i in xrange(len(sweeps)):
      sweep = sweeps[i]
      v = sweep.Vsweep
      noise = sweep.noise.value
      if sweep.fitting_flag:
        (fmin, fmax) = sweep.fit_region.get()
        (nmin, nmax) = sweep.noise_region.get()
        vcont = sweep.varcont.value
      else:
        (fmin, fmax) = ('Not used', None)
        (nmin, nmax) = (None, None)
        vcont = None
      fmtstr = "   %5d  %7.3g  %-10.5g %10.5g  %-10.5g %10.5g  %10.4g  %9.4g"
      f.write(format_values(fmtstr, (i, v, fmin, fmax, nmin, nmax, vcont, noise)))
      f.write("\n")
    f.write("\n")

def report_time_constants(f, T):
  voltages = T.keys()
  voltages.sort()
  for v in voltages:
    for tc in T[v]:
      report_param(f, tc)

def report_param_header(f):
  f.write("  %-10s  %15s  %15s  %15s  %4s\n" %
          ("Name", "Initial Value", "Fitted Value", "Standard Error", "Fit?"))
  
def report_param(f, param):
  fmtstr = "  %-10s  %15.9g  %15.9g  %15.9g  %4s\n"
  if param.fitting_flag:
    fitted_value = param.fitted_value.value
    std_err = param.standard_error.value
  else:
    fitted_value = None
    std_err = None
  f.write(format_values(fmtstr,(param.label, param.initial_value.value,
                                fitted_value, std_err,
                                ("no", "yes")[param.fitting_flag])))

def format_values(fmtstr, values):
  """ Format a list of values according to the format specifiers in the
  string fmtstr.  Errors in formatting will be caught nicely by changing
  the format specifier to "s" if necessary.  Values of None which cause
  a format error are replaced with blanks.  The escape sequence '%%' is
  dealt with.  If there are more values than format specifiers, the extra
  values are ignored.  If there are more format specifiers than values
  then values of None are appended.
  """
  escapes = count(fmtstr, '%%')
  specs = count(fmtstr, '%') - escapes
  st = find(fmtstr, '%')
  conv = fmtstr[0:st]
  k = 0
  for i in range(specs):
    en = st + 1
    if fmtstr[en] == '%':  # This is an escape sequence: '%%'.
      conv = conv + '%'
      en = en + 1
    else:
      while en < len(fmtstr) and find('diouxXeEfFgGcrs', fmtstr[en]) == -1:
        en = en + 1
      en = en + 1
      if k >= len(values):
        val = None
      else:
        val = values[k]
      conv = conv + format(fmtstr[st:en], val)
      k = k + 1
    j = find(fmtstr[en:], '%')
    if j != -1:
      st = en + j
    else:
      st = len(fmtstr)
    conv = conv + fmtstr[en:st]
  return conv

def format(fmt, value):
  """ Format a single value with ability to catch an error and reformat
  according to: 1) if value is None, then output a blank string of
  appropriate length 2) otherwise, format as Python object "s" of
  appropriate size.  It is assumed that the first character of fmt is %.
  Anything in the format string after the format specifier is ignored."""
  try:
    return fmt % value
  except:
    if value is None:
      value = ' '
    i = 1
    while i < len(fmt) and find(digits, fmt[i]) != -1:
      i = i + 1
    return (fmt[0:i] + 's') % value

  
