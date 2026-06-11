
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include "../include/signals.h"
#include "../include/job_list.h"
#include "../include/colores.h" 
volatile pid_t pid_primer_plano = 0;//aqui es donde definimos la verdadera varible que guarada el pid de el proceso que esta ctaulemnte en primer plano
char comando_primer_plano[1024] = "";//aqui guardaremos el comando que esta en primer plano 

struct Job* lista_jobs; //aqui se crea la lista de jobs 

void capturar_senal(int senal){//como vimos en clses, hacemso el manejador de senales, determiian que se hace si se recibe una senal
    if(senal == SIGINT){ // Ctrl+C, si sla senal es  esa 
        if (pid_primer_plano > 0) {//si el pid de el proceso es mayor a cero( es un proceos que esta creado)
            kill(pid_primer_plano, SIGINT);//le enviamos una senal con la llamada al sistema de qu ahora debe terinar 
        } else {//en caso contrrio el proceso no esta
            printf("\n");//no se imprime nada 
            fflush(stdout);//se vacia el buffer 
        }//cierre de la verificaciones
    } //cierre del if
    else if (senal == SIGTSTP){ // Ctrl+Z, si la senal resulta ser esta
        if (pid_primer_plano > 0) {//si el proceso esta creado
            kill(pid_primer_plano, SIGTSTP); // Pausa el proceso hijo, se el envia la senal a traves de el kill
        } else{//si no esta creado
            printf("\n");//no imprimos nada 
            fflush(stdout);//vacioamos el buffer
        }//cieere de verificacones
    }//cieere de verificaciones
}//fin de la funcion 

void Manejadores_senales(){//aquies donde usamos sigaction() como semenciono en clases y labs
    struct sigaction sa;//definimos la estructura de la senal
    sa.sa_handler = capturar_senal;//el manejador que tendra
    sigemptyset(&sa.sa_mask);//cualquier otra senal la puedes atender cualquiero otra que no sea la definida por mi con el menejador  cuando se haga este manejador el kernel de el SO ya no devuelva errores y solo regresse alpunto donde se quedo , interrumpe abruptamente cualquier llamada al sistema que la shell estuviera haciendo en ese momento
    sa.sa_flags = SA_RESTART; //se configura sus banderas, esta para que 

    if (sigaction(SIGINT, &sa, NULL) == -1) {//se define la senal a capturar para Ctrl+C si esta devuelve error
        perror(ROJO LETRA_NEGRITA"Error al configurar SIGINT"LETRA_NORMAL);//se el notifica a el usuario
    }//cierre del if
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {//se define la senal a capturar para Ctrl+Z si esta devuelve error
        perror(ROJO LETRA_NEGRITA"Error al configurar SIGTSTP"LETRA_NORMAL);//se le notifica a el usuario
    }//cierre de veirifcacones
}//cieere de la funcion 
