# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
BUILD ?= .

CPP  := g++.exe
CC   := gcc.exe
LD := gcc
CXXFLAGS := -g -ansi -pedantic -Wall -mno-cygwin -I../../util/include
CFLAGS := -g -ansi -pedantic -Wall -mno-cygwin -I../../util/include
LDFLAGS := -L../../util/src -mwindows
#LIBS :=  
LIBS :=  -lcomdlg32 -lcomctl32
WINDRES := windres.exe
DLLWRAP = dllwrap.exe
RES  := diff_ext_setup.res
DEFFILE = diff_ext.def
STATICLIB = diff_ext.a
EXPLIB = diff_ext.exp

SRC-CPP = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

#SRC-CPP += string/string.cpp

OBJ  := $(SRC-CPP:.cpp=.o) $(SRC-C:.c=.o)
OBJ += $(RES)

INCS :=
EXE  := $(BUILD)/diff_ext_setup.exe

.PHONY: all all-before all-after clean clean-custom

all: all-before $(EXE) all-after

clean: clean-custom
	${RM}  $(OBJ) $(EXE) ${RES} ${EXPLIB} $(STATICLIB)

$(EXE): $(OBJ)
	$(LD) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

.cpp.o:
	$(CPP) -c $? -o $@ $(CXXFLAGS)

.c.o:
	$(CC) -c $? -o $@ $(CFLAGS)

${RES}: diff_ext_setup.rc
	$(WINDRES)  diff_ext_setup.rc -I rc -o $@ -O coff -DMING
