PROJ = diff_ext_setup.exe
!ifndef ROOT
ROOT = ..\..
!endif

DEF=diff_ext_setup.def
OBJ=about.obj \
    debug_page.obj \
    main.obj \
    diff_ext_setup.res
LIBS =  $(olelibs) shlwapi.lib version.lib comctl32.lib shell32.lib layout.lib util.lib 

!include $(ROOT)\make\executable.mak

# any modifications to the flags should go _after_ win32.mak
cflags = $(cflags) -D_CRT_SECURE_NO_DEPRECATE -I$(ROOT)\include -I$(ROOT)\util\include -I$(ROOT)\dialog-layout\include -I..\include
rcflags = $(rcflags) -I..\include

#dependency information goes here
diff_ext_setup.res: ..\include\resource.h
