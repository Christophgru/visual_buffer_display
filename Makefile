CC := g++
CFLAGS := -std=c++17 -O2 -Wall -Iexternal/SDL/include -Iexternal/SDL/include/config -Isrc
LDFLAGS := -Lexternal/SDL/build -lSDL3


SRC_DIR := src
SRCS := $(SRC_DIR)/main.cpp $(SRC_DIR)/renderer/renderer.cpp
BUILD_DIR := build
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS)) 
EXEC := $(BUILD_DIR)/buffer_display

# SDL2 build settings
SDL_BUILD_DIR := external/SDL/build
SDL_LIB_DIR := $(SDL_BUILD_DIR)/lib

all: build_sdl $(BUILD_DIR) $(EXEC)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)
	cp $(SDL_BUILD_DIR)/SDL3.dll $(BUILD_DIR)/

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

build_sdl:
	if not exist $(SDL_BUILD_DIR) mkdir $(SDL_BUILD_DIR)
	cd $(SDL_BUILD_DIR) && cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
	cd $(SDL_BUILD_DIR) && cmake --build . --config Release

clean:
	rm -rf $(BUILD_DIR)
	if exist $(SDL_BUILD_DIR) rmdir /s /q $(SDL_BUILD_DIR)
