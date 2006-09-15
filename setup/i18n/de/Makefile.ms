PROJ = diff_ext_setup1031.dll
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

$(BUILD)\$(PROJ): diff_ext_setup.res diff_ext_setup.def
    $(link) $(linkdebug) $(lflags) -dll    \
    /NOENTRY \
    -out:$(BUILD)\$(PROJ)   \
    -def:diff_ext_setup.def \
    diff_ext_setup.res
    
.rc.res: 
    $(rc) $(rcvars) $(rcflags) /fo $@ $<
