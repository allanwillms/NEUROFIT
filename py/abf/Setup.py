from distutils.core import setup, Extension
setup(name="ABFInter", version="1.0",
  ext_modules=[
    Extension("ABFInter", [
      "ABFInter.cpp"
    ],
    library_dirs = ["../../AxonDev/lib"],
    libraries = ["Axabffio32"],
    )
  ]
)