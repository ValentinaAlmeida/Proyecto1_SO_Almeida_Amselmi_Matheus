#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "../include/colores.h"
extern volatile pid_t pid_primer_plano;//es extern porque la verdadera esta ya incializada en un archivo, varibale que guarda el pid del proceso que esta actuando en el proceso actualmente
int builtin_jobs(Job* cabeza){//recibo el inicio de la lista, no requiero de doble puntero porque no la voy amodificar aqui como tal
    int estado_kernel;//guarda el estado actual de el kernel para las enales que usaremos
    Job *actual = cabeza;//asignamos a un nodod temporal el incio de el alista ara movernos
    while(actual != NULL){//mientras el ndod actual no sea nulo(no hemos llegado al final)
        pid_t resultado = waitpid(actual->pgid, &estado_kernel, WNOHANG | WUNTRACED);//rebisamos que el estado de el proceso actual 
        if(resultado > 0){//si es mayor a cero entonces  no se esta ejecutando 
            if(WIFEXITED(estado_kernel) || WIFSIGNALED(estado_kernel)){//si fue terminado de forma abruptao o segura 
                actual->estado = 3;//actualizalo en la lista
            } else if(WIFSTOPPED(estado_kernel)){//o en caso contrario si fue detenido
                actual->estado = 2;//cmabia su estado actual
            }//cierre de el if-else if
        }//cierre de el  if- else if
        actual = actual->next;//seugiemos moviendonos a el sigueinte nodo de la lista
    }//cierre de el bucle
    MostrarJobs(cabeza);//una vez actalizados imprimimos la lista completa
    return 0;//retrona cero si ya termino
}

int builtin_fg(Job** cabeza, int id_job){// recibimos el inicio de la lita(doble puntero porque no me interesa una copia),y el id interno de el job la cual traeremos aprimer plano
    Job *actual = Buscar_job_porid(id_job, *cabeza);//buscamos el job en la lista y el resultado se lo asignamos a un nodo temporal
    if(actual == NULL){//si es nulo es porque el job no esta en la lista
        printf(ROJO "Error: " LETRA_NEGRITA);
        printf("Trabajo no encontrado, intente otra vez \n"LETRA_NORMAL);//se lo informamos a el usuario con una impresion
        return 1;//retorna 1 para indicar que hubo un error
    }//cierre del if
    printf(VERDE "trayendo a primer plano :%s" LETRA_NORMAL, actual->comando);//si lo entontramos entonces imprmeos a quien estamos trayendo
    pid_primer_plano = actual->pgid;//informamos que el proceso en primer plano corriendo actaulmente es este 
    kill(actual->pgid, SIGCONT);//a traves de kill se le informa a el proceso que puede continuar de forma normal 
    actual->estado = 1;//actualizamos su  estado a ejecutando
    int estado_kernel;//parra gaurdar el estado actaul 
    int retorno_status = 0;//para retornar el estado actaul 
    pid_t resultado = waitpid(actual->pgid, &estado_kernel, WUNTRACED);//de el proceso recuperamos su estado actual, deteniendo lo hasta que haya un cambio de estado 
    if(resultado > 0){//si el proceso ya hizo un mcbaio de estado, 0 si todo saili bien 
        if(WIFEXITED(estado_kernel)){//si termino con tranquilidad
            retorno_status = WEXITSTATUS(estado_kernel);//le asigamos a el valor de retorno su valor 
            Eliminar_Job(cabeza, actual->idInterno);//se saca de la lista ya que termino su ejecucion
        } else if(WIFSIGNALED(estado_kernel)){//si moriste de forma abrupta
            retorno_status = 1;//reorna 1 para indicar que este cerro con error
            Eliminar_Job(cabeza, actual->idInterno);//secaalo de la lista, ya termino
        } else if(WIFSTOPPED(estado_kernel)){//si esta detenido
            actual->estado = 2;//cmabia su estado
            printf(AMARILLO "\n[%d]+  Stopped  %s\n" LETRA_NORMAL, actual->idInterno, actual->comando);//muestra que el proceso esta detenido
            retorno_status = 0;//cero ya que esta en pausa
        }//cierre de la verificaciones
    }//cierre del if completo
    pid_primer_plano = 0;//reinciamos la varible para otro proceso
    return retorno_status;//retornamos el estado de como esta el proceso actaualemnte
}//cierre de la funcion

int builtin_exit(Job* cabeza){//para cerrar la estructura completa,nesecitamos el inicio de la lista
    Job *actual = cabeza;//asigamos la direciond el nodo inicio a un nodo temporal 
    printf(ROSA_CHICLE "Cerrando ucvsh... ¡Hasta luego!\n");//se notifica que se esta cerrando
    while(actual != NULL){//mientras el ndod catual n sea nulo ( no he llegao al final)
        kill(actual->pgid, SIGKILL);//mata a el proceso de raiz, a todos 
        actual = actual->next;//muevete al sigueinte de la lista 
    }//cieere del bucle
    LimpiarJobs(cabeza);//libera la estrutura una vez muertos todos 
    exit(0);//sales con exito
}//cierre de la funcion 

int builtin_bg(Job **lista_jobs, Comando *comando){//nececitamos ala lista actual con la que trbajamos y el comando de el procesoa pasar a segundo plano
    pid_t pid = fork();//creamos el proceso hijo 
    if(pid < 0){//si es menor no se puedo crear 
        perror(ROJO LETRA_NEGRITA "Hubo un fallo para el proceso en segundo plano" LETRA_NORMAL);//se notifica que hubo un fallo
        return 1;//se retorna 1 para notifica que hubo error 
    } else if(pid == 0){//si es cero se pudo crear 
        if(execvp(comando->instruccion, comando->argumentos) == -1){//si al pasar la ejecucion a este proceso en segundo error la llamada al sistema dice que hay un error, este neuvo hijo tomara el comportamiento de el comando que el usuario quiere
            perror(ROJO LETRA_NEGRITA "Error: problemas al ejecutar el comando en segundo plano" LETRA_NORMAL);//se notifica que hubo un problema 
            exit(1);//se sale con error de l hijo, se termina la ejecucion de el hijo
        }//cierre del  if 
    } else {//en caso contrario
        Insertar_job(lista_jobs, pid, comando->instruccion, 1);//agreamos este neuvo proceso
        Job *trabajo = Buscar_job_porpid(pid, *lista_jobs);//lo buscamos 
        int id = trabajo->idInterno;//capturamos su id interno 
        printf(AMARILLO "[%d] %d\n" LETRA_NORMAL, id, pid);//notificamos que se encuntra en segundo plano con este cierto id
    }//cierre del else
    return 0;//retornamos cero si hubo exitos
}//cieere de la funcion


int builtin_cd(Comando* comando_actual) {//recibimos el comando actual para ver que escribieron, el usuario me preocupa
    char* destino_final = NULL;//para cauptrar la ruta final 
    char ruta_actual_antes[1024];//para capturar tods la ruta ctaul pasada con el cd

    // Se respalda el directorio actual antes de realizar el cambio de ruta
    if (getcwd(ruta_actual_antes, sizeof(ruta_actual_antes)) == NULL) {//llamnda al sistema,capturamos donde o bueno alruta donde estamos parados actaualente si no se pudo leer hubo un porblema 
        perror(ROJO LETRA_NEGRITA"ucvsh: cd: getcwd"LETRA_NORMAL);//se le notifica a el usuario
        return 1;//se retorna 21 para notificar que hubo error
    }//cieerre del if

    // El usuario escribe cd sin argumentos 
    if (comando_actual->cant_argumentos == 0 || strcmp(comando_actual->argumentos[0], "~") == 0) {// sel no tiene argumentos es porque solo escribio cd o si su arguemnto es una virgulilla
        destino_final = getenv("HOME");//entonces el destino final es el dierectorio raiz, usamos la llamda al sistema para encontrar la path ya hechapormis comapneras
        if (destino_final == NULL) {//si no ha path
            fprintf(stderr, ROJO LETRA_NEGRITA "ucvsh: cd: No se pudo obtener la variable de entorno HOME\n"LETRA_NORMAL);//se le notifica a el usuario el problema
            return 1;//se retorna uno para notificar que hubo error
        }//cerramos el if
    }//cerramos el if

    //  "cd -" (Regresa al directorio anterior)
    else if (strcmp(comando_actual->argumentos[0], "-") == 0) {//si en los argumentos hay un -
        destino_final = getenv("OLDPWD");//el destino final sera la obtenida con la varible de entorno con la llamada al sistema para saber el anteriori directoriao
        if (destino_final == NULL) {//si no se encuntra dicha ruta
            fprintf(stderr, ROJO LETRA_NEGRITA "ucvsh: cd: OLDPWD no se encuentra definido\n" LETRA_NORMAL);//se le notifica a el usuario
            return 1;//se retorna uno para identificar que hubo un error
        }//cieere del if
        //Se imprime la ruta a la que regresa cuando se usuario 
        printf("%s\n", destino_final);//se imprime la ruta ala que el usuario esta regresando 
    } //ciere del if-else if
    // El usuario especifica una ruta normal
    else {//si el susario espesifica una ruta normal y no las anteriores
        destino_final = comando_actual->argumentos[0];//el destino fnal sera la ruta espesificada
    }//cieere dl else

    // Se invoca la llamada al sistema para cambiar de directorio 
    if (chdir(destino_final) == 0) {//si con la llmada a el sistema se logro cambiar de directorio exitosamente 
        char ruta_actual_despues[1024];
        if (getcwd(ruta_actual_despues, sizeof(ruta_actual_despues)) != NULL) {//si se pudo leer el directrrio actaul donde estamos
            // Se actualizan las variables de entorno actual y anterior directorio
            setenv("OLDPWD", ruta_actual_antes, 1);//se usa la lllamda al sistema para identificar el repertorio anteriro a el antes de cmabair 
            setenv("PWD", ruta_actual_despues, 1);//llamada al sistema para atrpadr el directorio actual en al vaible de entorno
        }
        return 0;//retrorna cero para mostara que todo salio bien 
    } else {
        // Muestra un mensaje error si carpeta no está, simplemente o lo coloco mal o ya verá que hace 
        perror(ROJO LETRA_NEGRITA"ucvsh: cd"LETRA_NORMAL);
        return 1;//muestra 1 para msotrar que hubo un error
    }
}
