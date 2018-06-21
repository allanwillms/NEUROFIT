# Microsoft Developer Studio Generated NMAKE File, Based on Axabffio32.dsp
!IF "$(CFG)" == ""
CFG=axabffio32 - Win32 Release
!MESSAGE No configuration specified. Defaulting to axabffio32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "axabffio32 - Win32 Release" && "$(CFG)" !=\
 "axabffio32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Axabffio32.mak" CFG="axabffio32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "axabffio32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "axabffio32 - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Axabffio32.dll"

!ELSE 

ALL : "$(OUTDIR)\Axabffio32.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Abffiles.obj"
	-@erase "$(INTDIR)\Abfheadr.obj"
	-@erase "$(INTDIR)\Abfhwave.obj"
	-@erase "$(INTDIR)\Abfinfo.obj"
	-@erase "$(INTDIR)\Abfutil.obj"
	-@erase "$(INTDIR)\Abfvtags.obj"
	-@erase "$(INTDIR)\Axabffio32.obj"
	-@erase "$(INTDIR)\Axabffio32.pch"
	-@erase "$(INTDIR)\axabffio32.res"
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\BufferedArray.obj"
	-@erase "$(INTDIR)\Csynch.obj"
	-@erase "$(INTDIR)\DACFile.obj"
	-@erase "$(INTDIR)\Filedesc.obj"
	-@erase "$(INTDIR)\FileIO.obj"
	-@erase "$(INTDIR)\FileReadCache.obj"
	-@erase "$(INTDIR)\Msbincvt.obj"
	-@erase "$(INTDIR)\Oldheadr.obj"
	-@erase "$(INTDIR)\pkware.obj"
	-@erase "$(INTDIR)\StringResource.obj"
	-@erase "$(INTDIR)\TextBuffer.obj"
	-@erase "$(INTDIR)\UserList.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\VERSION.OBJ"
	-@erase "$(INTDIR)\Voicetag.obj"
	-@erase "$(INTDIR)\Wincpp.obj"
	-@erase "$(OUTDIR)\Axabffio32.dll"
	-@erase "$(OUTDIR)\Axabffio32.exp"
	-@erase "$(OUTDIR)\Axabffio32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WIN32_LEAN_AND_MEAN" /D "MAK_AXABFFIO32_DLL" /Fp"$(INTDIR)\Axabffio32.pch"\
 /Yu"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\axabffio32.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Axabffio32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winmm.lib version.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Axabffio32.pdb"\
 /machine:I386 /nodefaultlib:"LIBCMTD" /def:".\axabffio32.def"\
 /out:"$(OUTDIR)\Axabffio32.dll" /implib:"$(OUTDIR)\Axabffio32.lib" 
DEF_FILE= \
	".\axabffio32.def"
LINK32_OBJS= \
	"$(INTDIR)\Abffiles.obj" \
	"$(INTDIR)\Abfheadr.obj" \
	"$(INTDIR)\Abfhwave.obj" \
	"$(INTDIR)\Abfinfo.obj" \
	"$(INTDIR)\Abfutil.obj" \
	"$(INTDIR)\Abfvtags.obj" \
	"$(INTDIR)\Axabffio32.obj" \
	"$(INTDIR)\axabffio32.res" \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\BufferedArray.obj" \
	"$(INTDIR)\Csynch.obj" \
	"$(INTDIR)\DACFile.obj" \
	"$(INTDIR)\Filedesc.obj" \
	"$(INTDIR)\FileIO.obj" \
	"$(INTDIR)\FileReadCache.obj" \
	"$(INTDIR)\Msbincvt.obj" \
	"$(INTDIR)\Oldheadr.obj" \
	"$(INTDIR)\pkware.obj" \
	"$(INTDIR)\StringResource.obj" \
	"$(INTDIR)\TextBuffer.obj" \
	"$(INTDIR)\UserList.obj" \
	"$(INTDIR)\VERSION.OBJ" \
	"$(INTDIR)\Voicetag.obj" \
	"$(INTDIR)\Wincpp.obj" \
	"..\..\lib\AxonValidation.lib"

"$(OUTDIR)\Axabffio32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\.\Release
TargetName=Axabffio32
SOURCE=$(InputPath)
PostBuild_Desc=Copying DLL & LIB
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Axabffio32.dll"
   \AxonDev\Tools\Update .\.\Release\Axabffio32 dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Axabffio32.dll" "$(OUTDIR)\Axabffio32.bsc"

!ELSE 

ALL : "$(OUTDIR)\Axabffio32.dll" "$(OUTDIR)\Axabffio32.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Abffiles.obj"
	-@erase "$(INTDIR)\Abffiles.sbr"
	-@erase "$(INTDIR)\Abfheadr.obj"
	-@erase "$(INTDIR)\Abfheadr.sbr"
	-@erase "$(INTDIR)\Abfhwave.obj"
	-@erase "$(INTDIR)\Abfhwave.sbr"
	-@erase "$(INTDIR)\Abfinfo.obj"
	-@erase "$(INTDIR)\Abfinfo.sbr"
	-@erase "$(INTDIR)\Abfutil.obj"
	-@erase "$(INTDIR)\Abfutil.sbr"
	-@erase "$(INTDIR)\Abfvtags.obj"
	-@erase "$(INTDIR)\Abfvtags.sbr"
	-@erase "$(INTDIR)\Axabffio32.obj"
	-@erase "$(INTDIR)\Axabffio32.pch"
	-@erase "$(INTDIR)\axabffio32.res"
	-@erase "$(INTDIR)\Axabffio32.sbr"
	-@erase "$(INTDIR)\Axodebug.obj"
	-@erase "$(INTDIR)\Axodebug.sbr"
	-@erase "$(INTDIR)\BufferedArray.obj"
	-@erase "$(INTDIR)\BufferedArray.sbr"
	-@erase "$(INTDIR)\Csynch.obj"
	-@erase "$(INTDIR)\Csynch.sbr"
	-@erase "$(INTDIR)\DACFile.obj"
	-@erase "$(INTDIR)\DACFile.sbr"
	-@erase "$(INTDIR)\Filedesc.obj"
	-@erase "$(INTDIR)\Filedesc.sbr"
	-@erase "$(INTDIR)\FileIO.obj"
	-@erase "$(INTDIR)\FileIO.sbr"
	-@erase "$(INTDIR)\FileReadCache.obj"
	-@erase "$(INTDIR)\FileReadCache.sbr"
	-@erase "$(INTDIR)\Msbincvt.obj"
	-@erase "$(INTDIR)\Msbincvt.sbr"
	-@erase "$(INTDIR)\Oldheadr.obj"
	-@erase "$(INTDIR)\Oldheadr.sbr"
	-@erase "$(INTDIR)\pkware.obj"
	-@erase "$(INTDIR)\pkware.sbr"
	-@erase "$(INTDIR)\StringResource.obj"
	-@erase "$(INTDIR)\StringResource.sbr"
	-@erase "$(INTDIR)\TextBuffer.obj"
	-@erase "$(INTDIR)\TextBuffer.sbr"
	-@erase "$(INTDIR)\UserList.obj"
	-@erase "$(INTDIR)\UserList.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\VERSION.OBJ"
	-@erase "$(INTDIR)\VERSION.SBR"
	-@erase "$(INTDIR)\Voicetag.obj"
	-@erase "$(INTDIR)\Voicetag.sbr"
	-@erase "$(INTDIR)\Wincpp.obj"
	-@erase "$(INTDIR)\Wincpp.sbr"
	-@erase "$(OUTDIR)\Axabffio32.bsc"
	-@erase "$(OUTDIR)\Axabffio32.dll"
	-@erase "$(OUTDIR)\Axabffio32.exp"
	-@erase "$(OUTDIR)\Axabffio32.ilk"
	-@erase "$(OUTDIR)\Axabffio32.lib"
	-@erase "$(OUTDIR)\Axabffio32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MTd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "MAK_AXABFFIO32_DLL" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\Axabffio32.pch" /Yu"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

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
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\axabffio32.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Axabffio32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Abffiles.sbr" \
	"$(INTDIR)\Abfheadr.sbr" \
	"$(INTDIR)\Abfhwave.sbr" \
	"$(INTDIR)\Abfinfo.sbr" \
	"$(INTDIR)\Abfutil.sbr" \
	"$(INTDIR)\Abfvtags.sbr" \
	"$(INTDIR)\Axabffio32.sbr" \
	"$(INTDIR)\Axodebug.sbr" \
	"$(INTDIR)\BufferedArray.sbr" \
	"$(INTDIR)\Csynch.sbr" \
	"$(INTDIR)\DACFile.sbr" \
	"$(INTDIR)\Filedesc.sbr" \
	"$(INTDIR)\FileIO.sbr" \
	"$(INTDIR)\FileReadCache.sbr" \
	"$(INTDIR)\Msbincvt.sbr" \
	"$(INTDIR)\Oldheadr.sbr" \
	"$(INTDIR)\pkware.sbr" \
	"$(INTDIR)\StringResource.sbr" \
	"$(INTDIR)\TextBuffer.sbr" \
	"$(INTDIR)\UserList.sbr" \
	"$(INTDIR)\VERSION.SBR" \
	"$(INTDIR)\Voicetag.sbr" \
	"$(INTDIR)\Wincpp.sbr"

"$(OUTDIR)\Axabffio32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winmm.lib version.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Axabffio32.pdb" /debug\
 /machine:I386 /nodefaultlib:"LIBCMT" /def:".\axabffio32.def"\
 /out:"$(OUTDIR)\Axabffio32.dll" /implib:"$(OUTDIR)\Axabffio32.lib" 
DEF_FILE= \
	".\axabffio32.def"
LINK32_OBJS= \
	"$(INTDIR)\Abffiles.obj" \
	"$(INTDIR)\Abfheadr.obj" \
	"$(INTDIR)\Abfhwave.obj" \
	"$(INTDIR)\Abfinfo.obj" \
	"$(INTDIR)\Abfutil.obj" \
	"$(INTDIR)\Abfvtags.obj" \
	"$(INTDIR)\Axabffio32.obj" \
	"$(INTDIR)\axabffio32.res" \
	"$(INTDIR)\Axodebug.obj" \
	"$(INTDIR)\BufferedArray.obj" \
	"$(INTDIR)\Csynch.obj" \
	"$(INTDIR)\DACFile.obj" \
	"$(INTDIR)\Filedesc.obj" \
	"$(INTDIR)\FileIO.obj" \
	"$(INTDIR)\FileReadCache.obj" \
	"$(INTDIR)\Msbincvt.obj" \
	"$(INTDIR)\Oldheadr.obj" \
	"$(INTDIR)\pkware.obj" \
	"$(INTDIR)\StringResource.obj" \
	"$(INTDIR)\TextBuffer.obj" \
	"$(INTDIR)\UserList.obj" \
	"$(INTDIR)\VERSION.OBJ" \
	"$(INTDIR)\Voicetag.obj" \
	"$(INTDIR)\Wincpp.obj" \
	"..\..\lib\AxonValidation.lib"

"$(OUTDIR)\Axabffio32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\.\Debug
TargetName=Axabffio32
SOURCE=$(InputPath)
PostBuild_Desc=Copying DLL & LIB
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Axabffio32.dll" "$(OUTDIR)\Axabffio32.bsc"
   \AxonDev\Tools\Update .\.\Debug\Axabffio32 dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(CFG)" == "axabffio32 - Win32 Release" || "$(CFG)" ==\
 "axabffio32 - Win32 Debug"
SOURCE=.\Abffiles.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_ABFFI=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\DACFile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	".\VOICETAG.HPP"\
	

"$(INTDIR)\Abffiles.obj" : $(SOURCE) $(DEP_CPP_ABFFI) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_ABFFI=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\DACFile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	".\VOICETAG.HPP"\
	

"$(INTDIR)\Abffiles.obj"	"$(INTDIR)\Abffiles.sbr" : $(SOURCE) $(DEP_CPP_ABFFI)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Abfheadr.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_ABFHE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	

"$(INTDIR)\Abfheadr.obj" : $(SOURCE) $(DEP_CPP_ABFHE) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_ABFHE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	

"$(INTDIR)\Abfheadr.obj"	"$(INTDIR)\Abfheadr.sbr" : $(SOURCE) $(DEP_CPP_ABFHE)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Abfhwave.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_ABFHW=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	

"$(INTDIR)\Abfhwave.obj" : $(SOURCE) $(DEP_CPP_ABFHW) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_ABFHW=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	

"$(INTDIR)\Abfhwave.obj"	"$(INTDIR)\Abfhwave.sbr" : $(SOURCE) $(DEP_CPP_ABFHW)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Abfinfo.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_ABFIN=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\ABFINFO.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\TextBuffer.hpp"\
	

"$(INTDIR)\Abfinfo.obj" : $(SOURCE) $(DEP_CPP_ABFIN) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_ABFIN=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\ABFINFO.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\TextBuffer.hpp"\
	

"$(INTDIR)\Abfinfo.obj"	"$(INTDIR)\Abfinfo.sbr" : $(SOURCE) $(DEP_CPP_ABFIN)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Abfutil.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_ABFUT=\
	"..\axonvalidation\axonvalidation.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	

"$(INTDIR)\Abfutil.obj" : $(SOURCE) $(DEP_CPP_ABFUT) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_ABFUT=\
	"..\axonvalidation\axonvalidation.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	

"$(INTDIR)\Abfutil.obj"	"$(INTDIR)\Abfutil.sbr" : $(SOURCE) $(DEP_CPP_ABFUT)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Abfvtags.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_ABFVT=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\waitcursor.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	

"$(INTDIR)\Abfvtags.obj" : $(SOURCE) $(DEP_CPP_ABFVT) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_ABFVT=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\waitcursor.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	

"$(INTDIR)\Abfvtags.obj"	"$(INTDIR)\Abfvtags.sbr" : $(SOURCE) $(DEP_CPP_ABFVT)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Axabffio32.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_AXABF=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\version.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	

"$(INTDIR)\Axabffio32.obj" : $(SOURCE) $(DEP_CPP_AXABF) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_AXABF=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\version.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	

"$(INTDIR)\Axabffio32.obj"	"$(INTDIR)\Axabffio32.sbr" : $(SOURCE)\
 $(DEP_CPP_AXABF) "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\axabffio32.rc
DEP_RSC_AXABFF=\
	".\Abferror.str"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\AXABFFIO32.ICO"\
	".\axabffio32.rc2"\
	

"$(INTDIR)\axabffio32.res" : $(SOURCE) $(DEP_RSC_AXABFF) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\Common\Axodebug.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_AXODE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	

"$(INTDIR)\Axodebug.obj" : $(SOURCE) $(DEP_CPP_AXODE) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_AXODE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	

"$(INTDIR)\Axodebug.obj"	"$(INTDIR)\Axodebug.sbr" : $(SOURCE) $(DEP_CPP_AXODE)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Common\BufferedArray.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_BUFFE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\FileIO.hpp"\
	

"$(INTDIR)\BufferedArray.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_BUFFE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\FileIO.hpp"\
	

"$(INTDIR)\BufferedArray.obj"	"$(INTDIR)\BufferedArray.sbr" : $(SOURCE)\
 $(DEP_CPP_BUFFE) "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Csynch.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_CSYNC=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	

"$(INTDIR)\Csynch.obj" : $(SOURCE) $(DEP_CPP_CSYNC) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_CSYNC=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	

"$(INTDIR)\Csynch.obj"	"$(INTDIR)\Csynch.sbr" : $(SOURCE) $(DEP_CPP_CSYNC)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\DACFile.CPP

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_DACFI=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\DACFile.hpp"\
	

"$(INTDIR)\DACFile.obj" : $(SOURCE) $(DEP_CPP_DACFI) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_DACFI=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\DACFile.hpp"\
	

"$(INTDIR)\DACFile.obj"	"$(INTDIR)\DACFile.sbr" : $(SOURCE) $(DEP_CPP_DACFI)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Filedesc.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_FILED=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\FileIO.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\DACFile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	

"$(INTDIR)\Filedesc.obj" : $(SOURCE) $(DEP_CPP_FILED) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_FILED=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\FileIO.hpp"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\DACFile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	

"$(INTDIR)\Filedesc.obj"	"$(INTDIR)\Filedesc.sbr" : $(SOURCE) $(DEP_CPP_FILED)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=..\Common\FileIO.CPP

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_FILEI=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	

"$(INTDIR)\FileIO.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_FILEI=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	

"$(INTDIR)\FileIO.obj"	"$(INTDIR)\FileIO.sbr" : $(SOURCE) $(DEP_CPP_FILEI)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Common\FileReadCache.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_FILER=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	

"$(INTDIR)\FileReadCache.obj" : $(SOURCE) $(DEP_CPP_FILER) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_FILER=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	

"$(INTDIR)\FileReadCache.obj"	"$(INTDIR)\FileReadCache.sbr" : $(SOURCE)\
 $(DEP_CPP_FILER) "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Msbincvt.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_MSBIN=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\Msbincvt.h"\
	

"$(INTDIR)\Msbincvt.obj" : $(SOURCE) $(DEP_CPP_MSBIN) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_MSBIN=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\Msbincvt.h"\
	

"$(INTDIR)\Msbincvt.obj"	"$(INTDIR)\Msbincvt.sbr" : $(SOURCE) $(DEP_CPP_MSBIN)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Oldheadr.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_OLDHE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\ABFOLDNX.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\Msbincvt.h"\
	".\Oldheadr.h"\
	

"$(INTDIR)\Oldheadr.obj" : $(SOURCE) $(DEP_CPP_OLDHE) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_OLDHE=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\ABFOLDNX.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\Msbincvt.h"\
	".\Oldheadr.h"\
	

"$(INTDIR)\Oldheadr.obj"	"$(INTDIR)\Oldheadr.sbr" : $(SOURCE) $(DEP_CPP_OLDHE)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=..\Common\pkware.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_PKWAR=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	"..\Common\pkware.h"\
	

"$(INTDIR)\pkware.obj" : $(SOURCE) $(DEP_CPP_PKWAR) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_PKWAR=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	"..\Common\pkware.h"\
	

"$(INTDIR)\pkware.obj"	"$(INTDIR)\pkware.sbr" : $(SOURCE) $(DEP_CPP_PKWAR)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\StringResource.CPP

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_STRIN=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\StringResource.h"\
	

"$(INTDIR)\StringResource.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_STRIN=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\StringResource.h"\
	

"$(INTDIR)\StringResource.obj"	"$(INTDIR)\StringResource.sbr" : $(SOURCE)\
 $(DEP_CPP_STRIN) "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\TextBuffer.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_TEXTB=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\TextBuffer.hpp"\
	

"$(INTDIR)\TextBuffer.obj" : $(SOURCE) $(DEP_CPP_TEXTB) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_TEXTB=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\TextBuffer.hpp"\
	

"$(INTDIR)\TextBuffer.obj"	"$(INTDIR)\TextBuffer.sbr" : $(SOURCE)\
 $(DEP_CPP_TEXTB) "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\UserList.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_USERL=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	

"$(INTDIR)\UserList.obj" : $(SOURCE) $(DEP_CPP_USERL) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_USERL=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	

"$(INTDIR)\UserList.obj"	"$(INTDIR)\UserList.sbr" : $(SOURCE) $(DEP_CPP_USERL)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=..\Common\VERSION.CPP

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_VERSI=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\version.hpp"\
	

"$(INTDIR)\VERSION.OBJ" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_VERSI=\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\version.hpp"\
	

"$(INTDIR)\VERSION.OBJ"	"$(INTDIR)\VERSION.SBR" : $(SOURCE) $(DEP_CPP_VERSI)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Voicetag.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_VOICE=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	"..\Common\pkware.h"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	

"$(INTDIR)\Voicetag.obj" : $(SOURCE) $(DEP_CPP_VOICE) "$(INTDIR)"\
 "$(INTDIR)\Axabffio32.pch"


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_VOICE=\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\FileIO.hpp"\
	"..\Common\pkware.h"\
	".\Abffiles.h"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	

"$(INTDIR)\Voicetag.obj"	"$(INTDIR)\Voicetag.sbr" : $(SOURCE) $(DEP_CPP_VOICE)\
 "$(INTDIR)" "$(INTDIR)\Axabffio32.pch"


!ENDIF 

SOURCE=.\Wincpp.cpp

!IF  "$(CFG)" == "axabffio32 - Win32 Release"

DEP_CPP_WINCP=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\Wincpp.hpp"\
	
CPP_SWITCHES=/nologo /Zp4 /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "WIN32_LEAN_AND_MEAN" /D "MAK_AXABFFIO32_DLL" /Fp"$(INTDIR)\Axabffio32.pch"\
 /Yc"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Wincpp.obj"	"$(INTDIR)\Axabffio32.pch" : $(SOURCE) $(DEP_CPP_WINCP)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "axabffio32 - Win32 Debug"

DEP_CPP_WINCP=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\Wincpp.hpp"\
	
CPP_SWITCHES=/nologo /Zp4 /MTd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "MAK_AXABFFIO32_DLL" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\Axabffio32.pch" /Yc"wincpp.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 

"$(INTDIR)\Wincpp.obj"	"$(INTDIR)\Wincpp.sbr"	"$(INTDIR)\Axabffio32.pch" : \
$(SOURCE) $(DEP_CPP_WINCP) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

