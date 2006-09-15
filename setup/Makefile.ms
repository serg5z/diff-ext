all:
  cd src
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
  cd i18n
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
        
clean:
  @cd src
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean 
  @cd ..
  @cd i18n
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean
  @cd ..
