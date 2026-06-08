<<<<<<< HEAD
//# {} [] > < || &&
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/ejecutar.h"
#include "../include/estructura_comando.h"
#include "../include/ejecutar.h"
#include "../include/builtins.h"
#include "../include/job_list.h"
 #include "../include/historial_comandos.h"
 //# {} [] > < || &&
extern void error_o_liberar(Comando* comando, int numero);
extern Comando* procesar_c(char *linea_picada, int *numero);

 int main(){
 char* ruta_home= getenv("HOME");
    char ruta_historial[1024];//por el tamaño de linea
    if(ruta_home != NULL){
        strcpy(ruta_historial, ruta_home);//copio la parte de la ruta donde este
        strcat(ruta_historial, "/.historial_ucvshell");//concateno el nombre del archivo privilegiado del historial
    }else{
        strcpy(ruta_historial, ".historial_ucvshell");//en todo caso de que no encuentre nada, por no dejar
    }

    carga_inicial(ruta_historial);

    char* linea_original=NULL;
    char* linea_copia=NULL;
    char* impresion="ucvsh";

    size_t tamano=0;
    //crea la estructura de los jobs por cada proceso hecho por Corina att val
    Job* lista_jobs = NULL;
    //aqui va el while true pero no lo voy a poner hasta que vea que funciona todo :) attm ale

    printf("%s", impresion);
    printf("%s",">");
    fflush(stdout);
    //aqui va lo de los ctrl, es el ulitmo modulo de valentina

    if(getline(&linea_copia, &tamano,stdin)!=-1){

        int numero=0;  
        linea_original=strdup(linea_copia);
        linea_original[strcspn(linea_original, "\n")]='\0';

        editar_historial(linea_original, ruta_historial);

        //aqui debo poner la funcion de guardar en el historial la linea original
        Comando* comando= procesar_c(linea_copia, &numero);
        
        //segun el comando reviso si es un builtin
        if(comando != NULL){ // si es null hubo fallo
            char* cmd_nombre = comando->instruccion;

            if (strcmp(cmd_nombre, "jobs") == 0) {
                builtin_jobs(lista_jobs);
                error_o_liberar(comando, numero);
            }
            else if (strcmp(cmd_nombre, "fg") == 0) {
                int id_trabajo = (comando->cant_argumentos > 0) ? atoi(comando->argumentos[0]) : 1;

                builtin_fg(&lista_jobs, id_trabajo);
                error_o_liberar(comando, numero);
            }
            else if (strcmp(cmd_nombre, "exit") == 0) {
                error_o_liberar(comando, numero); // Limpiamos el comando antes de salir
                builtin_exit(lista_jobs);         // Esta función ya tiene el exit(0) adentro
            }
            else {
                // Si no fue ninguno de mis builtins, es un comando externo
                ejecutar_comando(comando, numero);
                error_o_liberar(comando, numero);
            }
        }
        
        free(linea_original);
        free(linea_copia);
        linea_copia = NULL;  // Puntero a NULL vital para el próximo getline
        tamano = 0;
    } // Aquí cierra 

    return 0;
}
