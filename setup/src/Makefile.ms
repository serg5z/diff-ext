PROJ = diff_ext_setup.exe
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

DEF=diff_ext_setup.def
OBJ=about.obj \
    debug_page.obj \
    main.obj \
    diff_ext_setup.res

cflags = $(cflags) -I$(ROOT)\include -I$(ROOT)\util\include -I$(ROOT)/dialog-layout/include -I..\include
rcflags = $(rcflags) -I..\include

all: $(BUILD)\$(PROJ)

diff_ext_setup.res: ..\include\resource.h

clean:
    del *.obj *.res $(BUILD)\$(PROJ)

$(BUILD)\$(PROJ): $(OBJ) $(DEF)
    $(link) $(linkdebug) $(guilflags) \
    /LIBPATH:$(LIBRARY) \
    -out:$(BUILD)\$(PROJ) \
    -def:$(DEF) \
    $(OBJ) \
    $(olelibs) shlwapi.lib version.lib comctl32.lib layout.lib

.cpp.obj:
    $(cc) $(cflags) $(cvarsmt) $(cdebug) /WX $<

.c.obj:
    $(cc) $(cflags) $(cvarsmt) $(cdebug) /WX $<

.rc.res: 
    $(rc) $(rcvars) $(rcflags) /fo $@ $<
