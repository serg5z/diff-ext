PROJ = util.lib
!ifndef ROOT
ROOT = ..\..
!endif

OBJ=file_sink.obj \
    log.obj \
    menu.obj \
    string.obj \
    trace.obj \
    resource_string.obj \
    hyperlink.obj
    
!include $(ROOT)\make\static-library.mak

# any modifications to the flags should go _after_ win32.mak
cflags = $(cflags) -D_CRT_SECURE_NO_DEPRECATE -I..\include
rcflags = $(rcflags) -I..\include

#dependency information goes here
!include depend.mak