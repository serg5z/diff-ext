PROJ := libutil.a

ROOT ?= ../..

LIB ?= .

include $(ROOT)/make/static-library.mk

CXXFLAGS += -I../../util/include -I../include
CFLAGS += -I../../util/include -I../include
LDFLAGS += -mwindows
