export BUILD := $(shell pwd)/build

all:
	echo BUILD=$(BUILD)
	mkdir $(BUILD)
	$(MAKE) -C util
	$(MAKE) -C diff-ext
	$(MAKE) -C setup

clean:
	$(MAKE) -C util clean
	$(MAKE) -C diff-ext clean
	$(MAKE) -C setup clean
