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
bool is_branca(char c);
bool is_riscada(char c);
char to_lower(char c);
void dfs_connectivity(int row, int col, ESTADO *e, bool **visited);
bool verifica_branca(int row, int col, ESTADO *e);
bool verifica_riscada(int row, int col, ESTADO *e);
bool verifica_caminho(ESTADO *e);

#endif
