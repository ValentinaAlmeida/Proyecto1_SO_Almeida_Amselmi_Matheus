#ifndef BUILTINS_H
#define BUILTINS_H
#include "job_list.h"
#include "estructura_comando.h"
//funcion que maneja los jobs en segundo plano, para el comando jobs
int builtin_jobs(Job* cabeza);
//funcion para el comando fg pasar de segundo a primer plano
int builtin_fg(Job** cabeza, int id_job);
//funcion que libera toda la memoria para el comando exit
int builtin_exit(Job* cabeza);
//funcion que pasa a segundo plano segun el parser de Corina
int builtin_bg(Job ** lista_jobs,Comando * comando);
//funcion para el comando cd
int builtin_cd(Comando* comando_actual);
#endif
