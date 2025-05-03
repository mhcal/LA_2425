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
	for (int i = 0; i < 60; i++)
		comandos[i] = NULL;
	comandos['s' - 'A'] = sair;
	comandos['l' - 'A'] = ler;
	comandos['g' - 'A'] = gravar;
	comandos['b' - 'A'] = pintar;
	comandos['r' - 'A'] = riscar;
	comandos['d' - 'A'] = undo;
	comandos['v' - 'A'] = verificar;
	comandos['a' - 'A'] = ajuda;
	comandos['h' - 'A'] = help;
	comandos['A' - 'A'] = ajuda_repete;
	comandos['R' - 'A'] = resolver;
	ESTADO estado;
	estado.looping = true;
	estado.board = NULL;
	estado.board_loaded = false;
	estado.move_stack = NULL;
	estado.num_moves = 0;
	estado.num_ajuda = 0;
	estado.ajuda_dada = false;
	(void)help(NULL);
	putchar('\n');
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
		if (cmd < 0 || cmd >= 60 || comandos[(int)cmd] == NULL)
			fprintf(stderr, "Erro: código de comando inválido.\n\n");
		/*
		else if (comandos[(int)cmd]((num_args >= 2) ? arg : NULL, &estado)) {
			putchar('\n');
			print_board(&estado);
		}
		*/
		else {
			PARAMETROS p;
			p.arg = (num_args >= 2) ? arg : NULL;
			p.estado = &estado;
			p.suppress = false;
			(void)comandos[(int)cmd](&p);
			if (estado.looping && estado.board_loaded) {
				putchar('\n');
				print_board(&estado);
				putchar('\n');
			}
		}
	}
	free_board(&estado);
}
