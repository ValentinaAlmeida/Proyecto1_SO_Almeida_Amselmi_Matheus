#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/ejecutar.h"
#include "../include/estructura_comando.h"
#include "../include/redireccion.h"
#include "../include/colores.h"

int ejecutar_pipe(Comando *izquierda, Comando *derecha) {
    int fd[2];
    pid_t pid1, pid2;

    if (pipe(fd) == -1) {
        perror("pipe");
        return -1;
    }

    pid1=fork();
    if(pid1==0){
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        ejecutar_en_hijo(izquierda);
        exit(1);
    }

    pid2=fork();
    if(pid2==0){
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
            if(derecha->r_entrada != NULL && derecha->r_salida != NULL){
                if(derecha->orden == 1){
                    if(redirigir_entrada(derecha->r_entrada) == -1 || redirigir_salida(derecha->r_salida) == -1){
                        exit(1);
                    }
                }else if(derecha->orden == 2){
                    if(redirigir_salida(derecha->r_salida) == -1 || redirigir_entrada(derecha->r_entrada) == -1){
                        exit(1);
                    }
                }   
            
            }else if(derecha->r_entrada != NULL){
                if(redirigir_entrada(derecha->r_entrada) == -1){
                    exit(1);
                }
            }else if(derecha->r_salida != NULL){
    
                if(redirigir_salida(derecha->r_salida) == -1){
                    exit(1);
                }
            }
        ejecutar_en_hijo(derecha);
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    return WEXITSTATUS(status2);
    
}