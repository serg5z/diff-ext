export BIN := $(shell pwd)/bin
export BUILD := $(shell pwd)/build
export LIB := $(shell pwd)/lib
export INCLUDE := -I$(shell pwd)/dialog-layout/include
export CXX  := g++.exe
export CC   := gcc.exe
export AR := ar.exe -r
export WINDRES := windres.exe
export DLLWRAP = dllwrap.exe
export CXXFLAGS := -ansi -pedantic -Wall -W $(INCLUDE)
export CFLAGS := -ansi -pedantic -Wall -W $(INCLUDE)
export LDFLAGS := -L$(LIB) -L$(shell pwd)/dialog-layout/lib
export UPDATE_VERSION := $(BIN)/update_version.exe

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
