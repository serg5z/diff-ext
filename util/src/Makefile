CXXFLAGS += -I../include
LIBS :=
RES  := 
DEFFILE := 
STATICLIB :=
EXPLIB :=

SRC-CPP = $(wildcard *.cpp)
SRC-C$ = $(wildcard *.c)

OBJ  := $(SRC-CPP:.cpp=.o) $(SRC-C:.c=.o)

INCS :=
LIB := $(LIB)/libutil.a
DLL  := 

.PHONY: all all-before all-after clean clean-custom

all: all-before $(LIB) all-after

clean: clean-custom
	${RM}  $(OBJ) $(LIB) ${RES} ${EXPLIB} $(STATICLIB)

$(LIB): $(OBJ)
	$(AR) $@ $< 

.cpp.o:
	$(CPP) -c $? -o $@ $(CXXFLAGS)
