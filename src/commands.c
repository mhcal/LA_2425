#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "state.h"

bool gravar(char cmd, char *arg, ESTADO *e) {
	if (cmd == 'g') {
		if (arg == NULL) {
			fprintf(stderr, "Erro: o comando gravar precisa de um argumento.\n");
			return false;
		}
		char path[LINE_SIZE];
		snprintf(path, sizeof(path), "boards/%s", arg);
		FILE *f = fopen(path, "w");
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
	else
		return false;
}

bool sair(char cmd, char *arg, ESTADO *e) {
	if (arg != NULL)
		return false;
	if (cmd == 's') {
		e->looping = false;
		return true;
	}
	else
		return false;
}

bool ler(char cmd, char *arg, ESTADO *e) {
	if (cmd == 'l') {
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
		if (fscanf(f, "%d %d\n", &e->num_rows, &e->num_cols) != 2) {
			fprintf(stderr, "Erro: formato do arquivo inválido.\n");
			fclose(f);
			return false;
		}
		char line[COL_SIZE + 2];
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
	else
		return false;
}

bool pintar(char cmd, char *arg, ESTADO *e) {
	if (cmd == 'b') {
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
		if (e->board[row][col] <= 90)
			printf("Letra na posição %s já está maiúscula.\n", arg);
		else {
			e->board[row][col] = e->board[row][col] - 32;
			printf("Letra na posição %s foi colocada em maiúscula.\n", arg);
		}
		print_board(e);
		return true;
	}
	else
		return false;
}

bool riscar(char cmd, char *arg, ESTADO *e) {
	if (cmd == 'r') {
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
		if (e->board[row][col] == '#')
			printf("A posição %s já foi riscada.\n", arg);
		else {
			e->board[row][col] = '#';
			printf("Letra na posição %s foi riscada.\n", arg);
		}
		print_board(e);
		return true;
	}
	else
		return false;
}
