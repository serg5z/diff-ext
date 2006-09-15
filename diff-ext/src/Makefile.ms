PROJ = diff_ext.dll
!ifndef ROOT
ROOT = ../..
!endif
!ifndef BUILD
BUILD = .
!endif

# set NODEBUG _before_ win32.mak
!ifndef DEBUG
NODEBUG=yes 
!endif

!include <win32.mak>

# any modifications to the flags should go _after_ win32.mak
!ifdef UNICODE
cflags = $(cflags) -DUNICODE -D_UNICODE
!endif

DEF=diff_ext.def
OBJ=class_factory.obj \
    diff_ext.obj \
    server.obj \
    resource.res
    
cflags = $(cflags) -I$(ROOT)\include -I$(ROOT)\util\include -I..\include
rcflags = $(rcflags) -I..\include

all: $(BUILD)\$(PROJ)

clean:
    del *.obj *.res $(BUILD)\$(PROJ)

$(BUILD)\$(PROJ): $(OBJ) $(DEF)
    $(link) $(linkdebug) $(dlllflags)     \
    -out:$(BUILD)\$(PROJ)   \
    -def:$(DEF) \
    $(OBJ) \
    $(olelibs) shell32.lib 

.cpp.obj:
    $(cc) $(cflags) $(cvarsmt) $(cdebug) /WX $<

.rc.res: 
    $(rc) $(rcvars) $(rcflags) /fo $@ $<
