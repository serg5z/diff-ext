all:
  cd src
  $(MAKE) ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
  cd i18n
  $(MAKE) ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
        
clean:
  @cd src
  $(MAKE) ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean 
  @cd ..
  @cd i18n
  $(MAKE) ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean
  @cd ..
