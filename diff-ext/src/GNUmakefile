# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
PROJ := diff_ext.dll
ROOT ?= ../..

include $(ROOT)/make/dynamic-library.mk

LIBS :=  -lutil -luuid -luuid -lole32

RCFLAGS += -I../include --language=409
CXXFLAGS += -I$(ROOT)/util/include -I../include
CFLAGS += -I$(ROOT)/util/include -I../include
