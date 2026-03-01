# =============================================================================
# Makefile — Global vs Extern Demonstration
#
# Targets:
#   make / make all   — compile the project
#   make run          — compile and run
#   make clean        — remove build artefacts
# =============================================================================

CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Wshadow \
            -Wstrict-prototypes -Wmissing-prototypes \
            -g3 -O0

TARGET  := app
SRCS    := main.c system_config.c sensor.c actuator.c
OBJS    := $(SRCS:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "\nBuild successful → ./$(TARGET)\n"

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
