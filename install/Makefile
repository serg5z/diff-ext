VERSION := 1.6.1

ISCC := /c/Program\ Files/Inno\ Setup\ 5/ISCC

ifdef UNICODE
  INSTALLER := diff_ext-$(VERSION)-UNICODE.exe
else
  INSTALLER := diff_ext-$(VERSION).exe
endif
  
all: $(INSTALLER)

clean:
	
        
$(INSTALLER) : diff_ext.iss
	$(ISCC) $<
	mv diff_ext.exe $@
