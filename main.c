#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define LINE_SIZE 1024
#define ROW_SIZE 10
#define COL_SIZE 26

typedef struct estado {
	bool looping;
	char board[ROW_SIZE][COL_SIZE];
	int num_rows;
	int num_cols;
	bool board_loaded;
} ESTADO;

typedef bool (*COMANDO)(char cmd, char *arg, ESTADO *e);

void print_board(ESTADO *e) {
	if (e->board_loaded) {
		printf("  ");
		for (int i = 0; i < e->num_cols; i++)
			printf("%c ", 'a' + i);
		putchar('\n');
		for (int i = 0; i < e->num_rows; i++) {
			printf("%d ", i);
			for (int j = 0; j < e->num_cols; j++)
				printf("%c ", e->board[i][j]);
			putchar('\n');
		}
	}
	else {
		printf("O tabuleiro ainda não foi carregado.\n");
	}
}

bool parse_coord(char *coord, int *col, int *row, ESTADO *e) {
	if (strlen(coord) != 2) {
		fprintf(stderr, "Erro: formato inválido.\n");
		return false;
	}
	*col = coord[0] - 'a';
	*row = coord[1] - '0';
	if (*col < 0 || *col >= e->num_cols || *row < 0 || *row >= e->num_rows) {
		fprintf(stderr, "Erro: coordenadas estão fora dos limites do tabuleiro atual.\n");
		return false;
	}
	return true;
}

bool gravar(char cmd, char *arg, ESTADO *e) {
	if (cmd == 'g') {
		if (arg == NULL) {
			fprintf(stderr, "Erro: o comando gravar precisa de um argumento.\n");
			return false;
		}
		FILE *f = fopen(arg, "w");
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
		printf("Gravando em %s\n", arg);
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
		FILE *f = fopen(arg, "r");
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
		printf("Lendo de %s\n", arg);
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

int main() {
	COMANDO comandos[] = {sair, ler, gravar, pintar, riscar, NULL};
	ESTADO estado;
	estado.looping = true;
	while (estado.looping) {
		printf("> ");
		char line[LINE_SIZE] = {0};
		if (fgets(line, LINE_SIZE, stdin) != NULL) {
			assert(line[strlen(line) - 1] == '\n');
		}
		else
			estado.looping = false;
		char cmd;
		char arg[LINE_SIZE] = {0};
		line[strcspn(line, "\n")] = '\0';
		int num_args = sscanf(line, "%c %s", &cmd, arg);
		
		bool ret = false;
		for (int i = 0; !ret && comandos[i] != NULL; i++)
			ret = comandos[i](cmd, (num_args >= 2) ? arg: NULL, &estado);
		putchar('\n');
	}
	return 0;
}
