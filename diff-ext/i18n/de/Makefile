PROJ := diff_ext1031.dll
ROOT ?= ../../..

include $(ROOT)/make/dynamic-library.mk

RCFLAGS += -I../../include --language=407

clean:
	$(RM) $(RES)
#all-after: $(SRC-RC) $(PROJ:%.dll=%).res
#	echo $(OBJ)
#	echo $(RES)
#	$(UPDATE_VERSION)  --input $< --output $<
