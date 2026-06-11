#ifndef MODOS_SHELL_H
#define MODOS_SHELL_H

#include <termios.h>
#include <stdio.h>
//todas las funciones del .c que se van a utilizar las pongo aquí
void habilitar_no_canonico(struct termios* modo);
void habilitar_canonico(struct termios* modo);
int leer_actual(char* leido, size_t tamano);

#endif