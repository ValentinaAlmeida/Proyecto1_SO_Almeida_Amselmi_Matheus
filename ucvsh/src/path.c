#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "../include/path.h"
#include "../include/colores.h"

char *buscar_en_path(char *instruccion){
    char *path_env = getenv("PATH");
    if(path_env==NULL){
        fprintf(stderr, "Error: PATH no está definido\n");
        return NULL;
    }

    char *path_copia=strdup(path_env);
    if(path_copia==NULL){
        perror("Error al duplicar la cadena PATH");
        return NULL;
    }

    char *path_tokenizado = strtok(path_copia, ":");
    while(path_tokenizado != NULL){
        char ruta_completa[1024];
        snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", path_tokenizado, instruccion);


        if(access(ruta_completa, X_OK)==0){
            free(path_copia);
            return strdup(ruta_completa);
        }
        
        path_tokenizado = strtok(NULL, ":");
    }

    free(path_copia);
    return NULL;
}