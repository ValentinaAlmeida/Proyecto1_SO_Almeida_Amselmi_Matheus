#ifndef BUILTINS_H
#define BUILTINS_H
#include "job_list.h"
//funcion que maneja los jobs en segundo plano, para el comando jobs
void builtin_jobs(Job* cabeza);
//funcion para el comando fg pasar de segundo a primer plano
void builtin_fg(Job** cabeza, int id_job);
//funcion que libera toda la memoria para el comando exit
void builtin_exit(Job* cabeza);
#endif
