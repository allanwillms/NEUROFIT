############################################################
#
#  main.py - NEUROFIT main module
#  =======   Greg Ewing, Jan 2001
#
############################################################

debug_cmd_line_args = 0

import os
import string
import tkFileDialog, tkMessageBox
import webbrowser
from Tkinter import *

import Errors
from FileSelectors import DataFileSelector
from FtProject import FtProject, num_open_projects, open_projects
from Graphs import ExperimentView
import Layout
from Layout import grid, vbox, group_label, entry_label, column_label
from ModelView import View, ViewFrame, ParentChild
from NumberViews import NumberView, ParameterView, ParamViewHeader
import Reporting
import Root
import Units
from Validation import ValidationError, validate_int
from Widgets import NumberEntry, NumberDisplay

HELP_FILE = "Doc/help.html"
TUTORIAL_FILE = "Doc/tutorial.html"

def int_or_empty(x):
  if x:
    return int(x)
  else:
    return x

#------------------------------------------------------------
#
#   Dialog box for editing nh and nnonh
#
#------------------------------------------------------------


class HParamDialog(Toplevel):

  def __init__(self, owner, init_nh, init_nnonh):
    Toplevel.__init__(self)
    self.owner = owner
    self.wm_title("NEUROFIT - Channel Parameters")
    self.wm_transient(owner)
    self.wm_geometry("+%d+%d" % (owner.winfo_x() + 30,
                                 owner.winfo_y() + 30))
    lnh = entry_label(self, text = "No. of inactivating channel groups:")
    inh = self.nh_box = NumberEntry(self, initial = init_nh)
    lnn = entry_label(self, text = "No.of non-inactivating channel groups:")
    inn = self.nnonh_box = NumberEntry(self, initial = init_nnonh)
    bok = Button(self, text = "OK", command = self.ok)
    bca = Button(self, text = "Cancel", command = self.cancel)
    grid([lnh, inh],
         [lnn, inn],
         [bok, bca])

  def ok(self):
    try:
      nh = validate_int(self.nh_box, 0, 3,
        "No. of inactivating channel groups")
      nnonh = validate_int(self.nnonh_box, 0, 1,
        "No. of non-inactivating channel groups")
      if nh + nnonh == 0:
        raise ValidationError("nh and nnonh must not both be zero.")
      self.owner.set_h_params(nh, nnonh)
      self.destroy()
    except ValidationError, e:
      e.report()

  def cancel(self):
    self.destroy()

    
#------------------------------------------------------------
#
#   Project window
#
#------------------------------------------------------------

class WindowPositioner:
  min_x = 5
  min_y = 5
  max_x = 105
  max_y = 45
  dx = 10
  dy = 20
  
  def __init__(self):
    self.x = self.min_x
    self.y = self.min_y
  
  def next_position(self):
    x = self.x
    y = self.y
    self.x += self.dx
    self.y += self.dy
    if self.x > self.max_x:
      self.x = self.min_x
    if self.y > self.max_y:
      self.y = self.min_y
    return x, y

project_window_positioner = WindowPositioner()
    
#------------------------------------------------------------

class ProjectWindow(Toplevel, ParentChild):

  def __init__(self, project):
    Toplevel.__init__(self)
    ParentChild.__init__(self)
    self.project = project
    self.data_file_selector = None
    self.update_title()
    self.wm_geometry("+%s+%s" % project_window_positioner.next_position())
    self.wm_protocol('WM_DELETE_WINDOW', self.close)
    menubar = Menu(self)
    self.configure(menu = menubar)
    file_menu = Menu(menubar, tearoff = 0)
    menubar.add_cascade(label = "File", menu = file_menu)
    self.add_menu_items(file_menu, [
      ("New", "N", new_project),
      ("Open...", "O", self.open),
      "-",
      ("Close", "W", self.close),
      ("Save", "S", self.save),
      ("Save As...", None, self.save_as),
      "-",
      ("Save Report...", "R", self.save_report),
      "-",
      ("Exit", "Q", quit)
    ])
    data_menu = Menu(menubar, tearoff = 0)
    menubar.add_cascade(label = "Data", menu = data_menu)
    self.add_menu_items(data_menu, [
      ("Select Data Files...", "D", self.select_data_files),
      ("Measurement Units...", "U", self.edit_units),
      ])
    voltages_menu = Menu(data_menu, tearoff = 0)
    data_menu.add_cascade(label = "Voltage Levels", menu = voltages_menu)
    self.add_menu_items(voltages_menu, [
      ("Varying Step Data...", None, self.edit_varying_step_voltages),
      ("Varying Prestep Data...", None, self.edit_varying_prestep_voltages),
      ])
    self.add_menu_items(data_menu, [
      "-",
      ("Quick Fit", "G", project.guess_parameters),
      ("Full Fit", "F", project.fit),
      #"-",
      #("Test Exception Handler", None, test_exc)
      ])
    help_menu = Menu(menubar, tearoff = 0)
    menubar.add_cascade(label = "Help", menu = help_menu)
    self.add_menu_items(help_menu, [
      ("About NEUROFIT...", None, about),
      ("User's Manual", "H", help),
      ("Tutorial", "T", tutorial)
      ])
    view = ProjectView(self, project)
    view.grid()
    project.add_window(self)
    self.focus_set()
  
  def add_menu_items(self, menu, items):
    for item in items:
      if item == "-":
        menu.add_separator()
      else:
        lbl, key, cmd = item
        if key:
          acc = "Ctrl+%s" % key
        else:
          acc = None
        menu.add_command(
          label = lbl, accelerator = acc, command = cmd)
        if key:
          thunk = lambda e, cmd=cmd: cmd()
          self.bind("<Control-Key-%s>" % string.lower(key), thunk)
          self.bind("<Control-Key-%s>" % string.upper(key), thunk)

  def destroy(self):
    if self.data_file_selector:
      self.data_file_selector.destroy()
    ParentChild.destroy(self)
    Toplevel.destroy(self)
  
  def update_title(self):
    self.wm_title("NEUROFIT: %s" % self.project.get_filename())
  
  def open(self):
    open_project()
    # The selected project has opened in a new window.
    # If the project in this window is empty and unsaved,
    # close it. This avoids having a spurious empty project
    # window around if you launch the application and then
    # immediately open a project file.
    if self.project.untouched():
      self.close()

  def close(self):
    self.project.close()
    if num_open_projects() == 0:
      sys.exit(0)

  def save(self):
    self.project.save()

  def save_as(self):
    self.project.save_as()

  def select_data_files(self):
    """Open dialog for selecting input files."""
    if self.data_file_selector:
      self.data_file_selector.tkraise()
    else:
      self.data_file_selector = DataFileSelector(self, self.project)
  
  def edit_units(self):
    # first close all voltage editor windows for the project
    for view in self.project.views:
      for expview in view.experiment_views:
        expview.close_voltage_editor()
    uwin = self.project.find_window(Units.UnitsDialog)
    if not uwin:
      uwin = Units.UnitsWindow(self.project)
      self.project.add_window(uwin)
    uwin.tkraise()
    uwin.focus_set()

  def edit_varying_step_voltages(self):
    try:
      self.project.experiments[0].views[0].open_voltage_editor()
    except:
      pass

  def edit_varying_prestep_voltages(self):
    try:
      self.project.experiments[1].views[0].open_voltage_editor()
    except:
      pass

  def save_report(self):
    """Write a report of the results from the last fitting
    operation to a text file."""
    project = self.project
    dir = project.get_directory()
    projname = project.get_filename()
    name = os.path.splitext(projname)[0] + "_Report.txt"
    types = (
      ("Text Files", (".txt")),
      ("All Files", ("*")))
    path = tkFileDialog.asksaveasfilename(
      title = "Save report as:",
      defaultextension = ".txt", filetypes = types,
      initialdir = dir, initialfile = name)
    if path:
      Reporting.report(project, path)

#------------------------------------------------------------

class ProjectView(ViewFrame):

  def __init__(self, parent, project):
    ViewFrame.__init__(self, parent)
    project.add_view(self)
    # Parameters frame
    fpa = Frame(self, borderwidth = 5)
    fpa.sticky = 'ns'
    # Model Specifications frame
    fmp = Frame(fpa)
    fmp.sticky = 'w'
    lmp = group_label(fmp, text = "Model Specifications")
    lnh = entry_label(fmp, text = "No. of inactivating channel groups:")
    inh = self.nh_box = NumberDisplay(fmp, on_click = self.edit_h_params, width = 4)
    lnn = entry_label(fmp, text = "No.of non-inactivating channel groups:")
    inn = self.nnonh_box = NumberDisplay(fmp, on_click = self.edit_h_params, width = 4)
    vp = NumberView(fmp, project.p, 
                    title = "No. of independent activation gates:",
                    width = 4)

    grid([lmp,   lmp],
         [lnh,   inh],
         [lnn,   inn],
         [vp,    vp])
         
    # Algorithm Specifications frame
    
    fnp = ViewFrame(fpa)
    fnp.sticky = 'w'
    lnp = group_label(fnp, text = "Algorithm Specifications")
    vnd = NumberView(fnp, project.noise_deg, 
                     title = "Noise poly degree:", width = 4)
    vne = NumberView(fnp, project.nevalfac, 
                     title = "Max. function evals:", width = 4)
                     
    # Tolerances frame
    
    fto = Frame(fnp)
    lto = entry_label(fto, text = "Tolerances:")
    vtf = NumberView(fto, project.tol_f, title = "f")
    vtx = NumberView(fto, project.tol_x, title = "x")
    vtg = NumberView(fto, project.tol_g, title = "g")
    grid([lto, vtf],
         [0,   vtx],
         [0,   vtg])
         
    # Algorithm Specifications frame assembly
    
    grid([lnp, 0, 0  ],
         [vnd, 0, fto],
         [vne, 0, fto],
         [0,   0, fto])
    fnp.rowconfigure(3, weight = 1)
    fnp.columnconfigure(1, minsize = 10)

    # Model Parameters frame
    
    ffp = ViewFrame(fpa)
    ffp.sticky = 'nsew'
    for i in xrange(3):
      ffp.columnconfigure(i, weight = 1)
    lfp = group_label(ffp, "Model Parameters")
    lfp.sticky = 'nw'
    bqft_col = '#70E070'
    bfit_col = '#80FF80' # a slightly yellowish green, not too bright
    bqft = Button(ffp, text = "Quick Fit", width = 12, 
      background = bqft_col,
      activebackground = bqft_col,
      command = project.guess_parameters)
    bqft.sticky = 's'
    bfit = Button(ffp, text = "Full Fit", width = 12, 
      background = bfit_col,
      activebackground = bfit_col,
      command = project.fit)
    bfit.sticky = 's'
    hdr = ParamViewHeader(ffp, self.feed_back_all)
    tfp = ParamViewBox(ffp, background = self.cget("background"))
    self.fitting_parm_box = tfp
    tfp.sticky = 'nsew'
    sfp = Scrollbar(ffp, orient = 'v', command = tfp.yview)
    sfp.sticky = 'ns'
    tfp.configure(yscrollcommand = sfp.set)
    grid([lfp,  bqft, bfit, 0 ],
         [hdr,  hdr,  hdr,  0 ],
         [tfp,  tfp,  tfp,  sfp])
    ffp.rowconfigure(2, weight = 1)
    ffp.rowconfigure(0, minsize = 33)
    
    # Parameters frame assembly
    
    vbox([fmp, fnp, ffp, ffp])
    fpa.rowconfigure(3, weight = 1)
    
    # Experiment views frame
    
    fxp = Frame(self, borderwidth = 5)
    vx0 = ExperimentView(fxp, project.experiments[0])
    vx1 = ExperimentView(fxp, project.experiments[1])
    self.experiment_views = (vx0, vx1)

    # Fit results frame
    
    ffr = ViewFrame(fxp)
    ffr.sticky = 'w'
    lfr = group_label(ffr, "Fit Results")
    vnfev = NumberView(ffr, project.nfev, readonly = 1, width=5)
    vnjev = NumberView(ffr, project.njev, readonly = 1, width=5)
    vnpts = NumberView(ffr, project.npts, readonly = 1, width=6)
    vnfitp = NumberView(ffr, project.nfitp, readonly = 1, width=6)
    vvar = NumberView(ffr, project.variance, readonly = 1)
    grid([lfr,   0,     0,  0, 0   ],
         [vnfev, 0, vnpts,  0, vvar],
         [vnjev, 0, vnfitp, 0, 0   ])
    ffr.columnconfigure(1, minsize = 8)
    ffr.columnconfigure(3, minsize = 8)
    
    # Messages frame
    
    fmsg = ViewFrame(fxp)
    fmsg.sticky = 'ew'
    lmsg = group_label(fmsg, "Messages")
    vmsg = self.messages_view = Text(
      fmsg, width = 70, height = 5, background = 'white',
      borderwidth = 0, state = 'disabled', wrap = 'word')
    vmsg.sticky = 'ew'
    sbmsg = Scrollbar(fmsg, orient = 'v', command = vmsg.yview)
    sbmsg.sticky = 'ns'
    vmsg.configure(yscrollcommand = sbmsg.set)
    grid([lmsg, 0],
         [vmsg, sbmsg])
    fmsg.columnconfigure(0, weight = 1)
    
    # Experiment views frame assembly
    
    grid([vx0],
         [vx1],
         [ffr],
         [fmsg])
         
    # Final assembly
    
    grid([fpa, fxp])
    self.fitting_parm_views = []
    self.update_h_params()

  def destroy_fitting_parm_views(self):
    for v in self.fitting_parm_views:
      v.destroy()
    self.fitting_parm_views = []

  def project(self):
    return self.model

  def edit_h_params(self, *_):
    """Open dialog for entering nh and nnonh parameters."""
    HParamDialog(self, self.nh_box.get(), self.nnonh_box.get())

  def set_h_params(self, nh, nnonh):
    """Set project's nh and nnonh parameters to values supplied
    by the user."""
    self.project().set_h_params(nh, nnonh)

  def update_h_params(self):
    """Update views in response to a change in the project's
    nh and nnonh parameters."""
    project = self.project()
    self.nh_box.set(project.nh)
    self.nnonh_box.set(project.nnonh)
    self.update_fitting_params()

  def update_fitting_params(self):
    """Rebuild the array of fitting parameter entry boxes."""
    project = self.project()
    box = self.fitting_parm_box
    box.configure(state = 'normal')
    self.destroy_fitting_parm_views() #!
    box.delete("1.0", 'end')
    for p in project.get_scalar_params():
      self.add_fitting_param(p)
    for p in project.get_f_params():
      self.add_fitting_param(p)
    # TODO: display last f-param
    box.insert('end', "Time constants:\n")
    for p in project.get_timeconst_params():
      self.add_fitting_param(p)
    box.configure(state = 'disabled')

  def add_fitting_param(self, ft_parameter):
    box = self.fitting_parm_box
    view = ParameterView(self.fitting_parm_box, ft_parameter)
    view.index = box.index('end')
    box.window_create('end', window = view)
    box.insert('end', "\n")
    self.fitting_parm_views.append(view)

  def feed_back_all(self):
    """Feed back all fitted values to initial values."""
    for fpv in self.fitting_parm_views:
      fpv.feed_back()
  
  def update_before_fitting(self):
    self.display_messages(["Fitting, please wait..."])
    Frame.update(self)

  def update_after_fitting(self):
    for expview in self.experiment_views:
      expview.update_fitted_lines()
    self.update_messages()
  
  def update_after_guessing(self):
    self.update_after_fitting()

  def update_messages(self):
    self.display_messages(self.project().messages)
  
  def display_messages(self, message_list):
    vmsg = self.messages_view
    vmsg.configure(state = 'normal')
    vmsg.delete('1.0', 'end')
    for msg in message_list:
      vmsg.insert('end', msg + '\n')
    vmsg.configure(state = 'disabled')

#------------------------------------------------------------

class ParamViewBox(Text):
  """
  Scrolling area for parameter views.
  """
  # We use a text widget to hold the parameter views because
  # it's an easy way of getting an automatically-managed scrollbar.
  # But we have to play some tricks to stop selections from being
  # highlighted, etc.
  
  def __init__(self, master, **kw):
    Text.__init__(self, master,
      width = Layout.params.param_view_container_width, 
      font = Layout.params.entry_label_font, 
      cursor = "", 
      selectborderwidth = 0, 
      highlightthickness = 0,
      **kw)
    self.configure(
      selectforeground = self.cget('foreground'),
      selectbackground = self.cget('background'))
  
  def show_widget(self, widget):
    """
    Scroll the given parameter widget into view.
    """
    self.see(widget.index)

#------------------------------------------------------------
#
#   Global Functions
#
#------------------------------------------------------------

def new_project():
  project = FtProject()
  win = ProjectWindow(project)

def open_project():
  path = tkFileDialog.askopenfilename(
    title = "Open Project File",
    filetypes = (("Project Files", ".nfp"), ("All Files", "*")))
  if not path:
    raise Errors.Cancelled
  open_project_file(path)

def open_project_file(path):
  project = FtProject()
  try:
    project.open(path)
    win = ProjectWindow(project)
  except:
    project.destroy()
    raise

about_text = """
NEUROFIT 1.7  Released May 21, 2004
by Allan Willms, Gregory Ewing, and Howie Kuo

Department of Mathematics and Statistics
University of Guelph
Guelph, ON  N1G 2W1
Canada
http://www.uoguelph.ca/~awillms/neurofit/
"""

def about():
  tkMessageBox.showinfo("About NEUROFIT", about_text)

def help():
  path = os.path.normpath(os.path.join(os.getcwd(), HELP_FILE))
  webbrowser.open("file:%s" % path, autoraise = 1)

def tutorial():
  path = os.path.normpath(os.path.join(os.getcwd(), TUTORIAL_FILE))
  webbrowser.open("file:%s" % path, autoraise = 1)

def quit():
  projects = open_projects()
  while projects:
    projects[0].close()
  sys.exit(1)

#------------------------------------------------------------
#
#   Start here
#
#------------------------------------------------------------

def main():
  tk = Root.create_root()
  args = sys.argv[1:]
  if args:
    cwd = os.getcwd()
    for arg in args:
      path = os.path.normpath(os.path.join(cwd, arg))
      if debug_cmd_line_args:
        print "main:"
        print "...cwd =", cwd
        print "...arg =", arg
        print "...path =", path
      try:
        open_project_file(path)
      except Errors.Cancelled:
        pass
      except:
        if debug_cmd_line_args:
          print "Exception processing command line arg"
        Errors.report_exception(sys.exc_type, sys.exc_value, sys.exc_traceback)
  else:
    new_project()
  tk.mainloop()

def test_exc():
  raise Exception("This is a test exception. Had it been a real exception, "
    "it would have been more exceptional.")
