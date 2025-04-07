#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "state.h"
#include "commands.h"
#define LINE_SIZE 1024

int main() {
	COMANDO comandos[] = {sair, ler, gravar, pintar, riscar, NULL};
	ESTADO estado;
	estado.looping = true;
	while (estado.looping) {
		printf("> ");
		char line[LINE_SIZE] = {0};
		if (fgets(line, LINE_SIZE, stdin) != NULL)
			assert(line[strlen(line) - 1] == '\n');
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
