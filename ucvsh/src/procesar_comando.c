 //# {} [] > < || && 
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "../include/estructura_comando.h"

 Comando* procesar_c(char *linea_picada, int* numero){

    int cant_com=0;
    int i=0;

    while(linea_picada[i]!="\n"){
        if(strcmp(linea_picada[i],"|")== 0 ||strcmp(linea_picada[i],"||")== 0 || strcmp(linea_picada[i],"&&")== 0||strcmp(linea_picada[i],";")== 0){
            cant_com++;
        }
    }

    *numero= (cant_com+1);

    Comando* comando= (Comando*)malloc((*numero)*(sizeof(Comando))); //reservo la memoria dinámica que voy a después meterle al struct
    i=0;
    int num_arg=0;

    char* palabra= strtok(linea_picada, " \n\t");//corta hasta que veas el primer espacio, salto de linea o tabulador
    //veo si la primera es exit, si lo es no tiene sentido guardarla, simpemente salgo de una
    if(strcmp(palabra,"exit")== 0){
        //finalizar_ejecucion(); //esa funcion en algun momento estara
    }else if(strcmp(palabra,"|")== 0 || strcmp(palabra,"&&")== 0||strcmp(palabra,";")== 0 ||strcmp(palabra,"<")== 0 ||strcmp(palabra,">")== 0){
        //error porque no puede empezar con pipe, and, or o ; ni las redirecciones
        printf("error\n");
    }
    //como ya me traje la primera palabra entonces la voy a guardar
    //inicializo de base mi primer elemento del struct
    comando[0].instruccion= palabra; //guardo mi instrucción
    comando[0].argumentos=(char**)malloc(256*(sizeof(char*)));
    comando[0].hay_tuberia=0;
    comando[0].cant_argumentos=0;
    comando[0].modo=0;
    comando[0].r_entrada=NULL;
    comando[0].r_salida=NULL;

    while(palabra != NULL){

        palabra= strtok(NULL, " \n\t");

        if(strcmp(palabra,"|")== 0){
            comando[i].hay_tuberia=1;
            i++;
            num_arg=0;
            comando[i].argumentos=(char**)malloc(256*(sizeof(char*)));
            comando[i].hay_tuberia=0;
            comando[i].cant_argumentos=0;
            comando[i].modo=0;

        }else if(strcmp(palabra,"||")== 0){
            comando[i].modo=1; //indica que el siguiente se hara solo si este falla
            i++;
        }else if(strcmp(palabra,"&&")== 0){ //si este funciono hago el siguiente
            comando[i].modo=2;
            i++;
        }else if(strcmp(palabra,";")== 0){//indica que ejecuta el de la izquierda y luego el de la derecha de forma incondicional
            comando[i].modo=3;
            num_arg++;
        }else if(strcmp(palabra,"<")== 0){//redirección de entrada
            palabra= strtok(linea_picada, " \n\t");//me muevo a la palabra siguiente que debe ser el nombre de un archivo
            comando[i].r_entrada=palabra;
            i++;
        }else if(strcmp(palabra,">")== 0){ //redirección de salida
            palabra= strtok(linea_picada, " \n\t");//me muevo a la palabra siguiente que debe ser el nombre de un archivo
            comando[i].r_salida=palabra;
            num_arg++;
        }else{ //simplemente es algun argumento que hace algo
            comando[i].argumentos[num_arg]=palabra;
            num_arg++;
            comando[i].argumentos[num_arg]=num_arg;
        }

    }

    comando[i].argumentos[num_arg]=NULL;

    return comando;//retorno el inicializado para que se ingrese al del main y todas lo puedan usar
 }