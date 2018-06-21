############################################################
#
#  FileSelectors.py - NEUROFIT, file selection dialogs
#  ================   Greg Ewing, Jan 2001
#
############################################################

import os, string
from Tkinter import *

from ABFUtils import identify_abf_channel
from Layout import grid, group_label
from Scrolling import ScrolledListbox

def make_listbox(parent, height, double_click = None):
  w = ScrolledListbox(parent, width = 20, height = height, 
                      background = 'white',
                      selectmode = 'extended',
                      double_click = double_click)
  return w

#------------------------------------------------------------
#
#   Data file selection dialog
#
#------------------------------------------------------------

class DataFileSelector(Toplevel):

  suffixes = (".abf", ".dat", ".txt",)
  
  last_cwd = None # Class variable, to hold cwd between invocations

  def __init__(self, owner, project):
    Toplevel.__init__(self)
    self.wm_title("NF: Data Files")
    self.owner = owner
    self.project = project
    self.filtering = IntVar(self)
    self.filtering.set(1)
    self.filtering.trace('w', self.filtering_changed)
    #head  = group_label(self, "Select Data Files:")
    mbcwd = self.dir_menubutton = \
            Menubutton(self, relief = 'raised',
                       indicatoron = 1)
    self.dir_menu = Menu(mbcwd, tearoff = 0)
    mbcwd.configure(menu = self.dir_menu)
    lbdir = self.dir_listbox = \
            make_listbox(self, height = 20, double_click = self.open_dir)
    sfex0 = SelectedFilesBox(self, 0)
    sfex0.sticky = 'n'
    sfex1 = SelectedFilesBox(self, 1)
    sfex1.sticky = 's'
    self.selected_files_boxes = [sfex0, sfex1]
    ffilt = Frame(self)
    ffilt.sticky = 'w'
    rall  = Radiobutton(ffilt, text = "All Files", 
                        variable = self.filtering, value = 0)
    patterns = ["*" + suf for suf in self.suffixes]
    rfilt = Radiobutton(ffilt, text = string.join(patterns, ", "), 
                        variable = self.filtering, value = 1)
    grid([rall, rfilt])
    bok   = Button(self, text = "OK", command = self.ok)
    bcan  = Button(self, text = "Cancel", command = self.cancel)
    grid([mbcwd,  0],
         [lbdir, sfex0],
         [lbdir, sfex1],
         [ffilt, ffilt],
         [bok,   bcan ])
    if DataFileSelector.last_cwd is not None:
      self.set_cwd(DataFileSelector.last_cwd)
    else:
      self.set_cwd(os.getcwd())

  def destroy(self):
    self.owner.data_file_selector = None
    for box in self.selected_files_boxes:
      box.destroy()
    Toplevel.destroy(self)

  def set_cwd(self, path):
    self.cwd = path
    self.update_dir_menu()
    if path == "":
      self.cwd_contents = drive_list
    else:
      self.cwd_contents = os.listdir(self.cwd)
    self.update_dir_listbox()
    DataFileSelector.last_cwd = path

  def update_dir_menu(self):
    menu = self.dir_menu
    menu.delete(0, 'end')
    if self.cwd == "": # Drive list
      self.dir_menubutton.configure(text = "My Computer")
    else:
      head, tail = os.path.split(self.cwd)
      if not tail:
        tail = head
        head = ""
      self.dir_menubutton.configure(text = tail)
      n = 1
      while head:
        head, tail = os.path.split(head)
        if not tail:
          tail = head
          head = ""
        menu.add_command(label = tail, 
                         command = lambda self = self, n = n: self.go_up(n))
        n = n + 1
      if sys.platform == "win32":
        menu.add_command(label = "My Computer",
                         command = self.go_to_drive_list)

  def go_up(self, n):
    path = self.cwd
    while n:
      path, _ = os.path.split(path)
      n = n - 1
    self.set_cwd(path)
  
  def go_to_drive_list(self):
    self.set_cwd("")

  def open_dir(self, *_):
    box = self.dir_listbox
    sel = box.curselection()
    if sel:
      i = int(sel[0])
      name = box.get(i)
      path = os.path.join(self.cwd, name)
      if os.path.isdir(path):
        self.set_cwd(path)

  #def update_listboxes(self):
  #  self.update_dir_listbox()
  #  for box in self.selected_files_boxes:
  #    box.update_listbox()
    
  def update_dir_listbox(self):
    cwd = self.cwd
    box = self.dir_listbox
    self.cwd_contents.sort()
    box.delete(0, 'end')
    for name in self.cwd_contents:
      path = os.path.join(cwd, name)
      if self.passes_filter(path) and not self.file_selected(path):
        box.insert('end', name)

  def passes_filter(self, path):
    if self.cwd == "": # drive list
      return 1
    elif os.path.isdir(path):
      return 1
    elif self.filtering.get():
      base, ext = os.path.splitext(path)
      return string.lower(ext) in self.suffixes
    else:
      return 1

  def file_selected(self, path):
    for box in self.selected_files_boxes:
      if box.file_selected(path):
        return 1
    return 0

  def filtering_changed(self, *_):
    self.update_dir_listbox()

  def get_selected_files(self):
    "Get list of full pathnames of files selected in the dir listbox."
    result = []
    if self.cwd != "": # drive list
      box = self.dir_listbox
      for i in box.curselection():
        name = box.get(i)
        path = os.path.join(self.cwd, name)
        if os.path.isfile(path):
          result.append(path)
    return result

  def ok(self):
    sfbs = self.selected_files_boxes
    file_lists = [
      [(filename, identify_abf_channel(filename))
        for filename in box.cur_files]
      for box in self.selected_files_boxes
    ]
    self.project.set_data_files(file_lists)
    self.destroy()

  def cancel(self):
    self.destroy()
  

class SelectedFilesBox(Frame):

  titles = ("Varying Step:", "Varying Pre-Step:")

  def __init__(self, parent, experiment_number):
    Frame.__init__(self, parent, borderwidth = 0)
    self.parent = parent
    self.experiment_number = experiment_number
    self.cur_files = parent.project.get_data_files(experiment_number)[:]
    title = self.titles[experiment_number]
    head = group_label(self, title)
    lbox = self.listbox = make_listbox(self, height = 8)
    badd = Button(self, text = ">>", command = self.add)
    brmv = Button(self, text = "<<", command = self.remove)
    grid([0,    head],
         [badd, lbox],
         [brmv, lbox])
    self.update_listbox()

  def destroy(self):
    self.parent = None
    Frame.destroy(self)

  def update_listboxes(self):
    self.parent.update_dir_listbox()
    self.update_listbox()
    
  def update_listbox(self):
    box = self.listbox
    self.cur_files.sort()
    box.delete(0, 'end')
    for path in self.cur_files:
      dir, name = os.path.split(path)
      box.insert('end', name)

  def file_selected(self, path):
    return path in self.cur_files

  def add(self):
    self.cur_files.extend(self.parent.get_selected_files())
    self.update_listboxes()

  def remove(self):
    selection = self.listbox.curselection()
    old_files = self.cur_files
    new_files = []
    for i in xrange(len(old_files)):
      if str(i) not in selection:
        new_files.append(old_files[i])
    self.cur_files = new_files
    self.update_listboxes()


def get_drive_list():
  result = []
  for i in range(26):
    d = "%s:/" % chr(ord('A') + i)
    if os.path.exists(d):
      result.append(d)
  return result

drive_list = get_drive_list()
