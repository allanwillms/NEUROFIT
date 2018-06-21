############################################################
#
#  Widgets.py - NEUROFIT, miscellaneous widgets
#  ==========   Greg Ewing, Jan 2001
#
############################################################

from Tkinter import *

class NumberEntry(Entry):
  """
  Box for entering a numeric value. May be set read_only.
  """
  #TODO: Should be merged with NumberViews.NumberView

  def __init__(self, parent, initial = "", width = 11):
    Entry.__init__(self, parent, width = width, background = 'white')
    self.insert(0, initial)

  def set(self, value):
    state = self.cget('state')
    self.configure(state = 'normal')
    self.delete(0, 'end')
    if value is not None:
      self.insert(0, value)
    self.configure(state = state)

  def set_readonly(self, readonly):
    if readonly:
      self.configure(state = 'disabled')
      self.configure(background = 'grey')
    else:
      self.configure(state = 'normal')
      self.configure(background = 'white')
  
  def select_all(self):
    self.selection_range(0, 'end')
    self.focus_set()


class NumberDisplay(NumberEntry):
  """Box for displaying a numeric value. May also have a
  callback for when it is clicked."""

  def __init__(self, parent, initial = "", on_click = None, width = 11):
    NumberEntry.__init__(self, parent, initial, width)
    self.set_readonly(1)
    if on_click:
      self.bind('<Button-1>', on_click)
