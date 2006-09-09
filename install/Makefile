VERSION := 1.7.2

ISCC := /c/Program\ Files/Inno\ Setup\ 5/ISCC

ifdef UNICODE
  INSTALLER := diff_ext-$(VERSION)-UNICODE.exe
else
  INSTALLER := diff_ext-$(VERSION).exe
endif
  
all: $(INSTALLER)

clean:
	
        
$(INSTALLER) : diff_ext.iss $(wildcard $(BUILD)/*)
	$(ISCC) $<
	mv diff_ext.exe $@
