//#{}[] < >  || &&

 #ifndef HISTORIAL_COMANDOS_H
 #define HISTORIAL_COMANDOS_H

 #include <stddef.h>

 #define CANT_RECORDAR 128
 #define TAM_LINEAS 1024

 extern char arreglo_memoria[CANT_RECORDAR][TAM_LINEAS]; //para tener la carga que solicita el enunciado y facilitarnos la parte de las flechas
 extern int total_historial; //así se sabe cual es el actual y moverse con los anteriores y saber a partir de donde copio

void carga_inicial(const char* ruta);

void editar_historial(char* linea_original, const char* ruta);

 #endif