PROJ := diff_ext_setup.exe

ROOT ?= ..

include $(ROOT)/make/executable.mk

LIBS :=  ..\\lib\\liblayout.a comdlg32.lib comctl32.lib advapi32.lib version.lib ole32.lib shlwapi.lib shell32.lib user32.lib bufferoverflowu.lib

LDFLAGS += -mwindows
CXXFLAGS += -I$(ROOT)/dialog-layout/include
CFLAGS += -I$(ROOT)/dialog-layout/include -D_WIN32_WINNT=0x0501 -g
RCFLAGS += -I$(ROOT)/dialog-layout/include
LDFLAGS += -L$(ROOT)/dialog-layout/lib
DLLWRAP ?= dllwrap.exe

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
