# Include files to watch for changes
INCLUDE_FILES = $(wildcard src/*.h)

# Local source files
SOURCE_FILES = $(wildcard src/*.cpp)

# Object files
objects = $(subst src,build,$(SOURCE_FILES:.cpp=.o))

# Compiler flags
CFLAGS = -Isrc -Ilib/SOIL -DGL_GLEXT_PROTOTYPES

# Linker flags
LINKFLAGS = -Llib/SOIL -lsoil -lm -lGLEW -lftgl

# Compile command
COMPILE = g++ -O0 -ggdb -Wall -Werror -Wfatal-errors 

# OS Detection
OS=$(shell uname -s)
ifeq ($(OS), Linux)
    CFLAGS += `sdl-config --cflags` -I/usr/include/freetype2
    LINKFLAGS += -lopenal -lglut -lGLU `sdl-config --libs`
    COMPILE += -Dlinux
    OUTPUT = tws-linux
endif
ifeq ($(OS), Darwin)
    CFLAGS += -I/opt/local/include -I/opt/local/include/freetype2 -isysroot /Developer/SDKs/MacOSX10.6.sdk -mmacosx-version-min=10.6
    LINKFLAGS += -L/opt/local/lib -lobjc -framework Cocoa -framework OpenAL -framework OpenGL -framework SDL
    COMPILE += -arch x86_64 -Dmacosx
    COCOAWINDOW = build/SDLMain.o
    OUTPUT = tws-mac
endif

COMPILE += $(CFLAGS)

tws: $(objects) $(COCOAWINDOW)
	$(COMPILE) -o $(OUTPUT) $(objects) $(COCOAWINDOW) $(LINKFLAGS)

$(COCOAWINDOW): build/%.o: src/osx/%.m $(INCLUDE_FILES) Makefile
	$(COMPILE) $< -c -o $@

$(objects): build/%.o: src/%.cpp $(INCLUDE_FILES) Makefile
	$(COMPILE) $< -c -o $@

clean:
	rm build/*.o tws-mac tws-linux

