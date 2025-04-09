#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

bool push_move(int row, int col, char prev, ESTADO *e) {
	MOVE *move = (MOVE*)malloc(sizeof(MOVE));
	if (move == NULL) {
		fprintf(stderr, "Erro: falha na alocação de memória para histórico de jogadas.\n");
		return false;
	}
	move->row = row;
	move->col = col;
	move->prev = prev;
	move->next = e->move_stack;
	e->move_stack = move;
	e->stack_size++;
	return true;
}

bool pop_move(ESTADO *e) {
	if (e->move_stack == NULL) {
		fprintf(stderr, "Erro: move stack vazia.\n");
		return false;
	}
	e->board[e->move_stack->row][e->move_stack->col] = e->move_stack->prev;
	MOVE *temp = e->move_stack;
	e->move_stack = e->move_stack->next;
	free(temp);
	e->stack_size--;
	return true;
}

void free_move_stack(ESTADO *e) {
	if (e->move_stack == NULL)
		return;
	MOVE *current = e->move_stack;
	while (current != NULL) {
		MOVE *temp = current;
		current = current->next;
		free(temp);
	}
	e->move_stack = NULL;
	e->stack_size = 0;
}

bool allocate_board(int rows, int cols, ESTADO *e) {
	if (e->board != NULL) {
		for (int i = 0; i < e->num_rows; i++)
			free(e->board[i]);
		free(e->board);
	}
	e->board = (char**)malloc(rows * sizeof(char*));
	if (e->board == NULL) {
		fprintf(stderr, "Erro: falha na alocação de memória.\n");
		return false;
	}
	for (int i = 0; i < rows; i++) {
		e->board[i] = (char*)malloc(cols * sizeof(char));
		if (e->board[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(e->board[j]);
			free(e->board);
			e->board = NULL;
			fprintf(stderr, "Erro: falha na alocação de memória.\n");
			return false;
		}
		memset(e->board[i], ' ', cols);
	}
	e->num_rows = rows;
	e->num_cols = cols;
	return true;
}

void free_board(ESTADO *e) {
	if (e->board != NULL) {
		free_move_stack(e);
		for (int i = 0; i < e->num_rows; i++)
			free(e->board[i]);
		free(e->board);
		e->board = NULL;
	}
}

void print_board(ESTADO *e) {
	if (e->board_loaded) {
		printf("   ");
		for (int i = 0; i < e->num_cols; i++)
			printf("%c ", 'a' + i);
		putchar('\n');
		for (int i = 0; i < e->num_rows; i++) {
			printf("%2d ", i + 1);
			for (int j = 0; j < e->num_cols; j++)
				printf("%c ", e->board[i][j]);
			putchar('\n');
		}
	}
	else
		printf("O tabuleiro ainda não foi carregado.\n");
}

bool parse_coord(char *coord, int *col, int *row, ESTADO *e) {
	if (strlen(coord) < 2 || coord[0] < 97 || coord[0] > 122) {
		fprintf(stderr, "Erro: formato invalido.\n");
		return false;
	}
	*col = coord[0] - 'a';
	*row = atoi(coord + 1) - 1;
	if (*col < 0 || *col >= e->num_cols || *row < 0 || *row >= e->num_rows) {
		fprintf(stderr, "Erro: coordenadas estão fora dos limites do tabuleiro atual.\n");
		return false;
	}
	return true;
}
