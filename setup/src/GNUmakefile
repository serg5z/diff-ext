PROJ := diff_ext_setup.exe

ROOT ?= ../..

include $(ROOT)/make/executable.gmk

LIBS :=  -llayout -lcomdlg32 -lcomctl32 -ladvapi32 -lversion -lole32 -lshlwapi -lutil

LDFLAGS += -mwindows
CXXFLAGS += -I$(ROOT)/dialog-layout/include -I../include -I$(ROOT)/util/include -D_WIN32_WINNT=0x0501 -g
CFLAGS += -I$(ROOT)/dialog-layout/include -I../include -I$(ROOT)/util/include -D_WIN32_WINNT=0x0501 -g
RCFLAGS += -I$(ROOT)/dialog-layout/include -I../include --language=409
LDFLAGS += -L$(ROOT)/dialog-layout/lib
DLLWRAP ?= dllwrap.exe
