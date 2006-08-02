# Project: diff_ext
# Generates diff_ext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
PROJ := diff_ext.dll
ROOT ?= ../..

include $(ROOT)/make/dynamic-library.mk


LIBS :=  -lutil -luuid -luuid -lole32

CXXFLAGS += -I../../util/include
CFLAGS += -I../../util/include

LANG_RU := $(BUILD)/$(PROJ:%.dll=%)1049.dll

LANG := $(LANG_RU)

all-after: $(SRC-RC) lang/$(PROJ:%.dll=%)_ru.res
	echo $(OBJ)
	echo $(RES)
#	$(UPDATE_VERSION)  --input $< --output $<

clean: clean-custom
	$(RM)  $(OBJ) $(PROJ) $(EXPLIB) $(STATICLIB) $(LANG) lang/$(PROJ:%.dll=%)_ru.res

$(LANG_RU) : lang/$(PROJ:%.dll=%)_ru.res
	$(DLLWRAP) \
	--mno-cygwin \
	--def lang/$(PROJ:%.dll=%)_lang.def \
	$< -o $@
