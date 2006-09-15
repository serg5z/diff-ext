PROJ = diff_ext1041.dll
!ifndef ROOT
ROOT = ../../..
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

rcflags = $(rcflags) -I..\..\include

all: $(BUILD)\$(PROJ)

clean:
    del *.obj *.res $(BUILD)\$(PROJ)

$(BUILD)\$(PROJ): diff_ext.res diff_ext.def
    $(link) $(linkdebug) $(lflags) -dll    \
    /NOENTRY \
    -out:$(BUILD)\$(PROJ)   \
    -def:diff_ext.def \
    diff_ext.res
    
.rc.res: 
    $(rc) $(rcvars) $(rcflags) /fo $@ $<
