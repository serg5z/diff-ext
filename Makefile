
all:
	$(MAKE) -C util
	$(MAKE) -C diff-ext

clean:
	$(MAKE) -C util clean
	$(MAKE) -C diff-ext clean
