BUILD ?= .
LIB ?= .

CPP  ?= g++.exe
CC   ?= gcc.exe
AR ?= ar.exe 
WINDRES ?= windres.exe
DLLWRAP ?= dllwrap.exe

ifdef DEBUG
  CXXFLAGS ?= -g -ansi -pedantic -Wall
  CFLAGS ?= -g -ansi -pedantic -Wall
else
  CXXFLAGS ?= -ansi -pedantic -Wall -O2
  CFLAGS ?= -ansi -pedantic -Wall -O2
endif

CXXFLAGS += -I../../util/include
CFLAGS += -I../../util/include
LDFLAGS += -mwindows

CXXFLAGS += -I../include

ifdef UNICODE
  CXXFLAGS += -D_UNICODE -DUNICODE 
  CFLAGS += -D_UNICODE -DUNICODE 
endif

LIBS :=
RES  := 
DEFFILE := 
STATICLIB :=
EXPLIB :=

SRC-CXX = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

OBJ  := $(SRC-CXX:.cpp=.o) $(SRC-C:.c=.o)

INCS :=
LIBRARY := $(LIB)/libutil.a
DLL  := 

.PHONY: all all-before all-after clean clean-custom

all: .depend all-before $(LIBRARY) all-after

.depend: Makefile $(SRC-C) $(SRC-RC) $(SRC-CXX)
	$(CXX) -M $(CXXFLAGS) $(SRC-C) $(SRC-RC) $(SRC-CXX) > .depend
	
include .depend

clean: clean-custom
	${RM}  $(OBJ) $(LIBRARY) ${RES} ${EXPLIB} $(STATICLIB)

$(LIBRARY): $(OBJ)
	$(AR) $@ $(OBJ)

.cpp.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS)
