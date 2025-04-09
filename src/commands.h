#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>
#include "types.h"

bool sair(char *arg, ESTADO *e);
bool ler(char *arg, ESTADO *e);
bool gravar(char *arg, ESTADO *e);
bool pintar(char *arg, ESTADO *e);
bool riscar(char *arg, ESTADO *e);
bool undo(char *arg, ESTADO *e);

#endif
