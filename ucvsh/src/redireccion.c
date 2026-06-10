#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "../include/redireccion.h"
#include "../include/colores.h"

int redirigir_salida(char *archivo){
    int fd=open(archivo, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd==-1){
        perror(ROJO"Error al abrir el archivo para redirección de salida"LETRA_NEGRITA);
        return -1;
    }
    if(dup2(fd, STDOUT_FILENO)==-1) {
        perror(ROJO"Error al redirigir la salida estándar"LETRA_NEGRITA);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int redirigir_entrada(char *archivo){
    int fd=open(archivo, O_RDONLY);
    if(fd==-1){
        perror(ROJO"Error al abrir el archivo para redirección de entrada"LETRA_NEGRITA);
        return -1;
    }
    if(dup2(fd, STDIN_FILENO)==-1) {
        perror(ROJO"Error al redirigir la entrada estándar"LETRA_NEGRITA);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}