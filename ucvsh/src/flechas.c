//# {} [] > < || && 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../include/modos_shell.h"
#include "../include/historial_comandos.h"

void habilitar_no_canonico(struct termios* modo){
    struct termios modo2;
    //https://www.ibm.com/docs/es/aix/7.3.0?topic=files-termiosh-file
    //https://www.ibm.com/docs/es/aix/7.3.0?topic=t-tcgetattr-subroutine
    //https://www.ibm.com/docs/es/aix/7.3.0?topic=t-tcsetattr-subroutine
    tcgetattr(STDIN_FILENO, modo);//me da los parametros con el objeto de termios y el estado en el que esta, STDIN_FILENO es estandar para descriptor 0 o por defecto
    modo2 = *modo;//guardo el valor en el temporal
    modo2.c_lflag &= ~(ICANON | ECHO); // Apaga las banderas de eco para que pare la impresion, deshabilita la entrada canonica con ICANON asi la terminal no envia nada hasta que se de enter
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &modo2);//Por partes: 1. con el fileno es descriptor 0 asi que se aplica a la entrada estandar, el TCSAFLUSH como y en que 
    //preciso momento cambia y modo es lo que configure que pase
}
void habilitar_canonico(struct termios* modo){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, modo);//aplico lo mismo del final de arriba, pero ahora va a su configuracion inicial de nuevo
}

void borrar(size_t *posicion){

        while (*posicion > 0) {
        printf("\b \b");//mueve puntero un posicion atras, pone el espacio en blanco y luego mueve de nuevo el cursor a la izquierda así queda el cursor en la posicion del espacio
        (*posicion)--;//disminuyo uno la variable que tiene mi posicion actual
        }
        fflush(stdout);//me aseguro que imprima antes de continuar
}

int leer_actual(char* leido, size_t tamano){
    struct termios modo;

    habilitar_no_canonico(&modo);

    int caracter_ingresado;
    size_t posicion = 0;//posicion actual en la terminal
    int i = total_historial; //inicio el recorrido desde el fondo para que sepa hacia donde va a subir

    memset(leido, 0, tamano);//inicializo todo en 0 https://www.ibm.com/docs/es/i/7.5.0?topic=functions-memset-set-bytes-value

    while(1){

        caracter_ingresado = getchar(); //obtengo el caracter

        if (caracter_ingresado == EOF) {//si se desactiva no tengo porque seguir en esta funcion
            habilitar_canonico(&modo);
            return -1;//termina mal
        }

        //si da enter, primero debo terminar la cadena que estaba procesando, y luego se hace lo demas, para que no quede inconsistente
        if (caracter_ingresado == '\n') {
            leido[posicion] = '\0';
            putchar('\n');//pongo el salto de linea al final
            break;
        }
        if (caracter_ingresado == '\b' || caracter_ingresado  == 127) {//para borrar cosas
            if (posicion > 0) {
                posicion--;
                leido[posicion] = '\0';
                printf("\b \b"); 
                fflush(stdout);
            }
            continue;//saltate todo y continua
        }

        if (caracter_ingresado == 27) { // '\033' es decir el esc significa que puede ser que venga flecha y debe verificar los siguientes dos caracteres a ver si acerto y cual es, o no aceto
            int siguiente = getchar();//veo si es el otro corchete que me falta
            int direccion = getchar();//que direccion es

            if (siguiente == '[') {
                
                switch(direccion){
                    case 'A':
                    // esta corresponde a mi flecha hacia arriba, es decir, traeme el ultimo comando que ejecute
                    if (i > 0) {
                        i--;
                        borrar(&posicion);
                        // Copio el historial que vamos a poner en la terminal para editarse
                        strncpy(leido, arreglo_memoria[i], tamano);
                        posicion = strlen(leido);
                        printf("%s", leido);
                        fflush(stdout);
                    }
                    continue;
                    break;
                    case 'B': //flecha abajo
                    if (i < total_historial) {//porque no puedo logicamente bajas mas si estoy en el ultimo comando
                        i++; //para bajar
                        
                        borrar(&posicion);

                        if (i == total_historial) {
                            // Si volvimos abajo del todo, mostramos la línea vacía original
                            leido[0] = '\0';
                            posicion = 0; //ya estoy al final no vas a bajar mas
                        } else {
                            strncpy(leido, arreglo_memoria[i], tamano);
                            posicion = strlen(leido);
                            printf("%s", leido);
                            fflush(stdout);
                        }
                        
                    }
                    continue;
                    break;
                }
            }
            continue;
            
    }
    if (posicion < (tamano - 1)) {
            
            leido[posicion] = caracter_ingresado;
            posicion++;
            putchar(caracter_ingresado); // imprimo el caracter porque como echo esta apagado no imprime solo
            fflush(stdout);//asegurate de que todo se imprima antes de continuar
        }
    }

    habilitar_canonico(&modo);//vuelvo al modo en el que estaba porque ya debe de tener la linea del comando completa
    return 0; //retorno que termino bien

}