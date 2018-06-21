# Microsoft Developer Studio Generated NMAKE File, Based on Abflib.dsp
!IF "$(CFG)" == ""
CFG=Abflib - Win32 Release
!MESSAGE No configuration specified. Defaulting to Abflib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Abflib - Win32 Release" && "$(CFG)" != "Abflib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Abflib.mak" CFG="Abflib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Abflib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Abflib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Abflib - Win32 Release"

OUTDIR=.\LibRelease
INTDIR=.\LibRelease
# Begin Custom Macros
OutDir=.\.\LibRelease
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Abflib.lib"

!ELSE 

ALL : "$(OUTDIR)\Abflib.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Abffiles.obj"
	-@erase "$(INTDIR)\Abfheadr.obj"
	-@erase "$(INTDIR)\Abfhwave.obj"
	-@erase "$(INTDIR)\Abfinfo.obj"
	-@erase "$(INTDIR)\Abfutil.obj"
	-@erase "$(INTDIR)\Abfvtags.obj"
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
	-@erase "$(INTDIR)\Voicetag.obj"
	-@erase "$(OUTDIR)\Abflib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MT /W4 /Gi /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "WIN32_LEAN_AND_MEAN" /D "MAK_ABFLIB_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\LibRelease/
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

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Abflib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Abflib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Abffiles.obj" \
	"$(INTDIR)\Abfheadr.obj" \
	"$(INTDIR)\Abfhwave.obj" \
	"$(INTDIR)\Abfinfo.obj" \
	"$(INTDIR)\Abfutil.obj" \
	"$(INTDIR)\Abfvtags.obj" \
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
	"$(INTDIR)\Voicetag.obj"

"$(OUTDIR)\Abflib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

OutDir=.\.\LibRelease
TargetName=Abflib
SOURCE=$(InputPath)
PostBuild_Desc=Copying LIB
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\LibRelease
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Abflib.lib"
   \AxonDev\Tools\Update .\.\LibRelease\Abflib lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

OUTDIR=.\LibDebug
INTDIR=.\LibDebug
# Begin Custom Macros
OutDir=.\.\LibDebug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Abflib.lib"

!ELSE 

ALL : "$(OUTDIR)\Abflib.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Abffiles.obj"
	-@erase "$(INTDIR)\Abfheadr.obj"
	-@erase "$(INTDIR)\Abfhwave.obj"
	-@erase "$(INTDIR)\Abfinfo.obj"
	-@erase "$(INTDIR)\Abfutil.obj"
	-@erase "$(INTDIR)\Abfvtags.obj"
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
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\Voicetag.obj"
	-@erase "$(OUTDIR)\Abflib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MTd /W4 /Gm /Zi /Od /Gy /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "MAK_ABFLIB_LIB" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\LibDebug/
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

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Abflib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Abflib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Abffiles.obj" \
	"$(INTDIR)\Abfheadr.obj" \
	"$(INTDIR)\Abfhwave.obj" \
	"$(INTDIR)\Abfinfo.obj" \
	"$(INTDIR)\Abfutil.obj" \
	"$(INTDIR)\Abfvtags.obj" \
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
	"$(INTDIR)\Voicetag.obj"

"$(OUTDIR)\Abflib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

OutDir=.\.\LibDebug
TargetName=Abflib
SOURCE=$(InputPath)
PostBuild_Desc=Copying LIB
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\LibDebug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Abflib.lib"
   \AxonDev\Tools\Update .\.\LibDebug\Abflib lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(CFG)" == "Abflib - Win32 Release" || "$(CFG)" == "Abflib - Win32 Debug"
SOURCE=.\Abffiles.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_ABFFI=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\dacfile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	".\VOICETAG.HPP"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abffiles.obj" : $(SOURCE) $(DEP_CPP_ABFFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_ABFFI=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\dacfile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	".\VOICETAG.HPP"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abffiles.obj" : $(SOURCE) $(DEP_CPP_ABFFI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Abfheadr.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_ABFHE=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfheadr.obj" : $(SOURCE) $(DEP_CPP_ABFHE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_ABFHE=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\Oldheadr.h"\
	".\StringResource.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfheadr.obj" : $(SOURCE) $(DEP_CPP_ABFHE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Abfhwave.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_ABFHW=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfhwave.obj" : $(SOURCE) $(DEP_CPP_ABFHW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_ABFHW=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfhwave.obj" : $(SOURCE) $(DEP_CPP_ABFHW) "$(INTDIR)"


!ENDIF 

SOURCE=.\Abfinfo.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_ABFIN=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\ABFINFO.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\TextBuffer.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfinfo.obj" : $(SOURCE) $(DEP_CPP_ABFIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_ABFIN=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\ABFINFO.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\TextBuffer.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfinfo.obj" : $(SOURCE) $(DEP_CPP_ABFIN) "$(INTDIR)"


!ENDIF 

SOURCE=.\Abfutil.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_ABFUT=\
	"..\axonvalidation\axonvalidation.h"\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfutil.obj" : $(SOURCE) $(DEP_CPP_ABFUT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_ABFUT=\
	"..\axonvalidation\axonvalidation.h"\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfutil.obj" : $(SOURCE) $(DEP_CPP_ABFUT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Abfvtags.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_ABFVT=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\waitcursor.hpp"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfvtags.obj" : $(SOURCE) $(DEP_CPP_ABFVT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_ABFVT=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\waitcursor.hpp"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Abfvtags.obj" : $(SOURCE) $(DEP_CPP_ABFVT) "$(INTDIR)"


!ENDIF 

SOURCE=..\Common\BufferedArray.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_BUFFE=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\BufferedArray.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_BUFFE=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\BufferedArray.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Csynch.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_CSYNC=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Csynch.obj" : $(SOURCE) $(DEP_CPP_CSYNC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_CSYNC=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Csynch.obj" : $(SOURCE) $(DEP_CPP_CSYNC) "$(INTDIR)"


!ENDIF 

SOURCE=.\DACFile.CPP

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_DACFI=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\dacfile.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\DACFile.obj" : $(SOURCE) $(DEP_CPP_DACFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_DACFI=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\dacfile.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\DACFile.obj" : $(SOURCE) $(DEP_CPP_DACFI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Filedesc.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_FILED=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\dacfile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Filedesc.obj" : $(SOURCE) $(DEP_CPP_FILED) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_FILED=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\BufferedArray.hpp"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\csynch.hpp"\
	".\dacfile.hpp"\
	".\FILEDESC.HPP"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Filedesc.obj" : $(SOURCE) $(DEP_CPP_FILED) "$(INTDIR)"


!ENDIF 

SOURCE=..\Common\FileIO.CPP

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_FILEI=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\FileIO.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_FILEI=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\FileIO.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Common\FileReadCache.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_FILER=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\FileReadCache.obj" : $(SOURCE) $(DEP_CPP_FILER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_FILER=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\filereadcache.hpp"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\FileReadCache.obj" : $(SOURCE) $(DEP_CPP_FILER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Msbincvt.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_MSBIN=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\MSBINCVT.H"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Msbincvt.obj" : $(SOURCE) $(DEP_CPP_MSBIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_MSBIN=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\MSBINCVT.H"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Msbincvt.obj" : $(SOURCE) $(DEP_CPP_MSBIN) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oldheadr.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_OLDHE=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\ABFOLDNX.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\MSBINCVT.H"\
	".\Oldheadr.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Oldheadr.obj" : $(SOURCE) $(DEP_CPP_OLDHE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_OLDHE=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\ABFOLDNX.H"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\MSBINCVT.H"\
	".\Oldheadr.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Oldheadr.obj" : $(SOURCE) $(DEP_CPP_OLDHE) "$(INTDIR)"


!ENDIF 

SOURCE=..\Common\pkware.cpp
DEP_CPP_PKWAR=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\msdos.h"\
	"..\common\pkware.h"\
	"..\common\win32.h"\
	"..\common\wincpp.hpp"\
	

"$(INTDIR)\pkware.obj" : $(SOURCE) $(DEP_CPP_PKWAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StringResource.CPP

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_STRIN=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\ABFERROR.STR"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\StringResource.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\StringResource.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_STRIN=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\ABFERROR.STR"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\StringResource.h"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\StringResource.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"


!ENDIF 

SOURCE=.\TextBuffer.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_TEXTB=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\TextBuffer.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\TextBuffer.obj" : $(SOURCE) $(DEP_CPP_TEXTB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_TEXTB=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\TextBuffer.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\TextBuffer.obj" : $(SOURCE) $(DEP_CPP_TEXTB) "$(INTDIR)"


!ENDIF 

SOURCE=.\UserList.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_USERL=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\msdos.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\UserList.obj" : $(SOURCE) $(DEP_CPP_USERL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_USERL=\
	"..\common\adcdac.h"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\win32.h"\
	".\abfheadr.h"\
	".\abfutil.h"\
	".\axabffio32.h"\
	".\UserList.hpp"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\UserList.obj" : $(SOURCE) $(DEP_CPP_USERL) "$(INTDIR)"


!ENDIF 

SOURCE=.\Voicetag.cpp

!IF  "$(CFG)" == "Abflib - Win32 Release"

DEP_CPP_VOICE=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\msdos.h"\
	"..\common\pkware.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Voicetag.obj" : $(SOURCE) $(DEP_CPP_VOICE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Abflib - Win32 Debug"

DEP_CPP_VOICE=\
	"..\common\adcdac.h"\
	"..\common\arrayptr.hpp"\
	"..\common\axodebug.h"\
	"..\common\axodefn.h"\
	"..\common\colors.h"\
	"..\common\FileIO.hpp"\
	"..\common\pkware.h"\
	"..\common\win32.h"\
	".\ABFFILES.H"\
	".\abfheadr.h"\
	".\axabffio32.h"\
	".\notify.hpp"\
	".\VOICETAG.HPP"\
	".\WINCPP.HPP"\
	

"$(INTDIR)\Voicetag.obj" : $(SOURCE) $(DEP_CPP_VOICE) "$(INTDIR)"


!ENDIF 


!ENDIF 

