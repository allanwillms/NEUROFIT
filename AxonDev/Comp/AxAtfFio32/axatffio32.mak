# Microsoft Developer Studio Generated NMAKE File, Based on axatffio32.dsp
!IF "$(CFG)" == ""
CFG=axatffio32 - Win32 Release
!MESSAGE No configuration specified. Defaulting to axatffio32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "axatffio32 - Win32 Release" && "$(CFG)" !=\
 "axatffio32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "axatffio32.mak" CFG="axatffio32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "axatffio32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "axatffio32 - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "axatffio32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\axatffio32.dll"

!ELSE 

ALL : "$(OUTDIR)\axatffio32.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Axatffio32.obj"
	-@erase "$(INTDIR)\axatffio32.pch"
	-@erase "$(INTDIR)\Axatffio32.res"
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\Initlize.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\Wincpp.obj"
	-@erase "$(OUTDIR)\axatffio32.dll"
	-@erase "$(OUTDIR)\axatffio32.exp"
	-@erase "$(OUTDIR)\axatffio32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WIN32_LEAN_AND_MEAN" /D "_WINDLL" /D "MAK_AXATFFIO32_DLL"\
 /Fp"$(INTDIR)\axatffio32.pch" /Yu"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\Release/
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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\Axatffio32.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\axatffio32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\axatffio32.pdb" /machine:I386 /nodefaultlib:"LIBCMTD"\
 /def:".\axatffio32.def" /out:"$(OUTDIR)\axatffio32.dll"\
 /implib:"$(OUTDIR)\axatffio32.lib" 
DEF_FILE= \
	".\axatffio32.def"
LINK32_OBJS= \
	"$(INTDIR)\Axatffio32.obj" \
	"$(INTDIR)\Axatffio32.res" \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\Initlize.obj" \
	"$(INTDIR)\Wincpp.obj" \
	"..\..\lib\AxonValidation.lib"

"$(OUTDIR)\axatffio32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\.\Release
TargetName=axatffio32
SOURCE=$(InputPath)
PostBuild_Desc=Copying DLL & LIB
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\axatffio32.dll"
   \AxonDev\Tools\Update .\.\Release\axatffio32 dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "axatffio32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\axatffio32.dll"

!ELSE 

ALL : "$(OUTDIR)\axatffio32.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Axatffio32.obj"
	-@erase "$(INTDIR)\axatffio32.pch"
	-@erase "$(INTDIR)\Axatffio32.res"
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\Initlize.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\Wincpp.obj"
	-@erase "$(OUTDIR)\axatffio32.dll"
	-@erase "$(OUTDIR)\axatffio32.exp"
	-@erase "$(OUTDIR)\axatffio32.ilk"
	-@erase "$(OUTDIR)\axatffio32.lib"
	-@erase "$(OUTDIR)\axatffio32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MTd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "_WINDLL" /D "MAK_AXATFFIO32_DLL"\
 /Fp"$(INTDIR)\axatffio32.pch" /Yu"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\Debug/
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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\Axatffio32.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\axatffio32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\axatffio32.pdb" /debug /machine:I386 /nodefaultlib:"LIBCMT"\
 /def:".\axatffio32.def" /out:"$(OUTDIR)\axatffio32.dll"\
 /implib:"$(OUTDIR)\axatffio32.lib" 
DEF_FILE= \
	".\axatffio32.def"
LINK32_OBJS= \
	"$(INTDIR)\Axatffio32.obj" \
	"$(INTDIR)\Axatffio32.res" \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\Initlize.obj" \
	"$(INTDIR)\Wincpp.obj" \
	"..\..\lib\AxonValidation.lib"

"$(OUTDIR)\axatffio32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\.\Debug
TargetName=axatffio32
SOURCE=$(InputPath)
PostBuild_Desc=Copying DLL & LIB
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\axatffio32.dll"
   \AxonDev\Tools\Update .\.\Debug\axatffio32 dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(CFG)" == "axatffio32 - Win32 Release" || "$(CFG)" ==\
 "axatffio32 - Win32 Debug"
SOURCE=.\Axatffio32.cpp

!IF  "$(CFG)" == "axatffio32 - Win32 Release"

DEP_CPP_AXATF=\
	"..\axonvalidation\axonvalidation.h"\
	"..\common\axodebug.h"\
	".\Atfintl.h"\
	".\Atfutil.h"\
	".\axatffio32.H"\
	

"$(INTDIR)\Axatffio32.obj" : $(SOURCE) $(DEP_CPP_AXATF) "$(INTDIR)"\
 "$(INTDIR)\axatffio32.pch"


!ELSEIF  "$(CFG)" == "axatffio32 - Win32 Debug"

DEP_CPP_AXATF=\
	"..\axonvalidation\axonvalidation.h"\
	"..\common\axodebug.h"\
	".\Atfintl.h"\
	".\Atfutil.h"\
	".\axatffio32.H"\
	

"$(INTDIR)\Axatffio32.obj" : $(SOURCE) $(DEP_CPP_AXATF) "$(INTDIR)"\
 "$(INTDIR)\axatffio32.pch"


!ENDIF 

SOURCE=.\Axatffio32.rc
DEP_RSC_AXATFF=\
	".\atferror.str"\
	".\axatffio32.H"\
	".\AXATFFIO32.ICO"\
	".\axatffio32.rc2"\
	

"$(INTDIR)\Axatffio32.res" : $(SOURCE) $(DEP_RSC_AXATFF) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\Common\Axodebug.cpp
DEP_CPP_AXODE=\
	"..\common\axodebug.h"\
	

"$(INTDIR)\Axodebug.obj" : $(SOURCE) $(DEP_CPP_AXODE) "$(INTDIR)"\
 "$(INTDIR)\axatffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\fileio.CPP

!IF  "$(CFG)" == "axatffio32 - Win32 Release"

DEP_CPP_FILEI=\
	"..\common\axodebug.h"\
	".\Atfintl.h"\
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"\
 "$(INTDIR)\axatffio32.pch"


!ELSEIF  "$(CFG)" == "axatffio32 - Win32 Debug"

DEP_CPP_FILEI=\
	"..\common\axodebug.h"\
	".\Atfintl.h"\
	".\axatffio32.H"\
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"\
 "$(INTDIR)\axatffio32.pch"


!ENDIF 

SOURCE=.\Initlize.cpp

!IF  "$(CFG)" == "axatffio32 - Win32 Release"

DEP_CPP_INITL=\
	"..\common\axodebug.h"\
	".\axatffio32.H"\
	

"$(INTDIR)\Initlize.obj" : $(SOURCE) $(DEP_CPP_INITL) "$(INTDIR)"\
 "$(INTDIR)\axatffio32.pch"


!ELSEIF  "$(CFG)" == "axatffio32 - Win32 Debug"

DEP_CPP_INITL=\
	"..\common\axodebug.h"\
	".\axatffio32.H"\
	

"$(INTDIR)\Initlize.obj" : $(SOURCE) $(DEP_CPP_INITL) "$(INTDIR)"\
 "$(INTDIR)\axatffio32.pch"


!ENDIF 

SOURCE=.\Wincpp.cpp

!IF  "$(CFG)" == "axatffio32 - Win32 Release"

DEP_CPP_WINCP=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\Wincpp.hpp"\
	
CPP_SWITCHES=/nologo /Zp4 /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WIN32_LEAN_AND_MEAN" /D "_WINDLL" /D "MAK_AXATFFIO32_DLL"\
 /Fp"$(INTDIR)\axatffio32.pch" /Yc"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 

"$(INTDIR)\Wincpp.obj"	"$(INTDIR)\axatffio32.pch" : $(SOURCE) $(DEP_CPP_WINCP)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "axatffio32 - Win32 Debug"

DEP_CPP_WINCP=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\Wincpp.hpp"\
	
CPP_SWITCHES=/nologo /Zp4 /MTd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "_WINDLL" /D "MAK_AXATFFIO32_DLL"\
 /Fp"$(INTDIR)\axatffio32.pch" /Yc"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 

"$(INTDIR)\Wincpp.obj"	"$(INTDIR)\axatffio32.pch" : $(SOURCE) $(DEP_CPP_WINCP)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

