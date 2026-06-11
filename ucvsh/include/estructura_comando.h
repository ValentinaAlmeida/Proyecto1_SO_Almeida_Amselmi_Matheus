 //#{}[] > < || &&

 #ifndef ESTRUCTURA_COMANDO_H
 #define ESTRUCTURA_COMANDO_H

 typedef struct {
    int hay_tuberia; //despues de ese comando hay tubería o no, para control
    int modo; //valores 1,2,3 1 si hay un or , 2 si hay && , 3 si hay ;
    char* r_entrada;
    char* r_salida;
    int orden;//1 si entrada, 2 si salida
    char* instruccion; //arreglo con la instrucción base
    char** argumentos; //arreglo de cadenas de caracteres para cada argumento que tenga ese comando
    int cant_argumentos; //solamente por facilidad de guardar el número
    int bandera_2plano;
 }Comando;

 Comando* procesar_c(char *linea_picada, int* numero); //como voy a llenarlo
 extern int todo_2plano;
 #endif