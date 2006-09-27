PROJ = diff_ext.dll
!ifndef ROOT
ROOT = ..\..
!endif

DEF=diff_ext.def
OBJ=class_factory.obj \
    diff_ext.obj \
    server.obj \
    resource.res
LIBS=$(olelibs) shell32.lib util.lib uuid.lib
    
!include $(ROOT)\make\dynamic-library.mak

# any modifications to the flags should go _after_ win32.mak
cflags = $(cflags) -D_CRT_SECURE_NO_DEPRECATE  -I$(ROOT)\include -I$(ROOT)\util\include -I..\include
rcflags = $(rcflags) -I..\include

#dependency information goes here
!include depend.mak
