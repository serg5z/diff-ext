PROJ := diff_ext1041.dll
ROOT ?= ../../..

include $(ROOT)/make/dynamic-library.mk

RCFLAGS += -I../../include --language=411

clean:
	$(RM) $(RES)
#all-after: $(SRC-RC) $(PROJ:%.dll=%).res
#	echo $(OBJ)
#	echo $(RES)
#	$(UPDATE_VERSION)  --input $< --output $<
