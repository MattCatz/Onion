INCLUDE_CJSON=1
INCLUDE_SHMVAR=1
include ../common/config.mk

TARGET = bootScreen
CFLAGS := $(CFLAGS) $(SDL_FLAGS)
LDFLAGS := $(LDFLAGS) -lSDL -lSDL_image -lSDL_ttf -lSDL_gfx

include ../common/commands.mk
include ../common/recipes.mk
