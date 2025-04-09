CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
##LDLIBS = -lcunit

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BOARD_DIR = $(BUILD_DIR)/boards

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/repl.c $(SRC_DIR)/commands.c $(SRC_DIR)/utils.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all jogo clean

all: jogo

# Link all object files to produce the final binary
jogo: $(OBJ) | $(BOARD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/jogo $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

$(BOARD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/jogo

