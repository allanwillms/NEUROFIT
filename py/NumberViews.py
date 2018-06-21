############################################################
#
#  NumberViews.py - NEUROFIT, number entry & display module
#  ==============   Greg Ewing, Jan 2001
#
############################################################

import Layout
from string import capitalize
from Tkinter import *
from Layout import entry_label, column_label, grid
from ModelView import ViewFrame
from math import log10, ceil
import Units

#----------------------------------------------------------

class NumberView(ViewFrame):
  """This class provides a view of an FtValue. It may be
  editable or read-only."""

  sticky = 'ew'
  format = "limited"  # format precision may be limited by width (default),
                      # or unlimited -> precision is 12 digits

  def __init__(self, parent, ft_value, title = None, format = None, 
      readonly = 0, width = 9):
    self.width = width
    ViewFrame.__init__(self, parent)
    if format:
      self.format = format
    ft_value.add_view(self)
    if not title:
      title = ft_value.label
      if title:
        title = capitalize(title) + ":"
    if title:
      lbl = entry_label(self, title)
    else:
      lbl = None
    ent = self.entry = Entry(self, width = width, background = 'white')
    ent.sticky = 'e'
    if readonly:
      ent.configure(state = 'disabled', relief = 'flat')
    else:
      # Set up mechanism to be notified when the user enters something
      self.var = StringVar()
      self.var.trace("w", self.entry_changed)
      ent.configure(textvariable = self.var)
    self.columnconfigure(0, weight = 1)
    grid([lbl, ent])
    self.update()

  def ft_value(self):
    return self.model

  def update(self):
    """Update entry box in response to change of ft_value."""
    value = self.ft_value().value
    if value is not 'error':
      if value is not None:
        if self.format == "limited":
          if value == 0:
            a = 1
          else:
            a = ceil(log10(abs(value)))
          if 1 <= a <= self.width:
            prec = self.width
          elif 3 <= a <= 0:
            prec = self.width - a - 1
          else:
            prec = self.width - 5
          if value < 0:
              prec = prec - 1
        else:
          prec = 12
        value = "%.*g" % (prec, value)
      self.set_entry(value)

  def entry_changed(self, *args):
    """Update ft_value in response to user entry."""
    ft_value = self.ft_value()
    if ft_value:
      ft_value.set(self.var.get(), notify = 0)

  def set_entry(self, value):
    """Put a value into the entry box."""
    entry = self.entry
    state = entry.cget('state')
    entry.configure(state = 'normal')
    entry.delete(0, 'end')
    if value is not None:
      entry.insert(0, value)
    entry.configure(state = state)

  def select_all(self):
    self.entry.selection_range(0, 'end')
    self.entry.focus_set()

#----------------------------------------------------------

class ParamViewHeader(Frame):
  """Header for a table of ParameterViews."""

  def __init__(self, parent, feed_back_all):
    Frame.__init__(self, parent, borderwidth = 2)
    self.sticky = 'w'
    l0 = column_label(self, "Fit?")
    l1 = column_label(self, "Initial Value")
    l2 = column_label(self, "Fitted Value")
    l3 = column_label(self, "Std. Error")
    bf = Button(self, text = "<<", padx = 0, pady = 1, command = feed_back_all)
    self.columnconfigure(0, minsize = 90)
    self.columnconfigure(1, minsize = Layout.params.param_view_header_column1_width)
    self.columnconfigure(2, minsize = 30)
    self.columnconfigure(3, minsize = Layout.params.param_view_header_column3_width)
    grid([l0, l1, bf, l2, l3])
    

class ParameterView(ViewFrame):
  """Widget providing a view of an FtParameter."""

  def __init__(self, parent, ft_parameter):
    ViewFrame.__init__(self, parent)
    ft_parameter.add_view(self)
    self.flag_var = IntVar()
    self.flag_var.trace('w', self.flag_changed)
    cbf = Checkbutton(self, text = ft_parameter.label + ":", 
      font = Layout.params.entry_label_font, variable = self.flag_var)
    cbf.sticky = 'w'
    eiv = self.init_value_view = \
          NumberView(self, ft_parameter.initial_value, format="unlimited")
    eiv.sticky = 'ew'
    muv = Units.UnitView(self, ft_parameter.unit)
    bfb = Button(self, text = "<<", 
                 padx = 0, pady = 1, command = self.feed_back)
    efv = self.fitted_value_box = \
          NumberView(self, ft_parameter.fitted_value, readonly = 1)
    ese = self.std_err_box = \
          NumberView(self, ft_parameter.standard_error, readonly = 1)
    self.columnconfigure(0, minsize = 90) # check box
    self.columnconfigure(2, minsize = 25) # measurement unit
    self.columnconfigure(3, minsize = 30) # feedback button
    self.columnconfigure(5, minsize = 14) # space between fitted value & stderror
    grid([cbf, eiv, muv, bfb, efv, 0, ese])
    self.update()

  def destroy(self):
    self.flag_var = None
    ViewFrame.destroy(self)

  def ft_parameter(self):
    return self.model

  def flag_changed(self, *args):
    self.ft_parameter().set_fitting_flag(self.flag_var.get())

  def feed_back(self):
    ft_parameter = self.ft_parameter()
    fv = ft_parameter.fitted_value.value
    if fv is not None:
      ft_parameter.set_initial_value(fv)

  def update(self):
    ft_parameter = self.ft_parameter()
    #self.fitted_value_box.set(ft_parameter.fitted_value)
    #self.std_err_box.set(ft_parameter.standard_error)
    self.flag_var.set(ft_parameter.fitting_flag)
