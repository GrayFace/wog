# Microsoft Developer Studio Project File - Name="t1b" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=t1b - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "t1b.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "t1b.mak" CFG="t1b - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "t1b - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "t1b - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "t1b - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G3 /Zp1 /MT /W4 /vmg /Od /Gy /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FAcs /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib /nologo /base:"0x700000" /entry:"main" /subsystem:windows /map /machine:I386 /out:"t1.exe"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "t1b - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /Zp1 /MT /W4 /vmg /Od /Gy /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FAcs /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib /nologo /base:"0x700000" /entry:"main" /subsystem:windows /incremental:no /map /machine:I386 /out:"t1.exe" /pdbtype:sept
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "t1b - Win32 Release"
# Name "t1b - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_B1.cpp
# End Source File
# Begin Source File

SOURCE=.\ai.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\anim.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\artifact.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\casdem.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\common.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\CrExpo.CPP
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\erm.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\herospec.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\monsters.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\npc.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\service.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\string.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\txtfile.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\wogsetup.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\womo.cpp
# ADD CPP /Zp1 /W4 /GX- /Od /FAcs /FR
# End Source File
# Begin Source File

SOURCE=.\zzzz.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ai.h
# End Source File
# Begin Source File

SOURCE=.\anim.h
# End Source File
# Begin Source File

SOURCE=.\artifact.h
# End Source File
# Begin Source File

SOURCE=.\b1.h
# End Source File
# Begin Source File

SOURCE=.\casdem.h
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\CrExpo.h
# End Source File
# Begin Source File

SOURCE=.\erm.h
# End Source File
# Begin Source File

SOURCE=.\herospec.h
# End Source File
# Begin Source File

SOURCE=.\monsters.h
# End Source File
# Begin Source File

SOURCE=.\npc.h
# End Source File
# Begin Source File

SOURCE=.\service.h
# End Source File
# Begin Source File

SOURCE=.\sound.h
# End Source File
# Begin Source File

SOURCE=.\string.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\txtfile.h
# End Source File
# Begin Source File

SOURCE=.\wogsetup.h
# End Source File
# Begin Source File

SOURCE=.\womo.h
# End Source File
# Begin Source File

SOURCE=.\zzzz.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\h3struct.txt
# End Source File
# End Target
# End Project
