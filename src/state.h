#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#define LINE_SIZE 1024
#define ROW_SIZE 10
#define COL_SIZE 26

typedef struct estado {
	bool looping;
	char board[ROW_SIZE][COL_SIZE];
	int num_rows;
	int num_cols;
	bool board_loaded;
} ESTADO;

void print_board(ESTADO *e);
bool parse_coord(char *coord, int *col, int *row, ESTADO *e);

#endif
