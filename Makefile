CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDLIBS = -lcunit

SRC_DIR = src
TEST_DIR = test
BOARD_DIR = boards

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/state.c $(SRC_DIR)/commands.c
OBJ = $(SRC:.c=.o)

TEST_SRC = $(TEST_DIR)/test_state.c $(TEST_DIR)/test_commands.c $(SRC_DIR)/state.c $(SRC_DIR)/commands.c
TEST_OBJ = $(TEST_SRC:.c=.o)

.PHONY: all jogo testar clean

all: jogo

jogo: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

testar: $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	./testar

$(BOARD_DIR):
	mkdir -p $(BOARD_DIR)

jogo : | $(BOARD_DIR)

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o jogo testar
