# Mastermind - MinGW build
#
#   mingw32-make          build build/Mastermind.exe
#   mingw32-make run      build and launch
#   mingw32-make clean    remove objects and executable

CC      = gcc
WINDRES = windres

# The sources are stored in Windows-1252, like the original Dev-C++ project.
CFLAGS  ?= -O2 -g -Wall -std=c99 -finput-charset=CP1252 -fexec-charset=CP1252
CPPFLAGS += -I src

LDFLAGS ?= -mwindows
LDLIBS  += -lcomctl32 -lgdi32

BUILD  := build
TARGET := $(BUILD)/Mastermind.exe
OBJS   := $(BUILD)/main.o $(BUILD)/resources.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD)/main.o: src/main.c src/resources.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD)/resources.o: resources/resources.rc src/resources.h resources/master.ico resources/master.bmp
	$(WINDRES) -I src -I resources resources/resources.rc -o $@

run: $(TARGET)
	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
