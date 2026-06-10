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
#include "../include/job_list.h"
#include "../include/builtins.h"
#include "../include/colores.h"


extern Job* lista_jobs;
extern int todo_2plano; 

int es_builtin(char *instruccion){
    return strcmp(instruccion, "cd") == 0 ||
            strcmp(instruccion, "exit") == 0 ||
            strcmp(instruccion, "jobs") == 0 || 
            strcmp(instruccion, "fg") == 0;
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
    free(args);

    if(todo_2plano == 1){
        Insertar_job(&lista_jobs, pid, cmd->instruccion,1);
        return 0;
    }else{
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }

}

void ejecutar_comando(Comando *comandos, int numero){
    int i=0;
    while(i<numero){
        if(es_builtin(comandos[i].instruccion)) {
            if(strcmp(comandos[i].instruccion, "cd") == 0){
                builtin_cd(&comandos[i]);
            } else if(strcmp(comandos[i].instruccion, "exit") == 0){
                builtin_exit(lista_jobs);
            } else if(strcmp(comandos[i].instruccion, "jobs") == 0){
                builtin_jobs(lista_jobs);
            } else if(strcmp(comandos[i].instruccion, "fg") == 0){
                int id = (comandos[i].cant_argumentos > 0) ? atoi(comandos[i].argumentos[0]) : 1;
                builtin_fg(&lista_jobs, id);
            }
            i++;
        }else{
            if(comandos[i].hay_tuberia==1){
                int fin = i;
                while(comandos[fin].hay_tuberia == 1){
                    fin++;
                }
                int codigo_salida=ejecutar_cadena_pipes(comandos, i, fin);
                if(comandos[fin].modo==1){
                    if(codigo_salida==0){
                        break;
                    }
                }else if(comandos[fin].modo==2){
                    if(codigo_salida!=0){
                        break;
                    }
                }
                
                i=fin + 1;
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