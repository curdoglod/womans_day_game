# Compiler
CXX = c++

OS := $(shell uname -s 2>/dev/null || echo Windows)

ifeq ($(OS),Darwin)
  # macOS — SDL2 installed via Homebrew
  SDL2_PREFIX   := $(shell brew --prefix sdl2 2>/dev/null || echo /usr/local)
  SDL2IMG_PREFIX := $(shell brew --prefix sdl2_image 2>/dev/null || echo /usr/local)
  SDL2TTF_PREFIX := $(shell brew --prefix sdl2_ttf 2>/dev/null || echo /usr/local)

  CXXFLAGS = -std=c++17 \
    -I$(SDL2_PREFIX)/include/SDL2 \
    -I$(SDL2IMG_PREFIX)/include/SDL2 \
    -I$(SDL2TTF_PREFIX)/include/SDL2

  LDFLAGS = \
    -L$(SDL2_PREFIX)/lib \
    -L$(SDL2IMG_PREFIX)/lib \
    -L$(SDL2TTF_PREFIX)/lib \
    -lSDL2 -lSDL2_image -lSDL2_ttf

  TARGET = $(BUILD_DIR)/game
else
  # Windows (MinGW)
  CXXFLAGS = -std=c++17 \
    -Iexternal/SDL2/include/SDL2 \
    -Iexternal/SDL2_image/include/SDL2 \
    -Iexternal/SDL2_ttf/include/SDL2

  # SDL2 libraries (CORRECT ORDER)   #-mwindows
  LDFLAGS = \
    -Lexternal/SDL2/lib \
    -Lexternal/SDL2_image/lib \
    -Lexternal/SDL2_ttf/lib \
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

  TARGET = $(BUILD_DIR)/game.exe
endif

# Directories
SRC_DIRS = Components Core Scenes
BUILD_DIR = build

# Source & objects
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/Components \
  rm -rf $(BUILD_DIR)/Core \
  rm -rf $(BUILD_DIR)/Scenes

re: clean all

# --- Emscripten (Web) Build ---
WEB_DIR = web_build
WEB_CXX = emcc
WEB_CXXFLAGS = -std=c++17 -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 \
  -s SDL2_IMAGE_FORMATS='["png"]'
WEB_LDFLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 \
  -s SDL2_IMAGE_FORMATS='["png"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=67108864 \
  --preload-file Assets --preload-file DefaultAssets \
  --shell-file web/shell.html
WEB_OBJS = $(patsubst %.cpp,$(WEB_DIR)/%.o,$(SRCS))

web: $(WEB_DIR)/game.html

$(WEB_DIR)/game.html: $(WEB_OBJS)
	$(WEB_CXX) $^ -o $@ $(WEB_LDFLAGS)

$(WEB_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(WEB_CXX) $(WEB_CXXFLAGS) -c $< -o $@

web-clean:
	rm -rf $(WEB_DIR)

web-serve: web
	@lsof -ti:8080 | xargs kill -9 2>/dev/null || true
	python3 -m http.server 8080 --directory $(WEB_DIR)

.PHONY: all clean re web web-clean web-serve