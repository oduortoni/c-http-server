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

#
# Application
#

# Function to convert source path to unique object file path
define src_to_obj
$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(1))
endef

# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR := $(BIN_DIR)/$(BUILD_TYPE)

LIBRARY_SOURCES  = $(shell find $(SRC_DIR)/lib -name '*.c')
LIBRARY_OBJECTS  = $(foreach src,$(LIBRARY_SOURCES),$(call src_to_obj,$(src)))
# LIBRARY_OBJECTS := $(filter-out $(OBJ_DIR)/main.o, $(LIBRARY_OBJECTS))

# Source files
APPLICATION_SOURCES = $(shell find $(SRC_DIR)/app -name '*.c') $(LIBRARY_SOURCES) $(SRC_DIR)/main.c

# Object files with unique names
APPLICATION_OBJECTS = $(foreach src,$(APPLICATION_SOURCES),$(call src_to_obj,$(src)))

# Run target
.PHONY: run
run: ./$(BIN_DIR)/server
	./$(BIN_DIR)/server

# Compilation target
$(BIN_DIR)/server: $(APPLICATION_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(APPLICATION_OBJECTS)

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

TEST_OBJECTS := $(filter-out $(OBJ_DIR)/main.o, $(APPLICATION_OBJECTS))

$(BIN_DIR)/gtest: $(TESTS) $(TEST_OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) -I. $(CXX_FLAGS) $(TEST_OBJECTS) $(TESTS_DIR)/gtest.cpp -o $@ $(CXX_LIBS)

.PHONY: test
test: $(BIN_DIR)/gtest
	$(BIN_DIR)/gtest

#
# Compile template files
#

ZC_TOOL_PATH ?= zc

TEMPLATES_DIR = $(SRC_DIR)/app/templates
TEMPLATES = $(shell find $(TEMPLATES_DIR) -name '*.html')

$(TEMPLATES_DIR)/%.h: $(TEMPLATES_DIR)/%.html
	$(ZC_TOOL_PATH) $^ > $@

define template_to_header
$(patsubst $(TEMPLATES_DIR)/%.html,$(TEMPLATES_DIR)/%.h,$(1))
endef

TEMPLATES_COMPILED = $(foreach src,$(TEMPLATES),$(call template_to_header,$(src)))

compile_templates: $(TEMPLATES_COMPILED)

# Clean target
clean:
	rm -rf $(BIN_DIR) $(TEMPLATES_COMPILED)

# Debugging target to print variables
print-%:
	@echo '$*=$($*)'

.PHONY: debug
debug:
	$(MAKE) BUILD_TYPE=debug
