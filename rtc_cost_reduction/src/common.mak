STDHEADER = StdAfx.h
LIBNAME   = core
OBJ_PATH  = object
VPATH     = $(OBJ_PATH)

SRC = \
    rtc_func.cpp     \
    cosmos.cpp       \
    basic_planet.cpp \
    magnet_model.cpp \
    plasma_model.cpp \
    ray.cpp          \
    wave_param.cpp   \
    plasma_cavity.cpp\

OBJ = $(SRC:%.cpp=$(OBJ_PATH)/%$(OBJECT_EXP))
.SUFFIXES:$(OBJECT_EXP).cpp

OUTPUT = ../lib/$(LIBNAME)$(EXEC_EXP)

##################################################
all : $(OUTPUT)

$(OUTPUT): $(OBJ) $(RES_OBJ) $(LIBRT)
	$(LD) $(LD_FLAG) $(LD_OUTPUT) $^

$(OBJ_PATH)/%$(OBJECT_EXP) :: %.cpp
	mkdir -p $(OBJ_PATH)
	$(CC) $(CC_FLAG) $< $(CC_OUTPUT_FLAG) $@

$(OBJ): $(STDHEADER) $(PCH)

.PHONY: clean local-clean
clean: local-clean
	rm -f ./$(OBJ_PATH)/*$(OBJECT_EXP)
	rm -f ./$(OBJ_PATH)/*.idb
	rm -f $(OUTPUT)

