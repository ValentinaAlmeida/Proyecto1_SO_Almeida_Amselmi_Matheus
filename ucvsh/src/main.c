 //# {} [] > < || &&
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
#include "../include/ejecutar.h"
 #include "../include/estructura_comando.h"
#include "../include/ejecutar.h"

 extern void error_o_liberar(Comando* comando, int numero);
 extern Comando* procesar_c(char *linea_picada, int *numero);

 int main(){

    char* linea_original=NULL;
    char* linea_copia=NULL;
    char* impresion="UCVShell";

    size_t tamano=0;
    //aqui va el while true pero no lo voy a poner hasta que vea que funciona todo :) attm ale

    printf("%s", impresion);
    printf("%s",">");
    fflush(stdout);
    //aqui va lo de los ctrl

    if(getline(&linea_copia, &tamano,stdin)!=-1){

        int numero=0;
        linea_original=strdup(linea_copia);
        //aqui debo poner la funcion de guardar en el historial la linea original
        Comando* comando= procesar_c(linea_copia, &numero);

        if(comando!= NULL){// si es null hubo fallo y debe volver a empezar luego de escribir el error
        ejecutar_comando(comando, numero);

        error_o_liberar(comando, numero);

        }
    free(linea_original);
    free(linea_copia);
    linea_copia = NULL;  // Puntero a NULL vital para el próximo getline
    tamano = 0;

    //aqui cierra
    return 0;
 }
}