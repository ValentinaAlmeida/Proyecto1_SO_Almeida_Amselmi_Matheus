#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/ejecutar.h"
#include "../include/estructura_comando.h"
#include "../include/redireccion.h"
#include "../include/pipes.h"
#include "../include/job_list.h"
#include "../include/colores.h"

extern Job* lista_jobs;
extern int todo_2plano;
extern volatile pid_t pid_primer_plano;

int ejecutar_cadena_pipes(Comando *comandos, int inicio, int fin) {
    
    int N = fin - inicio + 1;

    int fd[N-1][2];
    pid_t pids[N];

    for(int i = 0; i < fin - inicio; i++) {
        if(pipe(fd[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }

    for(int i = 0; i < fin - inicio + 1; i++) {
        pids[i] = fork();
        if(pids[i] == -1) {
            perror("fork");
            return -1;
        }
        if(pids[i] == 0) {
            if(i > 0) {
                dup2(fd[i-1][0], STDIN_FILENO);
            }
            if(i < fin - inicio) {
                dup2(fd[i][1], STDOUT_FILENO);
            }
            for(int j = 0; j < fin - inicio; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }

            if (i == 0 && comandos[inicio].r_entrada != NULL) {
                redirigir_entrada(comandos[inicio].r_entrada);
            }

            if (i == fin - inicio && comandos[inicio + i].r_salida != NULL) {
                redirigir_salida(comandos[fin].r_salida);
            }   

            ejecutar_en_hijo(&comandos[inicio + i]);
            exit(1);
        }
        if(i > 0) {
            close(fd[i-1][0]);
            close(fd[i-1][1]);
        }
    }


        close(fd[N-2][0]);
        close(fd[N-2][1]);
    
    int ultimo_status=0;
    pid_primer_plano = pids[0];
    for(int i = 0; i < N; i++) {
        if(todo_2plano == 1){
            Insertar_job(&lista_jobs, pids[i], comandos[inicio + i].instruccion,1);
        }else{
            int status;
            waitpid(pids[i], &status, WUNTRACED);
            if(i==N-1){
                ultimo_status = WEXITSTATUS(status);
            }
        }
    }
    pid_primer_plano = 0;
    return ultimo_status;
}

