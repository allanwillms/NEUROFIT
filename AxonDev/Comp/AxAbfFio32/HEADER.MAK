# Microsoft Developer Studio Generated NMAKE File, Based on HEADER.DSP
!IF "$(CFG)" == ""
CFG=Header - Win32 Release
!MESSAGE No configuration specified. Defaulting to Header - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Header - Win32 Release" && "$(CFG)" != "Header - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HEADER.MAK" CFG="Header - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Header - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Header - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Header - Win32 Release"

OUTDIR=.\HeaderRelease
INTDIR=.\HeaderRelease
# Begin Custom Macros
OutDir=.\.\HeaderRelease
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\HEADER.exe"

!ELSE 

ALL : "Abflib - Win32 Release" "$(OUTDIR)\HEADER.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Abflib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\Cparam.obj"
	-@erase "$(INTDIR)\Header.obj"
	-@erase "$(INTDIR)\HEADER.pch"
	-@erase "$(INTDIR)\header.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\Wincpp.obj"
	-@erase "$(OUTDIR)\HEADER.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MT /W4 /Gi /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\HEADER.pch" /Yu"wincpp.hpp"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\HeaderRelease/
CPP_SBRS=.

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

RSC=rc.exe
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\header.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HEADER.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oldnames.lib kernel32.lib user32.lib gdi32.lib winmm.lib\
 winspool.lib version.lib setargv.obj /nologo /stack:0x1400 /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)\HEADER.pdb" /machine:IX86\
 /out:"$(OUTDIR)\HEADER.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\Cparam.obj" \
	"$(INTDIR)\Header.obj" \
	"$(INTDIR)\header.res" \
	"$(INTDIR)\Version.obj" \
	"$(INTDIR)\Wincpp.obj" \
	"..\..\Lib\Abflib.lib" \
	"..\AxonValidation\AxonValidation.lib" \
	".\LibRelease\Abflib.lib"

"$(OUTDIR)\HEADER.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\.\HeaderRelease
TargetName=HEADER
SOURCE=$(InputPath)
PostBuild_Desc=Copying EXE to bin
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\HeaderRelease
# End Custom Macros

$(DS_POSTBUILD_DEP) : "Abflib - Win32 Release" "$(OUTDIR)\HEADER.exe"
   \AxonDev\Tools\Update .\.\HeaderRelease\HEADER exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Header - Win32 Debug"

OUTDIR=.\HeaderDebug
INTDIR=.\HeaderDebug
# Begin Custom Macros
OutDir=.\.\HeaderDebug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\HEADER.exe"

!ELSE 

ALL : "Abflib - Win32 Debug" "$(OUTDIR)\HEADER.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Abflib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\Cparam.obj"
	-@erase "$(INTDIR)\Header.obj"
	-@erase "$(INTDIR)\HEADER.pch"
	-@erase "$(INTDIR)\header.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\Wincpp.obj"
	-@erase "$(OUTDIR)\HEADER.exe"
	-@erase "$(OUTDIR)\HEADER.ilk"
	-@erase "$(OUTDIR)\HEADER.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MTd /W4 /Gm /Zi /Od /Gy /D "_DEBUG" /D "WIN32" /D\
 "_CONSOLE" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\HEADER.pch" /Yu"wincpp.hpp"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\HeaderDebug/
CPP_SBRS=.

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

RSC=rc.exe
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\header.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HEADER.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oldnames.lib kernel32.lib user32.lib gdi32.lib winmm.lib\
 winspool.lib version.lib setargv.obj /nologo /stack:0x1400 /subsystem:console\
 /incremental:yes /pdb:"$(OUTDIR)\HEADER.pdb" /debug /machine:IX86\
 /out:"$(OUTDIR)\HEADER.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\Cparam.obj" \
	"$(INTDIR)\Header.obj" \
	"$(INTDIR)\header.res" \
	"$(INTDIR)\Version.obj" \
	"$(INTDIR)\Wincpp.obj" \
	"..\..\Lib\Abflib.lib" \
	"..\AxonValidation\AxonValidation.lib" \
	".\LibDebug\Abflib.lib"

"$(OUTDIR)\HEADER.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\.\HeaderDebug
TargetName=HEADER
SOURCE=$(InputPath)
PostBuild_Desc=Copying EXE to bin
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\HeaderDebug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "Abflib - Win32 Debug" "$(OUTDIR)\HEADER.exe"
   \AxonDev\Tools\Update .\.\HeaderDebug\HEADER exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(CFG)" == "Header - Win32 Release" || "$(CFG)" == "Header - Win32 Debug"
SOURCE=..\Common\Axodebug.cpp
DEP_CPP_AXODE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	

"$(INTDIR)\Axodebug.obj" : $(SOURCE) $(DEP_CPP_AXODE) "$(INTDIR)"\
 "$(INTDIR)\HEADER.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Cparam.cpp

!IF  "$(CFG)" == "Header - Win32 Release"

DEP_CPP_CPARA=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\AxABFFIO32.h"\
	".\CPARAM.H"\
	

"$(INTDIR)\Cparam.obj" : $(SOURCE) $(DEP_CPP_CPARA) "$(INTDIR)"\
 "$(INTDIR)\HEADER.pch"


!ELSEIF  "$(CFG)" == "Header - Win32 Debug"

DEP_CPP_CPARA=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\AxABFFIO32.h"\
	".\CPARAM.H"\
	

"$(INTDIR)\Cparam.obj" : $(SOURCE) $(DEP_CPP_CPARA) "$(INTDIR)"\
 "$(INTDIR)\HEADER.pch"


!ENDIF 

SOURCE=.\Header.cpp

!IF  "$(CFG)" == "Header - Win32 Release"

DEP_CPP_HEADE=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\version.hpp"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\ABFINFO.H"\
	".\abfutil.h"\
	".\AxABFFIO32.h"\
	".\CPARAM.H"\
	".\MSBINCVT.H"\
	

"$(INTDIR)\Header.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"\
 "$(INTDIR)\HEADER.pch"


!ELSEIF  "$(CFG)" == "Header - Win32 Debug"

DEP_CPP_HEADE=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\version.hpp"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\ABFINFO.H"\
	".\abfutil.h"\
	".\AxABFFIO32.h"\
	".\CPARAM.H"\
	".\MSBINCVT.H"\
	

"$(INTDIR)\Header.obj" : $(SOURCE) $(DEP_CPP_HEADE) "$(INTDIR)"\
 "$(INTDIR)\HEADER.pch"


!ENDIF 

SOURCE=.\header.rc

"$(INTDIR)\header.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\Common\Version.cpp

!IF  "$(CFG)" == "Header - Win32 Release"

DEP_CPP_VERSI=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\version.hpp"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\Version.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"\
 "$(INTDIR)\HEADER.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Header - Win32 Debug"

DEP_CPP_VERSI=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\version.hpp"\
	

"$(INTDIR)\Version.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"\
 "$(INTDIR)\HEADER.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Wincpp.cpp

!IF  "$(CFG)" == "Header - Win32 Release"

DEP_CPP_WINCP=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\WINCPP.HPP"\
	
CPP_SWITCHES=/nologo /Zp4 /MT /W4 /Gi /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE"\
 /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\HEADER.pch" /Yc"wincpp.hpp"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Wincpp.obj"	"$(INTDIR)\HEADER.pch" : $(SOURCE) $(DEP_CPP_WINCP)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Header - Win32 Debug"

DEP_CPP_WINCP=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\WINCPP.HPP"\
	
CPP_SWITCHES=/nologo /Zp4 /MTd /W4 /Gm /Zi /Od /Gy /D "_DEBUG" /D "WIN32" /D\
 "_CONSOLE" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\HEADER.pch" /Yc"wincpp.hpp"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Wincpp.obj"	"$(INTDIR)\HEADER.pch" : $(SOURCE) $(DEP_CPP_WINCP)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

!IF  "$(CFG)" == "Header - Win32 Release"

"Abflib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Abflib.mak CFG="Abflib - Win32 Release" 
   cd "."

"Abflib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\Abflib.mak CFG="Abflib - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "Header - Win32 Debug"

"Abflib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Abflib.mak CFG="Abflib - Win32 Debug" 
   cd "."

"Abflib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\Abflib.mak CFG="Abflib - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 


!ENDIF 

