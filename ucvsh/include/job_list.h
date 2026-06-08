#ifndef JOB_LIST_H
#define JOB_LIST_H
#include <sys/types.h> //para los tipos de datos de procesos e ids
#include <stdbool.h>//para la verificacion 
//los estados ahora si estan defindos, oos pondre como macros 
// ESTADO_EJECUTANDO 1
// ESTADO_DETENIDO   2
// ESTADO_TERMINADO  3
//estructura base simple de el proceso que alexandra y Corina usara
 typedef struct Proceso
{
pid_t pid;
struct Proceso* next;
   
}Proceso;

//job 
typedef struct Job
{
  struct Job *next; //puntero al siguiente nodo activo.
  char *comando; //comando relacionado a el job. 
  Proceso *first_process; //lista de procesos de el job.
  pid_t pgid;//el id asignado para el trbajao asociado al proceso de alexandra 
  int estado;//estado de el trabjo que lleva la cueta de el proceso.
  int idInterno;
} Job;

//Funciones que va a usar la lista
//buscar job
Job* Buscar_job_porpid(pid_t id, Job* lista_jobs);
Job* Buscar_job_porid(int id_job,Job* lista_jobs);
//Insertar un nuevo job
void Insertar_job(Job** cabeza, pid_t id_proceso,const char*comando,int estado);
//Eliminar un cierto trabajo
void Eliminar_Job(Job** cabeza,int id_job);
//Cmabiar el estado de un job
void CambiarEstadoJob(Job** cabeza, int id_job, int estado_nuevo);
//Liberar todos los job para cerrar la shell de forma segura 
void LimpiarJobs(Job* cabeza);
//comando Jobs, mostrar todos los jobs y sus detalles
void MostrarJobs(Job * cabeza);
#endif 
