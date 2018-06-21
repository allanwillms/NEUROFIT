############################################################
#
#  Layout.py - NEUROFIT, widget layout module
#  =========   Greg Ewing, Jan 2001
#
############################################################

from Tkinter import *

import sys
params = __import__("Layout_" + sys.platform)

def grid(*rows, **kw):
  """Lay out a matrix of widgets in a grid within their parent.
  Widgets may be repeated in the matrix to span more than one
  row or column. A false value in the matrix leaves an empty cell.
  If a widget has a 'sticky' attribute, it is used for the 'sticky'
  grid configuration option. Keyword arguments are passed on to
  the Tk grid manager for each widget."""
  done = []
  nrows = len(rows)
  rownum = 0
  while rownum < nrows:
    ncols = len(rows[rownum])
    colnum = 0
    while colnum < ncols:
      w = rows[rownum][colnum]
      rowspan = 1
      colspan = 1
      if w and w not in done:
        done.append(w)
        while rownum + rowspan < nrows and rows[rownum + rowspan][colnum] == w:
          rowspan = rowspan + 1
        while colnum + colspan < ncols and rows[rownum][colnum + colspan] == w:
          colspan = colspan + 1
        w.grid(row = rownum, col = colnum,
               rowspan = rowspan, columnspan = colspan,
               **kw)
        if hasattr(w, 'sticky'):
          w.grid(sticky = w.sticky)
      colnum = colnum + colspan
    rownum = rownum + 1

def weight_columns(widget, *weights):
  """Set grid manager column weights for the given widget."""
  for i in range(len(weights)):
    widget.columnconfigure(i, weight = weights[i])

def configure_columns(widget, minsizes = None, weights = None):
  if minsizes:
    for i in range(len(minsizes)):
      widget.columnconfigure(i, minsize = minsizes[i])
  if weights:
    for i in range(len(weights)):
      widget.columnconfigure(i, weight = weights[i])
    

def vbox(items):
  """Lay out a list of widgets in a vertical column within their parent.
  If a widget has a 'sticky' attribute, it is used for the 'sticky'
  grid configuration option."""
  row = 0
  for w in items:
    w.grid(row = row)
    if hasattr(w, 'sticky'):
      w.grid(sticky = w.sticky)
    row = row + 1

def group_label(parent, text):
  """Create a Label widget suitable for labelling a group of other
  widgets."""
  w = Label(parent, text = text, font = params.group_label_font, anchor = 'w')
  w.sticky = 'w'
  return w

def entry_label(parent, text, **kw):
  """Create a Label widget suitable for labelling an Entry widget."""
  w = Label(parent, text = text, font = params.entry_label_font, anchor = 'w')
  w.configure(kw)
  w.sticky = 'w'
  return w

def column_label(parent, text):
  """Create a Label widget suitable for labelling a table column."""
  return entry_label(parent, text)
