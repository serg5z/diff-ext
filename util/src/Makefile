BUILD ?= .

CPP  ?= g++.exe
CC   ?= gcc.exe
AR ?= ar.exe -r
WINDRES ?= windres.exe
DLLWRAP ?= dllwrap.exe

CXXFLAGS += -I../../util/include
CFLAGS += -g -I../../util/include
LDFLAGS += -mwindows

CXXFLAGS += -I../include
LIBS :=
RES  := 
DEFFILE := 
STATICLIB :=
EXPLIB :=

SRC-CXX = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

OBJ  := $(SRC-CPP:.cpp=.o) $(SRC-C:.c=.o)

INCS :=
LIB := $(LIB)/libutil.a
DLL  := 

.PHONY: all all-before all-after clean clean-custom

all: .depend all-before $(LIB) all-after

.depend: Makefile $(SRC-C) $(SRC-RC) $(SRC-CXX)
	$(CXX) -M $(CXXFLAGS) $(SRC-C) $(SRC-RC) $(SRC-CXX) > .depend
	
include .depend

clean: clean-custom
	${RM}  $(OBJ) $(LIB) ${RES} ${EXPLIB} $(STATICLIB)

$(LIB): $(OBJ)
	$(AR) $@ $< 

.cpp.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS)
