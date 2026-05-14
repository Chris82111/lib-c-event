CC      = gcc
CXX     = g++

FLAGS  = -Wall -Wextra -g -MMD -MP -std=c11
CXXFLAGS= -Wall -Wextra -g -MMD -MP -std=c++23
 
SRC_DIR = ./../../src
INC_DIR = ./../../inc
OBJ_DIR = ./obj
BIN_DIR = ./bin

MAIN    = ./main.cpp   # can also be main.cpp

# basic  : sudo apt install build-essential
# doxygen: sudo apt install doxygen
# dot    : sudo apt install graphviz

# Find sources
C_SRC   := $(shell find $(SRC_DIR) -type f -name '*.c') \
           $(shell find $(.) -type f -name '*.c')
CPP_SRC := $(shell find $(SRC_DIR) -type f -name '*.cpp') \
           $(shell find $(.) -type f -name '*.cpp')

# Add main to correct list
ifeq ($(suffix $(MAIN)),.cpp)
	CPP_SRC += $(MAIN)
else
	C_SRC += $(MAIN)
endif

# Map to object files
C_OBJ   := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SRC))
C_OBJ   := $(patsubst ./%.c,$(OBJ_DIR)/%.o,$(C_OBJ))

CPP_OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))
CPP_OBJ := $(patsubst ./%.cpp,$(OBJ_DIR)/%.o,$(CPP_OBJ))

OBJ = $(C_OBJ) $(CPP_OBJ)
DEP = $(OBJ:.o=.d)

# --- Automatic linker selection ---
ifneq ($(CPP_OBJ),)
	LINKER  = $(CXX)
	LDFLAGS = $(CXXFLAGS)
else
	LINKER  = $(CC)
	LDFLAGS = $(FLAGS)
endif
# ----------------------------------

TARGET = $(BIN_DIR)/$(notdir $(CURDIR))

all: $(TARGET)

# Link
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(LINKER) $(LDFLAGS) -o $@ $^

# Compile C
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

# Compile C++
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Compile local files (main)
$(OBJ_DIR)/%.o: ./%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: ./%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Ensure bin exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)


PROJDIR := $(C_SRC) $(CPP_SRC) $(INC_DIR)
export PROJDIR
DOXYGEN_HTML_OUTPUT = $(BIN_DIR)/doxy_html
export DOXYGEN_HTML_OUTPUT
LATEX_HTML_OUTPUT = $(BIN_DIR)/doxy_latex
export LATEX_HTML_OUTPUT
HAVE_DOT := YES
export HAVE_DOT


doxy: all
	@echo "doxygen ..."
	@doxygen Doxyfile

ifeq ($(OS),Windows_NT)
    OPEN_CMD := start
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        OPEN_CMD := open
    else
        OPEN_CMD := xdg-open
    endif
endif

open:
	$(OPEN_CMD) $(BIN_DIR)/doxy_html/index.html 2>/dev/null

-include $(DEP)
