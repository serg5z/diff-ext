PROJ := libutil.a

ROOT ?= ../..

LIB ?= .

include $(ROOT)/make/static-library.gmk

CXXFLAGS += -I../../util/include -I../include
CFLAGS += -I../../util/include -I../include
LDFLAGS += -mwindows
