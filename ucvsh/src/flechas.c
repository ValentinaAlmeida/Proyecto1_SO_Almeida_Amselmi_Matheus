//# {} [] > < || && 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../include/modos_shell.h"
#include "../include/historial_comandos.h"
#include "../include/colores.h"

#define BARRERA 7

void habilitar_no_canonico(struct termios* modo){
    struct termios modo2;
    //Los siguientes links me sirvieron de investigación y consulta de los comandos que utilicçe posteriormente y deseo dejarlos a la mano
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

    habilitar_no_canonico(&modo);//llama a la función que invoca el modo no canónico, es decir, permite al interprete de comandos que pueda editar, borrar, incluir y moverse, porque el canónico solo permite hacer cosas luego de dar enter, 
    //así que el no canónico permite ir guardando en un buffer y editando y ya luego al dar enter se lleva todo
    memset(leido, 0, tamano);//inicializo todo en 0 https://www.ibm.com/docs/es/i/7.5.0?topic=functions-memset-set-bytes-value
    
    char escrito_actual[TAM_LINEAS]="";//lo que lleva escrito hasta el momento
    int caracter_ingresado;
    size_t posicion = 0;//posicion actual en la terminal
    int i = total_historial; //inicio el recorrido desde el fondo para que sepa hacia donde va a subir
    int tam_actual=0;
    

    while(1){

        caracter_ingresado = getchar(); //obtengo el caracter

        if (caracter_ingresado == EOF) {//si se desactiva no tengo porque seguir en esta funcion
            habilitar_canonico(&modo);
            return -1;//termina mal
        }

        //si da enter, primero debo terminar la cadena que estaba procesando, y luego se hace lo demas, para que no quede inconsistente
        if (caracter_ingresado == '\n') {
            leido[tam_actual] = '\0';
            putchar('\n');//pongo el salto de linea al final
            break;
        }
        if (caracter_ingresado == '\b' || caracter_ingresado  == 127) {//para borrar cosas
            if (posicion <= 0) {//si te pasas del límite no hagas nada, no puedes borrar el promt
                    continue;
                }
            if (posicion > 0) {//si estoy en cualquier otra posicion en la cadena entonces verifico desde donde debo empezar a borrar
                
                if (tam_actual==posicion) {//si es igual al tamaño estamos al final, así que simplemente borra el último caracter y no tiene que devolver visualmente la barra de escritura
                
                posicion--; //disminuyo 1 en la posicion actual en la que me voy a ubicar
                tam_actual--;//le quito 1 al tamaño actual
                leido[posicion] = '\0';//sustituyo el último por nulo para cerrar la cadena
                
                printf("\b \b"); //muevo el cursor, imprimo vacio y vuelvo a mover el cursor a la izquierda
                fflush(stdout);//me aseguro que imprima
                }else{//no busca borrar al final
                    
                    printf("\b");//muevete visualmente 1 a la izquierda
                    posicion--;//restale 1 a la posicion
                    
                    for (size_t a = posicion; a < (size_t)tam_actual; a++) {
                        leido[a] = leido[a + 1];//ruedo todo uno a la izquierda desde la posicion que borre
                    }
                    tam_actual--;//disminuyo 1 la posicion
                    
                    leido[tam_actual] = '\0';//finalizo en nulo
                    printf("%s\033[K", &leido[posicion]);//vuelvo a imprimir para que se pueda ver
                
                    for (int h2 = tam_actual; h2 > (int)posicion; h2--) {//como el cursor se movio debo devolverlo
                    printf("\b");//moverme a la izquierda
                }
                    fflush(stdout);//me aseguro de que todo imprima
                }
            }
            continue;//saltate todo y continua
        }

        if (caracter_ingresado == 27) { // '\033' es decir el esc significa que puede ser que venga flecha y debe verificar los siguientes dos caracteres a ver si acerto y cual es, o no aceto
            int siguiente = getchar();//veo si es el otro corchete que me falta
            int direccion = getchar();//que direccion es

            if (siguiente == '[') {//es probable que se trate de una flecha, ahora a ver cual
                
                switch(direccion){
                    case 'A':
                    // esta corresponde a mi flecha hacia arriba, es decir, traeme el ultimo comando que ejecute
                    
                    if (i > 0) {
                        if (i == total_historial) { //si esta en el actual, es decir el último comando
                            strncpy(escrito_actual, leido, tamano - 1);//copia al escrito actual lo que estaba en el historial en la posicion anterior
                            escrito_actual[tamano - 1] = '\0';//cierro la cadena
                        }
                        i--;
                        borrar(&posicion);//borro lo actual
                        // Copio el historial que vamos a poner en la terminal para editarse
                        strncpy(leido, arreglo_memoria[i], tamano);//sustituyo lo nuevo
                        leido[tamano-1]='\0';//cierro cadena con null
                        posicion = strlen(leido);
                        tam_actual=posicion;
                        printf("%s", leido);//como esta desactivada la bandera, debo imprimir yo manualmente
                        fflush(stdout);
                    }
                    continue;//continua a la siguiente iteración sin ver lo de más abajo
                    break;
                    case 'B': //flecha abajo
                    if (i < total_historial) {//porque no puedo logicamente bajas mas si estoy en el ultimo comando
                        i++; //para bajar
                        
                        borrar(&posicion);

                        if (i == total_historial) {
                            // Si volvimos abajo del todo, mostramos la línea vacía original
                            strncpy(leido, escrito_actual, tamano-1);
                            leido[tamano-1] = '\0';

                        } else {//si no estamos abajo el todo, simplemente imprimo lo que vaya después en terminal
                            strncpy(leido, arreglo_memoria[i], tamano-1);
                            leido[tamano-1] = '\0';
                        }
                        posicion = strlen(leido);
                        tam_actual=posicion;
                        printf("%s", leido);
                        fflush(stdout);
                    }
                    continue;
                    break;
                    case 'C'://derecha
                    if (tam_actual > posicion) {
                            printf("\033[C");//la instruccion para que se mueva uno a la derecha pero sin sobreescribir nada
                            //ANSI SCAPE CODE en wikipedia de las referencias bibliograficas lo explica :)
                            posicion++;   // aumento porque me movi
                            
                            fflush(stdout);
                        }
                    break;
                    case 'D'://izquierda
                        if (posicion > 0) {
                            printf("\b"); // Me muevo visualmente a la izquierda, ya abajo trabajo inserciones

                            posicion--;   // decremento la posicion ya que me movi
                            
                            fflush(stdout);
                        }
                        continue;
                    break;
                }
            }
            continue;
            
    }
    if (posicion < (tamano - 1)) {
            if (posicion == tam_actual) {//si esta al final simplemente escribe
            leido[posicion] = caracter_ingresado;
            posicion++;
            tam_actual++;
            leido[tam_actual] = '\0';
            putchar(caracter_ingresado); // imprimo el caracter porque como echo esta apagado no imprime solo
            fflush(stdout);//asegurate de que todo se imprima antes de continuar
            }else{

                for(int g=tam_actual; (int)posicion < g; g--){
                    leido[g]=leido[g-1];//para evitar cortar cadenas y concatenaciones, ruedo todo un espacio hacia la derecha y ya antes vi si habia espacio
                }
                tam_actual++;//incremento tamaño
                leido[posicion]= caracter_ingresado;//añado el caracter donde abri espacio
                posicion++;//cambio de posicion
                leido[tam_actual]='\0';//añado el null
                printf("%s", &leido[posicion - 1]);//vuelvo a imprimir para que se pueda ver
                
                for(int h=posicion; h < tam_actual; h++){//como volvi a imprimir el cursor me quedaba al final, asi que debia devolverlo
                    printf("\b");//me muevo a la izquierda
                }
                fflush(stdout);//me aseguro de que todo imprima
            }
            
        }
    }

    habilitar_canonico(&modo);//vuelvo al modo en el que estaba porque ya debe de tener la linea del comando completa
    return 0; //retorno que termino bien

}