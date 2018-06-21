print "Testing ABFInter"

import sys
sys.path.insert(0, "..")
from ABFInter import get_abf_info, read_abf_file

filenames = [
  "../../data/abf/99317000.dat",
  "../../data/abf/99317001.dat",
  "../../data/abf/99317002.dat",
  #"../data/98o16000.abf", # bad
  #"../data/99421000.ABF", # good
  #"../data/99421001.ABF", # bad
  #"../data/Minis01.abf"   # bad
]

if len(sys.argv) >= 2:
  n = int(sys.argv[1])
else:
  n = 1

info = get_abf_info(filenames[n])
#print "Info:", info
channels, dac_units, epochs = info

log_chan = None
print "Channels:"
for i, name, units in channels:
  print i, name, units
  if name == "Current" or "A" in units:
    log_chan = i

print "DAC Units:", dac_units

print "Epochs:"
for type, init_level, level_inc, init_dur, dur_inc in epochs:
  print "%d %12g %12g %12d %12d" % (type, init_level, level_inc, init_dur, dur_inc)

if log_chan is None:
  print "Current channel not found"
else:
  x = read_abf_file(filenames[n], log_chan)
  times, sweeps = x
  print "Times len:", len(times)
  print "times:", times[:5]
  print "Num sweeps:", len(sweeps)
  i = 0
  while i < 5 and i < len(sweeps):
    print "Sweep", i, "len", len(sweeps[i]), ":", sweeps[i][:5]
    i = i + 1