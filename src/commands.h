#ifndef COMMANDS_H
#define COMMANDS_H

#include "state.h"

typedef bool (*COMANDO)(char cmd, char *arg, ESTADO *e);

bool sair(char cmd, char *arg, ESTADO *e);
bool ler(char cmd, char *arg, ESTADO *e);
bool gravar(char cmd, char *arg, ESTADO *e);
bool pintar(char cmd, char *arg, ESTADO *e);
bool riscar(char cmd, char *arg, ESTADO *e);

#endif
