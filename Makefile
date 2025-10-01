# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99

# --- Directory names ---
BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src

# --- Executable file name ---
TARGET = $(BIN_DIR)/lister

# --- List of source files (.c) ---
SOURCES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/options/options.c \
	$(SRC_DIR)/directory_reader/directory_reader.c \
	$(SRC_DIR)/file_info/file_info.c \
	$(SRC_DIR)/display/display.c

# --- Paths to header files (.h) ---
INCLUDE_PATHS = \
	-I $(SRC_DIR)/options \
	-I $(SRC_DIR)/directory_reader \
	-I $(SRC_DIR)/file_info \
	-I $(SRC_DIR)/display

# Automatically generate the list of object files (.o) and place them in build/
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# --- Build rules ---
all: $(TARGET)

# Linking rule: Create the executable from .o files
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(CFLAGS)
	@echo "Build completed: $(TARGET)"

# Compilation rule: Create .o files from .c files
# $(@D) is the directory of the .o file (e.g., build/display)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# --- Cleanup ---
clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean