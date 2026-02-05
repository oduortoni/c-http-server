# Compiler
CC := gcc-14
CFLAGS = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700  # Enable GNU and POSIX extensions with C23
CFLAGS += -I$(SRC_DIR)/lib  # Include path for headers
CFLAGS += -ggdb3
CFLAGS += -std=gnu23
CFLAGS += -Wall -Wextra -pedantic
CFLAGS += -Werror
CFLAGS += -fsanitize=address -fsanitize=undefined


# Directories
SRC_DIR = src
LIB_DIR = $(SRC_DIR)/lib
APP_DIR = $(SRC_DIR)/app
NET_DIR = $(LIB_DIR)/net
HTTP_DIR = $(LIB_DIR)/http
ENV_DIR = $(LIB_DIR)/env
BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj

# Function to convert source path to unique object file path
define src_to_obj
$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(1))
endef

# Source files
MAIN_SRC = $(SRC_DIR)/main.c
NET_SRCS = $(wildcard $(NET_DIR)/*.c)
HTTP_SRCS = $(wildcard $(HTTP_DIR)/*.c)
ENV_SRCS = $(wildcard $(ENV_DIR)/*.c)
APP_SRCS = $(wildcard $(APP_DIR)/*.c)
ALL_SRCS = $(MAIN_SRC) $(NET_SRCS) $(HTTP_SRCS) $(ENV_SRCS) $(APP_SRCS)

# Object files with unique names
OBJECTS = $(foreach src,$(ALL_SRCS),$(call src_to_obj,$(src)))

# Default target
go: init compile run

# Compilation target
compile: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $^

# Run target
run:
	./$(BIN_DIR)/server

# Initialization target
init:
	mkdir -p $(OBJ_DIR)

# Generic rule to compile any source file to an object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(NET_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(HTTP_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(ENV_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(APP_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/server

# Debugging target to print variables
print-%:
	@echo '$*=$($*)'
