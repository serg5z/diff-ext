BUILD ?= .

CXXFLAGS += -I../../util/include
CFLAGS += -I../../util/include
LIBS :=  -lcomdlg32 -lcomctl32
LDFLAGS += -mwindows
RES  := diff_ext_setup.res
DEFFILE = diff_ext.def
STATICLIB = diff_ext.a
EXPLIB = diff_ext.exp

SRC-CPP = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

OBJ  := $(SRC-CPP:.cpp=.o) $(SRC-C:.c=.o)
OBJ += $(RES)

INCS :=
EXE  := $(BUILD)/diff_ext_setup.exe

.PHONY: all all-before all-after clean clean-custom

all: all-before $(EXE) all-after

clean: clean-custom
	${RM}  $(OBJ) $(EXE) ${RES} ${EXPLIB} $(STATICLIB)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)
	strip $@

.cpp.o:
	$(CPP) -c $? -o $@ $(CXXFLAGS)

.c.o:
	$(CC) -c $? -o $@ $(CFLAGS)

${RES}: diff_ext_setup.rc
	$(WINDRES)  diff_ext_setup.rc -I rc -o $@ -O coff -DMING
