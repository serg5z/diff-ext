BUILD ?= .

CPP  ?= g++.exe
CC   ?= gcc.exe
AR ?= ar.exe -r
WINDRES ?= windres.exe
DLLWRAP ?= dllwrap.exe

CXXFLAGS += -I../../util/include
CFLAGS += -g -I../../util/include
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
PROJ := diff_ext_setup
EXE  := $(BUILD)/$(PROJ).exe

LANG_RU := $(BUILD)/$(PROJ)1049.dll

LANG := $(LANG_RU)

.PHONY: all all-before all-after clean clean-custom

.SUFFIXES: .rc .res

all: all-before $(EXE) $(LANG) all-after

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
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)
#	strip $@

.cpp.o:
	$(CPP) -c $? -o $@ $(CXXFLAGS)

.c.o:
	$(CC) -c $? -o $@ $(CFLAGS)

.rc.res:
	$(WINDRES)  $< -I rc -o $@ -O coff -DMING
