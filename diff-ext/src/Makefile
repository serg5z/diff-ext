# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
BUILD ?= .

CPP  ?= g++.exe
CC   ?= gcc.exe
AR ?= ar.exe -r
WINDRES ?= windres.exe
DLLWRAP ?= dllwrap.exe

CXXFLAGS += -I../../util/include
LIBS :=  -lutil -luuid -luuid -lole32
RES  := diff_ext.res
DEFFILE = diff_ext.def
STATICLIB = diff_ext.a
EXPLIB = diff_ext.exp

SRC-CPP = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

OBJ  := $(SRC-CPP:.cpp=.o) $(SRC-C:.c=.o)
OBJ += $(RES)

INCS :=
PROJ := diff_ext
DLL  := $(BUILD)/diff_ext.dll

LANG_RU := $(BUILD)/$(PROJ)1049.dll

LANG := $(LANG_RU)

.PHONY: all all-before all-after clean clean-custom

.SUFFIXES: .rc .res

all: all-before $(DLL) $(LANG) all-after

clean: clean-custom
	${RM}  $(OBJ) $(DLL) ${RES} ${EXPLIB} $(STATICLIB) $(LANG) lang/$(PROJ)_ru.res

$(LANG_RU) : lang/$(PROJ)_ru.res
	$(DLLWRAP) \
	--mno-cygwin \
	--def lang/$(PROJ)_lang.def \
	--output-exp ${EXPLIB} \
	$< -o $@
	strip $@

$(DLL): $(OBJ)
	$(DLLWRAP) \
		--mno-cygwin \
		--def $(DEFFILE) \
		--output-exp ${EXPLIB} \
		--driver-name c++ -L/usr/local/lib -L/usr/lib/mingw \
		--implib $(STATICLIB) \
		$(OBJ) $(LDFLAGS) $(LIBS) \
		-o $@
	strip $@

.cpp.o:
	$(CPP) -c $? -o $@ $(CXXFLAGS)

.rc.res:
	$(WINDRES)  $< -I rc -o $@ -O coff -DMING

#~ ${RES}:
	#~ $(WINDRES)  diff_ext.rc -I rc -o $@ -O coff -DMING
