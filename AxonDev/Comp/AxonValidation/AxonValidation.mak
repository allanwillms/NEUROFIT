# Microsoft Developer Studio Generated NMAKE File, Based on AxonValidation.dsp
!IF "$(CFG)" == ""
CFG=AxonValidation - Win32 Release
!MESSAGE No configuration specified. Defaulting to AxonValidation - Win32\
 Release.
!ENDIF 

!IF "$(CFG)" != "AxonValidation - Win32 Release" && "$(CFG)" !=\
 "AxonValidation - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AxonValidation.mak" CFG="AxonValidation - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AxonValidation - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "AxonValidation - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "AxonValidation - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AxonValidation.lib"

!ELSE 

ALL : "$(OUTDIR)\AxonValidation.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\FloatFormat.obj"
	-@erase "$(INTDIR)\TextFormat.obj"
	-@erase "$(INTDIR)\Validation.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\AxonValidation.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp4 /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\AxonValidation.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AxonValidation.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\AxonValidation.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\FloatFormat.obj" \
	"$(INTDIR)\TextFormat.obj" \
	"$(INTDIR)\Validation.obj"

"$(OUTDIR)\AxonValidation.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

OutDir=.\.\Release
TargetName=AxonValidation
SOURCE=$(InputPath)
PostBuild_Desc=Copying lib to \axon\lib
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\AxonValidation.lib"
   \AxonDev\Tools\update .\.\Release\AxonValidation lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "AxonValidation - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AxonValidation.lib"

!ELSE 

ALL : "$(OUTDIR)\AxonValidation.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\FloatFormat.obj"
	-@erase "$(INTDIR)\TextFormat.obj"
	-@erase "$(INTDIR)\Validation.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\AxonValidation.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp4 /MTd /W4 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\AxonValidation.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AxonValidation.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\AxonValidation.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\FloatFormat.obj" \
	"$(INTDIR)\TextFormat.obj" \
	"$(INTDIR)\Validation.obj"

"$(OUTDIR)\AxonValidation.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

OutDir=.\.\Debug
TargetName=AxonValidation
SOURCE=$(InputPath)
PostBuild_Desc=Copying lib to \axon\lib
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\AxonValidation.lib"
   \AxonDev\Tools\update .\.\Debug\AxonValidation lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "AxonValidation - Win32 Release" || "$(CFG)" ==\
 "AxonValidation - Win32 Debug"
SOURCE=..\Common\Axodebug.cpp
DEP_CPP_AXODE=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\Axodebug.obj" : $(SOURCE) $(DEP_CPP_AXODE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\FloatFormat.cpp

!IF  "$(CFG)" == "AxonValidation - Win32 Release"

DEP_CPP_FLOAT=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\win32.h"\
	".\AxonValidation.h"\
	".\wincpp.hpp"\
	

"$(INTDIR)\FloatFormat.obj" : $(SOURCE) $(DEP_CPP_FLOAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "AxonValidation - Win32 Debug"

DEP_CPP_FLOAT=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\win32.h"\
	".\AxonValidation.h"\
	".\wincpp.hpp"\
	

"$(INTDIR)\FloatFormat.obj" : $(SOURCE) $(DEP_CPP_FLOAT) "$(INTDIR)"


!ENDIF 

SOURCE=.\TextFormat.cpp

!IF  "$(CFG)" == "AxonValidation - Win32 Release"

DEP_CPP_TEXTF=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\win32.h"\
	".\AxonValidation.h"\
	".\wincpp.hpp"\
	

"$(INTDIR)\TextFormat.obj" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "AxonValidation - Win32 Debug"

DEP_CPP_TEXTF=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\win32.h"\
	".\AxonValidation.h"\
	".\wincpp.hpp"\
	

"$(INTDIR)\TextFormat.obj" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"


!ENDIF 

SOURCE=.\Validation.cpp

!IF  "$(CFG)" == "AxonValidation - Win32 Release"

DEP_CPP_VALID=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\win32.h"\
	".\AxonValidation.h"\
	".\wincpp.hpp"\
	

"$(INTDIR)\Validation.obj" : $(SOURCE) $(DEP_CPP_VALID) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "AxonValidation - Win32 Debug"

DEP_CPP_VALID=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\win32.h"\
	".\AxonValidation.h"\
	".\wincpp.hpp"\
	

"$(INTDIR)\Validation.obj" : $(SOURCE) $(DEP_CPP_VALID) "$(INTDIR)"


!ENDIF 


!ENDIF 

