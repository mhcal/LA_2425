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
	if (e->ajuda_dada)
		e->num_ajuda++;
	return true;
}

bool pop_move(ESTADO *e, bool suppress) {
	if (e->move_stack == NULL) {
		fprintf(stderr, "Erro: move stack vazia.\n");
		return false;
	}
	if (!suppress)
		printf("%c%d : %c => %c\n", e->move_stack->col + 'a', e->move_stack->row + 1, e->board[e->move_stack->row][e->move_stack->col], e->move_stack->prev);
	e->board[e->move_stack->row][e->move_stack->col] = e->move_stack->prev;
	MOVE *temp = e->move_stack;
	e->move_stack = e->move_stack->next;
	free(temp);
	e->num_moves--;
	if (e->num_ajuda > 0)
		e->num_ajuda--;
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
		int dr = row + directions[i][0];
		int dc = col + directions[i][1];
		if (is_valid_coord(dr, dc, e) && is_minuscula(e->board[dr][dc])) {
			push_move(dr, dc, e->board[dr][dc], e);
			e->board[dr][dc] -= 32;
		}
	}
}

void risca_iguais(int row, int col, ESTADO *e) {
	if (!is_branca(e->board[row][col]))
		return;
	char letter = to_lower(e->board[row][col]);
	// procura na linha (linha fixa, coluna variavel)
	for (int i = 0; i < e->num_cols; i++) {
		char current = to_lower(e->board[row][i]);
		if (i != col && current == letter) {
			push_move(row, i, e->board[row][i], e);
			e->board[row][i] = '#';
		}
	}
	// procura na coluna (linha variavel, coluna fixa)
	for (int i = 0; i < e->num_rows; i++) {
		char current = to_lower(e->board[i][col]);
		if (i != row && current == letter) {
			push_move(i, col, e->board[i][col], e);
			e->board[i][col] = '#';
		}
	}
}

// a implementação atual não copia a stack de movimentos (não há necessidade, visto que os estados copiados só são utilizados para verificações/look-ups
ESTADO *copy_estado(ESTADO *src) {
	if (src == NULL) {
		fprintf(stderr, "Erro: tentando copiar um estado nulo.\n");
		return NULL;
	}
	ESTADO *dest = (ESTADO *)malloc(sizeof(ESTADO));
	if (dest == NULL) {
		fprintf(stderr, "Erro: falha na alocação de memória para estado.\n");
		return NULL;
	}
	dest->looping = src->looping;
	dest->num_rows = src->num_rows;
	dest->num_cols = src->num_cols;
	dest->board_loaded = src->board_loaded;
	dest->num_moves = 0;
	dest->num_ajuda = src->num_ajuda;
	dest->ajuda_dada = src->ajuda_dada;
	dest->move_stack = NULL;
	// copia board
	if (src->board_loaded && src->board != NULL) {
		dest->board = (char **)malloc(src->num_rows * sizeof(char *));
		if (dest->board == NULL) {
			fprintf(stderr, "Erro: falha na alocação de memória para tabuleiro.\n");
			free(dest);
			return NULL;
		}
		for (int i = 0; i < src->num_rows; i++) {
			dest->board[i] = (char *)malloc(src->num_cols * sizeof(char));
			if (dest->board[i] == NULL) {
				for (int j = 0; j < i; j++)
					free(dest->board[j]);
				free(dest->board);
				free(dest);
				fprintf(stderr, "Erro: falha na alocação de memória para linha do tabuleiro.\n");
				return NULL;
			}
			memcpy(dest->board[i], src->board[i], src->num_cols * sizeof(char));
		}
	}
	else
		dest->board = NULL;
	// copia stack de jogadas
	if (src->move_stack != NULL) {
		MOVE *temp_stack = NULL;
		MOVE *current = src->move_stack;
		while (current != NULL) {
			MOVE *new_move = (MOVE *)malloc(sizeof(MOVE));
			new_move->row = current->row;
			new_move->col = current->col;
			new_move->prev = current->prev;
			new_move->next = temp_stack;
			temp_stack = new_move;
			current = current->next;
		}
		current = temp_stack;
		// inverte a stack para a ordem correta
		while (current != NULL) {
			MOVE *new_move = (MOVE *)malloc(sizeof(MOVE));
			new_move->row = current->row;
			new_move->col = current->col;
			new_move->prev = current->prev;
			new_move->next = dest->move_stack;
			dest->move_stack = new_move;
			dest->num_moves++;
			MOVE *to_free = current;
			current = current->next;
			free(to_free);
		}
	}
	return dest;
}

void free_estado(ESTADO *e) {
	if (e == NULL)
		return;
	if (e->board != NULL) {
		for (int i = 0; i < e->num_rows; i++)
			free(e->board[i]);
		free(e->board);
	}
	free_move_stack(e);
	free(e);
}

bool isola(int row, int col, ESTADO *e) {
	char original = e->board[row][col];
	if (!is_minuscula(original) || !verifica_caminho(e))
		return false;
	e->board[row][col] = '#';
	bool result = !verifica_caminho(e);
	e->board[row][col] = original;
	return result;
}

// mesma lógica do verificar, no entanto dá um early return no primeiro erro encontrado (mais rápido para o dfs).
bool verifica_aux(ESTADO *e) {
	for (int i = 0; i < e->num_rows; i++) {
		for (int j = 0; j < e->num_cols; j++) {
			if (is_branca(e->board[i][j])) {
				if (!verifica_branca(i, j, e))
					return false;
			}
			if (is_riscada(e->board[i][j])) {
				if (!verifica_riscada(i, j, e))
					return false;
			}
		}
	}
	return verifica_caminho(e);
}

// conta o numero de jogadas possíveis de uma determinada casa do tabuleiro
int conta_jogadas(int row, int col, ESTADO *e) {
	int num_jogadas = 0;
	char original = e->board[row][col];
	e->board[row][col] = original - 32;
	if (verifica_branca(row, col, e)) num_jogadas++;
	e->board[row][col] = '#';
	if (verifica_riscada(row, col, e)) num_jogadas++;
	e->board[row][col] = original;
	return num_jogadas;
}

/*
 * preenche pos com a posição do tabuleiro com o menor número de jogadas possíveis
 * (reduz drasticamente o espaço de procura)
*/
bool melhor_posicao(ESTADO *e, POSITION *pos) {
	bool found = false;
	for (int i = 0; i < e->num_rows; i++) {
		for (int j = 0; j < e->num_cols; j++) {
			if (is_minuscula(e->board[i][j])) {
				int num_jogadas = conta_jogadas(i, j, e);
				if (num_jogadas > 0) {
					pos->row = i;
					pos->col = j;
					found = true;
					if (num_jogadas == 1) return true;
				}
			}
		}
	}
	return found;
}

bool dfs(ESTADO *e) {
	if (!verifica_aux(e)) return false;
	POSITION pos;
	/*
	 * como já validamos o tabuleiro, caso não exista nenhuma jogada possível, então
	 * podemos concluir que chegamos ao fim do jogo.
	*/
	if (!melhor_posicao(e, &pos)) return true;
	int row = pos.row;
	int col = pos.col;
	char prev = e->board[row][col];
	push_move(row, col, prev, e);
	// tenta pintar de branca
	e->board[row][col] -= 32;
	if (verifica_branca(row, col, e) && dfs(e)) return true;
	// tenta riscar
	e->board[row][col] = '#';
	if (verifica_riscada(row, col, e) && dfs(e)) return true;
	// Backtrack
	pop_move(e, true);
	return false;
}
