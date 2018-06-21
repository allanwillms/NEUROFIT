#
#   Neurofit: Top-level Makefile
#

DISTZIP = NEUROFITInstall.zip
SRCZIP = NEUROFITsrc.zip

all:
	@echo 'Targets:'
	@echo 'ext'    -- compile extension modules'
	@echo 'bin     -- build standalone binary distribution'
	@echo 'clean   -- remove object files, etc.'
	@echo 'src     -- build source package'

ext:
	CD py & $(MAKE) ext

bin:
	CD py & $(MAKE) bin

clean:
	CD py & $(MAKE) /I clean
	DEL $(DISTZIP)

src:
	@echo 'You should run  nmake clean  first to minimize size'
	zip -r $(SRCZIP) NEUROFIT_README.TXT Makefile py AxonDev