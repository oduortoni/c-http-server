BUILD_TYPE ?= release

# Compiler
CC ?= cc

CFLAGS = -I$(SRC_DIR)/lib  # Include path for headers
CFLAGS += -std=gnu23
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
ALL_SRCS  = $(wildcard $(SRC_DIR)/*.c)
ALL_SRCS += $(wildcard $(SRC_DIR)/*/*.c)
ALL_SRCS += $(wildcard $(SRC_DIR)/*/*/*.c)
ALL_SRCS += $(wildcard $(SRC_DIR)/*/*/*/*.c)

# Object files with unique names
OBJECTS = $(foreach src,$(ALL_SRCS),$(call src_to_obj,$(src)))

# Default target
all: $(BIN_DIR)/server run

# Compilation target
$(BIN_DIR)/server: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $^

# Run target
.PHONY: run
run:
	./$(BIN_DIR)/server

# Generic rule to compile any source file to an object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/server

# Debugging target to print variables
print-%:
	@echo '$*=$($*)'

.PHONY: debug
debug:
	$(MAKE) BUILD_TYPE=debug
