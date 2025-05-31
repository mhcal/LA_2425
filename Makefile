CC = gcc
CCOV = -fprofile-arcs -ftest-coverage 
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDLIBS = -lcunit -lgcov

SRC_DIR = src
BOARD_DIR = boards
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
COV_DIR = $(BUILD_DIR)/coverage

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/repl.c $(SRC_DIR)/commands.c $(SRC_DIR)/utils.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TEST_SRC_TESTS = $(TEST_DIR)/test_main.c $(TEST_DIR)/test_commands.c $(TEST_DIR)/test_utils.c $(TEST_DIR)/test_helpers.c
TEST_SRC_SRC = $(SRC_DIR)/commands.c $(SRC_DIR)/utils.c
TEST_OBJ = $(TEST_SRC_TESTS:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o) $(TEST_SRC_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all jogo testar clean

all: jogo

# Link all object files to produce the final binary
jogo: $(OBJ) | $(BOARD_DIR)
	$(CC) $(CCOV) $(CFLAGS) -o jogo $^ 
	#rm -f $(OBJ_DIR)/*.gc*
testar: $(TEST_OBJ) | $(BOARD_DIR) $(COV_DIR)
	$(CC) $(CCOV) $(CFLAGS) -o $(BUILD_DIR)/testar $^ $(LDLIBS)
	./$(BUILD_DIR)/testar
	gcov --source-prefix=$(SRC_DIR) $(OBJ_DIR)/commands.gcno
	gcov --source-prefix=$(SRC_DIR) $(OBJ_DIR)/utils.gcno
	mv *.gc* $(COV_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CCOV) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

$(BOARD_DIR):
	mkdir -p $@

$(COV_DIR):
	mkdir -p $@

clean:
	rm -f jogo $(BUILD_DIR)/testar $(OBJ_DIR)/*.o $(OBJ_DIR)/*.gc* $(COV_DIR)/*.gc*
	rm -rf $(BOARD_DIR)/test_board $(BOARD_DIR)/test_read_board
