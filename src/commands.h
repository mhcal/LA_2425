#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>
#include "types.h"

bool help(PARAMETROS *p);
bool sair(PARAMETROS *p);
bool ler(PARAMETROS *p);
bool gravar(PARAMETROS *p);
bool pintar(PARAMETROS *p);
bool riscar(PARAMETROS *p);
bool undo(PARAMETROS *p);
bool verificar(PARAMETROS *p);
bool ajuda(PARAMETROS *p);
bool ajuda_repete(PARAMETROS *p);
bool resolver(PARAMETROS *p);

#endif
