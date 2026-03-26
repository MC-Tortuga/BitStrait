CC := gcc
CFLAGS := -g -O0 -Iinclude -Wall -Wextra -Wpedantic -std=c11 -D_DEFAULT_SOURCE
LDFLAGS := -lncurses

SRC_DIR := src
BIN_DIR := bin
INC_DIR := include

SRC := $(shell find $(SRC_DIR) -name "*.c")
OBJ := $(SRC:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)
TARGET := $(BIN_DIR)/bitstrait

lint:
	cppcheck --enable=all --inconclusive --std=c11 --suppress=missingIncludeSystem \
	-I $(INC_DIR) $(SRC_DIR)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run lint
