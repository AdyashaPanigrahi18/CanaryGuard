CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
SOURCES := $(wildcard src/*.cpp)
TARGET := canary_monitor$(if $(filter Windows_NT,$(OS)),.exe,)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f canary_monitor canary_monitor.exe

.PHONY: all run clean
