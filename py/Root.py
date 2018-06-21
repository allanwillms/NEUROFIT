############################################################
#
#  Root.py - NEUROFIT Tk root module
#  =======   Greg Ewing, Dec 2001
#
############################################################

import Errors
from Tkinter import Tk

#------------------------------------------------------------
#
#   Global variables
#
#------------------------------------------------------------

tk = None # The Tk root window

#------------------------------------------------------------
#
#   Classes
#
#------------------------------------------------------------

class Root(Tk):
  """
  Tk root object with error catching facilities.
  """
  
  def report_callback_exception(self, exc_type, exc_val, tb):
    if not isinstance(exc_val, Errors.Cancelled):
      Errors.report_exception(exc_type, exc_val, tb)


#------------------------------------------------------------
#
#   Functions
#
#------------------------------------------------------------

def create_root():
  """
  Create the Tk root window and withdraw it.
  """
  global tk
  tk = Root()
  tk.wm_withdraw()
  return tk

def get_root():
  return tk
