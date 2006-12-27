VERSION := 1.9.0

ISCC := /c/Program\ Files/Inno\ Setup\ 5/ISCC

ifdef UNICODE
  INSTALLER := diff_ext-$(VERSION)-i386-UNICODE.exe
else
  INSTALLER := diff_ext-$(VERSION)-i386.exe
endif
  
all: $(INSTALLER)

clean:
	$(RM) $(INSTALLER)
        
$(INSTALLER) : diff_ext.iss $(wildcard $(BUILD)/*)
	$(ISCC) $<
	mv diff_ext.exe $@
