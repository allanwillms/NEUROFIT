############################################################
#
#  ModelView.py - NEUROFIT, Model-View framework support
#  ============   Greg Ewing, Feb 2001
#
############################################################

import os
import tkMessageBox
import tkFileDialog
from Tkinter import Frame, Toplevel

import Errors

debug_document_changed = 0

if int(os.environ.get("DEBUG_CHANGE", 0)):
  debug_document_changed = 1
  import traceback

class ParentChild:
  """An object which can participate in a parent-child
  hierarchy. Calling destroy() on a node in the hierarchy
  also destroys its children."""

  def __init__(self, parent = None):
    self.parent = parent
    self.descendants = [] # Can't call it 'children' because of conflict
                          # with Tkinter widgets
    if parent:
      #print "ParentChild.__init__: parent =", repr(parent), \ ###
      #      "parent.descendants =", repr(parent.descendants)  ###
      parent.descendants.append(self)

  def destroy(self):
    if self.parent:
      self.parent.descendants.remove(self)
    self.parent = None
    while self.descendants:
      self.descendants[0].destroy()


class Model(ParentChild):
  """A data structure which can have one or more Views attached.
  Support is provided for notifying views and parent Models when
  the Model changes."""

  def __init__(self, parent = None):
    ParentChild.__init__(self, parent)
    self.views = []

  def destroy(self):
    self.views = []
    ParentChild.destroy(self)

  def add_view(self, view):
    self.views.append(view)
    view.model = self

  def remove_view(self, view):
    if view in self.views:
      self.views.remove(view)
    view.model = None

  def notify_views(self, message = 'update', args = ()):
    for view in self.views:
      #print "Notifying", repr(view), "with", message, "of", args ###
      method = getattr(view, message, None)
      if method:
        apply(getattr(view, message), args)

  def changed(self):
    if self.parent:
      self.parent.changed()
  
  def changed_while_opening(self):
    if self.parent:
      self.parent.changed_while_opening()


class View(ParentChild):
  """User interface element for viewing a Model."""

  def __init__(self, parent = None):
    ParentChild.__init__(self, parent)
    self.model = None

  def destroy(self):
    if self.model:
      self.model.remove_view(self)
    ParentChild.destroy(self)

  def set_model(self, model):
    if model != self.model:
      if self.model:
        self.model.remove_view(self)
      if model:
        model.add_view(self)
      self.update()

class ViewFrame(Frame, View):
  """A View which is also a Tkinter Frame."""

  def __init__(self, master, **kwargs):
    Frame.__init__(self, master, kwargs)
    parent = master
    while parent and not isinstance(parent, ParentChild):
      parent = parent.master
    View.__init__(self, parent)

  def destroy(self):
    View.destroy(self)
    Frame.destroy(self)


class Document(Model):
  """A Document can have DocumentWindows associated with it,  
  and there is support for prompting the user for a file to save 
  in, etc."""

  next_document_serial_number = 1

  def __init__(self):
    Model.__init__(self)
    self.windows = []
    self.directory = None
    self.filename = None
    self.needs_saving = 0

  def destroy(self):
    #print "Document.destroy: windows =", self.windows ###
    while self.windows:
      win = self.windows[0]
      del self.windows[0]
      win.destroy()

  def add_window(self, win):
    self.windows.append(win)
    win.document = self
    win.update_title()

  def remove_window(self, win):
    if win in self.windows:
      self.windows.remove(win)
  
  def find_window(self, klass):
    """Search Document's window list for a window of
    the specified class."""
    for win in self.windows:
      if isinstance(win, klass):
        return win
    return None

  def changed(self):
    if debug_document_changed:
      traceback.print_stack()
      print "Document.Changed called"
    self.needs_saving = 1
    Model.changed(self)
    
  def changed_while_opening(self):
    self.needs_saving_after_opening = 1
  
  def untouched(self):
    """Return true if this document is not associated with a 
    and has no unsaved changes, implying that it is
    a newly_created document that has never been used."""
    return not self.needs_saving and not self.has_file()

  def open(self, path):
    """Associate with specified file and load from that file."""
    self.set_file(path)
    f = open(path, "r")
    try:
      self.needs_saving_after_opening = 0
      self.read_from_file(f)
      self.needs_saving = 0
      if self.needs_saving_after_opening:
        self.changed()
    finally:
      f.close()

  def close(self):
    """Ask whether to save changes if necessary, then close the
    document."""
    self.save_changes()
    self.destroy()

  def save_changes(self):
    """If the document has been changed, give the user the chance to
    save. Raises Cancelled if the user cancels."""
    if self.needs_saving:
      name = self.get_filename()
      answer = tkMessageBox._show(
        message = "Save changes to '%s'?" % name,
        icon = 'warning', type = 'yesnocancel')
      if answer == 'cancel':
        raise Errors.Cancelled
      if answer == 'yes':
        self.save()

  def save(self):
    """Ask user for filename if necessary, then save."""
    if not self.has_file():
      self.get_file_from_user()
    path = self.get_pathname()
    temp_path = path + ".tmp"
    f = open(temp_path, "w")
    self.write_to_file(f)
    f.close()
    os.rename(temp_path, path)

  def save_as(self):
    """Ask user for filename, then save."""
    self.get_file_from_user()
    self.save()

  def save(self):
    """Ask user for filename if necessary, then save."""
    if not self.has_file():
      self.get_file_from_user()
    path = self.get_pathname()
    #print "Document.save: path =", repr(path) ###
    temp_path = path + ".tmp"
    f = open(temp_path, "w")
    try:
      self.write_to_file(f)
    finally:
      f.close()
    if os.path.exists(path):
      os.unlink(path)
    #print "Model.save: renaming:" ###
    #print "   temp_path =", temp_path ###
    #print "   path =", path ###
    os.rename(temp_path, path)
    self.needs_saving = 0

  def get_file_from_user(self):
    """Ask user for filename to save in. Raises Cancelled if the
    user cancels."""
    dir = self.get_directory()
    name = self.get_filename()
    types = (
      ("Project Files", (".nfp")),
      ("All Files", ("*")))
    path = tkFileDialog.asksaveasfilename(
      title = "Save project as:",
      defaultextension = ".nfp", filetypes = types,
      initialdir = dir, initialfile = name)
    if not path:
      raise Errors.Cancelled
    self.set_file(path)

  def set_file(self, path):
    """Associate the document with the specified file."""
    self.directory, self.filename = os.path.split(path)
    for window in self.windows:
      window.update_title()

  def get_directory(self):
    """Return the directory of the associated file, or None."""
    return self.directory

  def get_filename(self):
    """Return the last pathname component of the associated file.
    A new name is generated if the document is not yet associated
    with a file."""
    if not self.filename:
      n = Document.next_document_serial_number
      Document.next_document_serial_number = n + 1
      self.filename = "Untitled-%d" % n
    return self.filename

  def get_pathname(self):
    """Return the pathname of the associated file, or None."""
    if self.has_file():
      return os.path.join(self.directory, self.get_filename())
    else:
      return None

  def has_file(self):
    """Test whether the document is associated with a file."""
    return self.directory is not None

                                          
class DocumentWindow(Toplevel):
  """Window which can be associated with a Document. The title is 
  kept up to date with the document's filename, and the window is 
  closed when the Document is destroyed."""
  
  def __init__(self, *args, **kw):
    Toplevel.__init__(self, *args, **kw)
    self.document = None
  
  def destroy(self):
    if self.document:
      self.document.remove_window(self)
    Toplevel.destroy(self)


    
