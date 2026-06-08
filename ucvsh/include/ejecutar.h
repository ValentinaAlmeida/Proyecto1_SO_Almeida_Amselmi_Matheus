#ifndef EJECUTAR_H
#define EJECUTAR_H
#include "../include/estructura_comando.h"

void ejecutar_comando(Comando *comados, int numero);
int ejecutar_uno(Comando *cmd);
void ejecutar_en_hijo(Comando *cmd);

#endif