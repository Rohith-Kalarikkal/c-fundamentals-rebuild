# ==============================================================================
# Makefile — local_vs_static_demo
#
# Usage:
#   make            Build the release binary
#   make debug      Build with debug symbols and AddressSanitizer
#   make clean      Remove all build artefacts
#   make help       Print available targets
#
# Conventions:
#   - All intermediate object files are placed in $(BUILD_DIR).
#   - Source files are discovered automatically via $(wildcard ...).
#   - Compiler flags follow MISRA-friendly / production-grade settings.
# ==============================================================================

# ------------------------------------------------------------------------------
# Toolchain
# ------------------------------------------------------------------------------
CC      := gcc
RM      := rm -rf

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
TARGET := $(BUILD_DIR)/demo.exe

# ------------------------------------------------------------------------------
# Sources — auto-discover all .c files in src/, utils/, and root
# ------------------------------------------------------------------------------
SRCS := main.c \
        $(wildcard $(SRC_DIR)/*.c) \
        $(wildcard $(UTILS_DIR)/*.c)

# Derive object file paths inside BUILD_DIR (flatten directory structure)
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

# VPATH tells make where to find source files by name
VPATH := . $(SRC_DIR) $(UTILS_DIR)

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
# Debug build
# ------------------------------------------------------------------------------
.PHONY: debug
debug: CFLAGS = $(DEBUG_FLAGS)
debug: $(TARGET)
	@echo ""
	@echo "  Debug build complete → $(TARGET)  (ASan + UBSan enabled)"
	@echo ""

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
	@echo "    make debug    Build with -g3, ASan, UBSan"
	@echo "    make clean    Remove $(BUILD_DIR)/"
	@echo "    make help     Show this message"
	@echo ""
