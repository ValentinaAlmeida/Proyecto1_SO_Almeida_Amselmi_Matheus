 //# {} [] > < || && 
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "../include/estructura_comando.h"

 void error_o_liberar(Comando* comando, int numero) {//multiuso, o por error o liberacion
    if (comando == NULL) {//simplemente no me escribieron nada
        return;
    }
    
    for (int i = 0; i < numero; i++) {
        if (comando[i].argumentos != NULL) {
            for (int a = 0; comando[i].argumentos[a] != NULL; a++) {
                free(comando[i].argumentos[a]);
            }
            free(comando[i].argumentos); 
        }
        if (comando[i].instruccion != NULL) {
            free(comando[i].instruccion);
        }
        if (comando[i].r_entrada != NULL) {
            free(comando[i].r_entrada);
        } 
        if (comando[i].r_salida != NULL) {
            free(comando[i].r_salida);
        }
    }
    free(comando);
}

 Comando* procesar_c(char *linea_picada, int *numero){

    char *linea_copia = strdup(linea_picada);
    int cant_c = 0;
    
    char* verificar = strtok(linea_copia, " \n\t");
    while(verificar != NULL) {
        
        if(strcmp(verificar, "|") == 0 || strcmp(verificar, "||") == 0 || strcmp(verificar, "&&") == 0 || strcmp(verificar, ";") == 0) {
            cant_c++;
        }
        verificar = strtok(NULL, " \n\t");
    }
    free(linea_copia);

    *numero= (cant_c+1);

    Comando* comando= (Comando*)calloc((*numero),(sizeof(Comando))); //reservo la memoria dinámica que voy a después meterle al struct
    
    int i=0;
    int num_arg=0;

    char* palabra= strtok(linea_picada, " \n\t");//corta hasta que veas el primer espacio, salto de linea o tabulador
    //veo si la primera es exit, si lo es no tiene sentido guardarla, simpemente salgo de una
    if (palabra == NULL) {
        *numero = 0;
        free(comando);
        return NULL;
    }
    if(strcmp(palabra,"|")== 0 || strcmp(palabra,"&&")== 0||strcmp(palabra,";")== 0 ||strcmp(palabra,"<")== 0 ||strcmp(palabra,">")== 0){
        //error porque no puede empezar con pipe, and, or o ; ni las redirecciones
        printf("error\n");
        printf("Error sintáctico: Comando no puede empezar sin instruccion\n");
        return NULL;
    }
    //como ya me traje la primera palabra entonces la voy a guardar
    //inicializo de base mi primer elemento del struct
    comando[0].instruccion= strdup(palabra); //guardo mi instrucción
    comando[0].argumentos=(char**)calloc(256,(sizeof(char*)));
    comando[0].hay_tuberia=0;
    comando[0].cant_argumentos=0;
    comando[0].modo=0;
    comando[0].r_entrada=NULL;
    comando[0].r_salida=NULL;
    comando[0].orden = 0;

    int cambio=0;

    while(palabra != NULL){
        palabra= strtok(NULL, " \n\t");

        if (palabra == NULL) {
            break;
        }

        if(cambio==1){
            
            if(strcmp(palabra,"|")== 0 || strcmp(palabra,"||")== 0 || strcmp(palabra,"&&")== 0 || strcmp(palabra,";")== 0 || strcmp(palabra,"<")== 0 || strcmp(palabra,">")== 0) {
                printf("Error de sintaxis, no se pueden tener juntos esos dos operadores\n");
                error_o_liberar(comando, i + 1); // libero todo por error
                *numero = 0;
                return NULL;
            }
            cambio=0;
            comando[i].instruccion= strdup(palabra);
            continue;
        }

        if(strcmp(palabra,"|")== 0){
            comando[i].hay_tuberia=1;
            comando[i].argumentos[num_arg]=NULL;
            i++;
            num_arg=0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));
            comando[i].hay_tuberia=0;
            comando[i].cant_argumentos=0;
            comando[i].modo=0;
            comando[i].orden = 0;
            cambio=1;

        }else if(strcmp(palabra,"||")== 0){
            comando[i].modo=1; //indica que el siguiente se hara solo si este falla
            comando[i].argumentos[num_arg]=NULL;
            i++;
            num_arg = 0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));
            comando[i].hay_tuberia=0;
            comando[i].cant_argumentos=0;
            comando[i].modo=0;
            comando[i].orden = 0;
            cambio=1;
        }else if(strcmp(palabra,"&&")== 0){ //si este funciono hago el siguiente
            comando[i].modo=2;
            comando[i].argumentos[num_arg]=NULL;
            i++;
            num_arg = 0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));
            comando[i].hay_tuberia=0;
            comando[i].cant_argumentos=0;
            comando[i].modo=0;
            comando[i].orden = 0;
            cambio=1;
        }else if(strcmp(palabra,";")== 0){//indica que ejecuta el de la izquierda y luego el de la derecha de forma incondicional
            comando[i].modo=3;
            comando[i].argumentos[num_arg]=NULL;
            i++;
            num_arg = 0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));
            comando[i].hay_tuberia=0;
            comando[i].cant_argumentos=0;
            comando[i].modo=0;
            comando[i].orden = 0;
            cambio=1;
        }else if(strcmp(palabra,"<")== 0){//redirección de entrada
            palabra= strtok(NULL, " \n\t");//me muevo a la palabra siguiente que debe ser el nombre de un archivo
            if(palabra == NULL || strcmp(palabra,"|")== 0 || strcmp(palabra,"||")== 0 || strcmp(palabra,"&&")== 0 || strcmp(palabra,";")== 0 || strcmp(palabra,"<")== 0 || strcmp(palabra,">")== 0){
                printf("Error de sintaxis, eso no es una redirección valida'<', se espera un archivo \n");
                error_o_liberar(comando, i + 1);
                *numero = 0;
                return NULL;
            }
            if(palabra != NULL){
                comando[i].r_entrada=strdup(palabra);
            }
            if(comando[i].r_salida == NULL){
                comando[i].orden = 1;
            }
        }else if(strcmp(palabra,">")== 0){ //redirección de salida
            palabra= strtok(NULL, " \n\t");//me muevo a la palabra siguiente que debe ser el nombre de un archivo
            if(palabra == NULL || strcmp(palabra,"|")== 0 || strcmp(palabra,"||")== 0 || strcmp(palabra,"&&")== 0 || strcmp(palabra,";")== 0 || strcmp(palabra,"<")== 0 || strcmp(palabra,">")== 0){
                printf("Error de sintaxis, eso no es una redirección valida'>', se espera un archivo \n");
                error_o_liberar(comando, i + 1);
                *numero = 0;
                return NULL;
            }
            
            if(palabra != NULL){
                comando[i].r_salida=strdup(palabra);
            }

            if(comando[i].r_entrada == NULL){
                comando[i].orden = 2;
            }
        }else{ //simplemente es algun argumento que hace algo
            //strpbrk verifica si alguno es de los caracteres especiales de operaciones, recorre todo y lo busca si lo consigue retorna la posicion
            if (strpbrk(palabra, "&|;><") != NULL) {// para contemplar el caso que se le ocurrio a una de mis compañeras de tener un &&&
                printf("Error ha escrito mal su comando, no puede tener operadores seguidos como pasa en: '%s'\n", palabra);
                error_o_liberar(comando, i + 1);
                *numero = 0;
                return NULL;
            }
            comando[i].argumentos[num_arg]=strdup(palabra);
            num_arg++;
            comando[i].cant_argumentos=num_arg;
        }

    }
    if (cambio == 1) {
        printf("Error de sintaxis, luego de los operadores se debe incluir una instrucción no puede colocar un espacio en blanco\n");
        error_o_liberar(comando, i + 1);
        *numero = 0;
        return NULL;
    }

    comando[i].argumentos[num_arg]=NULL;
    *numero = i + 1;
    return comando;//retorno el inicializado para que se ingrese al del main y todas lo puedan usar
 }