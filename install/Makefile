VERSION = 1.8.5

ISCC = "c:\Program Files\Inno Setup 5\ISCC"

!ifdef UNICODE
INSTALLER = diff_ext-$(VERSION)-$(CPU)-UNICODE.exe
!else
INSTALLER = diff_ext-$(VERSION)-$(CPU).exe
!endif
  
PRODUCT=$(BUILD)\diff_ext.dll \
        $(BUILD)\diff_ext1031.dll \
        $(BUILD)\diff_ext1041.dll \
        $(BUILD)\diff_ext1049.dll \
        $(BUILD)\diff_ext_setup.exe \
        $(BUILD)\diff_ext_setup1031.dll \
        $(BUILD)\diff_ext_setup1041.dll \
        $(BUILD)\diff_ext_setup1049.dll
  
all: $(INSTALLER)

clean:
  del $(INSTALLER)

$(INSTALLER) : diff_ext.iss $(PRODUCT)
  $(ISCC) diff_ext.iss
  move diff_ext.exe $@
