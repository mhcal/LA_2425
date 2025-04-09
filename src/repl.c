#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "types.h"
#include "utils.h"
#include "commands.h"

void repl() {
	COMANDO comandos[60];
	comandos['s' - 'A'] = sair;
	comandos['l' - 'A'] = ler;
	comandos['g' - 'A'] = gravar;
	comandos['b' - 'A'] = pintar;
	comandos['r' - 'A'] = riscar;
	comandos['d' - 'A'] = undo;
	ESTADO estado;
	estado.looping = true;
	estado.board = NULL;
	estado.board_loaded = false;
	estado.move_stack = NULL;
	estado.stack_size = 0;
	while (estado.looping) {
		printf("> ");
		char line[LINE_SIZE] = {0};
		if (fgets(line, LINE_SIZE, stdin) != NULL)
			assert(line[strlen(line) - 1] == '\n');
		else
			estado.looping = false;
		line[strcspn(line, "\n")] = '\0';
		char cmd;
		char arg[LINE_SIZE] = {0};
		int num_args = sscanf(line, "%c %s", &cmd, arg);
		cmd -= 'A';
		if (cmd < 0 || cmd >= 60)
			fprintf(stderr, "Erro: código de comando inválido.\n");
		else {
			bool ret = comandos[(int)cmd]((num_args >= 2) ? arg : NULL, &estado);
			if (!ret)
				fprintf(stderr, "--- ERROR ---\n");
		}
		putchar('\n');
	}
	free_board(&estado);
}
