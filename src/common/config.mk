ifeq (,$(BUILD_DIR))
BUILD_DIR=$(shell pwd -P)
endif

ifeq (,$(VERSION))
VERSION="4.x.x-dev-test"
endif

PLATFORM ?= $(UNION_PLATFORM)
ifeq (,$(PLATFORM))
PLATFORM=linux
endif

LIB = /mnt/SDCARD/.tmp_update/lib

CC 		?= $(CROSS_COMPILE)gcc
CXX 	?= $(CROSS_COMPILE)g++
STRIP 	?= $(CROSS_COMPILE)strip
PKG_CONFIG ?= $(CROSS_COMPILE)pkg-config

SOURCES := $(SOURCES) .
ifneq ($(INCLUDE_UTILS),0)
CFILES := $(CFILES) \
	../common/utils/str.c \
	../common/utils/log.c \
	../common/utils/file.c
endif
CFILES := $(CFILES) $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES := $(CPPFILES) $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.cpp))
OFILES = $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

SDL_FLAGS := $(shell $(PKG_CONFIG) --cflags --libs sdl SDL_image SDL_ttf)
CFLAGS := -I../common $(SDL_FLAGS) -DPLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z) -DONION_VERSION="\"$(VERSION)\"" -Wall

ifeq ($(DEBUG),1)
CFLAGS := $(CFLAGS) -DLOG_DEBUG -g3
endif

ifeq ($(TEST),1)
CFLAGS := $(CFLAGS) -I../include -I../src/common -I$(GTEST_INCLUDE_DIR)
endif

ifeq ($(SANITIZE),1)
CFILES := $(CFILES) ../common/utils/asan.c
CFLAGS := $(CFLAGS) -fno-omit-frame-pointer -fsanitize=address -static-libasan
LDFLAGS := -fsanitize=address -static-libasan $(LDFLAGS)
endif

CXXFLAGS := $(CFLAGS)
LDFLAGS := $(LDFLAGS) -L../../lib -L/usr/local/lib
ifeq ($(INCLUDE_CJSON),1)
LDFLAGS := $(LDFLAGS) -lcjson
endif

ifeq ($(PLATFORM),miyoomini)
CFLAGS := $(CFLAGS) -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -Wl,-rpath=$(LIB)

ifdef INCLUDE_SHMVAR
LDFLAGS := $(LDFLAGS) -lshmvar
endif

endif
