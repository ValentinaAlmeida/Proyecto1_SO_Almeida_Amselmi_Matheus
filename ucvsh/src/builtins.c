#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
extern volatile pid_t pid_primer_plano;
void builtin_jobs(Job* cabeza){
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

}

void builtin_fg(Job** cabeza, int id_job){
Job *actual = Buscar_job_porid(id_job,*cabeza);//busco el trabajo
if(actual == NULL){//si no esta , es porque no existe
printf("Trabajo no encontrado, intente otra vez \n");
return;
}
printf("trayendo a primer plano :%s",actual->comando);
pid_primer_plano = actual->pgid;
kill(actual->pgid, SIGCONT);//esperar a el hijo y decirle que continue
actual->estado = 1;//modificamos el estado leugo defino el numero para los que corren en segundo plano
int estado_kernel;
pid_t resultado = waitpid(actual->pgid, &estado_kernel, WUNTRACED);//recolecto a quin mande a segundo plano 
if(resultado>0){ 
if (WIFEXITED(estado_kernel) || WIFSIGNALED(estado_kernel)) {
            //termino debo liberarlo de la lista
            Eliminar_Job(cabeza,actual->idInterno);
        }else if(WIFSTOPPED(estado_kernel)) {

        //no se terino de forma normal, lo detuvieron
        actual->estado=2;//el que determine leugo que rayos es detenido
        printf("\n[%d]+  Stopped  %s\n", actual->idInterno, actual->comando);
}
}
pid_primer_plano = 0;
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
void builtin_bg(Job ** lista_jobs,Comando * comando){
pid_t pid= fork();
if(pid<0){
perror("Hubo en fallo para el proceso en segundo plano, intente otra vez");
return;
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

}
