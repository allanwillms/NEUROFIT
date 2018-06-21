############################################################
#
#  Validation.py - NEUROFIT, entry validation module
#  =============   Greg Ewing, Jan 2001
#
############################################################

from exceptions import Exception, ValueError
import string
from Tkinter import Toplevel

import Errors

INT = 'int'
FLOAT = 'float'
type_descriptions = {INT: "an integer", FLOAT: "a real number"}

#----------------------------------------------------------

class ValidationError(Exception):

  def __init__(self, msg, widget = None):
    Exception.__init__(self, msg)
    self.widget = widget
  
  def report(self):
    Errors.report_exception(None, self, None)
    # If there is a widget attached, make sure the window containing
    # it is in front and activated, select the contents of the
    # widget, and make sure the widget is visible inside any container.
    target_widget = self.widget
    #print "ValidationError.report: raising window containing", repr(widget) ###
    if target_widget:
      widget = target_widget
      while widget and not isinstance(widget, Toplevel):
        container = widget.master
        if hasattr(container, 'show_widget'):
          #print "ValidationError.report: showing", repr(widget) ###
          container.show_widget(widget)
        widget = container
      if widget:
        #print "ValidationError.report: raising", repr(widget) ###
        widget.tkraise()
        widget.focus_set()
        #print "ValidationError.report: selecting", repr(target_widget) ###
        if hasattr(target_widget, 'select_all'):
          target_widget.select_all()

#----------------------------------------------------------

def validate_number(type, value, min, max, description):
  #print "Validation.validate_number:", type, value, min, max, repr(description) ###
  if value is None:
    raise ValidationError("Value required for %s." % description)
  is_min = min is not None
  is_max = max is not None
  if (value == 'error' or
      (is_min and value < min) or
      (is_max and value > max)):
    if is_min and is_max:
      condition = " between %s and %s" % (min, max)
    elif is_min:
      condition = " >= %s" % min
    elif is_max:
      condition = " <= %s" % max
    else:
      condition = ""
    raise ValidationError("%s must be %s%s" % (
      string.capitalize(description), type_descriptions[type],
      condition))

def validate_int(widget, min_value, max_value, description):
  try:
    value = int(widget.get())
    if not (min_value <= value <= max_value):
      raise ValueError
  except ValueError:
    widget.selection_range(0, 'end')
    raise ValidationError("%s must be an integer from %d to %d." %
                          (string.capitalize(description), 
                           min_value, max_value), widget)
  return value

def validate_real(widget, description, required = 0):
  s = widget.get()
  if not s:
    if required:
      raise ValidationError("Value required for %s." % description, widget)
    value = None
  else:
    try:
      value = float(widget.get())
    except ValueError:
      raise ValidationError("%s must be a real number." %
                            string.capitalize(description), widget)
  return value
