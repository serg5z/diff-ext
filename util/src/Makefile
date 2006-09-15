PROJ = util.lib
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

OBJ=file_sink.obj \
    log.obj \
    string.obj \
    trace.obj \
    
cflags = $(cflags) -I..\include

all: $(LIBRARY)\$(PROJ)

clean:
    del *.obj *.res $(LIBRARY)\$(PROJ)

$(LIBRARY)\$(PROJ): $(OBJ)
    $(implib) -out:$(LIBRARY)\$(PROJ) \
    $(OBJ) 

.cpp.obj:
    $(cc) $(cflags) $(cvarsmt) $(cdebug) /WX $<
