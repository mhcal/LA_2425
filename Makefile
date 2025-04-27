CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDLIBS = -lcunit

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BOARD_DIR = $(BUILD_DIR)/boards

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/repl.c $(SRC_DIR)/commands.c $(SRC_DIR)/utils.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TEST_SRC = $(TEST_DIR)/test_main.c $(TEST_DIR)/test_utils.c $(TEST_DIR)/test_commands.c $(SRC_DIR)/utils.c $(SRC_DIR)/commands.c
TEST_OBJ = $(TEST_SRC:.c=.o)

.PHONY: all jogo testar clean gcov

all: jogo

# Link all object files to produce the final binary
jogo: $(OBJ) | $(BOARD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/jogo $^ 

testar: $(TEST_OBJ) | $(BOARD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/testar $^ $(LDLIBS)
	./$(BUILD_DIR)/testar

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

$(BOARD_DIR):
	mkdir -p $@

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(BUILD_DIR)/jogo $(BUILD_DIR)/testar $(OBJ_DIR)/*.o
	rm -rf boards
