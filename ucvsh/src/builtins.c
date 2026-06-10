#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#include "../include/colores.h"
extern volatile pid_t pid_primer_plano;
int builtin_jobs(Job* cabeza){
    int estado_kernel;
    Job *actual = cabeza;
    while(actual != NULL){
        pid_t resultado = waitpid(actual->pgid, &estado_kernel, WNOHANG | WUNTRACED);
        if(resultado > 0){
            if(WIFEXITED(estado_kernel) || WIFSIGNALED(estado_kernel)){
                actual->estado = 3;
            } else if(WIFSTOPPED(estado_kernel)){
                actual->estado = 2;
            }
        }
        actual = actual->next;
    }
    MostrarJobs(cabeza);
    return 0;
}

int builtin_fg(Job** cabeza, int id_job){
    Job *actual = Buscar_job_porid(id_job, *cabeza);
    if(actual == NULL){
        printf(ROJO "Error: " LETRA_NEGRITA);
        printf("Trabajo no encontrado, intente otra vez \n");
        return 1;
    }
    printf(VERDE "trayendo a primer plano :%s" LETRA_NORMAL, actual->comando);
    pid_primer_plano = actual->pgid;
    kill(actual->pgid, SIGCONT);
    actual->estado = 1;
    int estado_kernel;
    int retorno_status = 0;
    pid_t resultado = waitpid(actual->pgid, &estado_kernel, WUNTRACED);
    if(resultado > 0){
        if(WIFEXITED(estado_kernel)){
            retorno_status = WEXITSTATUS(estado_kernel);
            Eliminar_Job(cabeza, actual->idInterno);
        } else if(WIFSIGNALED(estado_kernel)){
            retorno_status = 1;
            Eliminar_Job(cabeza, actual->idInterno);
        } else if(WIFSTOPPED(estado_kernel)){
            actual->estado = 2;
            printf(AMARILLO "\n[%d]+  Stopped  %s\n" LETRA_NORMAL, actual->idInterno, actual->comando);
            retorno_status = 0;
        }
    }
    pid_primer_plano = 0;
    return retorno_status;
}

int builtin_exit(Job* cabeza){
    Job *actual = cabeza;
    printf(ROSA_CHICLE "Cerrando ucvsh... ¡Hasta luego!\n");
    while(actual != NULL){
        kill(actual->pgid, SIGKILL);
        actual = actual->next;
    }
    LimpiarJobs(cabeza);
    exit(0);
}

int builtin_bg(Job **lista_jobs, Comando *comando){
    pid_t pid = fork();
    if(pid < 0){
        perror(ROJO "Hubo un fallo para el proceso en segundo plano" LETRA_NEGRITA);
        return 1;
    } else if(pid == 0){
        if(execvp(comando->instruccion, comando->argumentos) == -1){
            perror(ROJO "Error: problemas al ejecutar el comando en segundo plano" LETRA_NEGRITA);
            exit(1);
        }
    } else {
        Insertar_job(lista_jobs, pid, comando->instruccion, 1);
        Job *trabajo = Buscar_job_porpid(pid, *lista_jobs);
        int id = trabajo->idInterno;
        printf(AMARILLO "[%d] %d\n" LETRA_NORMAL, id, pid);
    }
    return 0;
}


int builtin_cd(Comando* comando_actual) {
    char* destino_final = NULL;
    char ruta_actual_antes[1024];

    // Se respalda el directorio actual antes de realizar el cambio de ruta
    if (getcwd(ruta_actual_antes, sizeof(ruta_actual_antes)) == NULL) {
        perror(ROJO"ucvsh: cd: getcwd"LETRA_NEGRITA);
        return 1;
    }

    // El usuario escribe cd sin argumentos 
    if (comando_actual->cant_argumentos == 0 || strcmp(comando_actual->argumentos[0], "~") == 0) {
        destino_final = getenv("HOME");
        if (destino_final == NULL) {
            fprintf(stderr, ROJO "ucvsh: cd: No se pudo obtener la variable de entorno HOME\n"LETRA_NEGRITA);
            return 1;
        }
    }

    //  "cd -" (Regresa al directorio anterior)
    else if (strcmp(comando_actual->argumentos[0], "-") == 0) {
        destino_final = getenv("OLDPWD");
        if (destino_final == NULL) {
            fprintf(stderr, ROJO "ucvsh: cd: OLDPWD no se encuentra definido\n" LETRA_NEGRITA);
            return 1;
        }
        //Se imprime la ruta a la que regresa cuando se usuario 
        printf("%s\n", destino_final);
    } 
    // El usuario especifica una ruta normal
    else {
        destino_final = comando_actual->argumentos[0];
    }

    // Se invoca la llamada al sistema para cambiar de directorio 
    if (chdir(destino_final) == 0) {
        char ruta_actual_despues[1024];
        if (getcwd(ruta_actual_despues, sizeof(ruta_actual_despues)) != NULL) {
            // Se actualizan las variables de entorno actual y anterior 
            setenv("OLDPWD", ruta_actual_antes, 1);
            setenv("PWD", ruta_actual_despues, 1);
        }
        return 0;
    } else {
        // Muestra un mensaje error si carpeta no está, simplemente o lo coloco mal o ya verá que hace 
        perror(ROJO"ucvsh: cd"LETRA_NEGRITA);
        return 1;
    }
}