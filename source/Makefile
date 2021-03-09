# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Filename		: Makefile.
# Project		: AudioPrism.
# Purpose		: Build audio processing applications.
# Created On	: 24/08/2020.
# Author		: Sreekanth Sreekumar.
# Modified On	: 07/02/2021.
# Modified By	: Sreekanth Sreekumar.
# Changes Made	: Comments, description added.
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


# Set the target executables.
APF_EXEC := audioprism
APV_EXEC := audioprism-v
APD_EXEC := audioprism-d

# Set the build and source directories.
BUILD_DIR := ./build-test
BIN_DIR := ../bin/x64_linux
SRC_DIRS := ./apps ./credits ./include ./utils ./loader ./attrs ./verify \
            ./defects
# Find all the C++ files we want to compile.
SRCS := $(shell find $(SRC_DIRS) -name *.cpp)

# Set the libraries and include directories.
ILIB_ROOT := /usr/local/include/
GLIB_ROOT := /usr/include/
ELIB_ROOT := $(ILIB_ROOT)essentia/
ELIBS := scheduler/ streaming/ utils/
UILLIBS := gaia2/
UILIBS := taglib/ qt4/ QtCore/ eigen3/
INC_LIBS := $(ELIB_ROOT) \
			$(addprefix $(ELIB_ROOT),$(ELIBS)) \
            $(addprefix $(ILIB_ROOT),$(UILLIBS)) \
			$(addprefix $(GLIB_ROOT),$(UILIBS))
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INCFLAGS := $(addprefix -I,$(INC_LIBS)) $(addprefix -I,$(INC_DIRS))

# Set the object files and dependencies.
UTILOBJS := credits.o fnameparser.o getoptions.o apinput.o apoutput.o apmath.o 
UOWPATHS := $(addprefix $(BUILD_DIR)/,$(UTILOBJS))
#DEPS := $(OBJS:.o=.d)

# Set the linker options.
LNK_DIRS := /usr/local/lib
LNK_LIBS := essentia fftw3 yaml avcodec avformat avutil samplerate tag \
            fftw3f QtCore gaia2 chromaprint tensorflow avresample
LNKFLAGS := $(addprefix -L,$(LNK_DIRS)) $(addprefix -l,$(LNK_LIBS))

# Set the compiler options.
CC := g++
CPPFLAGS := -pipe -Wall -O2 -fPIC -g

# Set the targets and aliases.
all : af av ad

av : apveri

ad : apdiag

af : apfull

# The final applications build steps.
apveri : verifier

apdiag : diagnostics

apfull : veridiag

# The application source build steps.
verifier : ./apps/verifier.cpp $(UTILOBJS) loader.o attrs.o verify.o 
	$(CC) $(CPPFLAGS) $(INCFLAGS) ./apps/verifier.cpp $(UOWPATHS) \
	$(BUILD_DIR)/loader.o $(BUILD_DIR)/attrs.o $(BUILD_DIR)/verify.o \
	-o $(BIN_DIR)/$(APV_EXEC) $(LNKFLAGS)

diagnostics : ./apps/diagnostics.cpp $(UTILOBJS) loader.o attrs.o  defects.o
	$(CC) $(CPPFLAGS) $(INCFLAGS) ./apps/diagnostics.cpp $(UOWPATHS) \
	$(BUILD_DIR)/loader.o $(BUILD_DIR)/attrs.o $(BUILD_DIR)/defects.o \
	-o $(BIN_DIR)/$(APD_EXEC) $(LNKFLAGS)

veridiag : ./apps/veridiag.cpp $(UTILOBJS) loader.o attrs.o verify.o defects.o
	$(CC) $(CPPFLAGS) $(INCFLAGS) ./apps/veridiag.cpp $(UOWPATHS) \
	$(BUILD_DIR)/loader.o $(BUILD_DIR)/attrs.o $(BUILD_DIR)/verify.o \
	$(BUILD_DIR)/defects.o -o $(BIN_DIR)/$(APF_EXEC) $(LNKFLAGS)

# The app core library source build steps.
loader.o : ./loader/audioloader.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./loader/audioloader.cpp \
	-o $(BUILD_DIR)/loader.o

attrs.o : ./attrs/audioattrs.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./attrs/audioattrs.cpp \
	-o $(BUILD_DIR)/attrs.o

verify.o : ./verify/audioverify.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./verify/audioverify.cpp \
	-o $(BUILD_DIR)/verify.o

defects.o : ./defects/audiodefects.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./defects/audiodefects.cpp \
	-o $(BUILD_DIR)/defects.o

# The app utility library source build steps.
apinput.o : ./utils/apinput.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./utils/apinput.cpp \
	-o $(BUILD_DIR)/apinput.o

apoutput.o : ./utils/apoutput.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./utils/apoutput.cpp \
	-o $(BUILD_DIR)/apoutput.o

apmath.o : ./utils/apmath.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./utils/apmath.cpp \
	-o $(BUILD_DIR)/apmath.o

getoptions.o : ./utils/getoptions.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./utils/getoptions.cpp \
	-o $(BUILD_DIR)/getoptions.o

fnameparser.o : ./utils/fnameparser.cpp
	$(CC) $(CPPFLAGS) $(INCFLAGS) -c ./utils/fnameparser.cpp \
	-o $(BUILD_DIR)/fnameparser.o

credits.o : ./credits/libav.cpp
	$(CC) $(CPPFLAGS) -c ./credits/libav.cpp \
	-o $(BUILD_DIR)/credits.o

.PHONY: clean
clean :
	if [ -d "$(BUILD_DIR)" ]; then rm -Rf $(BUILD_DIR); fi
	mkdir $(BUILD_DIR)
