BUILD_TYPE ?= release
MAKEFLAGS += --jobs=$(shell nproc)

# Compiler
CC ?= cc

INCLUDES = -I$(SRC_DIR)/lib  # Include path for headers

CFLAGS  = -std=gnu23
CFLAGS += -Wall -Wextra -pedantic
# Some warnings and errors can only be catched at specific level of
# optimization. For testing purposed it is better to stay consistent.
OPTIMIZATION_LEVEL ?= -O2
CFLAGS += $(OPTIMIZATION_LEVEL)
ifeq (${BUILD_TYPE}, debug)
	CFLAGS += -ggdb3
	CFLAGS += -Werror
	CFLAGS += -fsanitize=address -fsanitize=undefined
endif

# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR := $(BIN_DIR)/$(BUILD_TYPE)

# Function to convert source path to unique object file path
define src_to_obj
$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(1))
endef

# Source files
ALL_SRCS  = $(shell find $(SRC_DIR) -name '*.c')

# Object files with unique names
OBJECTS = $(foreach src,$(ALL_SRCS),$(call src_to_obj,$(src)))

# Run target
.PHONY: run
run: ./$(BIN_DIR)/server
	./$(BIN_DIR)/server

# Compilation target
$(BIN_DIR)/server: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $^

# Generic rule to compile any source file to an object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

#
# Unit testing using GTest
#

CXX ?= c++

CXX_FLAGS  = -std=c++23
CXX_FLAGS += -Wall -Wextra -pedantic
CXX_FLAGS += $(OPTIMIZATION_LEVEL)
CXX_FLAGS += -ggdb3
CXX_FLAGS += -Werror
CXX_FLAGS += -fsanitize=address -fsanitize=undefined
CXX_FLAGS += $(shell pkg-config --cflags gtest)

CXX_LIBS = $(shell pkg-config --libs gtest)

TESTS_DIR = tests
TESTS  = $(shell find $(TESTS_DIR) -name '*.hpp')

TEST_OBJECTS := $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS))

$(BIN_DIR)/gtest: $(TESTS) $(TEST_OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) $(TEST_OBJECTS) $(TESTS_DIR)/gtest.cpp -o $@ $(CXX_LIBS)

.PHONY: test
test: $(BIN_DIR)/gtest
	$(BIN_DIR)/gtest

# Clean target
clean:
	rm -rf $(BIN_DIR)

# Debugging target to print variables
print-%:
	@echo '$*=$($*)'

.PHONY: debug
debug:
	$(MAKE) BUILD_TYPE=debug
