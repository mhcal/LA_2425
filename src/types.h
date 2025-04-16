#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define LINE_SIZE 1024
#define ROW_SIZE 26
#define COL_SIZE 26

/*
 * o uso de arrays 2d dinamicamente alocados nos permite armazenar 
 * tabuleiros arbitrariamente grandes, no entanto, impomos um limite 
 * artificial para simplicidade das funções de print_board e parse_coord.
*/

typedef struct move {
	int row;
	int col;
	char prev;
	struct move *next;
} MOVE;

typedef struct estado {
	bool looping;
	char **board;
	int num_rows;
	int num_cols;
	bool board_loaded;
	MOVE *move_stack;
	int num_moves;
	bool help_trigger;
} ESTADO;

typedef bool (*COMANDO)(char *arg, ESTADO *e);

#endif
