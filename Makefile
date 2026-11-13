CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude -O2
SRC = $(wildcard src/*.cpp)
TARGET = bin/hydra_lsm

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: all
	./$(TARGET)

clean:
	rm -rf bin/ build/ data/
