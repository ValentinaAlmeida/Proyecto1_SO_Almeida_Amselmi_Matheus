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
    
    int k=0;
    int hay_comilla=0;
    while(linea_copia[k]!='\0'){
        if((linea_copia[k]=='\'') || (linea_copia[k]=='\"')){
            hay_comilla=1;
        }
        if(hay_comilla==1 && ((linea_copia[k]=='\t') || (linea_copia[k]==' '))){
            linea_copia[k]='\x04';// le meto un caracter no imprimible cualquiera y que no se pone en carpetas o archivo, así me aseguro que el problema de espacios no sea problema
        }
        k++;
    }

    char* verificar = strtok(linea_copia, " \n\t");
    while(verificar != NULL) {
        
        if(strcmp(verificar, "|") == 0 || strcmp(verificar, "||") == 0 || strcmp(verificar, "&&") == 0 || strcmp(verificar, ";") == 0) {
            cant_c++;
        }
        verificar = strtok(NULL, " \n\t");
    }

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
        *numero = 0;
        free(comando);
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
    comando[0].bandera_2plano = 0;

    int cambio=0;

    free(linea_copia);

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
            comando[i].bandera_2plano = 0;
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
            comando[i].bandera_2plano = 0;
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
            comando[i].bandera_2plano = 0;
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
            comando[i].bandera_2plano = 0;
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
        } else { 

            size_t tam_linea = strlen(palabra);
            size_t anterior_final=((tam_linea) - 1);

            // Primero, si llegó aquí puede ser que sea el & para que vaya a segundo plano y deba activar la bandera extra.
            if (strcmp(palabra, "&") == 0) {
                comando[i].bandera_2plano = 1; 
                char* siguiente = strtok(NULL, " \n\t");
                if (siguiente != NULL) {//significa que hay algo más y como ya probé los && entonces no es algo valido
                    printf("Error: solo de puede tener un & al final de cada comando \n");
                    error_o_liberar(comando, i+1);
                    *numero = 0;
                    return NULL;
                }
                break; //Salte del ciclo porque no necesitoq ue siga bajando
            }

            if ((palabra[anterior_final] == '&') && (tam_linea >=2)) {
            //si el carácter antes del final nulo es &, verifico si el tamaño es mayor o igual a 2, porque ya verifique que && no está y si está y luego hay nulo que dicte error, Pero si hay un tercero, ya es error de sintaxis. O puede darse el caso que este pegado y debo ver. 
            //Problema, si el de antes es un & también, eso es sospechoso porque ya pasó el &&, así que debe ser algo como &&&&&&& o &&&|&& por ejemplo, y eso es error. Sino puede ser comando& Y que se olvidarán el espacio Pero sigue siendo válido.

                if (palabra[(anterior_final) - 1] != '&') { 

                    //si existe el segundo &, si es así muere como error de sintaxis, Pero como no es error de una prendo la bandera
                    comando[i].bandera_2plano = 1;
                    palabra[anterior_final] = '\0'; //como está pegado el argumento, para evitar problemas le quito el & y dejo el argumento tranquilito
                    int l2=0;
                    while(palabra[l2]!='\0'){
                        if(palabra[l2]=='\x04'){//revierto el cambio por si las dudas
                        palabra[l2]=' ';
                        }
                        l2++;
                    }
                    comando[i].argumentos[num_arg] = strdup(palabra);
                    num_arg++;
                    comando[i].cant_argumentos = num_arg;
                    char* siguiente = strtok(NULL, " \n\t");//corto el siguiente y vuelvo a asegurarme de que el final sea nulo, porque después del & no puede haber nada, si piensa ir a segundo plano, así que hago una validación como la que tiene &&
                        if (siguiente != NULL) {
                            printf("Error en la sintaxis, únicamente se puede colocar el & al final para pasar a segundo plano, no se acepta combinado con otros parámetros\n");
                            error_o_liberar(comando, i + 1);
                            *numero = 0;
                            return NULL;
                        }
                    break;
                }
            }
            //Si en caso remoto llega hasta aquí, es que por ejemplo tengo un caso como &&|& que no contemplan las validaciones anteriores y es error.
            if (strpbrk(palabra, "&|;><") != NULL) { //busca hasta que consigues alguno de esos en el argumento y devuelve su dirección 
                printf("Error: Ha escrito mal su comando, puede ser que uso muchos operadores que no deberían estar juntos o ha escrito mal, su problema exacto es: '%s'\n", palabra);
                error_o_liberar(comando, i + 1);
                *numero = 0;
                return NULL;
            }
            //Cualquier otra cosa Normal cae aquí y también las comillas, que las voy a quitar para facilidad del módulo de procesamiento 

            if(palabra[0]=='\'' && tam_linea>=2){
                if(palabra[anterior_final]== '\''){

                    for(int j=1;j<(anterior_final);j++){
                    palabra[j-1]=palabra[j];
                    }
                palabra[(anterior_final)-1]='\0';
                tam_linea=(anterior_final-1);
                }else{
                    printf("Error de sintaxis, uso incorrecto de comillas simples\n");
                    error_o_liberar(comando, i + 1);
                    *numero = 0;
                    return NULL;
                }
            }else if(palabra[0]=='\"'&& tam_linea>=2){
                if(palabra[anterior_final]== '\"'){
                    for(int j=1;j<(anterior_final);j++){
                        palabra[j-1]=palabra[j];
                    }
                palabra[(anterior_final)-1]='\0';
                tam_linea=(anterior_final-1);
                }else{
                    printf("Error de sintaxis, uso incorrecto de comillas dobles\n");
                    error_o_liberar(comando, i + 1);
                    *numero = 0;
                    return NULL;
                }
            }
            int l=0;
            while(palabra[l]!='\0'){//revierto cambios antes de guardar finalmente todo
                if(palabra[l]=='\x04'){
                    palabra[l]=' ';
                }
                l++;
            }

            comando[i].argumentos[num_arg] = strdup(palabra);
            num_arg++;
            comando[i].cant_argumentos = num_arg;
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