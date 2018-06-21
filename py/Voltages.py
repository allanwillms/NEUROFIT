############################################################
#
#  Voltages.py - NEUROFIT, voltage dialog module
#  ===========   Greg Ewing, Jan 2001
#
############################################################

import os
from Tkinter import *
from Layout import entry_label, group_label, grid
from Validation import validate_real, ValidationError
from Widgets import NumberEntry
from Units import *
from Scrolling import ScrolledListbox

class VoltageEditor(Toplevel):
  """Dialog box for editing the voltages for an experiment."""

  headings = (
    "Varying Step Voltages",
    "Varying Pre-Step Voltages"
    )

  Vhold_labels = ("Prestep", "Step")

  def __init__(self, parent, model):
    Toplevel.__init__(self)
    self.parent = parent  # an ExperimentView
    self.model = model    # an FtExperiment
    self.entries = [] # a list of:
                # (FtSweepSet, NumberEntry, [(FtSweep, NumberEntry), ...])
    proj_units = self.model.parent.units
    outerbox = Frame(self, background = self.cget("background"),
                     borderwidth = 0, width = 40)
    self.title(self.headings[model.experiment_number])
    colnum = 0
    for sweep_set in model.sweep_sets:
      dir, name = os.path.split(sweep_set.path)
      fset = Frame(outerbox, borderwidth = 3, relief='groove')
      if proj_units['Voltage'].scale is not None and \
         sweep_set.units['Voltage'].scale is not None:
        expo = proj_units['Voltage'].scale - sweep_set.units['Voltage'].scale
        factor = pow(10, expo)
      else:
        factor = 1
      glbl = group_label(fset, "Voltage levels for " + name)
      box = Text(fset, width=25, borderwidth = 0,
                 background = self.cget("background"))
      bar = fset.vscrollbar = Scrollbar(fset, orient = 'v', command = box.yview)
      box.configure(yscrollcommand = bar.set)
      bar.sticky = 'ns'
      box.grid(padx = 2, sticky = 'n')
      unitbox = UnitChooser(fset, sweep_set.units['Voltage'],
                            heading = 'Units:')
      unitbox.grid(padx = 5, sticky = 'n')
      # Hold voltage
      lbl = entry_label(box,
        text = self.Vhold_labels[model.experiment_number])
      ent = NumberEntry(box)
      val = sweep_set.Vhold
      if val is not None:
        val = val*factor
      ent.set(val)
      sweeps = []
      self.entries.append((sweep_set, ent, sweeps))
      box.window_create('end', window = lbl)
      box.insert('end', '\t')
      box.window_create('end', window = ent)
      box.insert('end', '\n')
      i = 1
      for sweep in sweep_set.sweeps:
        lbl = entry_label(box, text = "%3d" % i)
        ent = NumberEntry(box)
        val = sweep.Vsweep
        if val is not None:
          val = val*factor
        ent.set(val)
        sweeps.append((sweep, ent))
        box.window_create('end', window = lbl)
        box.insert('end', '\t')
        box.window_create('end', window = ent)
        box.insert('end', '\n')
        i = i + 1
      box.configure(state = 'disabled')
      grid([glbl, glbl, glbl],
           [box, unitbox, bar])
      fset.grid(row = 0, col = colnum, sticky = 'ne', padx = 3)
      colnum = colnum + 1
    fbut = Frame(self)
    bok = Button(fbut, width = 6, text = "OK", command = self.ok)
    bcan = Button(fbut, text = "Cancel", command = self.cancel)
    bcan.pack(side = 'right', padx = 5, pady = 5)
    bok.pack(side = 'right', padx = 5, pady = 5)
    grid([outerbox],
         [fbut])
    self.focus_set()

  def destroy(self):
    self.parent.voltage_editor = None
    Toplevel.destroy(self)

  def ok(self):
    try:
      for (sweep_set, entry, sweeps) in self.entries:
        dir, name = os.path.split(sweep_set.path)
        Vhold = validate_real(entry, "hold voltage")
        if Vhold is not None and sweep_set.units['Voltage'].scale is None:
          raise ValidationError("Units must be specified for %s." % name, None)
        sweep_set.Vhold = Vhold
        for (sweep, entry) in sweeps:
          Vsweep = validate_real(entry, "sweep voltage")
          if Vsweep is not None and sweep.parent.units['Voltage'].scale is None:
            raise ValidationError("Units must be specified for %s." % name, None)
          sweep.Vsweep = Vsweep
    except ValidationError, e:
      e.report()
      return
    proj_units = self.model.parent.units
    for sweep_set in self.model.sweep_sets:
      if sweep_set.units['Voltage'].scale is not None:
        self.model.parent.set_unknown_proj_units_to(sweep_set.units, ['Voltage'])
        if sweep_set.units['Voltage'].scale != proj_units['Voltage'].scale:
          factor = pow(10, sweep_set.units['Voltage'].scale - proj_units['Voltage'].scale)
          sweep_set.scale_units_for_file_data('Voltage', factor)
    self.model.voltages_changed()
    self.destroy()

  def cancel(self):
    self.destroy()
