CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -fsanitize=address -Iinclude -Wunused-variable

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = main

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@MallocNanoZone=0 ./$(BIN_DIR)/$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
