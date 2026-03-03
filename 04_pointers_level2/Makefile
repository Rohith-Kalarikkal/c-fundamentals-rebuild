# =============================================================================
# Makefile — C Pointers Level2 Tutorial
#
# Targets:
#   make              - compile the project
#   make run          - compile and run
#   make clean        - remove build artefacts
#	make help		  - Shows help messages
# =============================================================================

# ------------------------------------------------------------------------------
# Toolchain
# ------------------------------------------------------------------------------
CC      := gcc
RM      := rm -rf
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -Iinclude

# ------------------------------------------------------------------------------
# Directories
# ------------------------------------------------------------------------------
SRC_DIR   := src
UTILS_DIR := utils
INC_DIR   := include
BUILD_DIR := build

# ------------------------------------------------------------------------------
# Target binary
# ------------------------------------------------------------------------------
TARGET := $(BUILD_DIR)/pointers_demo.exe

# ------------------------------------------------------------------------------
# Sources — auto-discover all .c files in src/, utils/, and root
# ------------------------------------------------------------------------------
SRCS := main.c \
        $(wildcard $(SRC_DIR)/*.c)

# Derive object file paths inside BUILD_DIR (flatten directory structure)
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

# VPATH tells make where to find source files by name
VPATH := . $(SRC_DIR)

# ------------------------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------------------------
COMMON_FLAGS := \
    -std=c11              \
    -Wall                 \
    -Wextra               \
    -Wpedantic            \
    -Wshadow              \
    -Wstrict-prototypes   \
    -Wmissing-prototypes  \
    -Wcast-align          \
    -Wconversion          \
    -I$(INC_DIR)

RELEASE_FLAGS := $(COMMON_FLAGS) -O2
DEBUG_FLAGS   := $(COMMON_FLAGS) -O0 -g3 -fsanitize=address,undefined

CFLAGS ?= $(RELEASE_FLAGS)

# ------------------------------------------------------------------------------
# Default target
# ------------------------------------------------------------------------------
.PHONY: all
all: $(TARGET)
	@echo ""
	@echo "  Build complete → $(TARGET)"
	@echo "  Run with: ./$(TARGET)"
	@echo ""

# ------------------------------------------------------------------------------
# Link
# ------------------------------------------------------------------------------
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# ------------------------------------------------------------------------------
# Compile — pattern rule for all .c → .o
# ------------------------------------------------------------------------------
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------------------------------------
# Create build directory if it doesn't exist
# ------------------------------------------------------------------------------
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ------------------------------------------------------------------------------
# Run
# ------------------------------------------------------------------------------
.PHONY: run
run: $(TARGET)
	@./$(TARGET)

# ------------------------------------------------------------------------------
# Clean
# ------------------------------------------------------------------------------
.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)
	@echo "  Build artefacts removed."

# ------------------------------------------------------------------------------
# Help
# ------------------------------------------------------------------------------
.PHONY: help
help:
	@echo ""
	@echo "  Targets:"
	@echo "    make          Build release binary  → $(TARGET)"
	@echo "    make clean    Remove $(BUILD_DIR)/"
	@echo "    make help     Show this message"
	@echo ""

