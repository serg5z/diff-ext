export BUILD := $(shell pwd)/build
export LIB := $(shell pwd)/lib
export CPP  := g++.exe
export CC   := gcc.exe
export AR := ar.exe -r
export WINDRES := windres.exe
export DLLWRAP = dllwrap.exe
export CXXFLAGS := -g -ansi -pedantic -Wall -mno-cygwin
export CFLAGS := -g -ansi -pedantic -Wall -mno-cygwin
export LDFLAGS := -L$(LIB)

all:
	echo BUILD=$(BUILD)
	mkdir -p $(BUILD)
	mkdir -p $(LIB)
	$(MAKE) -C util
	$(MAKE) -C diff-ext
	$(MAKE) -C setup

clean:
	$(MAKE) -C util clean
	$(MAKE) -C diff-ext clean
	$(MAKE) -C setup clean
