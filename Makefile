.EXPORT_ALL_VARIABLES:

ROOT = $(shell pwd)
BIN = $(ROOT)/bin
BUILD = $(ROOT)/build
LIB = $(ROOT)/lib
INCLUDE = -I$(ROOT)/include
CXX = g++.exe
CC = gcc.exe
AR = ar.exe
RC = windres.exe
DLLWRAP = dllwrap.exe
CXXFLAGS = -ansi -pedantic -Wall -W $(INCLUDE)
CFLAGS = -ansi -pedantic -Wall -W $(INCLUDE)
RCFLAGS = -DMING $(INCLUDE)
LDFLAGS = -L$(LIB)
ARFLAGS = crv
UPDATE_VERSION = $(BIN)/update_version.exe

all:
	mkdir -p $(BIN)
	mkdir -p $(BUILD)
	mkdir -p $(LIB)
	$(MAKE) -C tools
	$(MAKE) -C util
	$(MAKE) -C dialog-layout
	$(MAKE) -C diff-ext
	$(MAKE) -C setup
	$(MAKE) -C install

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C util clean
	$(MAKE) -C dialog-layout clean
	$(MAKE) -C diff-ext clean
	$(MAKE) -C setup clean
	$(MAKE) -C install clean
        
debug:
	$(MAKE) DEBUG=YES UNICODE=YES

unicode:
	$(MAKE) UNICODE=YES
