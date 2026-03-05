# =============================================================================
# Makefile — volatile, const and ISR
# =============================================================================

# ------------------------------------------------------------------------------
# Toolchain
# ------------------------------------------------------------------------------
CC      := gcc
RM      := rm -rf

# ------------------------------------------------------------------------------
# Directories
# ------------------------------------------------------------------------------
INC_DIR   := include
BUILD_DIR := build

# ------------------------------------------------------------------------------
# Target binary
# ------------------------------------------------------------------------------
TARGET := $(BUILD_DIR)/vol_con_isr

# ------------------------------------------------------------------------------
# Sources — auto-discover all .c files in src/, utils/, and root
# ------------------------------------------------------------------------------
SRCS := \
    main.c \
    01_volatile_hardware_registers/hw_registers.c \
    01_volatile_hardware_registers/volatile_registers.c \
    02_const_flash_memory/flash_const_data.c \
    02_const_flash_memory/const_flash.c \
    03_isr_flag_simulation/isr_flags.c \
    03_isr_flag_simulation/run_isr_flags.c

# Derive object file paths inside BUILD_DIR (flatten directory structure)
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

# Find all directories containing .c files and add them to VPATH
VPATH := $(sort $(dir $(SRCS)))

# ------------------------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------------------------
COMMON_FLAGS := \
    -std=c11              \
    -Wall                 \
    -Wextra               \
	-Wpadded			  \
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
	@echo "  Build complete -> $(TARGET)"
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
	@echo "    make          Build release binary  -> $(TARGET)"
	@echo "    make clean    Remove $(BUILD_DIR)/"
	@echo "    make help     Show this message"
	@echo ""	

