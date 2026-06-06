 //# {} [] > < || &&
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "../include/estructura_comando.h"

 int main(){

    char* linea_original;
    char* linea_copia;
    char* impresion="UCVShell";

    size_t tamano=0;

    printf(impresion);
    printf(">");

    //aqui va el while true pero no lo voy a poner hasta que vea que funciona todo :) attm ale

    if(getline(&linea_copia, &tamano,stdin)!=-1){

        int num_comandos=0;
        linea_original=strdup(linea_copia);
        //aqui debo poner la funcion de guardar en el historial la linea original
        Comando* comando= procesar_c(*linea_copia, num_comandos);

        //Aqui pueden escribir el inicio del proceso crear los hijos y tal dependiendo de lo que esta en num comandos

        free(comando);
    }

    return 0;
 }