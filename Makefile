CXX = g++
CFLAGS = -Wall -std=c++11 $(shell sdl2-config --cflags)
LFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
PLATFORM = $(shell uname -s)

ifeq ($(RELEASE), yes)
	LFLAGS += -O3
endif
ifeq ($(COMPACT), yes)
	CFLAGS += -ffreestanding -fno-inline -fdata-sections -ffunction-sections \
		-fno-exceptions -fno-asynchronous-unwind-tables
	LFLAGS += -Os
endif
ifeq ($(DEBUG), yes)
	LFLAGS += -ggdb -g3 -pg -O0
endif
ifeq ($(PLATFORM), Linux)
	LFLAGS += -lGL -lGLU -lGLEW
else
	LFLAGS += -lopengl32 -lglu32 -lglew32
endif

target_file  := main
prog_name    := golos
dest_dir     := ./src/
object_files := $(patsubst %.cpp, %.o, $(wildcard $(dest_dir)*.cpp))

all: $(dest_dir)$(target_file)

$(dest_dir)$(target_file): $(object_files)
	$(CXX) $(object_files) -o $(prog_name) $(LFLAGS)

strip:
	strip -s $(prog_name)*

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(prog_name) $(object_files)
