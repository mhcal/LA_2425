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
	e->num_moves++;
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
	e->num_moves--;
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
	e->num_moves = 0;
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

bool is_valid_coord(int row, int col, ESTADO *e) {
	return (row >= 0 && row < e->num_rows && col >= 0 && col < e->num_cols);
}

bool parse_coord(char *coord, int *col, int *row, ESTADO *e) {
	if (strlen(coord) < 2 || coord[0] < 97 || coord[0] > 122) {
		fprintf(stderr, "Erro: formato invalido.\n");
		return false;
	}
	*col = coord[0] - 'a';
	*row = atoi(coord + 1) - 1;
	if (!(is_valid_coord(*row, *col, e)))
		return false;
	return true;
}

bool is_branca(char c) {
	return (c >= 'A' && c <= 'Z');
}

bool is_riscada(char c) {
	return (c == '#');
}

bool is_minuscula(char c) {
	return (c >= 97 && c <= 122);
}

char to_lower(char c) {
	if (is_branca(c))
		return c + 32;
	else
		return c;
}

void dfs_connectivity(int row, int col, ESTADO *e, bool **visited) {
	if (!(is_valid_coord(row, col, e)) || is_riscada(e->board[row][col]) || visited[row][col])
		return;
	visited[row][col] = true;
	int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
	for (int i = 0; i < 4; i++) {
		int dr = row + directions[i][0];
		int dc = col + directions[i][1];
		dfs_connectivity(dr, dc, e, visited);
	}
}

bool verifica_branca(int row, int col, ESTADO *e) {
	if (!is_branca(e->board[row][col]))
		return false;
	char letter = to_lower(e->board[row][col]);
	// verifica linha (linha fixa, coluna variavel)
	for (int i = 0; i < e->num_cols; i++) {
		char current = e->board[row][i];
		if (i != col && is_branca(current) && to_lower(current) == letter)
			return false;
	}
	// verifica coluna (linha variavel, coluna fixa)
	for (int i = 0; i < e->num_rows; i++) {
		char current = e->board[i][col];
		if (i != row && is_branca(current) && to_lower(current) == letter)
			return false;
	}
	return true;
}

// verifica_riscada agora apenas verifica se há vizinhos ortogonais riscados. a regra das casas vizinhas brancas deverá ser enforçada pelo comando ajuda.
bool verifica_riscada(int row, int col, ESTADO *e) {
	if (!is_riscada(e->board[row][col]))
		return false;
	// up, down, left right
	int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
	for (int i = 0; i < 4; i++) {
		int dr = row + directions[i][0];
		int dc = col + directions[i][1];
		if (is_valid_coord(dr, dc, e) && is_riscada(e->board[dr][dc]))
			return false;
	}
	return true;
}

bool verifica_caminho(ESTADO *e) {
	int start_row = -1;
	int start_col = -1;
	// encontrar coord. da primeira letra pintada
	for (int i = 0; i < e->num_rows && start_row == -1; i++) {
		for (int j = 0; j < e->num_cols && start_col == -1; j++) {
			if (is_branca(e->board[i][j])) {
				start_row = i;
				start_col = j;
			}
		}
	}
	// caso trivial: nenhuma casa pintada
	if (start_row == -1 || start_col == -1)
		return true;
	// alocação de memória para matriz de conectividade
	bool **visited = (bool **)malloc(e->num_rows * sizeof(bool *));
	for (int i = 0; i < e->num_rows; i++) {
		// calloc inicializa os blocos de memória a 0s (valor padrão da matriz)
		visited[i] = (bool *)calloc(e->num_cols, sizeof(bool));
	}
	dfs_connectivity(start_row, start_col, e, visited);
	bool all_connected = true;
	for (int i = 0; i < e->num_rows; i++) {
		for (int j = 0; j < e->num_cols; j++) {
			if (is_branca(e->board[i][j]) && !visited[i][j])
				all_connected = false;
		}
	}
	// liberta memória
	for (int i = 0; i < e->num_rows; i++)
		free(visited[i]);
	free(visited);
	return all_connected;
}

void pinta_vizinhos(int row, int col, ESTADO *e) {
	if (!is_riscada(e->board[row][col]))
		return;
	int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];
		if (is_valid_coord(dr, dc, e) && is_minuscula(e->board[dr][dc]))
			e->board[dr][dc] -= 32;
	}
}

void risca_iguais(int row, int col, ESTADO *e) {
	if (!is_branca(e->board[row][col]))
		return;
	char letter = to_lower(e->board[row][col]);
	// procura na linha (linha fixa, coluna variavel)
	for (int i = 0; i < e->num_cols; i++) {
		char current = e->board[row][i];
		if (i != col && current == letter)
			e->board[row][i] = '#';
	}
	// procura na coluna (linha variavel, coluna fixa)
	for (int i = 0; i < e->num_rows; i++) {
		char current = e->board[i][col];
		if (i != row && current == letter)
			e->board[i][col] = '#';
	}
}
