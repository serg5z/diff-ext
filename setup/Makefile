BUILD ?= .
UPDATE_VERSION ?= ../bin/update_version

CXX  ?= g++.exe
CC   ?= gcc.exe
AR ?= ar.exe -r
WINDRES ?= windres.exe
DLLWRAP ?= dllwrap.exe

CXXFLAGS += -g -I../../util/include
CFLAGS += -Wall -ansi -pedantic -g -I../../util/include
LIBS :=  -lcomdlg32 -lcomctl32 -lversion
LDFLAGS += -mwindows
RES  := diff_ext_setup.res
DEFFILE = diff_ext.def
STATICLIB = diff_ext.a
EXPLIB = diff_ext.exp

SRC-CXX = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)
SRC-RC = $(wildcard *.rc)

OBJ  := $(SRC-CXX:.cpp=.o) $(SRC-C:.c=.o)
OBJ += $(RES)

INCS :=
PROJ := diff_ext_setup
EXE  := $(BUILD)/$(PROJ).exe

LANG_RU := $(BUILD)/$(PROJ)1049.dll

LANG := $(LANG_RU)

.PHONY: all all-before all-after clean clean-custom

.SUFFIXES: .rc .res

all: .depend all-before $(EXE) $(LANG) all-after

.depend: Makefile $(SRC-C) $(SRC-RC) $(SRC-CXX)
	$(CXX) -M $(CXXFLAGS) $(SRC-C) $(SRC-RC) $(SRC-CXX) > .depend
	
include .depend

all-after: $(SRC-RC)
	$(UPDATE_VERSION)  --input $< --output $<

clean: clean-custom
	${RM}  $(OBJ) $(EXE) ${RES} ${EXPLIB} $(STATICLIB) $(LANG) lang/$(PROJ)_ru.res

$(LANG_RU) : lang/$(PROJ)_ru.res
	$(DLLWRAP) \
	--mno-cygwin \
	--def lang/$(PROJ)_lang.def \
	--output-exp ${EXPLIB} \
	$< -o $@
	strip $@

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)
#	strip $@

.cpp.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

.rc.res:
	$(WINDRES)  $< -J rc -o $@ -O coff -DMING
