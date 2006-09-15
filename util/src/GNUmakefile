PROJ := libutil.a

ROOT ?= ../..

include $(ROOT)/make/static-library.mk

CXXFLAGS += -I../../util/include -I../include
CFLAGS += -I../../util/include -I../include
LDFLAGS += -mwindows

clean: clean-custom
	$(RM)  $(OBJ) $(PROJ)

