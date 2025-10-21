# プリコンパイルヘッダを指定
STDHEADER=StdAfx.h
LIBNAME  = libraytrace
PCH      = $(LIBNAME).pch

SRC = \
	env.cpp          \
	magnet_model.cpp \
	plasma_model.cpp \
	rtc_func.cpp     \
	StdAfx.cpp       \
	tracer.cpp       \
	wave_param.cpp   \
	model_sato_earth.cpp   \
	model_nsumei_earth.cpp \
	Lathys_europa_3d.cpp  \
	model_aon_earth.cpp    \

#.oファイルを置く場所
OBJ_PATH = object
VPATH    = $(OBJ_PATH) lib

OBJ = $(SRC:%.cpp=$(OBJ_PATH)/%.$(OBJECT_EXP))
.SUFFIXES:.$(OBJECT_EXP).cpp
