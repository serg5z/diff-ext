# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
BUILD ?= .
UPDATE_VERSION ?= echo "Version has not been changed for this build."

CXX  ?= g++.exe
CC   ?= gcc.exe
AR ?= ar.exe -r
WINDRES ?= windres.exe
DLLWRAP ?= dllwrap.exe
CXXFLAGS ?= -g -ansi -pedantic -Wall
CFLAGS ?= -g -ansi -pedantic -Wall
LDFLAGS ?= -L$(LIB)
LIB ?= ../../lib

CXXFLAGS += -I../../util/include
LIBS :=  -lutil -luuid -luuid -lole32
DEFFILE = diff_ext.def
STATICLIB = diff_ext.a
EXPLIB = diff_ext.exp

SRC-CXX = $(wildcard *.cpp)
SRC-C = $(wildcard *.c)
SRC-RC = $(wildcard *.rc)

OBJ  := $(SRC-CXX:.cpp=.o) $(SRC-C:.c=.o)
#RES  := $(SRC-RC:.rc=.res)
OBJ += $(RES)

INCS :=
PROJ := diff_ext
DLL  := $(BUILD)/diff_ext.dll

LANG_RU := $(BUILD)/$(PROJ)1049.dll

LANG := $(LANG_RU)

.PHONY: all all-before all-after clean clean-custom

.SUFFIXES: .rc .res

all: .depend all-before $(DLL) $(LANG) all-after

.depend: Makefile $(SRC-C) $(SRC-RC) $(SRC-CXX)
	$(CXX) -M $(CXXFLAGS) $(SRC-C) $(SRC-RC) $(SRC-CXX) > .depend
	
include .depend

all-before: $(SRC-RC)
	$(UPDATE_VERSION) --input $< --output $<

clean: clean-custom
	${RM}  $(OBJ) $(DLL) ${RES} ${EXPLIB} $(STATICLIB) $(LANG) lang/$(PROJ)_ru.res

$(LANG_RU) : lang/$(PROJ)_ru.res
	$(DLLWRAP) \
	--mno-cygwin \
	--def lang/$(PROJ)_lang.def \
	--output-exp ${EXPLIB} \
	$< -o $@
#	strip $@

$(DLL): $(OBJ)
	$(DLLWRAP) \
		--mno-cygwin \
		--def $(DEFFILE) \
		--output-exp ${EXPLIB} \
		--driver-name c++ -L/usr/local/lib -L/usr/lib/mingw \
		--implib $(STATICLIB) \
		$(OBJ) $(LDFLAGS) $(LIBS) \
		-o $@
#	strip $@

.cpp.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS)

.rc.res:
	$(WINDRES)  $< -I rc -o $@ -O coff -DMING

#~ ${RES}:
	#~ $(WINDRES)  diff_ext.rc -I rc -o $@ -O coff -DMING
