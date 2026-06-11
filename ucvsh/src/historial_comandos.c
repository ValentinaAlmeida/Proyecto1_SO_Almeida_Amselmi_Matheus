#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/historial_comandos.h"
#include "../include/colores.h"

//#{}[] < >  || &&

char arreglo_memoria[CANT_RECORDAR][TAM_LINEAS]; //el arreglo de comandos que puede recordar en el historial
int total_historial=0;

void carga_inicial(const char* ruta){//al iniciar el programa debe de traerse del archivo del historial lo que haya y quepa en el espacio que reservé
    FILE* nombre_historial= fopen(ruta,"r");//abro el documento en modo lectura
    if(nombre_historial==NULL){//si el archivo no abrió resulta nulo no puedo hacer nada
        return;
    }
    char linea[TAM_LINEAS];//creo una cadena para ir guardando las lineas que voy buscando del historial

    while(fgets(linea,sizeof(linea),nombre_historial)!=NULL){//obtiene la línea

        if(total_historial>= (CANT_RECORDAR)){//si llego al numero máximo que puedo traerme a la memoria, debo rodar todo si necesito meter uno nuevo, para mantener los más recientes
            for(int i = 1; i < CANT_RECORDAR; i++){
                strcpy(arreglo_memoria[i-1], arreglo_memoria[i]);//ruedo todo un espacio a la izquierda
            }
            //guardo el nuevo en el último puesto que ahora si esta disponible
            strncpy(arreglo_memoria[CANT_RECORDAR-1], linea, TAM_LINEAS);
        }
        linea[strcspn(linea, "\n")]='\0';//quito el salto de linea para que no traiga problemas con las flechas
        strncpy(arreglo_memoria[total_historial], linea, TAM_LINEAS);//copio en el arreglo
        total_historial++;//aumento la cantidad de comandos que recuerdo
    }
    
    fclose(nombre_historial);//cierro el archivo, todo se abre, se usa y se cierra.
}

void reiniciar_arreglo_historial(char* linea_original){//si se pasa del límite empieza a sobreescribir el último con el nuevo, luego de haber rodado todo lo demás un espacio
    for(int i=1; i<CANT_RECORDAR; i++){
        strcpy(arreglo_memoria[i-1], arreglo_memoria[i]);//ruedo todo un espacio para abrirle hueco al nuevo
    }
    strncpy(arreglo_memoria[CANT_RECORDAR-1], linea_original, TAM_LINEAS);
}//basicamente un swapping donde si se llenan los 1024 espacios, agarro y borro el primero de mi pseudo cahce del historial y añado el nuevo

void editar_historial(char* linea_original, const char* ruta){//ingresar algo al historial
    
    if(linea_original==NULL || strlen(linea_original)==0 || strcmp(linea_original, "\n")==0){ 
        return; //casos donde es invalido que si quiera escriba algo en el archivo
    }

   FILE* nombre_historial= fopen(ruta,"a");//abro el archivo como append para poder incluir al final del archivo
    if(nombre_historial==NULL){//no abrio el archivo
        return;
    } 
    
        fprintf(nombre_historial, "%s\n", linea_original);//escribo en el historial
        fflush(nombre_historial);//me aseguro que se escriba
        fclose(nombre_historial);//cierro el archivo

    if(total_historial<CANT_RECORDAR){
        strncpy(arreglo_memoria[total_historial], linea_original, TAM_LINEAS);//copio en el arreglo
        total_historial++;//aumento la cantidad de comandos que recuerdo
    }else{
        reiniciar_arreglo_historial(linea_original);//si llegó al límite entonces que llame a la funcion para mover todo un espacio y quedar el de último
    }

}

