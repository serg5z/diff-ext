BIN ?= .

CXX  ?= g++.exe
CC   ?= gcc.exe
AR ?= ar.exe -r
WINDRES ?= windres.exe
DLLWRAP ?= dllwrap.exe

CXXFLAGS += -g
CFLAGS += -g
LIBS :=  -lrx -lkernel32
LDFLAGS += -mwindows
RES  := 
DEFFILE = 
STATICLIB = 
EXPLIB = 

SRC-CXX = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

OBJ  := $(SRC-CXX:.cpp=.o) $(SRC-C:.c=.o)
OBJ += $(RES)

INCS :=
PROJ := update_version
EXE  := $(BIN)/$(PROJ).exe

LANG :=

.PHONY: all all-before all-after clean clean-custom

.SUFFIXES: .rc .res

all: .depend all-before $(EXE) $(LANG) all-after

.depend: Makefile $(SRC-C) $(SRC-RC) $(SRC-CXX)
	$(CXX) -M $(CXXFLAGS) $(SRC-C) $(SRC-RC) $(SRC-CXX) > .depend
	
include .depend

clean: clean-custom
	${RM}  $(OBJ) $(EXE) ${RES} ${EXPLIB} $(STATICLIB) $(LANG) 

$(LANG_RU) : lang/$(PROJ)_ru.res
	$(DLLWRAP) \
	--mno-cygwin \
	--def lang/$(PROJ)_lang.def \
	--output-exp ${EXPLIB} \
	$< -o $@
	strip $@

$(EXE): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)
	strip $@

.cpp.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

.rc.res:
	$(WINDRES)  $< -I rc -o $@ -O coff -DMING
