//--Modulo de ejecucion de comandos--
//Se encarga de:
// -La ejecucion de comandos mediante la busqueda de binarios en el PATH (ver path.c)
// -Bifurcacion de procesos para la ejecucion de comandos (fork())
// -Manejo redirecciones de entrada y salida (ver redireccion.c)
// -Manejo de operadores logicos (&& || ;)
// -Manejo de tuberias (ver pipes.c)
// -Manejo de procesos en segundo plano (ver job_list.c)

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/ejecutar.h"
#include "../include/estructura_comando.h"
#include "../include/path.h"
#include "../include/redireccion.h"
#include "../include/pipes.h"
#include "../include/job_list.h"
#include "../include/builtins.h"
#include "../include/colores.h"


extern Job* lista_jobs;
extern int todo_2plano;
extern volatile pid_t pid_primer_plano;

// Verifica si el comando es un built in  implementado en ucvsh 
int es_builtin(char *instruccion){
    return strcmp(instruccion, "cd") == 0 ||
            strcmp(instruccion, "exit") == 0 ||
            strcmp(instruccion, "jobs") == 0 || 
            strcmp(instruccion, "fg") == 0;
}

//Ejecuta un comando directamente sin fork(), llamada en pipes.c 
void ejecutar_en_hijo(Comando *cmd){
    char *ruta = buscar_en_path(cmd->instruccion); // Busca la ruta del comando en el PATH y la guarda
    if(ruta == NULL){ //El bin no existe en PATH
        exit(1);
    }
    char **args = malloc(sizeof(char*) * (cmd->cant_argumentos + 2)); // Reserva memoria para los argumentos del comando, el nombre del comando y el NULL final
    args[0] = cmd->instruccion;
    for(int i = 0; i < cmd->cant_argumentos; i++){
        args[i+1] = cmd->argumentos[i];
    }
    args[cmd->cant_argumentos + 1] = NULL;
    execv(ruta, args); // Ejecuta el binario con la ruta encontrada y los argumentos guardados
    perror(ROJO LETRA_NEGRITA"Error al ejecutar el comando"LETRA_NORMAL); //Si execv falla se imprime un mensaje de error
    exit(1);
}


//Ejecuta un comando utilizando fork(), llamada en ejecutar_comando()
int ejecutar_uno(Comando *cmd){
    char *ruta=buscar_en_path(cmd->instruccion); // Busca la ruta del comando en el PATH y la guarda
    
    if(ruta==NULL){ //El bin no existe en PATH
        fprintf(stderr, ROJO LETRA_NEGRITA "Error: comando no encontrado: %s\n" LETRA_NORMAL, cmd->instruccion);
        return -1;
    }

    char **args=malloc(sizeof(char*)*(cmd->cant_argumentos+2)); // Reserva memoria para los argumentos del comando, el nombre del comando y el NULL final
        args[0]=cmd->instruccion;
        for(int i=0; i<cmd->cant_argumentos; i++){
            args[i+1]=cmd->argumentos[i];
        }
        args[cmd->cant_argumentos+1]=NULL;

    pid_t pid=fork(); // Crea un proceso hijo para ejecutar el comando

    if(pid==-1){ // Si fork() falla se imprime un mensaje de error
        perror(ROJO LETRA_NEGRITA"Error al crear el proceso hijo"LETRA_NORMAL);
        return -1; // Retorna -1 para indicar que hubo un error en la ejecución del comando, util para operadores
    }
    if(pid==0){ // fork() no falla
        // Proceso hijo
            //hay redirecciones?
            if(cmd->r_entrada != NULL && cmd->r_salida != NULL){ // Si hay ambas redirecciones
                if(cmd->orden == 1){ //Si el orden es 1, se redirige primero la entrada y luego la salida
                    if(redirigir_entrada(cmd->r_entrada) == -1 || redirigir_salida(cmd->r_salida) == -1){ //funciones de redireccion.c, si alguna falla se imprime un mensaje de error
                        exit(1);
                    }
                }else if(cmd->orden == 2){//si el orden es 2, se redirige primero la salida y luego la entrada
                    if(redirigir_salida(cmd->r_salida) == -1 || redirigir_entrada(cmd->r_entrada) == -1){
                        exit(1);
                    }
                }   
            
            }else if(cmd->r_entrada != NULL){ // Si solo hay una redirección de entrada
                if(redirigir_entrada(cmd->r_entrada) == -1){ //funcion de redireccion.c, si falla se imprime un mensaje de error
                    exit(1);
                }
            }else if(cmd->r_salida != NULL){// Si solo hay una redirección de salida
                if(redirigir_salida(cmd->r_salida) == -1){ //funcion de redireccion.c, si falla se imprime un mensaje de error
                    exit(1);
                }
            }
        
        execv(ruta, args); // Ejecuta el binario con la ruta encontrada y los argumentos guardados
        perror(ROJO LETRA_NEGRITA"Error al ejecutar el comando"LETRA_NORMAL); //Si execv falla se imprime un mensaje de error
        
        exit(1);
    }
    free(ruta); // Libera la memoria reservada para la ruta del comando
    free(args); // Libera la memoria reservada para los argumentos del comando

    if(todo_2plano == 1){ // Si el comando se ejecuta en segundo plano, se agrega a la lista de jobs 
        Insertar_job(&lista_jobs, pid, cmd->instruccion,1);
        return 0;
    }else{ // Si el comando se ejecuta en primer plano
        pid_primer_plano = pid; // Actualiza el PID a PID del proceso en primer plano para poder manejar señales
        int status; 
        waitpid(pid, &status, WUNTRACED); // Espera a que el proceso hijo termine o se detenga (en caso de Ctrl+Z)
        pid_primer_plano = 0; // Reinicia el PID del proceso en primer plano 
        return WEXITSTATUS(status); // Retorna el código de salida del proceso hijo, util para operadores
    }

}


//Recorre el arreglo de comandos y decide como ejecutarlos, llamada en main.c

void ejecutar_comando(Comando *comandos, int numero){
    int i=0;
    while(i<numero){ // Recorre el arreglo de comandos con el número de comandos ingresados
        if(es_builtin(comandos[i].instruccion)) { // Es built-in?
            if(strcmp(comandos[i].instruccion, "cd") == 0){ // es cd?
                builtin_cd(&comandos[i]); // funcion en builtins.c
            } else if(strcmp(comandos[i].instruccion, "exit") == 0){ //es exit?
                builtin_exit(lista_jobs); // funcion en builtins.c
            } else if(strcmp(comandos[i].instruccion, "jobs") == 0){ //es jobs?
                builtin_jobs(lista_jobs); // funcion en builtins.c
            } else if(strcmp(comandos[i].instruccion, "fg") == 0){ // es fg?
                int id = (comandos[i].cant_argumentos > 0) ? atoi(comandos[i].argumentos[0]) : 1;
                builtin_fg(&lista_jobs, id); // funcion en builtins.c
            }
            i++;
        }else{
            if(comandos[i].hay_tuberia==1){ //hay tuberias?
                int fin = i;
                while(comandos[fin].hay_tuberia == 1){ // Busca el ultimo comando en la cadena de pipes 
                    fin++; 
                }
                int codigo_salida=ejecutar_cadena_pipes(comandos, i, fin);
                if(comandos[fin].modo==1){ // || no ejecutar siguiente si el comando tuvo exito 
                    if(codigo_salida==0){
                        break;
                    }
                }else if(comandos[fin].modo==2){ // && no ejecutar siguiente si el comando fallo 
                    if(codigo_salida!=0){
                        break;
                    }
                }
                
                i=fin + 1;
            }else{ // no hay tuberias, ejecutar un comando simple
                
                int codigo_salida=ejecutar_uno(&comandos[i]);
                if(comandos[i].modo==1){ // || no ejecutar siguiente si el comando tuvo exito 
                    if(codigo_salida==0){
                        break;
                    }
                }else if(comandos[i].modo==2){ // && no ejecutar siguiente si el comando fallo 
                    if(codigo_salida!=0){
                        break;
                    }
                }
            
                i++; //avanzar al siguiente comando
            }
        }
    }
}