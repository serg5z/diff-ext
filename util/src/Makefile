# $Id$
# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
CPP  := g++.exe
CC   := gcc.exe
CXXFLAGS := -g -ansi -pedantic -Wall -mno-cygwin -I../include
LDFLAGS :=
#LIBS :=  -luuid -luuid -lole32 -ldmalloc
LIBS :=
WINDRES := windres.exe
DLLWRAP = dllwrap.exe
AR := ar.exe -r
RES  := 
DEFFILE = 
STATICLIB =
EXPLIB =

SRC-CPP = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

#SRC-CPP += string/string.cpp

OBJ  := $(SRC-CPP:.cpp=.o) $(SRC-C:.c=.o)
OBJ += $(RES)

INCS :=
LIB := libutil.a
DLL  := 

.PHONY: all all-before all-after clean clean-custom

all: all-before $(LIB) all-after
#	${RM} ${RES}

clean: clean-custom
	${RM}  $(OBJ) $(LIB) ${RES} ${EXPLIB} $(STATICLIB)

$(LIB): $(OBJ)
	$(AR) $@ $< 

.cpp.o:
	$(CPP) -c $? -o $@ $(CXXFLAGS)

${RES}:
	$(WINDRES)  diff_ext.rc -I rc -o $@ -O coff -DMING
