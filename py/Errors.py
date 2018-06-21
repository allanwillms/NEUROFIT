############################################################
#
#  Errors.py - Neurofit error reporting module
#  =========   Greg Ewing, Sep 2001
#
############################################################

import traceback
import tkMessageBox

class Cancelled(Exception):
  pass

def report_exception(exc_type, exc_val, tb):
  words = []
  n = 0
  for word in str(exc_val).split():
    words.append(word)
    n += len(word)
    if n >= 60:
      words.append("\n")
      n = 0
  msg = " ".join(words)
  tkMessageBox.showerror(title = "NEUROFIT: Error", message = msg)
  if tb:
    traceback.print_exception(exc_type, exc_val, tb)
  
  
