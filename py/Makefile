#
#   py/Makefile -- Neurofit: Python sources & extension modules
#

DISTZIP = NEUROFITInstall.zip

all:
	@echo 'Specify target:'
	@echo '  ext     -- compile extension modules'
	@echo '  bin     -- build standalone binary distribution'
	@echo '  clean   -- remove object files, etc.'

ext:
	cd lscode & $(MAKE)
	cd abf & $(MAKE)

bin: ext exe zip

exe:
	C:\Python22\python setup.py py2exe --windows --includes Layout_win32

zip:
	CD .. & zip -r py\$(DISTZIP) NEUROFIT_README.TXT
	MOVE Doc dist\NEUROFIT
	CD dist & zip -r ..\$(DISTZIP) NEUROFIT
	MOVE dist\NEUROFIT\Doc .
	MOVE /Y $(DISTZIP) ..\$(DISTZIP)

clean:
	DEL /Q *.pyd *.pyc
	RMDIR /S/Q build
	RMDIR /S/Q dist
	cd lscode & $(MAKE) /I clean
	cd abf & $(MAKE) /I clean
	