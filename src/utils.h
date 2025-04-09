#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "types.h"

void print_board(ESTADO *e);
bool parse_coord(char *coord, int *col, int *row, ESTADO *e);
void free_board(ESTADO *e);
bool allocate_board(int rows, int cols, ESTADO *e);
bool push_move(int row, int col, char prev, ESTADO *e);
bool pop_move(ESTADO *e);
void free_move_stack(ESTADO *e);

#endif
