#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/ejecutar.h"
#include "../include/estructura_comando.h"
#include "../include/path.h"


int es_builtin(char *instruccion){
    return strcmp(instruccion, "cd") == 0 ||
            strcmp(instruccion, "exit") == 0 ||
            strcmp(instruccion, "jobs") == 0;
}

int ejecutar_uno(Comando *cmd){
    char *ruta=buscar_en_path(cmd->instruccion);
    
    if(ruta==NULL){
        fprintf(stderr, "Error: comando no encontrado: %s\n", cmd->instruccion);
        return -1;
    }

    char **args=malloc(sizeof(char*)*(cmd->cant_argumentos+2));
        args[0]=cmd->instruccion;
        for(int i=0; i<cmd->cant_argumentos; i++){
            args[i+1]=cmd->argumentos[i];
        }
        args[cmd->cant_argumentos+1]=NULL;

    pid_t pid=fork();

    if(pid==-1){
        perror("Error al crear el proceso hijo");
        return -1;
    }
    if(pid==0){
        // Proceso hijo
        execv(ruta, args);
        perror("Error al ejecutar el comando");
        
        exit(1);
    }
    free(ruta);
    int status;
    waitpid(pid, &status, 0);
    free(args);
    return WEXITSTATUS(status);
}

void ejecutar_comando(Comando *comados, int numero){
    int i=0;
    while(i<numero){
        if(es_builtin(comados[i].instruccion)) {
            printf("Lo sentimos, el comando %s no está implementado por que es un builtin\n", comados[i].instruccion);
            i++;
        }else{
            int codigo_salida=ejecutar_uno(&comados[i]);
            if(comados[i].modo==1){
                if(codigo_salida==0){
                    break;
                }
            }else if(comados[i].modo==2){
                if(codigo_salida!=0){
                    break;
                }
            }
        
        i++;
        }
    
    }
}