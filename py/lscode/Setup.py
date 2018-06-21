from distutils.core import setup, Extension
setup(name="LSInter", version="1.0",
  ext_modules=[
    Extension("LSInter", [
      "LSInter.c",
      "auxfuncs.c",
      "enorm.c", 
      "fitlsdata.c",
      "hhftvc.c",
      "lmder.c",
      "lmpar.c",
      "messages.c",
      "parmfuncs.c",
      "qrfac.c",
      "qrsolv.c"
    ])
  ]
)