PROJ := diff_ext_setup.exe

ROOT ?= ..

include $(ROOT)/make/executable.mk

LIBS :=  -llayout -lcomdlg32 -lcomctl32 -ladvapi32 -lversion -lole32

LDFLAGS += -mwindows
CXXFLAGS += -I$(ROOT)/dialog-layout/include
CFLAGS += -I$(ROOT)/dialog-layout/include
RCFLAGS += -I$(ROOT)/dialog-layout/include
LDFLAGS += -L$(ROOT)/dialog-layout/lib

LANG_RU := $(BUILD)/$(PROJ:%.exe=%)1049.dll

LANG := $(LANG_RU)
        
all-after: $(SRC-RC) $(LANG)
	$(UPDATE_VERSION)  --input $< --output $<

clean: clean-custom
	$(RM)  $(OBJ) $(PROJ) $(LANG) lang/$(PROJ:%.exe=%)_ru.res

$(LANG_RU) : lang/$(PROJ:%.exe=%)_ru.res
	$(DLLWRAP) \
	--mno-cygwin \
	--def lang/$(PROJ:%.exe=%)_lang.def \
	$< -o $@
