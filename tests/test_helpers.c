#include <stdio.h>
#include <stdlib.h>
#include "../src/types.h"

void p_init(PARAMETROS *p) {
	p->estado = (ESTADO*)malloc(sizeof(ESTADO));
	p->estado->board = NULL;
	p->estado->num_rows = 0;
	p->estado->num_cols = 0;
	p->estado->board_loaded = false;
	p->estado->move_stack = NULL;
}
void example_init(PARAMETROS *p) {
		char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};
	
	for (int i = 0; i < p->estado->num_rows; i++) {
		for (int j = 0; j < p->estado->num_cols; j++)
			p->estado->board[i][j] = example[i][j];
	}
}