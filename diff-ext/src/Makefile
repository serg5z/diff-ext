# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
BUILD ?= .

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
DLL  := $(BUILD)/diff_ext.dll

.PHONY: all all-before all-after clean clean-custom

all: all-before $(DLL) all-after

clean: clean-custom
	${RM}  $(OBJ) $(DLL) ${RES} ${EXPLIB} $(STATICLIB)

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

${RES}:
	$(WINDRES)  diff_ext.rc -I rc -o $@ -O coff -DMING
