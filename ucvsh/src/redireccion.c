//--Modulo de redirecciones--
// Se encarga de: 
// -Redirigir la salida estandar hacia un archivo (>)
// -Redirigir la entrada estandar desde un archivo (<)

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "../include/redireccion.h"
#include "../include/colores.h"

//
int redirigir_salida(char *archivo){
    int fd=open(archivo, O_WRONLY | O_CREAT | O_TRUNC, 0644); //Abre el descriptor de archivo en solo escritura, si no existe el archivo lo crea y si existe lo vacia
    if(fd==-1){ //Si abrir el archivo falla imprimimos un eror
        perror(ROJO LETRA_NEGRITA"Error al abrir el archivo para redirección de salida"LETRA_NORMAL);
        return -1;
    }
    //Redirigir stdout al archivo abierto
    if(dup2(fd, STDOUT_FILENO)==-1) { //Si la redireccion falla imprimimos un error
        perror(ROJO LETRA_NEGRITA"Error al redirigir la salida estándar"LETRA_NORMAL);
        close(fd);
        return -1;
    }
    close(fd); // cerramos el descriptor de archivo
    return 0;
}

//
int redirigir_entrada(char *archivo){
    int fd=open(archivo, O_RDONLY); //Abre el descriptor de archivos en solo lectura
    if(fd==-1){ //Si abrir el archivo falla imprimimos un eror
        perror(ROJO LETRA_NEGRITA"Error al abrir el archivo para redirección de entrada"LETRA_NORMAL);
        return -1;
    }

    //Redirigir stdin al archivo abierto
    if(dup2(fd, STDIN_FILENO)==-1) { //Si la redireccion falla imprimimos un error
        perror(ROJO LETRA_NEGRITA"Error al redirigir la entrada estándar"LETRA_NORMAL);
        close(fd);
        return -1;
    }
    close(fd); // cerramos el descriptor de archivo
    return 0;
}