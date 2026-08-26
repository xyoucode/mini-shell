CC      = gcc
CFLAGS  = -Wall -Wextra -std=gnu11 -g
SRC_DIR = src
BIN     = mini-shell

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/shell.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(SRC_DIR)/*.o $(BIN)
