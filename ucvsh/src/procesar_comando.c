 //# {} [] > < || && \
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "../include/estructura_comando.h"

 Comando* procesar_c(char *linea_picada, int* numero){

    Comando* argumentos= (Comando*)malloc((*numero)*(sizeof(Comando))); //reservo la memoria dinámica que voy a después meterle al struct
    int i=0;
    int num_arg=0;


    char* palabra= strtok(linea_picada, " \n\t");//corta hasta que veas el primer espacio, salto de linea o tabulador
    //veo si la primera es exit, si lo es no tiene sentido guardarla, simpemente salgo de una
    if(strcmp(palabra,"exit")== 0){
        //finalizar_ejecucion(); //esa funcion en algun momento estara
    }
    //como ya me traje la primera palabra entonces la voy a guardar
    //inicializo de base mi primer elemento del struct
    argumentos->instruccion= palabra; //guardo mi instrucción
    argumentos->argumentos=(char**)malloc(256*(sizeof(char*)));
    argumentos->hay_tuberia=0;
    argumentos->cant_argumentos=0;
    argumentos->modo=0;


    while(palabra != NULL){

    }

 }