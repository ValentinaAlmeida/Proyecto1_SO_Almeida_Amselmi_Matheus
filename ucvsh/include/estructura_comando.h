 //# {} [] > < || &&
 #ifndef ESTRUCTURA_COMANDO_H
 #define ESTRUCTURA_COMANDO_H

 typedef struct {
    int hay_tuberia; //despues de ese comando hay tubería o no, para control
    int modo; //valores 1,2,3, 1 si hay un ; . 2 si hay && y 3 si hay 
    char* instruccion; //arreglo con la instrucción base
    char** argumentos; //arreglo de cadenas de caracteres para cada argumento que tenga ese comando
    int cant_argumentos; //solamente por facilidad de guardar el número
 }Comando;

 Comando* procesar_c(char *linea_picada, int* num_comandos); //como voy a llenarlo

 #endif