//cabeceras 
#include <stdio.h>
#include <stdlib.h>
#include "job_list.h"
#include <sys/types.h>
#include <string.h>
#include "../include/colores.h"
//Funciones que va a usar la lista
//buscar job
Job* Buscar_job_porpid(pid_t id, Job* lista_jobs){
    while(lista_jobs != NULL){
        if(id==lista_jobs->pgid){
            return lista_jobs;
        }
        lista_jobs = lista_jobs->next;
    }
    return NULL;
}

Job* Buscar_job_porid(int id_job, Job* lista_jobs){
    while(lista_jobs != NULL){
        if(id_job==lista_jobs->idInterno){
            return lista_jobs;
        }
        lista_jobs = lista_jobs->next;
    }
    return NULL;
}
//Insertar un nuevo job
void Insertar_job(Job** cabeza, pid_t id_proceso,const char*comando,int estado){
    //si la cabeza de ela lista es nula, es porque no hay todavia un job
    if(*cabeza == NULL){//creamos el primer job
      *cabeza= (Job*)malloc(sizeof(Job));
      (*cabeza)->next= NULL;
      (*cabeza)->comando=strdup(comando);;
      (*cabeza)->pgid=id_proceso;
      (*cabeza)->estado= estado;
      (*cabeza)->idInterno=1;
        //le asignamos todo lo que nesecita 
        return;
        
    }
    //en caso contrario buscamos el final de la lista
    Job* temp = *cabeza;
    while(temp->next != NULL){
    temp= temp->next;
    }//una vez encontrmaos el final, cremaos el nuevo nodo
    Job* nuevo = malloc(sizeof(Job));
    nuevo->next= NULL;
    nuevo->comando=strdup(comando);;
    nuevo->pgid=id_proceso;
    nuevo->estado= estado;
    nuevo->idInterno=temp->idInterno + 1;
    temp->next=nuevo;//lo asignamos a el final de la lista 
}

//Eliminar un cierto trabajo
void Eliminar_Job(Job** cabeza,int id_job){
// si la cabeza e nula es porque todavia no hay jobs no hay nada que eliminar 
if(*cabeza == NULL){
    printf(AMARILLO"No hay Jobs todavia!\n"LETRA_NORMAL);
    return;
}
//una nodo tmeporal para movernos en la lista.
Job* temp= *cabeza;
//si el id coincide porque me pides borrar la cabeza
if((*cabeza)->idInterno == id_job){
*cabeza= temp->next;
free(temp->comando);
free(temp);
return;
}
//en caso contririo recorremos la lista en busca de la posicion
while(temp->next != NULL && temp->next->idInterno != id_job){//busco su anterior a el 
    temp=temp->next;
}
if(temp == NULL || temp->next == NULL){
    printf(ROJO"Usted esta fuera de rango, este id no existe \n"LETRA_NEGRITA);
    return;
}
//si lo enconre preservo el siguiente a el que voy a eliminar 
Job *next_next= temp->next->next;
free(temp->next->comando);
free(temp->next);//libero su memoria 
temp->next= next_next;// el siguiente a mi actual job es el siguiente de el que elimine

}

//Cmabiar el estado de un job
void CambiarEstadoJob(Job** cabeza, int id_job, int estado_nuevo){
    Job *jobCambiado=Buscar_job_porid(id_job, *cabeza);
    if(jobCambiado != NULL){
        jobCambiado->estado=estado_nuevo;
        return;
    }else{
        printf(ROJO"Intente de nuevo, job no encontrado \n"LETRA_NEGRITA);
    }
}
//esta es la mas fuerte, si se me va un nodo, se nos frego el proyecto, en fin, veamos que tal
//Liberar todos los job para cerrar la shell de forma segura 
void LimpiarJobs(Job* cabeza){
    //comanzamos desde el actual, mientras que el actual no sea NULL, eso indicaria que ya llegue al final
    while(cabeza != NULL){
        Job *siguientejob=cabeza->next;//preservamos el siguiente para no perdernos
        free(cabeza->comando);
        free(cabeza);
        cabeza=siguientejob;
    }
}

void MostrarJobs(Job* lista_jobs) {
    if (lista_jobs == NULL) {
        return;
    }
    while (lista_jobs != NULL) {
        printf("[%d]  PID: %d  Estado: %d  Comando: %s\n", 
               lista_jobs->idInterno, 
               lista_jobs->pgid, 
               lista_jobs->estado, 
               lista_jobs->comando);
        lista_jobs = lista_jobs->next;
    }
}
