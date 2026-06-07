#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/historial_comandos.h"

//#{}[] < >  || &&

char arreglo_memoria[CANT_RECORDAR][TAM_LINEAS]; 
int total_historial=0;

void carga_inicial(const char* ruta){
    FILE* nombre_historial= fopen(ruta,"r");
    if(nombre_historial==NULL){
        return;
    }
    char linea[TAM_LINEAS];

    while(fgets(linea,sizeof(linea),nombre_historial)!=NULL){

        if(total_historial>= (CANT_RECORDAR)){//si llego al numero máximo que puedo traerme a la memoria, paro
            break;
        }
        linea[strcspn(linea, "\n")]='\0';//quito el salto de linea para que no traiga problemas con las flechas
        strncpy(arreglo_memoria[total_historial], linea, TAM_LINEAS);//copio en el arreglo
        total_historial++;//aumento la cantidad de comandos que recuerdo
    }
    
    fclose(nombre_historial);//cierro el archivo, todo se abre, se usa y se cierra.
}

void reiniciar_arreglo_historial(char* linea_original){
    for(int i=1; i<CANT_RECORDAR; i++){
        strcpy(arreglo_memoria[i-1], arreglo_memoria[i]);
    }
    strncpy(arreglo_memoria[CANT_RECORDAR-1], linea_original, TAM_LINEAS);
}//basicamente un swapping donde si se llenan los 1024 espacios, agarro y borro el primero de mi pseudo cahce del historial y añado el nuevo

void editar_historial(char* linea_original, const char* ruta){
    
    if(linea_original==NULL || strlen(linea_original)==0 || strcmp(linea_original, "\n")==0){ 
        return; //casos donde es invalido que si quiera escriba algo en el archivo
    }

   FILE* nombre_historial= fopen(ruta,"a");
    if(nombre_historial==NULL){
        return;
    } 
    
        fprintf(nombre_historial, "%s\n", linea_original);
        fflush(nombre_historial);
        fclose(nombre_historial);

    if(total_historial<CANT_RECORDAR){
        strncpy(arreglo_memoria[total_historial], linea_original, TAM_LINEAS);//copio en el arreglo
        total_historial++;//aumento la cantidad de comandos que recuerdo
    }else{
        reiniciar_arreglo_historial(linea_original);
    }

}

