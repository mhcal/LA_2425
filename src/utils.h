#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "types.h"

bool is_valid_coord(int row, int col, ESTADO *e);
void print_board(ESTADO *e);
bool parse_coord(char *coord, int *col, int *row, ESTADO *e);
void free_board(ESTADO *e);
bool allocate_board(int rows, int cols, ESTADO *e);
bool push_move(int row, int col, char prev, ESTADO *e);
bool pop_move(ESTADO *e, bool suppress);
void free_move_stack(ESTADO *e);
bool is_branca(char c);
bool is_riscada(char c);
char to_lower(char c);
void dfs_connectivity(int row, int col, ESTADO *e, bool **visited);
bool verifica_branca(int row, int col, ESTADO *e);
bool verifica_riscada(int row, int col, ESTADO *e);
bool verifica_caminho(ESTADO *e);
bool is_minuscula(char c);
void pinta_vizinhos(int row, int col, ESTADO *e);
void risca_iguais(int row, int col, ESTADO *e);
bool isola(int row, int col, ESTADO *e);
ESTADO *copy_estado(ESTADO *src);
void free_estado(ESTADO *e);
bool verifica_aux(ESTADO *e);
bool acha_minuscula(ESTADO *e, POSITION *pos);
bool dfs(ESTADO *e);

#endif
