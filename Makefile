CC := gcc
CFLAGS :=-g -O0 -Iinclude -Wall -Wextra -std=c11
LDFLAGS := -lncurses

SRC := $(shell find src -name "*.c")
OBJ := $(SRC:src/%.c=bin/%.o)
TARGET := bin/bitstrait_bin

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

bin/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin/*
	
.PHONY: all clean
