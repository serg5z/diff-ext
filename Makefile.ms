ROOT=$(MAKEDIR)
BUILD=$(MAKEDIR)\build
LIBRARY=$(MAKEDIR)\lib

all:
  @if not exist $(BUILD) mkdir $(BUILD)
  @if not exist $(LIBRARY) mkdir $(LIBRARY)
  cd dialog-layout
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
  cd util
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
  cd diff-ext
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
  cd setup
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY)
  @cd ..
        
clean:
  cd dialog-layout
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean 
  @cd ..
  cd util
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean 
  @cd ..
  cd diff-ext
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean 
  @cd ..
  cd setup
  $(MAKE) -f Makefile.ms ROOT=$(ROOT) BUILD=$(BUILD) LIBRARY=$(LIBRARY) clean 
  @cd ..
