from distutils.core import setup
import py2exe

setup(
  name = "NEUROFIT",
  scripts = ["NEUROFIT.py"],
  data_files = [("images", ["images/stripes.xbm"])]
)