#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/ejecutar.h"
#include "../include/estructura_comando.h"
#include "../include/path.h"
#include "../include/redireccion.h"
#include "../include/pipes.h"
#include "../include/colores.h"


int es_builtin(char *instruccion){
    return strcmp(instruccion, "cd") == 0 ||
            strcmp(instruccion, "exit") == 0 ||
            strcmp(instruccion, "jobs") == 0;
}

void ejecutar_en_hijo(Comando *cmd){
    char *ruta = buscar_en_path(cmd->instruccion);
    if(ruta == NULL){
        exit(1);
    }
    char **args = malloc(sizeof(char*) * (cmd->cant_argumentos + 2));
    args[0] = cmd->instruccion;
    for(int i = 0; i < cmd->cant_argumentos; i++){
        args[i+1] = cmd->argumentos[i];
    }
    args[cmd->cant_argumentos + 1] = NULL;
    execv(ruta, args);
    perror("Error al ejecutar el comando");
    exit(1);
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
            //redirecciones?
            if(cmd->r_entrada != NULL && cmd->r_salida != NULL){
                if(cmd->orden == 1){
                    if(redirigir_entrada(cmd->r_entrada) == -1 || redirigir_salida(cmd->r_salida) == -1){
                        exit(1);
                    }
                }else if(cmd->orden == 2){
                    if(redirigir_salida(cmd->r_salida) == -1 || redirigir_entrada(cmd->r_entrada) == -1){
                        exit(1);
                    }
                }   
            
            }else if(cmd->r_entrada != NULL){
                if(redirigir_entrada(cmd->r_entrada) == -1){
                    exit(1);
                }
            }else if(cmd->r_salida != NULL){
                if(redirigir_salida(cmd->r_salida) == -1){
                    exit(1);
                }
            }
        
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

void ejecutar_comando(Comando *comandos, int numero){
    int i=0;
    while(i<numero){
        if(es_builtin(comandos[i].instruccion)) {
            printf("Lo sentimos, el comando %s no está implementado por que es un builtin\n", comandos[i].instruccion);
            i++;
        }else{
            if(comandos[i].hay_tuberia==1){
                int codigo_salida=ejecutar_pipe(&comandos[i], &comandos[i+1]);
                if(comandos[i+1].modo==1){
                    if(codigo_salida==0){
                        break;
                    }
                }else if(comandos[i+1].modo==2){
                    if(codigo_salida!=0){
                        break;
                    }
                }
                
                i=i+2;
            }else{
                
                int codigo_salida=ejecutar_uno(&comandos[i]);
                if(comandos[i].modo==1){
                    if(codigo_salida==0){
                        break;
                    }
                }else if(comandos[i].modo==2){
                    if(codigo_salida!=0){
                        break;
                    }
                }
            
                i++;
            }
        }
    }
}