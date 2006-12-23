.EXPORT_ALL_VARIABLES:

ROOT ?= $(shell pwd)
BIN ?= $(ROOT)/bin
BUILD ?= $(ROOT)/build
LIB ?= $(ROOT)/lib
INCLUDE ?= -I$(ROOT)/include
CXX ?= g++.exe
CC ?= gcc.exe
AR ?= ar.exe
RC ?= windres.exe
DLLWRAP ?= dllwrap.exe
CXXFLAGS ?= -ansi -pedantic -Wall -W $(INCLUDE)
CFLAGS ?= -ansi -pedantic -Wall -W $(INCLUDE)
RCFLAGS ?= -DMING $(INCLUDE)
LDFLAGS ?= -L$(LIB)
ARFLAGS ?= crv
UPDATE_VERSION ?= $(BIN)/update_version.exe

all: all-before tools util dialog-layout diff-ext setup install

all-before:
	mkdir -p $(BIN)
	mkdir -p $(BUILD)
	mkdir -p $(LIB)

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C util clean
	$(MAKE) -C dialog-layout clean
	$(MAKE) -C diff-ext clean
	$(MAKE) -C setup clean
	$(MAKE) -C install clean
        
tools: FORSE
	$(MAKE) -C tools
        
util: FORSE
	$(MAKE) -C util
        
dialog-layout: FORSE
	$(MAKE) -C dialog-layout
        
diff-ext: FORSE
	$(MAKE) -C diff-ext
        
setup: FORSE
	$(MAKE) -C setup
        
install: FORSE
	$(MAKE) -C install
        
FORSE:

debug:
	$(MAKE) DEBUG=YES UNICODE=YES

unicode:
	$(MAKE) UNICODE=YES
