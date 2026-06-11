//cabeceras 
#include <stdio.h>
#include <stdlib.h>
#include "job_list.h"
#include <sys/types.h>
#include <string.h>
#include "../include/colores.h"
//Funciones que va a usar la lista
//buscar job
Job* Buscar_job_porpid(pid_t id, Job* lista_jobs){//recibe el pid de el job y la lista de jobs con la que se esta trabajando
    while(lista_jobs != NULL){//Mientras el nodo actual de la lista sea distinto a nulo( cuando lo sea es porque llegamos a el final)
        if(id==lista_jobs->pgid){//si el  pid de el job a buscar es igual a el de el nodo actual 
            return lista_jobs;//retorna el nodo actual, ya que lo encontramos 
        }//cierre de el if
        lista_jobs = lista_jobs->next;//si aun no lo hemos encontrado busca en el siguiente nodo
    }//cierre del bucle
    return NULL;//si no lo encontre en esta lista es porque no esta, retorna nulo
}//fin de la funcion que busca por pid asigndo por el SO

Job* Buscar_job_porid(int id_job, Job* lista_jobs){//busca por id asignado por nosotros en la creacion, se le pasa el id(si es un id debe ser entero) recibe tambien la lista catual de trabajos (nuestra tabla)
    while(lista_jobs != NULL){//Mientras le nodo donde estemos no sea nulo
        if(id_job==lista_jobs->idInterno){//comparo el id pasado con el id interno actual de el nodo(asignado por nosotros)
            return lista_jobs;//si hay coincidencia entremos al if y retorna el nodo ( el trabajo como tal)
        }//cierre del if
        lista_jobs = lista_jobs->next;//si aun no hay coinicidencia continua a el siguiente nodo
    }//cierre del while
    return NULL;//si no lo encontramos en la lista regresa nulo, simplemente no esta
}
//Insertar un nuevo job
void Insertar_job(Job** cabeza, pid_t id_proceso,const char*comando,int estado){//agrega un nuevo trabajo a la lista, pasamos todos los argumentos que debe tener un job, el pid de su proceso relacionado, el puntero a donde se guarda el arreglo de caracteres que forma el comando relacionado a el, el estado que tendra(ejecutando 1, detenido 2, terminado 3)
    //si la cabeza de ela lista es nula, es porque no hay todavia un job, doble puntero en la cabeza para no trabajar con una copia 
    if(*cabeza == NULL){//Si la cbaeza, el inicio de la lista es nulo
      *cabeza= (Job*)malloc(sizeof(Job));//reservamos memoria para un nuevo job del tamanio de la estructura que teniamos
      (*cabeza)->next= NULL;//el sigueinte a el es nulo, solo agregamos un elemento
      (*cabeza)->comando=strdup(comando);//
      (*cabeza)->pgid=id_proceso;//le asignamos el pid del proceso relacionado
      (*cabeza)->estado= estado;//le asignamos el estado ya definido
      (*cabeza)->idInterno=1;//para mayor comodidad se asigna  1 a la cabeza y asi sucecivamente a todos incrementando en una unidad
        //le asignamos todo lo que nesecita 
        return;//salimos de la funcion, ya asignamos el inicio
        
    }//cierre del if
    //en caso contrario buscamos el final de la lista
    Job* temp = *cabeza;//asignamos a un dodo temporal el inicio
    while(temp->next != NULL){//mientras el siguiente nodo no sea nulo( no llegemos a el nodo antes de el final)
    temp= temp->next;//nos movemeos en la lista a el siguiente nodo
    }//una vez encontrmaos el final, cremaos el nuevo nodo
    Job* nuevo = malloc(sizeof(Job));//reservamos memoria a el para el nuevo nodo
    nuevo->next= NULL;//el sigueinte a ael sera nulo ya que sera el nuevo final 
    nuevo->comando=strdup(comando);//le asignamos su comando 
    nuevo->pgid=id_proceso;//su pid se lo asignamos el recicbido por parametros
    nuevo->estado= estado;//le asignamos el estado recibido por parametros
    nuevo->idInterno=temp->idInterno + 1;//de el nodo anterior el nuevo id seria el id anterior mas 1 ya que lo decidimos aunmentar en una unidad 
    temp->next=nuevo;//lo asignamos a el final de la lista 
}//cierre de la funcion

//Eliminar un cierto trabajo
void Eliminar_Job(Job** cabeza,int id_job){//para elimnar requiero de el id interno de el nodod que quiero quitar, y el inicio de la lista
// si la cabeza e nula es porque todavia no hay jobs no hay nada que eliminar 
if(*cabeza == NULL){//sino hay nodos todavia es porque no hay trbajos 
    printf(AMARILLO"No hay Jobs todavia!\n"LETRA_NORMAL);//anunciamos que no hay trbajos, por ello no podemos eliminar 
    return;//salimos, no podemos hacer nada mas 
}//cierre del if
//una nodo tmeporal para movernos en la lista.
Job* temp= *cabeza;
//si el id coincide porque me pides borrar la cabeza
if((*cabeza)->idInterno == id_job){//nos movemos en la lista si donde estamos es la cabeza y el id coincide tenemos que eliminar el inicio
*cabeza= temp->next; // vamos a eliminar la cabeza por ello a la nueva cabeza le asignamos el siguiente a el tmeporal(el suiente a la direccionde la cabeza a eliminar)
free(temp->comando);//para el comando ya que reservamos para guardar su nombre lo liberamos
free(temp);//borramos el temporal ( la cabeza en si) liberando su esapcio en memoria
return;//salimos de la funcion, ya hicimos lo que se pedia
}//cierre del if
//en caso contririo recorremos la lista en busca de la posicion
while(temp->next != NULL && temp->next->idInterno != id_job){//mientras mi siguiente no sea nulo y mi id sea distinto al que se busca  
    temp=temp->next;//me asigno el siguiente de la lista y asi recorrer hasta encontrar, avanzar
}//cierre del bucle, este hasta escontrar el anterior al que voy a eliminar
if(temp == NULL || temp->next == NULL){//si yo llego a ser nulo o mi siguiente 
    printf(ROJO LETRA_NEGRITA"Usted esta fuera de rango, este id no existe \n"LETRA_NORMAL);//no esta en la lista el job que quiere eliminar 
    return;//salimos de la funcion, lo que busca no esta, tal vez se confundio
}//cierre del if
//si lo enconre preservo el siguiente a el que voy a eliminar 
Job *next_next= temp->next->next;//cre un nodo que almacene al siguiente de mi siguiente 
free(temp->next->comando);//libero la memoria reservada en memoria para guardar el comando relacionado al job
free(temp->next);//libero su memoria libero mi obejtivo
temp->next= next_next;// el siguiente a mi actual job es el siguiente de el que elimine

}//cierre de la funcion

//Cmabiar el estado de un job
void CambiarEstadoJob(Job** cabeza, int id_job, int estado_nuevo){//recibo el inicio de la lista, el id interno y el nuevo estado a asignar
    Job *jobCambiado=Buscar_job_porid(id_job, *cabeza);//en un nuevo job guardo el resultado que arroje busaca a mi job objetivo en la lista
    if(jobCambiado != NULL){//si no es nulo es porque lo encontro
        jobCambiado->estado=estado_nuevo;//entro a el campo estado y le asigno el nuevo estado
        return;//retorno porque ya cumpli
    }else{//si es nulo es porque no lo encontre
        printf(ROJO LETRA_NEGRITA"Intente de nuevo, job no encontrado \n"LETRA_NORMAL);//se le notifica a el usuario
    }//cierre del if-else
}//cierre de la funcion 

//esta es la mas fuerte, si se me va un nodo, se nos frego el proyecto, en fin, veamos que tal
//Liberar todos los job para cerrar la shell de forma segura 
void LimpiarJobs(Job* cabeza){//como minimo requiero de el nodo inicial de la lista con la que trabajamos
    //comanzamos desde el actual, mientras que el actual no sea NULL, eso indicaria que ya llegue al final
    while(cabeza != NULL){//mientras mi nodo actual no sea nulo(no hemos llegado al final)
        Job *siguientejob=cabeza->next;//preservamos el siguiente para no perdernos
        free(cabeza->comando);//liebramos la memoria reservada para el nodo actual, en esta caso la usada para gauradr su comando relacionado
        free(cabeza);//liebramos la memoria de el nodo actaul, el espcaio reservado
        cabeza=siguientejob;//le asigamos el sigueinte para avanzar
    }//cierre del while
}//ya liberamos todo, nos salimos

void MostrarJobs(Job* lista_jobs) {//para mosotrar toda lalista requiero de su nodo inicial 
    if (lista_jobs == NULL) {//si fuera nula la cabeza no hay lista todavia
        return;//no tenemos nada que hacer retornamos
    }//cierre del if
    while (lista_jobs != NULL) {//mientras mi nodo actual no sea nulo( no sea yo el final)
        printf("[%d]  PID: %d  Estado: %d  Comando: %s\n", 
               lista_jobs->idInterno, 
               lista_jobs->pgid, 
               lista_jobs->estado, 
               lista_jobs->comando);
        lista_jobs = lista_jobs->next;//a vanzamos a el sigueinte nodo
    }//imprimos todos los cmapos que requiere para cada nodo
}//cierre de la funcion
