//--Modulo de busqueda de binarios en el PATH del sistema--
// Se encarga de:
// -Buscar el binario de un comando en las rutas del PATH
// -Verifica permisos de ejecucion con access()
// -Retorna la ruta completa del binario o NULL si no existe 

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "../include/path.h"
#include "../include/colores.h"

char *buscar_en_path(char *instruccion){
    char *path_env = getenv("PATH"); // obtenemos el PATH del sistema
    if(path_env==NULL){ // Si el PATH es NULL imprime error
        fprintf(stderr, ROJO LETRA_NEGRITA"Error: PATH no está definido\n"LETRA_NORMAL);
        return NULL;
    }

    char *path_copia=strdup(path_env); //Copia de PATH para no modificar el PATH del sistema
    if(path_copia==NULL){ // Si la copia es NULL imprime error
        perror(ROJO LETRA_NEGRITA"Error al duplicar la cadena PATH"LETRA_NORMAL);
        return NULL;
    }

    char *path_tokenizado = strtok(path_copia, ":"); //Tokenizamos con separador : 
    while(path_tokenizado != NULL){
        char ruta_completa[1024];
        snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", path_tokenizado, instruccion); //construye la ruta completa directorio/comando


        if(access(ruta_completa, X_OK)==0){ //Verifica si el archivo existe y es ejecutable 
            free(path_copia);
            return strdup(ruta_completa); //Retorna una copia de la ruta encontrada
        }
        
        path_tokenizado = strtok(NULL, ":"); // Avanzamos al siguiente token
    }

    free(path_copia); //Liberar la copia del PATH
    return NULL;
}