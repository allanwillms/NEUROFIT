############################################################
#
#  Scrolling.py - NEUROFIT, scrolled widgets
#  ============   Greg Ewing, Jan 2001
#
############################################################

from Tkinter import *

from Layout import grid

#------------------------------------------------------------
#
#   List box with vertical scrolling
#
#------------------------------------------------------------

class ScrolledListbox(Frame):

  def __init__(self, parent, double_click = None, **args):
    Frame.__init__(self, parent)
    box = self.listbox = Listbox(self)
    box.configure(args)
    bar = self.vscrollbar = Scrollbar(self, orient = 'v', command = box.yview)
    box.configure(yscrollcommand = bar.set)
    bar.sticky = 'ns'
    grid([box, bar])
    if double_click:
      box.bind("<Double-Button-1>", double_click)
    self.insert = box.insert
    self.delete = box.delete
    self.get = box.get
    self.curselection = box.curselection
