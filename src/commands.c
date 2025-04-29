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
	if (is_riscada(current))
		printf("Letra na posição %s está riscada.\n", arg);
	else if (is_branca(current))
		printf("Letra na posição %s já está maiúscula.\n", arg);
	else {
		push_move(row, col, current, e);
		e->board[row][col] = e->board[row][col] - 32;
		printf("Letra na posição %s foi colocada em maiúscula.\n", arg);
	}
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
	if (is_riscada(current))
		printf("A posição %s já foi riscada.\n", arg);
	else {
		push_move(row, col, current, e);
		e->board[row][col] = '#';
		printf("Letra na posição %s foi riscada.\n", arg);
	}
	return true;
}

bool undo(char *arg, ESTADO *e) {
	if (!e->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro foi carregado.\n");
		return false;
	}
	if (e->move_stack == NULL || e->num_moves == 0) {
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
	if (passos > e->num_moves) {
		fprintf(stderr, "Erro: só existem %d jogadas no histórico.\n", e->num_moves);
		return false;
	}
	for (int i = 0; i < passos; i++)
		pop_move(e);
	printf("Desfazendo %d jogadas.\n", passos);
	return true;
}

bool verificar(char *arg, ESTADO *e) {
	if (!e->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro foi carregado.\n");
		return false;
	}
	if (arg != NULL) {
		fprintf(stderr, "Erro: comando verificar não recebe argumentos.\n");
		return false;
	}
	bool violation_found = false;
	for (int i = 0; i < e->num_rows; i++) {
		for (int j = 0; j < e->num_cols; j++) {
			if (is_branca(e->board[i][j])) {
				if (!(verifica_branca(i, j, e))) {
					printf("Violação encontrada: letra '%c' na posição %c%d tem réplica pintada a branco na mesma linha ou coluna.\n", e->board[i][j], 'a' + j, i + 1);
					violation_found = true;
				}
			}
		}
	}
	for (int i = 0; i < e->num_rows; i++) {
		for (int j = 0; j < e->num_cols; j++) {
			if (is_riscada(e->board[i][j])) {
				if (!(verifica_riscada(i, j, e))) {
					printf("Violação encontrada: posição %c%d está riscada e tem vizinhos ortogonais riscados.\n", 'a' + j, i + 1);
					violation_found = true;
				}
			}
		}
	}
	if (!(verifica_caminho(e))) {
		printf("Violação encontrada: não existe um caminho ortogonal entre todas as casas pintadas de branco.\n");
		violation_found = true;
	}
	if (!violation_found)
		printf("Nenhuma violação de regras encontrada no estado atual.\n");
	return !violation_found;
}

/*
bool ajuda(char *arg, ESTADO *e) {
	if (!e->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro foi carregado.\n");
		return false;
	}
	if (arg != NULL) {
		fprintf(stderr, "Erro: comando ajuda não recebe argumentos.\n");
		return false;
	}
	if (!verificar(arg, e)) {
		fprintf(stderr, "Erro: o tabuleiro atual é inválido. Por favor corrija antes de invocar comando ajuda");
		return false;
	}
	for (int r = 0; r < e->num_rows; r++) {
		for (int c = 0; c < e->num_cols; c++) {
			if (is_riscada(e->board[r][c]))
				pinta_vizinhos(r, c, e);
			if (is_branca(e->board[r][c]))
				risca_iguais(r, c, e);
		}
	}
	return true;
}
*/
