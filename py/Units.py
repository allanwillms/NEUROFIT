############################################################
#
#  Units.py - NEUROFIT measurement units module
#  ========   Greg Ewing, Nov 2001
#
############################################################

from Tkinter import *

import Layout
import Busy
from ModelView import Model, View, ViewFrame, DocumentWindow

# Master list of scale factors corresponding unit prefixes.
# Scale factors are represented as log10(multiplier).

prefixes = (
  (-15, 'f'), (-12, 'p'), (-9, 'n'), (-6, 'u'), (-3, 'm'), 
  (0, ''), 
  (3, 'k'), (6, 'M'), (9, 'T'), (12, 'E')
)

# Calculate tables for mapping between scales and prefixes

scale_to_prefix_dict = {}
prefix_to_scale_dict = {}
for scale, prefix in prefixes:
  scale_to_prefix_dict[scale] = prefix
  prefix_to_scale_dict[prefix] = scale

#
#   Functions for converting between scales and prefixes
#
  
def scale_to_prefix(scale):
  if scale is None:
    return "?"
  else:
    try:
      name = scale_to_prefix_dict[scale]
    except KeyError:
      name = "10^%s" % scale
    return name

def prefix_to_scale(name):
  try:
    scale = prefix_to_scale_dict[name]
  except KeyError:
    scale = None
  return scale

#---------------------------------------------------------------------------
#
#   Classes for representing measurement units
#
#---------------------------------------------------------------------------

class Unit(Model):
  """Measurement units."""
  
  def __init__(self, parent, quantity_name, quantity_symbol, scale = None,
               projectunit = 0):
    Model.__init__(self, parent)
    self.scale = scale
    self.prev_scale = None
    self.projectunit = projectunit   # 1 if this is a project unit else 0
    self.quantity_name = quantity_name
    self.quantity_symbol = quantity_symbol
    self.derived_units = []

  def destroy(self):
    self.derived_units = []
    Model.destroy(self)

  def set_scale(self, new_scale):
    self.prev_scale = self.scale
    self.scale = new_scale
    self.changed()
    if self.projectunit:
      Busy.with_busy_cursor(self.update)
    else:
      self.update()

  def add_derived_unit(self, unit):
    self.derived_units.append(unit)
  
  def update(self):
    """Notify views of this unit and any derived units that
    this unit has changed."""
    #print "Unit.update:", repr(self) ###
    #print "...derived units:", self.derived_units ###
    if self.projectunit and \
       self.prev_scale is not None and self.scale is not None and \
       self.prev_scale != self.scale:
      factor = pow(10, self.prev_scale - self.scale)
      self.parent.scale_units(self.quantity_name, factor) 
    self.notify_views()
    for unit in self.derived_units:
      #print "Unit.update: updating derived unit", repr(unit) ###
      unit.update()
  
  def __str__(self):
    return scale_to_prefix(self.scale) + self.quantity_symbol

class QuotientUnit(Unit):
  """A unit defined as the quotient of two other units."""
  
  def __init__(self, parent, quantity_name, quantity_symbol, numer, denom):
    Unit.__init__(self, parent, quantity_name, quantity_symbol)
    self.numer = numer
    self.denom = denom
    self.projectunit = self.numer.projectunit or self.denom.projectunit
    numer.add_derived_unit(self)
    denom.add_derived_unit(self)
    self.update()
  
  def update(self):
    if self.numer.scale is not None and self.denom.scale is not None:
      self.prev_scale = self.scale
      self.scale = self.numer.scale - self.denom.scale
    else:
      self.scale = None
    Unit.update(self)
  
  def __repr__(self):
    return "Units.QuotientUnit(%s,%s)" % (self.numer, self.denom)


#---------------------------------------------------------------------------
#
#   Views for displaying a measurement unit
#
#---------------------------------------------------------------------------

class UnitView(Label, View):
  
  def __init__(self, master, model, **kw):
    Label.__init__(self, master, **kw)
    View.__init__(self)
    self.set_model(model)
  
  def destroy(self):
    View.destroy(self)
    Label.destroy(self)
  
  def update(self):
    self.configure(text = str(self.model))


class CanvasUnitView(Canvas, View):

  def __init__(self, parent, canvas, model, *args, **kw):
    self.canvas = canvas
    self.text = canvas.create_text(*args, **kw)
    View.__init__(self, parent)
    self.set_model(model)
  
  def destroy(self):
    View.destroy(self)
    self.canvas.delete(self.text)
  
  def update(self):
    self.canvas.itemconfigure(self.text, text = str(self.model))

#
#   Dialog box for entering units
#

class UnitsDialog(DocumentWindow):

  def __init__(self, units, ok_button):
    DocumentWindow.__init__(self)
    x = (self.winfo_screenwidth() - 200) / 2
    y = (self.winfo_screenheight() - 200) / 2
    self.geometry("+%d+%d" % (x, y))
    heading = self.heading = Label(self, wrap = 300, anchor = 'w', just = 'left')
    chooser_frame = Frame(self)
    choosers = [UnitChooser(chooser_frame, unit) for unit in units]
    Layout.grid(choosers, padx = 10)
    heading.grid(row = 0, padx = 5, pady = 5, sticky = 'ew')
    chooser_frame.grid(row = 1, padx = 5)
    if ok_button:
      b = Button(self, text = "OK", width = 6, command = self.ok)
      b.grid(row = 2, sticky = 'e')
  
  def set_title(self, title):
    self.wm_title("%s: Units" % title)
    self.heading.configure(text = "Measurement units for '%s':" % title)
  
  def ok(self):
    self.destroy()
  

#
#   Window for editing a project's units
#

class UnitsWindow(UnitsDialog):

  def __init__(self, project):
    UnitsDialog.__init__(self, project.get_units(), ok_button = 0)
  
  def update_title(self):
    self.set_title(self.document.get_filename())
  

#
#   Widget for selecting a measurement unit
#

class UnitChooser(ViewFrame):

  def __init__(self, master, unit, heading = None):
    ViewFrame.__init__(self, master)
    quantity_symbol = unit.quantity_symbol
    if heading is None:
      heading = unit.quantity_name
    headinglbl = Layout.group_label(self, heading)
    num_units = len(prefixes)
    listbox = self.listbox = Listbox(self, width = 5, height = num_units, 
      selectmode = 'browse', exportselection = 0, background = 'white')
    listbox.bind("<ButtonRelease>", self.selection_changed)
    listbox.bind("<KeyRelease>", self.selection_changed)
    for _, prefix in prefixes:
      listbox.insert('end', prefix + quantity_symbol)
    listbox.selection_set((num_units / 2))
    Layout.grid([headinglbl],
                [listbox])
    self.set_model(unit)
  
  def selection_changed(self, event):
    index = int(self.listbox.curselection()[0])
    scale, _ = prefixes[index]
    self.model.set_scale(scale)
  
  def update(self):
    self.listbox.selection_clear(0, 'end')
    scale = self.model.scale
    for i in range(len(prefixes)):
      if prefixes[i][0] == scale:
        self.listbox.selection_set(i)
        break
  
  #def get_unit(self):
  #  index = int(self.listbox.curselection()[0])
  #  scale, _ = prefixes[index]
  #  return Unit(scale, self.quantity_symbol)

#---------------------------------------------------------------------------
#
#   Functions for asking user about units
#
#---------------------------------------------------------------------------

def get_missing_units_from_user(units, keys, default_units, title):
  """Given a dictionary of Unit instances, if any of them are unspecified
  (scale == None), ask the user to specify them; use the default_units
  as the defaults."""

  missing_units = []
  for key in keys:
    if units[key].scale is None:
      missing_units.append(units[key])
      units[key].scale = default_units[key].scale
  if missing_units:
    dlog = UnitsDialog(missing_units, ok_button = 1)
    dlog.set_title(title)
    dlog.tkraise()
    dlog.focus_set()
    dlog.wait_window()


#---------------------------------------------------------------------------
#
#   Test code
#
#---------------------------------------------------------------------------

if __name__ == "__main__":
  from Tkinter import Tk
  
  class TestClient:
  
    def set_units(self, *args):
      print "TestClient got:", args
      
  root = Tk()
  root.wm_withdraw()
  client = TestClient()
  dlog = UnitsDialog("SomethingWithQuiteALongName", client)
  print "Entering main loop"
  dlog.wait_window()
  print "Main loop exited"
