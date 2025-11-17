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
	$(SRC_DIR)/display/display.c \
	$(SRC_DIR)/sort/sort.c \
	$(SRC_DIR)/utils/path.c

# --- Paths to header files (.h) ---
INCLUDE_PATHS = \
	-I $(SRC_DIR) \
	-I $(SRC_DIR)/options \
	-I $(SRC_DIR)/directory_reader \
	-I $(SRC_DIR)/file_info \
	-I $(SRC_DIR)/display \
	-I $(SRC_DIR)/sort \
	-I $(SRC_DIR)/utils

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

# --- Installation ---
# Default installation directory
INSTALL_DIR = /usr/local/bin

# Install target: build and copy to system path
install: $(TARGET)
	@echo "Installing lister to $(INSTALL_DIR)..."
	@mkdir -p $(INSTALL_DIR)
	@cp $(TARGET) $(INSTALL_DIR)/lister
	@chmod +x $(INSTALL_DIR)/lister
	@echo "Installation completed! You can now use 'lister' from anywhere."

# Install to user's local bin directory (no sudo required)
install-user: $(TARGET)
	@echo "Installing lister to ~/bin..."
	@mkdir -p $$HOME/bin
	@cp $(TARGET) $$HOME/bin/lister
	@chmod +x $$HOME/bin/lister
	@echo "Installation completed to $$HOME/bin/lister"

# Uninstall target: remove from system
uninstall:
	@echo "Uninstalling lister from $(INSTALL_DIR)..."
	@rm -f $(INSTALL_DIR)/lister
	@echo "Uninstallation completed!"

# Uninstall from user's local bin
uninstall-user:
	@echo "Uninstalling lister from $$HOME/bin..."
	@rm -f $$HOME/bin/lister
	@echo "Uninstallation completed!"

.PHONY: all clean install install-user uninstall uninstall-user