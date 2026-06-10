#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#include "../include/colores.h"
extern volatile pid_t pid_primer_plano;

int builtin_cd(Comando* comando_actual) {
    char* destino_final = NULL;
    char ruta_actual_antes[1024];

    // Se respalda el directorio actual antes de realizar el cambio de ruta
    if (getcwd(ruta_actual_antes, sizeof(ruta_actual_antes)) == NULL) {
        perror("ucvsh: cd: getcwd");
        return 1;
    }

    // El usuario escribe cd sin argumentos 
    if (comando_actual->cant_argumentos == 0) {
        destino_final = getenv("HOME");
        if (destino_final == NULL) {
            fprintf(stderr, "ucvsh: cd: No se pudo obtener la variable de entorno HOME\n");
            return 1;
        }
    } 
    //  "cd -" (Regresa al directorio anterior)
    else if (strcmp(comando_actual->argumentos[0], "-") == 0) {
        destino_final = getenv("OLDPWD");
        if (destino_final == NULL) {
            fprintf(stderr, "ucvsh: cd: OLDPWD no se encuentra definido\n");
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
        perror("ucvsh: cd");
        return 1;
    }
}

int builtin_bg(Job ** lista_jobs,Comando * comando){
pid_t pid= fork();
if(pid<0){
perror("Hubo en fallo para el proceso en segundo plano, intente otra vez");
return 1;
}else if(pid==0){
if (execvp(comando->instruccion, comando->argumentos) == -1) {
            perror("Error al ejecutar el comando en segundo plano");
            exit(1); 
        }
}else{
Insertar_job(lista_jobs, pid, comando->instruccion,1);
Job * trabajo=Buscar_job_porpid(pid,*lista_jobs);
int id=trabajo->idInterno;
printf("[%d] %d\n", id, pid);


}
return 0;
}

int builtin_fg(Job** cabeza, int id_job){
Job *actual = Buscar_job_porid(id_job,*cabeza);//busco el trabajo
if(actual == NULL){//si no esta , es porque no existe
printf("Trabajo no encontrado, intente otra vez \n");
return 1;
}
printf("trayendo a primer plano :%s",actual->comando);
pid_primer_plano = actual->pgid;
kill(actual->pgid, SIGCONT);//esperar a el hijo y decirle que continue
actual->estado = 1;//modificamos el estado leugo defino el numero para los que corren en segundo plano
int estado_kernel;
int retorno_status = 0;
pid_t resultado = waitpid(actual->pgid, &estado_kernel, WUNTRACED);//recolecto a quin mande a segundo plano 
if(resultado>0){ 
if (WIFEXITED(estado_kernel)) {
            retorno_status = WEXITSTATUS(estado_kernel);
            Eliminar_Job(cabeza,actual->idInterno);
        }else if (WIFSIGNALED(estado_kernel)) {
            retorno_status = 1;
            Eliminar_Job(cabeza,actual->idInterno);
        }else if(WIFSTOPPED(estado_kernel)) {

        //no se terino de forma normal, lo detuvieron
        actual->estado=2;//el que determine leugo que rayos es detenido
        printf("\n[%d]+ Stopped %s\n", actual->idInterno, actual->comando);
        retorno_status = 0;
}
}
pid_primer_plano = 0;
return retorno_status;
}

int builtin_jobs(Job* cabeza){
int estado_kernel;
Job * actual=cabeza;
while(actual != NULL){
pid_t resultado = waitpid(actual->pgid, &estado_kernel, WNOHANG | WUNTRACED);
if (resultado > 0) {
            //el proceso cmabio de estado, debo saber que el paso, uso las senales de la doc de waitpid
            
            if (WIFEXITED(estado_kernel) || WIFSIGNALED(estado_kernel)) {

                actual->estado = 3; // lo terminaron
            } 
            else if (WIFSTOPPED(estado_kernel)) {
                
                actual->estado = 2; // luego veo que nuemro uso para detenido
            }
        }
actual=actual->next;//avanzoa el siguiente
}
MostrarJobs(cabeza);//ahora si, imprimo la lista actualizada
return 0;
}


void builtin_exit(Job* cabeza) {
    Job* actual = cabeza;
    
    printf("Cerrando ucvsh... ¡Hasta luego!\n");

    // Matamos los procesos uno a uno para que no se queden por alli  
    while (actual != NULL) {
        kill(actual->pgid, SIGKILL);
        actual = actual->next;
    }

    // Limpiar la RAM libero mi tbala de trabajos
    LimpiarJobs(cabeza); 

    //salimos de el interprete.
    exit(0);
}
