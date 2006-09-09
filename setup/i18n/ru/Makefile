PROJ := diff_ext_setup1049.dll
ROOT ?= ../../..

include $(ROOT)/make/dynamic-library.mk

RCFLAGS += -I../../include --language=419

clean:
	$(RM) $(RES)
#all-after: $(SRC-RC) $(PROJ:%.dll=%).res
#	echo $(OBJ)
#	echo $(RES)
#	$(UPDATE_VERSION)  --input $< --output $<
