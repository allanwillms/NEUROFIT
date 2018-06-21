############################################################
#
#  ABFUtils.py - Neurofit, ABF-related routines module
#  ===========   Greg Ewing, Sep 2001
#
############################################################

import os
from Tkinter import Toplevel, Label, Listbox, Button

from ABFInter import get_abf_info, is_abf_file

def identify_abf_channel(filename):
  """
  Attempt to intuit which channel of an ABF file contains
  the electric current data, by looking for a unit name
  containing the letter "A". If in doubt, ask the user. 
  Returns None for non-ABF files.
  """
  channel = None
  if is_abf_file(filename):
    channels, _ = get_abf_info(filename)
    for i in range(len(channels)):
      _, _, units = channels[i]
      if 'A' in units or 'a' in units:
        if channel is None:
          channel = i
        else:
          channel = None
          break
    #print "DataFileSelector.identify_channel:" ###
    #print "...file =", filename
    #print "...channel =", channel
    if channel is None:
      dlog = ABFChannelSelector(filename, channels)
      dlog.present()
      channel = dlog.channel
  return channel

class ABFChannelSelector(Toplevel):

  def __init__(self, pathname, channels):
    Toplevel.__init__(self)
    x = (self.winfo_screenwidth() - 200) / 2
    y = (self.winfo_screenheight() - 200) / 2
    self.geometry("+%d+%d" % (x, y))
    self.channels = channels
    header = Label(self, text = "Select channel from '%s':" %
      os.path.basename(pathname), width = 30, anchor = 'w')
    self.chanbox = Listbox(self, width = 30, height = 5, bg = 'white')
    ok_button = Button(self, text = "OK", command = self.ok)
    cancel_button = Button(self, text = "Cancel", command = self.cancel)
    header.pack(side = 'top', padx = 10, pady = 10)
    self.chanbox.pack(side = 'top', padx = 10)
    cancel_button.pack(side = 'right', padx = 10, pady = 10)
    ok_button.pack(side = 'right')
    for n, name, units in channels:
      self.chanbox.insert('end', "%d  %-10s  %-8s" % (n+1, name, units))
    self.channel = None
    self.tkraise()
    self.focus_set()
  
  def present(self):
    self.wait_window()
  
  def ok(self):
    sel = self.chanbox.curselection()
    if sel:
      self.channel = self.channels[int(sel[0])][0]
    self.destroy()
  
  def cancel(self):
    self.destroy()

#
#  Testing
#

if __name__ == "__main__":
  from Tkinter import Tk
  tk = Tk()
  tk.wm_withdraw()
  dlog = ABFChannelSelector("foo.abf", 
    [(17, "Cheese", "kg"), (42, "Marmite", "ml")])
  dlog.present()
  print "Result:", dlog.channel
