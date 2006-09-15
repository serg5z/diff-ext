all:
  cd src
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
        
clean:
  @cd src
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean 
  @cd ..
