#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "../include/signals.h"
#include "../include/job_list.h"
#include "../include/colores.h" 

// VARIABLES GLOBALES DE SEÑALES
volatile pid_t pid_primer_plano = 0;
char comando_primer_plano[1024] = "";

struct Job* lista_jobs; 

void capturar_senal(int senal){
    if(senal == SIGINT){ // Ctrl+C
        if (pid_primer_plano > 0) {
            kill(pid_primer_plano, SIGINT);
        } else {
            printf("\nucvsh> ");
            fflush(stdout);
        }
    } 
    else if (senal == SIGTSTP){ // Ctrl+Z
        if (pid_primer_plano > 0) {
            kill(pid_primer_plano, SIGSTOP); // Pausa el proceso hijo
            
            // Buscamos el proceso en la lista para cambiar su estado al número 2
            Job* trabajo = Buscar_job_porpid(pid_primer_plano, lista_jobs);
            if (trabajo != NULL) {
                trabajo->estado = 2; 
            } else {
                Insertar_job(&lista_jobs, pid_primer_plano, comando_primer_plano, 2);
            }

            printf("\n[Detenido] Usa 'jobs' o 'fg' para gestionarlo\nucvsh> ");
            fflush(stdout);
        } else {
            printf("\nucvsh> ");
            fflush(stdout);
        }
    }
}

void Manejadores_senales(){
    struct sigaction sa;
    sa.sa_handler = capturar_senal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; 

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error al configurar SIGINT");
    }
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("Error al configurar SIGTSTP");
    }
}
