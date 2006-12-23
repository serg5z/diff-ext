# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
PROJ := diff_ext.dll
ROOT ?= ../..

include $(ROOT)/make/dynamic-library.gmk

LIBS :=  -lutil -luuid -luuid -lole32 -lgdi32 -lshlwapi

RCFLAGS += -I../include --language=409
CXXFLAGS += -I$(ROOT)/util/include -I../include -D_WIN32_WINNT=0x0500
CFLAGS += -I$(ROOT)/util/include -I../include -D_WIN32_WINNT=0x0500
