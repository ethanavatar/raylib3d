CC=clang
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -O3 -g
OUT_DIR=bin

RAYLIB_SRC=raylib/src
RAYLIB_INC=raylib/src
RAYLIB_MAKE_FLAGS=CC=$(CC) PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC RAYLIB_BUILD_MODE=RELEASE

LINK_FLAGS=-L$(RAYLIB_SRC) -static -m64 -lraylib -lm
WINDOWS_LIBS=-lgdi32 -lwinmm

.PHONY: all clean run raylib raylib_clean

all: raylib
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -I$(RAYLIB_INC) -L$(RAYLIB_SRC) -o $(OUT_DIR)/raylib3d raylib3d.c $(LINK_FLAGS) $(WINDOWS_LIBS)

run: all
	$(OUT_DIR)/raylib3d

raylib:
	cd $(RAYLIB_SRC) && make $(RAYLIB_MAKE_FLAGS)

raylib_clean:
	cd $(RAYLIB_SRC) && make clean

clean: raylib_clean
	rm -rf $(OUT_DIR)
