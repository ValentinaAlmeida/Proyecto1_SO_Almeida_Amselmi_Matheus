//# {} [] > < || &&
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
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
            
            printf("\n");
            fflush(stdout);
        }
    } 
    else if (senal == SIGTSTP){ // Ctrl+Z
        if (pid_primer_plano > 0) {
            kill(pid_primer_plano, SIGTSTP); // Pausa el proceso hijo
        } else{
            printf("\n");
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
        perror(ROJO LETRA_NEGRITA"Error al configurar SIGINT"LETRA_NORMAL);
    }
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror(ROJO LETRA_NEGRITA"Error al configurar SIGTSTP"LETRA_NORMAL);
    }
}
//# {} [] > < || &&