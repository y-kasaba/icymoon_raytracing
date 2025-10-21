# Microsoft Developer Studio Project File - Name="libraytrace" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libraytrace - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "libraytrace.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "libraytrace.mak" CFG="libraytrace - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "libraytrace - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "libraytrace - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libraytrace - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "E:\usr\share\boost\1_31" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\libraytrace.lib"

!ELSEIF  "$(CFG)" == "libraytrace - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "E:\usr\share\boost\1_31" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_STLP_USE_DYNMIC_LIB" /D "_STLP_DEBUG" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\libraytraceD.lib"

!ENDIF 

# Begin Target

# Name "libraytrace - Win32 Release"
# Name "libraytrace - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\env.cpp
# End Source File
# Begin Source File

SOURCE=.\magnet_model.cpp
# End Source File
# Begin Source File

SOURCE=.\model_aon_earth.cpp
# End Source File
# Begin Source File

SOURCE=.\model_nsumei_earth.cpp
# End Source File
# Begin Source File

SOURCE=.\Lathys_europa_3d.cpp
# End Source File
# Begin Source File

SOURCE=.\model_sato_earth.cpp
# End Source File
# Begin Source File

SOURCE=.\plasma_model.cpp
# End Source File
# Begin Source File

SOURCE=.\rtc_func.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\tracer.cpp
# End Source File
# Begin Source File

SOURCE=.\wave_param.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\env.h
# End Source File
# Begin Source File

SOURCE=.\model_aon_earth.h
# End Source File
# Begin Source File

SOURCE=.\model_nsumei_earth.h
# End Source File
# Begin Source File

SOURCE=.\Lathys_europa_3d.h
# End Source File
# Begin Source File

SOURCE=.\model_sato_earth.h
# End Source File
# Begin Source File

SOURCE=.\raytrace.h
# End Source File
# Begin Source File

SOURCE=.\rtc_def.h
# End Source File
# Begin Source File

SOURCE=.\rtc_func.h
# End Source File
# Begin Source File

SOURCE=.\rtc_type.h
# End Source File
# Begin Source File

SOURCE=.\sample_model.h
# End Source File
# Begin Source File

SOURCE=.\parameter_plasma_model.h
# End Source File
# Begin Source File

SOURCE=.\spline.h
# End Source File
# Begin Source File

SOURCE=.\spline.inl
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tracer.h
# End Source File
# Begin Source File

SOURCE=.\tracer.inl
# End Source File
# Begin Source File

SOURCE=.\wave_param.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
