#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "state.h"

void print_board(ESTADO *e) {
	if (e->board_loaded) {
		printf("   ");
		for (int i = 0; i < e->num_cols; i++)
			printf("%c ", 'a' + i);
		putchar('\n');
		for (int i = 0; i < e->num_rows; i++) {
			printf("%2d ", i);
			for (int j = 0; j < e->num_cols; j++)
				printf("%c ", e->board[i][j]);
			putchar('\n');
		}
	}
	else
		printf("O tabuleiro ainda não foi carregado.\n");
}

bool parse_coord(char *coord, int *col, int *row, ESTADO *e) {
	*col = coord[0] - 'a';
	*row = atoi(coord + 1);
	if (*col < 0 || *col >= e->num_cols || *row < 0 || *row >= e->num_rows) {
		fprintf(stderr, "Erro: coordenadas estão fora dos limites do tabuleiro atual.\n");
		return false;
	}
	return true;
}
