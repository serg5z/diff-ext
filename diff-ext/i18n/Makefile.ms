all:
  @cd ru
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
  @cd de
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
  @cd ja
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
        
clean:
  @cd ru
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean
  @cd ..
  @cd de
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean
  @cd ..
  @cd ja
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean
  @cd ..
