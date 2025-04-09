#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "utils.h"

bool gravar(char *arg, ESTADO *e) {
	if (arg == NULL) {
		fprintf(stderr, "Erro: o comando gravar precisa de um argumento.\n");
		return false;
	}
	char path[LINE_SIZE];
	snprintf(path, sizeof(path), "boards/%s", arg); FILE *f = fopen(path, "w");
	if (!f) {
		fprintf(stderr, "Erro: não foi possível abrir o arquivo para gravação.\n");
		return false;
	}
	fprintf(f, "%d %d\n", e->num_rows, e->num_cols);
	for (int i = 0; i < e->num_rows; i++) {
		for (int j = 0; j < e->num_cols; j++)
			fprintf(f, "%c", e->board[i][j]);
		fprintf(f, "\n");
	}
	printf("Gravando em %s\n", path);
	fclose(f);
	return true;
}

bool sair(char *arg, ESTADO *e) {
	if (arg != NULL) {
		fprintf(stderr, "Erro: comando sair não recebe argumentos.\n");
		return false;
	}
	e->looping = false;
	return true;
}

bool ler(char *arg, ESTADO *e) {
	if (arg == NULL) {
		fprintf(stderr, "Erro: o comando ler precisa de um argumento.\n");
		return false;
	}
	char path[LINE_SIZE];
	snprintf(path, sizeof(path), "boards/%s", arg);
	FILE *f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "Erro: não foi possível abrir o arquivo para leitura.\n");
		return false;
	}
	int rows, cols;
	if (fscanf(f, "%d %d\n", &rows, &cols) != 2) {
		fprintf(stderr, "Erro: formato do arquivo inválido.\n");
		fclose(f);
		return false;
	}
	if (rows >= ROW_SIZE || cols >= COL_SIZE) {
		fprintf(stderr, "Erro: tabuleiro maior do que dimensões máximas.\n");
		fclose(f);
		return false;
	}
	free_move_stack(e);
	if (!allocate_board(rows, cols, e)) {
		fclose(f);
		return false;
	}
	char line[cols + 2];
	for (int i = 0; i < e->num_rows; i++) {
		if (!fgets(line, sizeof(line), f)) {
			fprintf(stderr, "Erro ao ler tabuleiro.\n");
			fclose(f);
			return false;
		}
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = 0;
		for (int j = 0; j < e->num_cols; j++)
			e->board[i][j] = line[j];
	}
	fclose(f);
	e->board_loaded = true;
	printf("Lendo de %s\n", path);
	print_board(e);
	return true;
}

bool pintar(char *arg, ESTADO *e) {
	if (!e->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (arg == NULL) {
		fprintf(stderr, "Erro: o comando pintar precisa de um argumento.\n");
		return false;
	}
	int col, row; 
	if (!parse_coord(arg, &col, &row, e))
		return false;
	char current = e->board[row][col];
	if (current == '#')
		printf("Letra na posição %s está riscada.\n", arg);
	else if (current <= 90)
		printf("Letra na posição %s já está maiúscula.\n", arg);
	else {
		push_move(row, col, current, e);
		e->board[row][col] = e->board[row][col] - 32;
		printf("Letra na posição %s foi colocada em maiúscula.\n", arg);
	}
	print_board(e);
	return true;
}

bool riscar(char *arg, ESTADO *e) {
	if (!e->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (arg == NULL) {
		fprintf(stderr, "Erro: o comando riscar precisa de um argumento.\n");
		return false;
	}
	int col, row;
	if (!parse_coord(arg, &col, &row, e))
		return false;
	char current = e->board[row][col];
	if (current == '#')
		printf("A posição %s já foi riscada.\n", arg);
	else {
		push_move(row, col, current, e);
		e->board[row][col] = '#';
		printf("Letra na posição %s foi riscada.\n", arg);
	}
	print_board(e);
	return true;
}

bool undo(char *arg, ESTADO *e) {
	if (!e->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro foi carregado.\n");
		return false;
	}
	if (e->move_stack == NULL || e->stack_size == 0) {
		fprintf(stderr, "Erro: não há jogadas para desfazer.\n");
		return false;
	}
	int passos = 1;
	if (arg != NULL) {
		passos = atoi(arg);
		if (passos <= 0) {
			fprintf(stderr, "Erro: número de jogadas deve ser um inteiro positivo.\n");
			return false;
		}
	}
	if (passos > e->stack_size) {
		fprintf(stderr, "Erro: só existem %d jogadas no histórico.\n", e->stack_size);
		return false;
	}
	for (int i = 0; i < passos; i++)
		pop_move(e);
	printf("Desfazendo %d jogadas.\n", passos);
	print_board(e);
	return true;
}
