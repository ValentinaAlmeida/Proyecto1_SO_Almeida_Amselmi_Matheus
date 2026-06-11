//--Modulo de pipes--
// Se encarga de:
// -Crear y gestionar tuberias entre multiples procesos
// -Conectar la salida de un proceso con la entrada del siguiente
// -Manejar redirecciones en cadenas de pipes
// Gestionar procesos en background dentro de pipes

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
    
    int N = fin - inicio + 1; //Calcular cantidad de comandos

    int fd[N-1][2]; // Crear descriptores de archivo
    pid_t pids[N]; // Crear PIDs

    for(int i = 0; i < fin - inicio; i++) { //Crear una tuberia por cada comando
        if(pipe(fd[i]) == -1) { //Si falla la creacion de latuberia imprime un error y retorna -1 para usar pipes con opeadores
            perror(ROJO LETRA_NEGRITA "pipe" LETRA_NORMAL);
            return -1;
        }
    }

    for(int i = 0; i < fin - inicio + 1; i++) {
        pids[i] = fork(); // Crear un hijo por cada comando 
        if(pids[i] == -1) { // Si la creacion del proceso hijo falla imprimimos un error
            perror(ROJO LETRA_NEGRITA "fork" LETRA_NORMAL);
            return -1; /// Retorna -1 para indicar que hubo un error, util para operadores
        }
        if(pids[i] == 0) {
            // Proceso hijo
            if(i > 0) { //Si no es el primer comando, conectar stdin al pipe anterior
                dup2(fd[i-1][0], STDIN_FILENO);
            }
            if(i < fin - inicio) { // Si no es el ultimo comando, conectar stdout al pipe siguiente
                dup2(fd[i][1], STDOUT_FILENO);
            }
            for(int j = 0; j < fin - inicio; j++) { // Cerrar todos los extremos del pipe para evitar bloqueos
                close(fd[j][0]);
                close(fd[j][1]);
            }

            if (i == 0 && comandos[inicio].r_entrada != NULL) { // hay una redirecion de entrada para el primer comano?
                redirigir_entrada(comandos[inicio].r_entrada);
            }

            if (i == fin - inicio && comandos[inicio + i].r_salida != NULL) { // hay una redireccion de salida para el ultimo comando?
                redirigir_salida(comandos[fin].r_salida);
            }   

            ejecutar_en_hijo(&comandos[inicio + i]); // Ejecutar el comando, ver en ejecutar.c
            exit(1);
        }
        if(i > 0) { // Cerrar pipe anterior para evitar bloqueos
            close(fd[i-1][0]);
            close(fd[i-1][1]);
        }
    }


        close(fd[N-2][0]);
        close(fd[N-2][1]);
    
    int ultimo_status=0;
    pid_primer_plano = pids[0]; // Actualiza el PID a PID del proceso en primer plano para poder manejar señales
    for(int i = 0; i < N; i++) {
        if(todo_2plano == 1){ //Si el comando se ejecuta en segundo plano, se agrega a la lista de jobs 
            Insertar_job(&lista_jobs, pids[i], comandos[inicio + i].instruccion,1);
        }else{
            int status;
            waitpid(pids[i], &status, WUNTRACED); //Esperar a que termine p se detenga
            if(i==N-1){ //Capturar el codigo de de salida del ultimo camando
                ultimo_status = WEXITSTATUS(status);
            }
        }
    }
    pid_primer_plano = 0; // Reinicia el PID del proceso en primer plano 
    return ultimo_status; // Retorna el código de salida del ultimo proceso hijo, util para operadores
}

