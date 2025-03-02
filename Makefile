CC := g++
CFLAGS := -std=c++17 -O2 -Wall -DGLEW_STATIC -Iexternal/SDL/include -Iexternal/SDL/include/config -Iexternal/glew/include -Isrc

LDFLAGS := -Lexternal/SDL/build -lSDL3 -Lexternal/glew/lib -lglew32s -lopengl32
LDFLAGS := -Lexternal/SDL/build -lSDL3 -Lexternal/glew/lib -lglew32s -lopengl32



SRC_DIR := src
SRCS := $(SRC_DIR)/main.cpp \
        $(SRC_DIR)/renderer/renderer.cpp \
        $(SRC_DIR)/physics_engine/physics_engine.cpp \
        $(SRC_DIR)/camera/camera.cpp \
        $(SRC_DIR)/shapes/shape.cpp
BUILD_DIR := build
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
EXEC := $(BUILD_DIR)/buffer_display

# SDL build settings
SDL_BUILD_DIR := external/SDL/build
SDL_LIB_DIR := $(SDL_BUILD_DIR)/lib

# Extract unique directories where object files will be placed
OBJ_DIRS := $(sort $(dir $(OBJS)))

all: build_sdl $(BUILD_DIR) $(EXEC)

# Ensure the base build directory and all required subdirectories exist
$(BUILD_DIR):
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

$(OBJ_DIRS):
	@if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)
	@if exist "$(SDL_BUILD_DIR)/SDL3.dll" ( \
		copy /Y "$(SDL_BUILD_DIR)\SDL3.dll" "$(BUILD_DIR)\" \
	) else ( \
		echo "Error: SDL3.dll not found!" && exit 1 \
	)

# Ensure the target directory exists before compiling
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

build_sdl:
	@if not exist "$(SDL_BUILD_DIR)" mkdir "$(SDL_BUILD_DIR)"
	cd $(SDL_BUILD_DIR) && cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
	cd $(SDL_BUILD_DIR) && cmake --build . --config Release

clean:
	rm -rf $(BUILD_DIR)
	if exist $(SDL_BUILD_DIR) rmdir /s /q $(SDL_BUILD_DIR)

slim: $(BUILD_DIR) $(EXEC)
