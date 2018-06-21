############################################################
#
#  FtProject.py - NEUROFIT, model data structure module
#  ============   Greg Ewing, Jan 2001
#
############################################################

import os
from string import digits, lower, lstrip, strip, split, rfind, join
import tkFileDialog
import tkMessageBox

import ABFInter
from ABFUtils import identify_abf_channel
import Busy
import LSInter
from ModelView import Model, Document
import PathUtils
import Units
from Validation import validate_number, INT, FLOAT, ValidationError

#----------------------------------------------------------
#
#  Project file version history
#
#  Version 1:               Initial version
#  Version 2:  14 Sep 2001  Added "channel" line to SweepSet
#  Version 3:  17 Oct 2001  Each SweepSet has its own Vhold
#  Version 4:  21 Nov 2001  Erev is a fitted parameter
#  Version 5:   3 Dec 2001  Units saved in project file
#  Version 6:   5 Dec 2001  Time of first sample saved
#  Version 7:   8 Nov 2002  Fit epoch saved for each sweep rather than first sample
#                           time for each sweep set. Custom axis limits saved.
#                           File units saved.
#
#----------------------------------------------------------
  
PROJECT_FILE_VERSION = 7
READABLE_PROJECT_FILE_VERSIONS = (1,2,3,4,5,6,7)

# Implemented model types

MODEL_TYPES = (1,2) # Hodgkin-Huxley model
model_type_names = ("Voltage-Clamp, Hodgkin-Huxley, Boltzmann Steady-State Curves, Independent Time Constants",
                    "Voltage-Clamp, Hodgkin-Huxley, Quadratic Theromodynamic Rate Functions")

# Experiment numbers

VARYING_STEP    = 0
VARYING_PRESTEP = 1

# List of all currently open projects

projects = []

def open_projects():
  return projects

def num_open_projects():
  return len(projects)

#----------------------------------------------------------
# Functions for the NFDataRegions
def calc_region_limits(items):
  """ Calculate the smallest region from a set of items with max_fit_regions."""
  if len(items) > 0:
    regions = []
    for item in items:
      regions.append(item.max_fit_region)
    min = None
    max = None
    for region in regions:
      rgmin, rgmax = region.get()
      if min is None or min < rgmin:
        min = rgmin
      if max is None or max > rgmax:
        max = rgmax
    return [min, max]

#----------------------------------------------------------
# Utility functions
def find_sample(val, y, type):
  """Do a binary search of the nondecreasing vector y which gives
       type=='ge'  - the smallest index, i, such that y[i] >= val
       type=='le'  - the largest index, i, such that y[i] <= val
       type=='min' - the smallest index, i, such that |y[i] - val| is a minimum
     The index 0 is returned if val < y[0].
     The index len(y)-1 is returned if val > y[len(y)-1]."""
  bot = 0
  top = len(y)-1
  if val < y[0] or (val == y[0] and type != 'le'):
    index = 0
  elif val > y[top] or (val == y[top] and type == 'le'):
    index = top
  else:
    while (top - bot) > 1:
      i = (top + bot)/2
      if val < y[i] or (val == y[i] and type != 'le'):
        top = i
      else:
        bot = i     
    if type == 'le':
      index = bot
    elif type == 'ge':
      index = top
    else:  # type == 'min'
      if abs(y[bot] - val) <= abs(y[top] - val):
        index = bot
      else:
        index = top
  return index

def magnitude_cmp(x,y):
  """Compare x and y by magnitude."""
  if abs(x) < abs(y):
    return -1
  elif abs(x) == abs(y):
    return 0
  else:
    return 1

#----------------------------------------------------------
  
class FtObject:
  """An object forming part of the parent-child hierarchy of
  an Ft project."""
  
  def traverse(self, method_name, args = ()):
    """
    Traverse parent-child hierarchy calling the named
    method wherever it exists.
    """
    method = getattr(self, method_name, None)
    if method:
      #print "FtObject.traverse: calling", method_name, "of", self ###
      apply(method, args)
    for child in self.descendants:
      if hasattr(child, 'traverse'):
        child.traverse(method_name, args)

#----------------------------------------------------------
  
class FtReader:
  """Scanner for .nfp files"""

  def __init__(self, file, filename):
    self.filename = filename
    self.file = file
    self.line_num = 0

  def read_line(self):
    """Read the next line not consisting entirely of whitespace
    or a comment. Leading and trailing whitespace and any trailing
    comment are stripped. Raises an error if EOF is reached."""
    while 1:
      self.line_num = self.line_num + 1
      line = self.file.readline()
      if not line:
        self.error("Unexpected end of file")
      c = rfind(line, '%')
      if c >= 0:
        line = line[:c]
      line = strip(line)
      if line:
        return line

  def error(self, mess):
    raise FormatError("'%s', line %d: %s" % 
                      (self.filename, self.line_num, mess))

  def read_header(self, expected_key, expected_value = None):
    """Look for a line of the form 'key: value' and return the
    value. Raises an error if the expected values don't match."""
    line = self.read_line()
    tokens = split(line)
    if len(tokens) != 2:
      self.error("Expected '%s value'" % expected_key)
    key = strip(tokens[0])
    value = strip(tokens[1])
    if key != expected_key:
      self.error("Expected '%s value'" % expected_key)
    if expected_value and value != str(expected_value):
      self.error("Expected '%s %s'" % (expected_key, expected_value))
    return value

  def read_labelled_value(self, expected_label = None):
    """Look for a line of the form 'label = value' and return
    the tuple (label, value). Raises and error if an expected
    label is specified and it does not match."""
    line = self.read_line()
    tokens = split(line, '=')
    if len(tokens) != 2:
      self.error("Expected 'label = value'")
    label = strip(tokens[0])
    value = strip(tokens[1])
    if expected_label and label != expected_label:
      self.error("Expected '%s'" % expected_label)
    return label, value

  def read_values(self, label, format):
    """Read a line of the form 'label = value ...' and convert the
    values according to the given format string."""
    _, values = self.read_labelled_value(label)
    return self.convert(values, format)

  def convert(self, value_string, format):
    """Convert a string of values according to a format string
    consisting of the letters 'i' for integer and 'f' for float.
    Raises an error if the formats don't match."""
    value_list = split(value_string)
    if len(value_list) != len(format):
      self.error("Wrong number of values (expected %d)" % len(format))
    result_list = []
    for i in xrange(len(value_list)):
      value = value_list[i]
      if value == 'None':
        result = None
      else:
        fmt = format[i]
        if fmt == 'i':
          func, descr = int, 'an integer'
        elif fmt == 'f':
          func, descr = float, 'a floating-point number'
        else:
          raise ValueError("Invalid format character '%s'" % fmt)
        try:
          result = func(value_list[i])
        except ValueError:
          self.error("Expected %s for item %d" % (i+1))
      result_list.append(result)
    if len(format) == 1:
      return result_list[0]
    else:
      return result_list

  def read_int(self, label):
    """Read an integer value with the given label."""
    return self.read_values(label, 'i')

  def read_float(self, label):
    """Read a floating point value with the given label."""
    return self.read_values(label, 'f')

  def read_string(self, label):
    """Read a quoted string with the given label."""
    _, value = self.read_labelled_value(label)
    if value[:1] != '"' or value[-1:] != '"':
      self.error("Expected a quoted string")
    return value[1:-1]

class FormatError(Exception):
  pass

#----------------------------------------------------------
  
class FtProject(Document, FtObject):
  """This class holds all the info related to one pair
  of experiments."""

  def __init__(self):
    Document.__init__(self)
    projects.append(self)
    # Measurement units
    self.units = {'Time' : Units.Unit(self, "Time", 's', None, projectunit=1),
                  'Current' : Units.Unit(self, "Current", 'A', None, projectunit=1),
                  'Voltage' : Units.Unit(self, "Voltage", 'V', None, projectunit=1)}
    self.units['Conductance'] = Units.QuotientUnit(self, "Conductance", 'S',
                        self.units['Current'], self.units['Voltage'])
    # Model parameters
    self.mtype = 1
    self.nh = 1
    self.nnonh = 0
    self.p = FtIntValue(self, 
      "p", "no. of independent activation gates", min = 0, initial = 1)
    # Numerical parameters
    self.noise_deg = FtIntValue(self, 
      "noise_deg", "noise polynomial degree", min = 0, max = 3, initial = 2)
    self.tol_f = FtFloatValue(self, 
      "tol_f", "function tolerance", min = 0.0, initial = 1e-6)
    self.tol_x = FtFloatValue(self, 
      "tol_x", "solution tolerance", min = 0.0, initial = 1e-6)
    self.tol_g = FtFloatValue(self, 
      "tol_g", "gradient tolerance", min = 0.0, initial = 1e-6)
    self.nevalfac = FtIntValue(self, 
      "nevalfac", "max function evals", min = 1, initial = 500)
    # Experiments
    self.experiments = [FtExperiment(self, VARYING_STEP), \
                        FtExperiment(self, VARYING_PRESTEP)]
    # Fitting parameters
    self.Erev = FtParameter(self, 
      "Erev", "reversal potential", unit = self.units['Voltage'])
    self.gmax = FtParameter(self, 
      "gmax", "maximum conductance", unit = self.units['Conductance'], 
      min = 0.0)
    self.f = [] # list of FtParameter
    self.dimension_f(self.nh + self.nnonh - 1)
    self.V2m = FtParameter(self, 
      "V2m", "half activation voltage", unit = self.units['Voltage'])
    self.sm = FtParameter(self, 
      "sm", "activation slope factor", max = 0.0, unit = self.units['Voltage'])
    self.V2h = FtParameter(self, 
      "V2h", "half inactivation voltage", unit = self.units['Voltage'])
    self.sh = FtParameter(self, 
      "sh", "inactivation slope factor", min = 0.0, unit = self.units['Voltage'])
    self.T = FtVoltageMapping() # voltage -> list of FtParameter: [Tm, Th1, Th2, ...]
    # Fitting results
    self.nfev = FtIntValue(self, 
      "no. of function evaluations", output = 1)
    self.njev = FtIntValue(self, 
      "no. of jacobian evaluations", output = 1)
    self.npts = FtIntValue(self, 
      "no. of points fitted", output = 1)
    self.nfitp = FtIntValue(self,
      "no. of fitted parameters", output = 1)
    self.variance = FtFloatValue(self,
      "noise scaled variance", output = 1)
    self.messages = [] # Messages from fitting routine

  def destroy(self):
    if self in projects:
      projects.remove(self)
    Document.destroy(self)

  def set_unknown_proj_units_to(self, new_units, keys):
    """Set any unknown project units to the new units values."""
    for key in keys:
      if self.units[key].scale is None and new_units[key].scale is not None:
        self.units[key].set_scale(new_units[key].scale)

  def scale_units(self, name, factor):
    """Scale the unit-ful data in the project to the new units."""
    self.changed()
    if name == 'Voltage':   # change the label and description of T params
      voltages = self.T.keys()
      voltages.sort(magnitude_cmp)
      if factor > 1.0:
        voltages.reverse()  # no chance of over-writing others
      for volt in voltages:
        newvolt = volt * factor
        for T_param in self.T[volt]:
          type = T_param.label[1] == 'h'
          label, description = self.make_T_param_label(type, newvolt)
          T_param.label = label
          T_param.description = description
        if not self.T.has_key(newvolt):  # won't happen if newvolt = volt as far as keys are concerned
          self.T[newvolt] = self.T[volt]
          del self.T[volt]
    for parm in self.get_scalar_params() + self.get_timeconst_params():
      parm.scale_units(name, factor)
    for exp in self.experiments:
      exp.scale_units(name, factor)
    self.notify_views('update_fitting_params')

  def get_scalar_params(self):
    """Return a list of all the non-array FtParameters."""
    result = [self.Erev, self.gmax, self.V2m, self.sm]
    if self.nh != 0:
      result.extend([self.V2h, self.sh])
    return result

  def get_f_params(self):
    """Return a list of the f-parameters."""
    return self.f

  def get_timeconst_params(self):
    """Return a list of all time constant FtParameters, sorted by
    subscript and then voltage."""
    result = []
    T = self.T
    voltages = T.keys()
    voltages.sort()
    for i in xrange(self.nh + 1):
      for v in voltages:
        result.append(T[v][i])
    return result

  def get_data_files(self, experiment_number):
    files = []
    for set in self.experiments[experiment_number].sweep_sets:
      files.append(set.path)
    return files

  def set_h_params(self, nh, nnonh):
    if self.nh != nh or self.nnonh != nnonh:
      self.nh = nh
      self.nnonh = nnonh
      self.changed()
      self.dimension_f(nh + nnonh - 1)
      self.dimension_T()
      self.notify_views('update_h_params')

  def set_data_files(self, file_lists):
    """Install data files in experiment. file_lists =
    [varying_step_files, varying_prestep_files] where each
    file list is a list of (filename, channel) pairs."""
    exp = self.experiments
    for i in (0, 1):
      exp[i].set_data_files(file_lists[i])

  def voltages_changed(self):
    """Called when any voltages in any of the experiments have been
    changed. (Not just scaled by changing the project units.)"""
    self.changed()
    voltages = []
    for set in self.experiments[VARYING_STEP].sweep_sets:
      for sweep in set.sweeps:
        voltages.append(sweep.Vsweep)
    for set in self.experiments[VARYING_PRESTEP].sweep_sets:
      voltages.append(set.Vhold)
    old_T = self.T
    new_T = FtVoltageMapping()
    for v in voltages:
      if v is not None:
        if not new_T.has_key(v):
          a = old_T.get(v)
          if not a:
            a = []
          new_T[v] = a
    self.T = new_T
    self.dimension_T()
    self.notify_views('update_fitting_params')
      
  def dimension_f(self, n):
    """Adjust dimension of f-parameter array."""
    f = self.f
    for p in f[n:]:
      p.destroy()
    del f[n:]
    while len(f) < n:
      m = len(f) + 1
      label = "f%d" % m
      description = "fraction of channels in group %d" % m
      f.append(FtParameter(self, label, description, min = 0.0, max = 1.0))

  def dimension_T(self):
    """Adjust the number of T-parameters for each voltage."""
    n = self.nh + 1
    for v, a in self.T.items():
      for p in a[n:]:
        p.destroy()
      del a[n:]
      while len(a) < n:
        a.append(self.make_T_param(len(a), v))

  def make_T_param(self, i, v):
    """Make a new time constant parameter."""
    label, description = self.make_T_param_label(i, v)
    return FtParameter(self, label, description, unit = self.units['Time'],
      min = 0.0)

  def make_T_param_label(self, i, v):
    """Make a new time constant parameter label."""
    if i == 0:
      subscript = "m"
      description = "activation time constant at %g" % v
    else:
      subscript = "h%d" % i
      description = "inactivation time constant for group %d at %g" % (i, v)
    label = "T%s(%g)" % (subscript, v)
    return (label, description)
  
  def get_lsdata(self):
    """Get data in tuple form for passing to least squares fitting routine."""
    timeconsts = self.T.items()
    voltage_to_index = FtVoltageMapping()
    for i in xrange(len(timeconsts)):
      voltage_to_index[timeconsts[i][0]] = i
    fitresults = (
      self.nfev.set,
      self.njev.set,
      self.npts.set,
      self.nfitp.set,
      self.variance.set,
      self.messages
    )
    return (
      self.get_modspecs(),
      self.get_algspecs(),
      self.get_dataspecs(voltage_to_index),
      self.get_parameters(timeconsts),
      fitresults
      )

  def get_modspecs(self):
    """Get 'struct modspecs' data."""
    return (
      self.mtype,
      self.nh,
      self.nnonh,
      self.p.value
      )

  def get_algspecs(self):
    """Get 'struct algspecs' data."""
    return (
      self.noise_deg.value,
      self.tol_f.value,
      self.tol_x.value,
      self.tol_g.value,
      self.nevalfac.value
      )

  def get_dataspecs(self, voltage_to_index):
    """Get 'struct dataspecs' data."""
    result = []
    for exp in self.experiments:
      exp.get_lsdata(voltage_to_index, result)
    return result

  def get_parameters(self, timeconsts):
    """Get 'struct parameters' data."""
    f_data = []
    for f_param in self.f:
      f_data.append(f_param.get_lsdata())
    v_data_list = []
    for v, tc_list in timeconsts:
      tc_data_list = []
      for tc in tc_list:
        tc_data_list.append(tc.get_lsdata())
      v_data = (
        tc_data_list[0], # Tm
        tc_data_list[1:], # Th1, Th2, ...
        v
        )
      v_data_list.append(v_data)
    enable_V2h_sh = self.nh != 0;
    if self.mtype == 1:
      parms = (
        [self.Erev.get_lsdata(),
        self.gmax.get_lsdata(),
        self.V2m.get_lsdata(),
        self.sm.get_lsdata(),
        self.V2h.get_lsdata(enable_V2h_sh),
        self.sh.get_lsdata(enable_V2h_sh)],
        f_data,
        v_data_list
        )
    elif self.mtype == 2:
      setparms = [self.Erev.get_lsdata(),
         self.gmax.get_lsdata(),
         self.V2m.get_lsdata(),
         self.sm.get_lsdata(),
         self.T[-40][0].get_lsdata(),
         self.T[-30][0].get_lsdata(),
         self.T[-20][0].get_lsdata(),
         self.T[-10][0].get_lsdata()]
      if self.nh > 0:
        setparms = setparms + [
           self.V2h.get_lsdata(),
           self.sh.get_lsdata(),
           self.T[-40][1].get_lsdata(),
           self.T[-30][1].get_lsdata(),
           self.T[-20][1].get_lsdata(),
           self.T[-10][1].get_lsdata()]
      if self.nh > 1:
        setparms = setparms + [
           self.T[0][1].get_lsdata(),
           self.T[10][1].get_lsdata(),
           self.T[20][1].get_lsdata()]
      if self.nh > 2:
        setparms = setparms + [
           self.T[-40][2].get_lsdata(),
           self.T[-30][2].get_lsdata(),
           self.T[-20][2].get_lsdata()]
      parms = (setparms, f_data, [])
    return parms

  def write_to_file(self, f):
    f.write("% NEUROFIT Project File. Do not edit.\n")
    f.write("Version = %d\n" % PROJECT_FILE_VERSION)
    f.write("% Model parameters\n")
    f.write("ModelType = %d\n" % self.mtype)
    f.write("nh = %s\n" % self.nh)
    f.write("nnonh = %s\n" % self.nnonh)
    self.p.write_to_file(f)
    self.noise_deg.write_to_file(f)
    self.tol_f.write_to_file(f)
    self.tol_x.write_to_file(f)
    self.tol_g.write_to_file(f)
    self.nevalfac.write_to_file(f)
    f.write("units = %s %s %s %% time current voltage\n" % 
      (self.units['Time'].scale, self.units['Current'].scale, self.units['Voltage'].scale))
    f.write("% Varying step potential experiment\n")
    self.experiments[VARYING_STEP].write_to_file(f)
    f.write("% Varying pre-step potential experiment\n")
    self.experiments[VARYING_PRESTEP].write_to_file(f)
    f.write("% Fitting parameters\n")
    f.write("% initial_value fitted_value fit?\n")
    self.Erev.write_to_file(f)
    self.gmax.write_to_file(f)
    for fi in self.f:
      fi.write_to_file(f)
    self.V2m.write_to_file(f)
    self.sm.write_to_file(f)
    self.V2h.write_to_file(f)
    self.sh.write_to_file(f)
    f.write("% Time constants\n")
    self.write_time_constants(f)

  def read_from_file(self, f):
    r = FtReader(f, self.filename)
    version = r.read_int('Version')
    if version not in READABLE_PROJECT_FILE_VERSIONS:
      r.error("Found a version %d project file. This version of NEUROFIT "
        "is only able to read the following project file versions: %s" %
          (version, 
           ", ".join([str(i) for i in READABLE_PROJECT_FILE_VERSIONS])))
    r.project_file_version = version
    self.mtype = r.read_int('ModelType')
    if self.mtype not in MODEL_TYPES:
      r.error("Unknown model type %d" % self.mtype)
    nh = r.read_int('nh')
    nnonh = r.read_int('nnonh')
    self.set_h_params(nh, nnonh)
    self.p.read_from(r)
    if version <= 3:
      self.Erev.initial_value.value = r.read_float('Erev')
    self.noise_deg.read_from(r)
    self.tol_f.read_from(r)
    self.tol_x.read_from(r)
    self.tol_g.read_from(r)
    self.nevalfac.read_from(r)
    if version >= 5:
      t, i, v = r.read_values("units", "iii")
      if version < 7:  # order before version 7 was time, voltage, current
        tmp = i
        i = v
        v = tmp
      self.units['Time'].set_scale(t)
      self.units['Current'].set_scale(i)
      self.units['Voltage'].set_scale(v)
    self.experiments[VARYING_STEP].read_from(r)
    self.experiments[VARYING_PRESTEP].read_from(r)
    if version >=4:
      self.Erev.read_from(r)
    self.gmax.read_from(r)
    for fi in self.f:
      fi.read_from(r)
    self.V2m.read_from(r)
    self.sm.read_from(r)
    self.V2h.read_from(r)
    self.sh.read_from(r)
    self.read_time_constants(r)

  def write_time_constants(self, f):
    voltages = self.T.keys()
    voltages.sort()
    f.write("nVoltages = %d\n" % len(voltages))
    for v in voltages:
      f.write("Voltage: %s\n" % v)
      tc_list = self.T[v]
      for i in xrange(len(tc_list)):
        tc_list[i].write_to_file(f)

  def read_time_constants(self, r):
    self.T = FtVoltageMapping()
    nT = self.nh + 1
    nVoltages = r.read_int('nVoltages')
    for i in xrange(nVoltages):
      value = r.read_header('Voltage:')
      v = r.convert(value, 'f')
      tc_list = []
      self.T[v] = tc_list
      for i in xrange(nT):
        tc = self.make_T_param(i, v)
        tc_list.append(tc)
        tc.read_from(r)
  
  def guess_parameters(self):
    """
    Try to guess initial values of parameters.
    """
    try:
      self.traverse('validate', ('quick',))
    except ValidationError, e:
      e.report()
      return
    Busy.with_busy_cursor(self.do_quick_fitting)
  
  def do_quick_fitting(self):
    lsdata = self.get_lsdata()
    del self.messages[:]
    self.notify_views('update_before_fitting')
    LSInter.guess(lsdata)
    self.notify_views('update_after_guessing')
  
  def fit(self):
    """
    Fit curves to the data using the given initial
    parameter values.
    """
    try:
      self.traverse('validate', ('full',))
    except ValidationError, e:
      e.report()
      return
    Busy.with_busy_cursor(self.do_fitting)
  
  def do_fitting(self):
    lsdata = self.get_lsdata()
    del self.messages[:]
    self.notify_views('update_before_fitting')
    ##print "FtProject.fit: lsdata:" ###
    ##pp(lsdata) ###
    LSInter.fit(lsdata)
    self.notify_views('update_after_fitting')

  def validate(self, type):
    """
    Validate various FtValues.
    """
    self.p.validate_value()
    self.noise_deg.validate_value()
    self.tol_x.validate_value()
    self.tol_f.validate_value()
    self.tol_g.validate_value()
    self.nevalfac.validate_value()
    
  def get_units(self):
    """Return sequence of Unit objects for basic measurement units."""
    return [self.units['Time'], self.units['Current'], self.units['Voltage']]
  
def pp(stuff, indent = ""):
  """
  Debugging function for pretty-printing lsdata.
  """
  if isinstance(stuff, type(())):
    print "%s(" % indent
    for item in stuff:
      pp(item, indent + "   ")
    print "%s)" % indent
  elif isinstance(stuff, type([])):
    if len(stuff) > 10:
      print "%s%s" % (indent, stuff[:5]), "..."
    else:
      print "%s[" % indent
      for item in stuff:
        pp(item, indent + "   ")
      print "%s]" % indent
  else:
    print "%s%s" % (indent, stuff)
   
#----------------------------------------------------------

class FtExperiment(Model, FtObject):
  """This class holds the data from a set of files belonging to
  one experiment (Varying Step or Varying Pre-step)."""

  def __init__(self, project, experiment_number):
    Model.__init__(self, project)
    self.experiment_number = experiment_number
    self.sweep_sets = [] # List of FtSweepSet
    self.default_fit_region = NFDataRegion(self, None)
    self.default_noise_region = NFDataRegion(self, None)
    self.max_fit_region = NFDataRegion(self, None, None)
    self.fit_region = NFDataRegion(self, self.default_fit_region, self.max_fit_region)
    self.noise_region = NFDataRegion(self, self.default_noise_region, self.max_fit_region)
    self.regions = [self.fit_region, self.noise_region]
    self.all_regions = [self.default_fit_region, self.default_noise_region,\
                        self.max_fit_region] + self.regions
    zeros = ((0.0, 0.0), (0.0, 0.0))
    self.data_range = {'all': zeros, 'step': zeros}
    self.custom_axis_limits_to_transfer = None

  def project(self):
    return self.parent
    
  def set_data_files(self, files):
    """
    Install data from the given set of files, replacing
    any existing data. The files list is a list of (filename,
    channel) pairs. Units determined from the files are merged
    with the given project units.
    """
    self.destroy_sweep_sets()
    for (path, channel) in files:
      abffile = ABFInter.is_abf_file(path)
      set = FtSweepSet(self, path, channel, abffile)
      set.load_data()
      set.merge_units()
      self.sweep_sets.append(set)
    # calculate the region limits
    self.max_fit_region.set(calc_region_limits(self.sweep_sets))
    # set the default regions
    self.calc_default_regions()
    self.update_ranges()
    self.notify_views()
    self.voltages_changed()

  def scale_units(self, name, factor):
    """Scale the unit-ful data in the experiment to the new units."""
    for sweep_set in self.sweep_sets:
      sweep_set.scale_units(name, factor)
    if name == 'Time':
      for region in self.all_regions:
        region.scale_units(factor)
      for key in self.data_range.keys():
        r = self.data_range[key]
        self.data_range[key] = ((r[0][0] * factor, r[0][1] * factor), r[1])
      self.notify_views('scale_axis_limits', (name, factor))
    elif name == 'Current':
      for key in self.data_range.keys():
        r = self.data_range[key]
        self.data_range[key] = (r[0], (r[1][0] * factor, r[1][1] * factor))
      self.notify_views('scale_axis_limits', (name, factor))
    elif name == 'Voltage':  # updates done by sweep_set.scale_units
      self.notify_views('voltages_changed')
    self.notify_views()

  def define_step_epoch(self, t, target, end):
    """
    Offset time values so that what is currently time t
    becomes time 0.
    """
    if target is self:  # do for all sweep sets
      for set in self.sweep_sets:
        set.define_step_epoch(t, None, end)
    elif target in self.sweep_sets:  # do for this sweep set only
      self.sweep_sets[self.sweep_sets.index(target)].define_step_epoch(t, None, end)
    else:  #do for specified sweep only
      for set in self.sweep_sets:
        if target in set.sweeps:
          set.define_step_epoch(t, target, end)
          break
    self.max_fit_region.set(calc_region_limits(self.sweep_sets))
    for region in self.regions:
      if region.is_set():
        region.set(region.get())
    self.calc_default_regions()
    self.update_ranges()
    self.notify_views('update_after_step_epoch_change')

  def calc_default_regions(self):
    # set the default fit region to the maximum allowed and the default
    # noise region to the last tenth or so of this
    if self.max_fit_region is not None:
      low, high = self.max_fit_region.get()
      self.default_fit_region.set([low, high])
      self.default_noise_region.set([(low+9.0*high)/10.0, high])
                                      
  def destroy_sweep_sets(self):
    for set in self.sweep_sets:
      set.destroy()
    self.sweep_sets = []

  def dump_data(self):
    print "Experiment", self.experiment_number
    for i in xrange(len(self.sweep_sets)):
      print "Sweep set %d:" % i
      self.sweep_sets[i].dump_data()

  def update_ranges(self):
    """
    Find minimum and maximum time and data values over all sweeps, for all the
    data and for step epoch only.
    """
    tmin = None
    tmax = None
    tmaxstep = None
    for set in self.sweep_sets:
      if set.times:
        for sweep in set.sweeps:
          origin = set.times[sweep.step_epoch_ends[0]]
          t0 = set.times[0] - origin
          t1 = set.times[-1] - origin
          t2 = set.times[sweep.step_epoch_ends[1]] - origin
          if tmin is None or t0 < tmin:
            tmin = t0
          if tmax is None or t1 > tmax:
            tmax = t1
          if tmaxstep is None or t2 > tmaxstep:
            tmaxstep = t2
    if tmin is None:
      tmin = 0.0
    if tmax is None:
      tmax = 0.0
    if tmaxstep is None:
      tmaxstep = 0.0

    # Include fitted_data as well?
    ymin = None
    yminstep = None
    ymax = None
    ymaxstep = None
    for set in self.sweep_sets:
      for sweep in set.sweeps:
        for y in sweep.sweep_data[sweep.step_epoch_ends[0]:sweep.step_epoch_ends[1]+1]:
          if yminstep is None or y < yminstep:
            yminstep = y
          if ymaxstep is None or y > ymaxstep:
            ymaxstep = y
        for y in sweep.sweep_data[0:sweep.step_epoch_ends[0]] + \
                 sweep.sweep_data[sweep.step_epoch_ends[1]+1:len(sweep.sweep_data)]:
          if ymin is None or y < ymin:
            ymin = y
          if ymax is None or y > ymax:
            ymax = y
    if ymin is None:
      ymin = 0.0
    if ymax is None:
      ymax = 0.0
    if yminstep is None:
      yminstep = 0.0
    if ymaxstep is None:
      ymaxstep = 0.0
    ymin = min(ymin, yminstep)
    ymax = max(ymax, ymaxstep)
    self.data_range['all'] = ((tmin, tmax), (ymin, ymax))
    self.data_range['step'] = ((0.0, tmaxstep), (yminstep, ymaxstep))

  def voltages_changed(self):
    """Called when any of the voltages in this experiment have
    been changed."""
    self.notify_views('voltages_changed')
    self.project().voltages_changed()

  def get_lsdata(self, voltage_to_index, result):
    """Get 'struct sweep' data."""
    expnum = self.experiment_number
    for sweep_set in self.sweep_sets:
      sweep_set.get_lsdata(expnum, voltage_to_index, result)

  def write_to_file(self, f):
    nsets = len(self.sweep_sets)
    f.write("nSweepSets = %d\n" % nsets)
    for i in xrange(nsets):
      f.write("SweepSet: %d\n" % i)
      self.sweep_sets[i].write_to_file(f)
    f.write("% Custom axis settings\n")
    if self.views[0] is None or self.views[0].axis_limits['custom'] is None:
      f.write("CustomAxisLimits_x = None\n")
      f.write("CustomAxisLimits_y = None\n")
    else:
      let = ('x', 'y')
      for i in range(0,2):
        f.write("CustomAxisLimits_%c = " % let[i])
        f.write("%f %f\n" % self.views[0].axis_limits['custom'][i])
    f.write("% specified data regions for experiment\n")
    f.write("fitRegion = ")
    self.fit_region.write_to_file(f)
    f.write("noiseRegion = ")
    self.noise_region.write_to_file(f)

  def read_from(self, r):
    if r.project_file_version < 7:
      self.noise_region.read_from(r, 'noiseWindow')
      self.fit_region.read_from(r, 'fitWindow')
    nSweepSets = r.read_int('nSweepSets')
    self.sweep_sets = []
    for i in xrange(nSweepSets):
      set = FtSweepSet(self, None, None, None)
      self.sweep_sets.append(set)
      r.read_header('SweepSet:', i)
      set.read_from(r)
    self.max_fit_region.set(calc_region_limits(self.sweep_sets))
    self.calc_default_regions()
    self.update_ranges()
    if r.project_file_version >= 7:
      let = ('x', 'y')
      customlimits = []
      for i in range(0,2):
        _, value = r.read_labelled_value('CustomAxisLimits_' + let[i])
        if value == 'None':
          customlimits.append(None)
        else:
          customlimits.append(tuple(r.convert(value, 'ff')))
      if None in customlimits:
        customlimits = None
      else:
        customlimits = tuple(customlimits)
    else:
      customlimits = None
    self.custom_axis_limits_to_transfer = customlimits
    if r.project_file_version >= 7:
      self.fit_region.read_from(r, 'fitRegion')
      self.noise_region.read_from(r, 'noiseRegion')
    else:  # re-set these old version values to ensure they are within limits
      self.noise_region.set(self.noise_region.get())
      self.fit_region.set(self.fit_region.get())

#----------------------------------------------------------

class FtSweepSet(Model, FtObject):
  """This class holds the sweep data from a single file,
  plus other info relating to that file."""

  def __init__(self, experiment, pathname, channel, abffile):
    Model.__init__(self, experiment)
    self.path = pathname # File from which the data comes
    self.channel = channel # Channel number (ABF files only)
    self.isabffile = abffile # 1 if data is from an ABF file, 0 for text file
    self.Vhold = None    # Voltage which is constant for all sweeps
    self.times = [] # Time values for the sweeps in this sweep set
    self.sweeps = [] # List of FtSweep
    self.max_fit_region = NFDataRegion(self, None, None)
    self.fit_region = NFDataRegion(self, experiment.fit_region, self.max_fit_region)
    self.noise_region = NFDataRegion(self, experiment.noise_region, self.max_fit_region)
    self.regions = [self.fit_region, self.noise_region]
    self.all_regions = [self.max_fit_region] + self.regions
    self.units = {'Time' : Units.Unit(self, "Time", "s", None),
                  'Current': Units.Unit(self, "Current", "A", None),
                  'Voltage' : Units.Unit(self, "Voltage", "V", None)}
  
  def validate(self, type):
    """
    Validate in preparation for a Quick or Full Fit.
    Ensure that hold voltage for this sweep set is specified.
    """
    if self.Vhold is None:
      raise ValidationError("%s voltage required for '%s'."
        % (("Prestep", "Step")[self.parent.experiment_number], self.get_filename()))
  
  def get_filename(self):
    """
    Return last component of pathname.
    """
    return os.path.basename(self.path)
  
  def sweep_number(self, sweep):
    """
    Return 1-based index of given sweep in this sweep set.
    """
    return self.sweeps.index(sweep) + 1

  def define_step_epoch(self, t, target, end):
    """
    Define the step epoch.
    """
    if not self.isabffile:
      if target is None:
        sweeps = self.sweeps   # do for all sweeps
      else:
        sweeps = [target]  # target is just one sweep
      for sweep in sweeps:
        sweep.step_epoch_ends[end] = find_sample(t + self.times[sweep.step_epoch_ends[0]], self.times, 'min')
        if sweep.step_epoch_ends[0] > sweep.step_epoch_ends[1]:
          sweep.step_epoch_ends[end-1] = sweep.step_epoch_ends[end]
        sweep.max_fit_region.set([0.0, self.times[sweep.step_epoch_ends[1]] - self.times[sweep.step_epoch_ends[0]]])
        for region in sweep.regions:  # ensure regions are within new limits
          if region.is_set():
            region.set(region.get())
      # calculate the region limits for the sweep set based on those of the sweeps
      self.max_fit_region.set(calc_region_limits(self.sweeps))
      for regions in self.regions:   # ensure regions are within new limits
        if region.is_set():
          region.set(region.get())

  def merge_units(self):
    """
    Merge units determined for this file with those
    determined for the project from other files. If units cannot
    be determined, ask the user.  If project units aren't set, use
    these file units.
    """
    # Get any units from the user that weren't specified by the file.
    # ABF files should specify all units, text files need just time and
    # current at this point.  If units need to be specified, choose as
    # default the project units if they exist.
    proj_units = self.parent.parent.units
    if self.isabffile:
      keys = self.units.keys()
    else:
      keys = ['Time', 'Current']
    Units.get_missing_units_from_user(self.units, keys, proj_units,
                                      os.path.basename(self.path))
    # If any of the project units aren't specified, use the file units.
    self.parent.parent.set_unknown_proj_units_to(self.units, keys)
    # Now scale all data in this file to coincide with project units.
    for key in keys:
      if self.units[key].scale != proj_units[key].scale:
        factor = pow(10, self.units[key].scale - proj_units[key].scale)
        self.scale_units_for_file_data(key, factor)

  def scale_units_for_file_data(self, name, factor):
    """Scale the file-associated data to the new units."""
    if name == 'Time':
      for i in xrange(len(self.times)):
        self.times[i] = self.times[i] * factor
    elif name == 'Current':
      for sweep in self.sweeps:
        for i in xrange(len(sweep.sweep_data)):
          sweep.sweep_data[i] = sweep.sweep_data[i] * factor
    elif name == 'Voltage':
      if self.Vhold is not None:
        self.Vhold = self.Vhold * factor
      for sweep in self.sweeps:
        if sweep.Vsweep is not None:
          sweep.Vsweep = sweep.Vsweep * factor

  def scale_units(self, name, factor):
    """Scale the unit-ful data in the sweep_set to the new units."""
    if name == 'Time':
      for region in self.all_regions:
        region.scale_units(factor)
    for sweep in self.sweeps:
      sweep.scale_units(name, factor)
    self.scale_units_for_file_data(name, factor)

  def load_data(self):
    """Read data from attached file."""
    #print "FtSweepSet.load_data", repr(self.path) ###
    if self.isabffile:
      self.read_abf_file()
    else:
      f = open(self.path, "r")
      try:
        self.read_text_file(f)
      finally:
        f.close()

  def read_abf_file(self):
    """
    Read time/sweep data and voltages from ABF file, and locate
    the start and end of the step epoch.
    """
    # Read the data
    if self.channel is not None:
      self.Vhold = None
      time_data, sweep_info_and_data = ABFInter.read_abf_file(self.path, self.channel,
                                       self.parent.experiment_number)
    else:
      time_data, sweep_info_and_data = [], []
    self.times = time_data
    for sweep_i_and_d in sweep_info_and_data:
      sweep = FtSweep(self)
      epoch_start, epoch_end, step_epoch_V, prev_epoch_V, sweep.sweep_data = sweep_i_and_d
      sweep.step_epoch_ends = [epoch_start, epoch_end]
      sweep.max_fit_region.set([0.0, self.times[epoch_end] - self.times[epoch_start]])
      if self.parent.experiment_number == VARYING_STEP:
        sweep.Vsweep = step_epoch_V
        self.Vhold = prev_epoch_V
      else:  # VARYING PRESTEP - previous epoch is the varying one
        sweep.Vsweep = prev_epoch_V
        if self.Vhold is None:
          self.Vhold = step_epoch_V
        else:
          if self.Vhold != step_epoch_V:
            tkMessageBox.showwarning("NEUROFIT",
               "Warning: holding potentials for varying prestep experiments are not all identical")
      self.sweeps.append(sweep)
    # calculate the limiting region for the sweep set based on those of the sweeps
    self.max_fit_region.set(calc_region_limits(self.sweeps))
    # Get units and epoch info
    channel_info, dac_unit_name = ABFInter.get_abf_info(self.path)
    voltage_unit_name = dac_unit_name.strip()
    # Figure out measurement units
    current_unit_name = ""
    for chan_num, _, chan_unit_name in channel_info:
      if chan_num == self.channel:
        current_unit_name = chan_unit_name.strip()
        break
    current_unit_scale = Units.prefix_to_scale(current_unit_name[:1])
    voltage_unit_scale = Units.prefix_to_scale(voltage_unit_name[:1])
    self.units['Current'].set_scale(current_unit_scale)
    self.units['Voltage'].set_scale(voltage_unit_scale)
    # Time returned by ABFInter is in ms.
    self.units['Time'].set_scale(-3)
  
  def read_text_file(self, f):
    self.sweeps = []
    sweeps = self.sweeps
    line = f.readline()
    linenum = 1
    # Skip header lines which don't look like data
    while line and lstrip(line)[:1] not in digits:
      #print "Skipping:", line ###
      line = f.readline()
      linenum = linenum + 1
    while line:
      #print "Processing:", line ###
      items = split(line)
      if not items:
        continue # Ignore empty lines
      values = []
      for item in items:
        try:
          value = float(item)
        except ValueError:
          raise DataFileError(self.path, linenum, "Invalid number: %s" % item)
        values.append(value)
      time_value = values[0]
      data_values = values[1:]
      num_data_values = len(data_values)
      self.times.append(time_value)
      if not sweeps:
        for i in xrange(num_data_values):
          sweeps.append(FtSweep(self))
      else:
        if num_data_values != len(sweeps):
          raise DataFileError(self.path, linenum, "Wrong number of data items")
      #print "Appending to sweeps" ###
      for i in xrange(num_data_values):
        sweeps[i].sweep_data.append(data_values[i])
      line = f.readline()
      linenum = linenum + 1
    # set the limits for the various data regions
    for sweep in self.sweeps:
      sweep.step_epoch_ends = [0, len(self.times) - 1]
      sweep.max_fit_region.set([self.times[0], self.times[-1]])
    self.max_fit_region.set([self.times[0], self.times[-1]])
    return sweeps

  def dump_data(self):
    for i in xrange(len(self.sweeps)):
      print "Sweep %d:" % i, self.sweeps[i].sweep_data

  def get_lsdata(self, expnum, voltage_to_index, result):
    """Get 'struct sweep' data."""
    for sweep in self.sweeps:
      sweep.get_lsdata(expnum, self.Vhold, voltage_to_index, 
        self.times, result)

  def project_dir(self):
    project_path = self.parent.parent.get_pathname()
    return os.path.dirname(project_path)
  
  def write_to_file(self, f):
    proj_dir = self.project_dir()
    rel_path = PathUtils.relative_path(proj_dir, self.path)
    f.write('file = "%s"\n' % rel_path)
    f.write('channel = %s\n' % self.channel)
    f.write("fileUnits = %s %s %s %% time current voltage\n" % 
      (self.units['Time'].scale, self.units['Current'].scale, self.units['Voltage'].scale))
    # The hold and sweep voltages are the only thing in the nfp file stored in
    # file units.  Note, the time parameter names use voltages in project units.
    if self.Vhold is None:
      f.write("Vhold = None\n")
    else:
      proj_units = self.parent.parent.units
      if proj_units['Voltage'].scale is not None:
        factor = pow(10, proj_units['Voltage'].scale - self.units['Voltage'].scale)
      else:
        factor = 1.0
      f.write("Vhold = %s\n" % (self.Vhold * factor))
    f.write("% specified data regions for sweep set\n")
    f.write("fitRegion = ")
    self.fit_region.write_to_file(f)
    f.write("noiseRegion = ")
    self.noise_region.write_to_file(f)
    nsweeps = len(self.sweeps)
    f.write("nSweeps = %d\n" % nsweeps)
    for i in xrange(nsweeps):
      f.write("Sweep: %d\n" % i)
      self.sweeps[i].write_to_file(f)

  def read_from(self, r):
    rel_path = r.read_string('file')
    proj_dir = self.project_dir()
    self.path = os.path.join(proj_dir, rel_path)
    self.look_for_path()
    self.isabffile = ABFInter.is_abf_file(self.path)
    if r.project_file_version >= 2:
      self.channel = r.read_int('channel')
    else:
      self.channel = None
    if self.channel is None and self.isabffile:
      self.channel = identify_abf_channel(self.path)
      self.changed_while_opening()
    self.load_data()
    if r.project_file_version >= 7:
      t, i, v = r.read_values("fileUnits", "iii")
      self.units['Time'].set_scale(t)
      self.units['Current'].set_scale(i)
      self.units['Voltage'].set_scale(v)
    else:  # load_data will have got user to enter time and current units, set voltage units here
      self.units['Voltage'].set_scale(self.parent.parent.units['Voltage'].scale)
    if r.project_file_version >= 3:
      self.Vhold = r.read_float('Vhold')
    if r.project_file_version == 6:
      t0 = r.read_float('first_sample_time')
      if self.times is not None and not self.isabffile:
        start = find_sample(self.times[0] - t0, self.times, 'min')
        for sweep in self.sweeps:
          sweep.step_epoch_ends = [start, len(self.times) - 1]
    if r.project_file_version < 7:
      self.noise_region.read_from(r, 'noiseWindow')
      self.fit_region.read_from(r, 'fitWindow')
    else:
      self.fit_region.read_from(r, 'fitRegion')
      self.noise_region.read_from(r, 'noiseRegion')
    nSweeps = r.read_int('nSweeps')
    if nSweeps != len(self.sweeps):
      tkMessageBox.showwarning(
        "NEUROFIT",
        "Warning: Data file '%s' has wrong number of sweeps " \
        "(expected %d, got %d)" %
        (self.path, len(self.sweeps), nSweeps))
    for i in xrange(nSweeps):
      r.read_header('Sweep:', i)
      if i < len(self.sweeps):
        sweep = self.sweeps[i]
      else:
        sweep = FtSweep(None) # dummy sweep set
      sweep.read_from(r)
      if not self.isabffile and self.times is not None:
        sweep.step_epoch_ends[0] = max(sweep.step_epoch_ends[0], 0)
        sweep.step_epoch_ends[1] = min(sweep.step_epoch_ends[1], len(self.times)-1)
        sweep.max_fit_region.set([0.0, self.times[sweep.step_epoch_ends[1]] - \
                                       self.times[sweep.step_epoch_ends[0]]])
    if not self.isabffile:
      self.max_fit_region.set(calc_region_limits(self.sweeps))
    self.merge_units()
  
  def look_for_path(self):
    """If the data file can't be found using the path loaded 
    from the project file, ask the user to locate it."""
    if not os.path.exists(self.path):
      reply = tkMessageBox.askquestion(
        message = 'Unable to find "%s". Do you want '
        'to look for it?' % self.path)
      if reply == 'yes':
        name = os.path.basename(self.path)
        path = tkFileDialog.askopenfilename(
          title = 'Where is "%s"?' % name,
          filetypes = [(name, name), ("All Files", "*")])
        if path:
          self.path = path
          self.changed_while_opening()

#----------------------------------------------------------

class FtSweep(Model, FtObject):
  """This class holds the data for a single sweep, plus
  other info related to that sweep."""

  def __init__(self, sweep_set):
    Model.__init__(self, sweep_set)
    self.Vsweep = None # Voltage for this sweep
    self.sweep_data = [] # Data from the file
    self.step_epoch_ends = None
    self.max_fit_region = NFDataRegion(self, None, None)
    self.fit_region = NFDataRegion(self, sweep_set.fit_region, self.max_fit_region)
    self.noise_region = NFDataRegion(self, sweep_set.noise_region, self.max_fit_region)
    self.regions = [self.fit_region, self.noise_region]
    self.all_regions = [self.max_fit_region] + self.regions
    self.fitted_data = [] # Values from evaluating the fitted function
    self.noise = FtFloatValue(self, "noise", "noise level", output = 1)
    self.varcont = FtFloatValue(
      self, "varcont", "percent contribution to variance", output = 1)
    self.fitting_flag = 1 # True if this sweep is to be fitted

  def scale_units(self, name, factor):
    """Scale the unit-ful data in the sweep to the new units."""
    # sweep_data and Vsweep are scaled from sweep_set.scale_units.
    if name == 'Time':
      for region in self.all_regions:
        region.scale_units(factor)
    if name == 'Current':
      for i in xrange(len(self.fitted_data)):
        self.fitted_data[i] = self.fitted_data[i] * factor
      if self.noise.value is not None:
        self.noise.set(self.noise.value * factor)
        
  def set_fitting_flag(self, x):
    self.fitting_flag = x
    self.changed()
    self.notify_views('fitting_flag_changed', (self.fitting_flag,))
  
  def validate(self, type):
    """
    Validate in preparation for a Quick or Full Fit.
    Ensure that voltage is specified for this sweep.
    """
    if self.Vsweep is None:
      if self.Vsweep is None:
        raise ValidationError("Voltage required for sweep %d of '%s'."
          % (self.parent.sweep_number(self), self.parent.get_filename()))

  def get_lsdata(self, 
      expnum, Vhold, voltage_to_index, times, result):
    """Get 'struct sweep' data."""
    if self.fitting_flag:
      if expnum == VARYING_STEP:
        prestep = Vhold
        step = self.Vsweep
      else:
        prestep = self.Vsweep
        step = Vhold
      Vpro = (
        expnum,
        prestep, 
        step, 
        voltage_to_index[step])
      shifted_times = times[self.step_epoch_ends[0]:self.step_epoch_ends[1]+1]
      for i in xrange(len(shifted_times)):
        shifted_times[i] = shifted_times[i] - times[self.step_epoch_ends[0]]
      reg_range = self.fit_region.get()
      loc = [find_sample(reg_range[0], shifted_times, 'ge'),
             find_sample(reg_range[1], shifted_times, 'le')]
      fit_win = (loc[0], loc[1]-loc[0]+1)
      reg_range = self.noise_region.get()
      loc = [find_sample(reg_range[0], shifted_times, 'ge'),
             find_sample(reg_range[1], shifted_times, 'le')]
      noise_win = (loc[0], loc[1]-loc[0]+1)
      result.append((
        Vpro,
        shifted_times,
        self.sweep_data[self.step_epoch_ends[0]:self.step_epoch_ends[1]+1],
        fit_win,
        noise_win,
        self.noise.set,   # OUTPUT: noise level
        self.varcont.set, # OUTPUT: percent contribution to variance
        self.fitted_data  # OUTPUT: func eval results
        ))
    else:
      self.noise.set('')
      self.varcont.set('')

  def write_to_file(self, f):
    proj_units = self.parent.parent.parent.units
    if self.Vsweep is None:
      f.write("Vsweep = None\n")
    else:
      # Voltages are written in file units.
      if proj_units['Voltage'].scale is not None:
        factor = pow(10, proj_units['Voltage'].scale - self.parent.units['Voltage'].scale)
      else:
        factor = 1.0
      f.write("Vsweep = %s\n" % (self.Vsweep * factor))
    f.write("step_epoch_ends = %i %i\n" % (self.step_epoch_ends[0], self.step_epoch_ends[1]))
    f.write("fit = %i\n" % self.fitting_flag)
    f.write("fitRegion = ")
    self.fit_region.write_to_file(f)
    f.write("noiseRegion = ")
    self.noise_region.write_to_file(f)

  def read_from(self, r):
    _, value = r.read_labelled_value('Vsweep')
    if value == 'None':
      self.Vsweep = None
    else:
      self.Vsweep = r.convert(value, 'f')
    if r.project_file_version >= 7:
      self.step_epoch_ends = r.read_values('step_epoch_ends', 'ii')
    if r.project_file_version >= 3:
      self.fitting_flag = r.read_int('fit')
    if r.project_file_version < 7:
      self.noise_region.read_from(r, 'noiseWindow')
      self.fit_region.read_from(r, 'fitWindow')
    else:
      self.fit_region.read_from(r, 'fitRegion')
      self.noise_region.read_from(r, 'noiseRegion')

#----------------------------------------------------------

class NFDataRegion(Model, FtObject):
  """ Data region specified by a range of values with support for a limiting region
  and a default region."""

  def __init__(self, parent, default_region=None, limiting_region=None):
    Model.__init__(self, parent)
    self.default_region = default_region
    self.limiting_region = limiting_region
    if default_region is not None:
      self.range = None
    else:
      self.range = [0.0, 0.0]

  def is_set(self):
    return self.range is not None

  def set(self, range):
    if range is not None:
      self[0] = range[0]
      self[1] = range[1]

  def get(self):
    if self.range is not None:
      return self.range
    else:
      return self.default_region.get()

  def __setitem__(self, i, x):
    if self.range is None:  # the range can only be None if a default exists
      self.range = self.default_region.get()[:]
    if self.limiting_region is not None:
      min, max = self.limiting_region.get()
      if x < min:
        x = min
      if x > max:
        x = max
    self.range[i] = x
    self.notify_views()
    self.changed()
    
  def __getitem__(self, i):
    return self.get()[i]

  def revert(self):
    if self.default_region:
      self.range = None
      self.notify_views()
      self.changed()

  def scale_units(self, factor):
    if self.range is not None:
      for i in range(2):
        self.range[i] = self.range[i] * factor

  def write_to_file(self, f):
    if self.is_set():
      low, high = self.range
      f.write("%g %g\n" % (low, high))
    else:
      f.write("None\n")

  def read_from(self, r, label):
    _, value = r.read_labelled_value(label)
    if value == 'None':
      self.range = None
    else:
      self.set(r.convert(value, 'ff'))

#----------------------------------------------------------

class FtParameter(Model, FtObject):
  """This class holds the initial value, fitted value,
  standard error and fitting flag of a fitting parameter."""

  def __init__(self, parent, label, description, unit = None,
               initial = None, min = None, max = None):
    Model.__init__(self, parent)
    self.label = label 
    self.description = description
    self.unit = unit
    self.initial_value = FtFloatValue(
      self, "", description or label, initial, min, max)
    self.fitted_value = FtFloatValue(self, output = 1)
    self.standard_error = FtFloatValue(self, output = 1)
    self.fitting_flag = 1
    self.constraint = None
  
  def set_initial_value(self, x):
    self.initial_value.set(x)
    self.changed()

  def set_fitting_flag(self, x):
    if self.fitting_flag != x:
      self.fitting_flag = x
      self.changed()

  def set_fitted_value(self, x):
    self.fitted_value.set(x)

  def set_standard_error(self, x):
    self.standard_error.set(x)

  def set_constraint(self, x):
    self.constraint = x

  def scale_units(self, name, factor):
    if name == self.unit.quantity_name:
      if self.initial_value.value is not None:
        self.set_initial_value(self.initial_value.value * factor)
      if self.fitted_value.value is not None:
        self.set_fitted_value(self.fitted_value.value * factor)
      if self.standard_error.value is not None:
        self.set_standard_error(self.standard_error.value * factor)

  def changed(self):
    Model.changed(self)
    self.notify_views()

  def validate(self, type):
    """
    Validate in preparation for a Quick or Full Fit.
    """
    if (not (self.parent.nh == 0 and (self.label == "sh" or self.label == "V2h")))\
       and (type == "full" or not self.fitting_flag or self.label == "Erev"):
      self.initial_value.validate_value()

  def get_lsdata(self, enable = 1):
    """Get 'struct param' data."""
    if enable:
      initial_value = self.initial_value.value
    else:
      initial_value = 0.0
    if initial_value is None:
      initial_value = 0.0
    return (
      initial_value,
      self.fitting_flag and enable,
      self.set_standard_error,
      self.set_constraint,
      self.set_fitted_value
      )

  def write_to_file(self, f):
    if self.description:
      comment = " %% %s" % self.description
    else:
      comment = ""
    f.write("%s = %s %s %s%s\n" % (
      self.label, self.initial_value.value, self.fitted_value.value, 
      self.fitting_flag, comment))

  def read_from(self, r):
    items = r.read_values(self.label, 'ffi')
    (self.initial_value.value, self.fitted_value.value,
     self.fitting_flag) = items

#----------------------------------------------------------

class FtValue(Model, FtObject):
  """This class holds a numeric value and provides a means of
  verifying it. A minimum and/or maximum value may be specified.
  The value may also be None if not yet specified, or 'error' if
  the user has entered something that can't be parsed as a number"""

  def __init__(self, parent, label = "", description = "", 
               initial = None, min = None, max = None,
               output = 0):
    Model.__init__(self, parent)
    if not description:
      description = label
    self.label = label
    self.description = description
    self.value = initial
    self.min = min
    self.max = max
    self.view = None
    self.output = output # If true, this is an output value

  def set(self, x, notify = 1):
    """Attempt to convert x to a number and set the value to it.
    If x is an empty string, the value is set to None. If x can't
    be converted, the value is set to 'error'."""
    if x == "":
      v = None
    else:
      try:
        v = self.to_number(x)
      except ValueError:
        v = "error"
    if self.value != v:
      self.value = v
      self.changed()
      if notify:
        self.notify_views()

  def validate_value(self):
    """
    Check that the value is a valid number and within range. Otherwise,
    the view (if any) is selected and a ValidationError is raised.
    """
    if not self.output:
      try:
        validate_number(self.type, self.value, self.min, self.max, 
                        self.description or self.label)
      except ValidationError, e:
        #self.notify_views('select_all')
        if self.views:
          e.widget = self.views[0]
        raise e

  def write_to_file(self, f):
    v = self.value
    if v == 'error':
      v = None
    f.write("%s = %s %% %s\n" % (self.label, v, self.description))


class FtIntValue(FtValue):

  type = INT

  def to_number(self, x):
    return int(x)

  def read_from(self, r):
    self.value = r.read_int(self.label)


class FtFloatValue(FtValue):

  type = FLOAT

  def to_number(self, x):
    return float(x)

  def read_from(self, r):
    self.value = r.read_float(self.label)

#----------------------------------------------------------

class FtVoltageMapping:
  """Dictionary-like object whose keys are voltage values, compared
  to a suitable tolerance."""

  tolerance = 1.0e-3 # must be a float

  def __init__(self):
    self.dict = {}

  def __setitem__(self, v, x):
    self.dict[self.voltage_to_key(v)] = x

  def __getitem__(self, v):
    return self.dict[self.voltage_to_key(v)]

  def __delitem__(self, v):
    del self.dict[self.voltage_to_key(v)]

  def get(self, v):
    """Lookup defaulting to None."""
    return self.dict.get(self.voltage_to_key(v))

  def has_key(self, v):
    return self.dict.has_key(self.voltage_to_key(v))

  def keys(self):
    return self.dict.keys()

  def items(self):
    return self.dict.items()

  def values(self):
    return self.dict.values()

  def voltage_to_key(self, v):
    keys = self.keys()
    if keys:
      keys.sort()
      i = find_sample(v, keys, 'min')
      if keys[i] == 0.0:
        comp = max(abs(keys[0]), abs(keys[-1])) * pow(self.tolerance, 2)
      else:
        comp = abs(keys[i]) * self.tolerance
      if abs(keys[i] - v) <= comp:
        key = keys[i]
      else:
        key = v
    else:
      key = v
    return key

#----------------------------------------------------------

class DataFileError(Exception):

  def __init__(self, path, linenum, message):
    self.path = path
    self.linenum = linenum
    self.message = message
    Exception.__init__(self, "'%s', line %d: %s" % (
      path, linenum, message))
