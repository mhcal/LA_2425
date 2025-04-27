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
	comandos['v' - 'A'] = verificar;
	ESTADO estado;
	estado.looping = true;
	estado.board = NULL;
	estado.board_loaded = false;
	estado.move_stack = NULL;
	estado.num_moves = 0;
	estado.help_trigger = false;
	printf("-------------------------------- COMANDOS --------------------------------\n"
			" * g <filename>   - grava o estado atual do jogo num ficheiro\n"
			" * l <filename>   - lê o estado do jogo de um ficheiro\n"
			" * b <coordenada> - coloca a letra na coordenada em maiúscula\n"
			" * r <coordenada> - risca a letra na coordenada correspondente\n"
			" * v              - aponta as restrições violadas no estado atual\n"
			" * a              - muda as casas que podem ser inferidas no estado atual\n"
			" * A              - invoca o comando a enquanto o jogo sofrer alterações\n"
			" * R              - resolve o jogo\n"
			" * d <num>        - desfaz as últimas <num> jogadas (default = 1)\n"
			" * s              - sair do jogo\n"
			"--------------------------------------------------------------------------\n\n");
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
		else if (comandos[(int)cmd]((num_args >= 2) ? arg : NULL, &estado)) {
			putchar('\n');
			print_board(&estado);
		}
		putchar('\n');
	}
	free_board(&estado);
}
