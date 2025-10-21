STDHEADER = testing.h
LIBNAME   = testing
OBJ_PATH  = object
VPATH     = $(OBJ_PATH)
LEX       = flex
LD_PATH   = /usr/lib/aarch64-linux-gnu
SRC = \
    main.cpp    \
    raytrace.cpp\
    lex.cmd.cpp \


OBJ = $(SRC:%.cpp=$(OBJ_PATH)/%$(OBJECT_EXP))
.SUFFIXES:$(OBJECT_EXP).cpp

OUTPUT = $(LIBNAME)$(EXEC_EXP)

##################################################
all : $(OUTPUT)

$(OUTPUT): $(OBJ) $(RES_OBJ) $(LIBRT)
	$(LD) $(LD_FLAG) $^ $(LD_OUTPUT)

$(OBJ_PATH)/%$(OBJECT_EXP) :: %.cpp
	mkdir -p $(OBJ_PATH)
	$(CC) $(CC_FLAG) $< $(CC_OUTPUT_FLAG) $@

$(OBJ): $(STDHEADER) $(PCH)

lex.cmd.cpp: cmdline.l
	$(LEX)	-Pcmd -olex.cmd.cpp cmdline.l

.PHONY: clean local-clean
clean: local-clean
	rm -f ./$(OBJ_PATH)/*$(OBJECT_EXP)
	rm -f ./$(OBJ_PATH)/*.idb
	rm -f $(OUTPUT)

