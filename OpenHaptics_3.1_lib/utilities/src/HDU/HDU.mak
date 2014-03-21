# Makefile - HDU

ifndef include_dir
include_dir = /usr/include
endif

ifndef libsrc_dir
libsrc_dir = /usr/share/3DTouch/libsrc
endif

ifndef libdir
libdir = /usr/lib
endif

#DEBUG = TRUE

ifdef DEBUG
CFG=HDU_DEBUG
else
CFG=HDU_RELEASE
endif

CC=gcc
CFLAGS=
CXX=g++
CXXFLAGS=$(CFLAGS)

INCLUDES = -I../../../include -I../../include

ifeq "$(CFG)"  "HDU_RELEASE"
CFLAGS+=-W -fexceptions -O2 $(INCLUDES) -Dlinux -DNDEBUG
AR=ar
ARFLAGS=rus
else

ifeq "$(CFG)"  "HDU_DEBUG"
CFLAGS+=-Wall -fexceptions -g -O0 $(INCLUDES) -Dlinux -D_DEBUG
AR=ar
TARGET=HDUD
ARFLAGS=rus
endif
endif

ifndef TARGET
TARGET=HDU
endif

TARGET_LIB := $(addprefix lib,$(TARGET))
TARGET_LIB := $(addprefix ../../lib/,$(TARGET_LIB))
TARGET_LIB := $(addsuffix .a,$(TARGET_LIB))

.PHONY: all
all: $(TARGET_LIB)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

SOURCE_FILES= \
	src/hdu.cpp \
	src/hduDecompose.cpp \
	src/hduLine.cpp \
	src/hduLineSegment.cpp \
	src/hduMatrix.cpp \
	src/hduPlane.cpp \
	src/hduQuaternion.cpp \
	src/hduRecord.cpp \
	src/hduAfx.cpp \
	src/hduError.cpp \
	src/hduHapticDevice.cpp

PUBLIC_HEADER_FILES= \
	../../include/HDU/hduBoundBox.h \
	../../include/HDU/hduGenericMatrix.h \
	../../include/HDU/hduLine.h \
	../../include/HDU/hduLineSegment.h \
	../../include/HDU/hduMath.h \
	../../include/HDU/hduMatrix.h \
	../../include/HDU/hduPlane.h \
	../../include/HDU/hduQuaternion.h \
	../../include/HDU/hduRecord.h \
	../../include/HDU/hduVector.h \
	../../include/HDU/hduVector.inl \
	../../include/HDU/hdu.h \
	../../include/HDU/hduError.h \
	../../include/HDU/hduHapticDevice.h    

PRIVATE_HEADER_FILES= \
	src/hduDecompose.h \
	src/hduAfx.h

SRCS=$(SOURCE_FILES)

OBJS=$(patsubst %.rc,%.res,$(patsubst %.cxx,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.c %.cc %.cpp %.cxx %.rc,$(SRCS)))))))

$(TARGET_LIB): $(OBJS) $(PRIVATE_HEADER_FILES) $(PUBLIC_HEADER_FILES)
	$(AR) $(ARFLAGS) $@ $(OBJS)

.PHONY: install
install: all
	# copy lib to libdir
	install -m 755 -o 0 -g 0 -d $(libdir)
	install -m 755 -o 0 -g 0 $(TARGET_LIB) $(libdir)
	# copy public include files to /usr/include
	install -m 755 -o 0 -g 0 -d $(include_dir)/$(TARGET)
	install -m 644 -o 0 -g 0 $(PUBLIC_HEADER_FILES) $(include_dir)/$(TARGET)
	# copy private include and source files to /usr/share/3DTouch/libsrc
	install -m 755 -o 0 -g 0 -d $(libsrc_dir)/$(TARGET)
	install -m 644 -o 0 -g 0 Makefile $(libsrc_dir)/$(TARGET)
	install -m 644 -o 0 -g 0 $(SOURCE_FILES) $(libsrc_dir)/$(TARGET)
	install -m 644 -o 0 -g 0 $(PRIVATE_HEADER_FILES) $(libsrc_dir)/$(TARGET)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET_LIB)


