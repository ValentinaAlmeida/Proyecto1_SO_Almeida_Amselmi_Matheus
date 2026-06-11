 //# {} [] > < || && 
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "../include/estructura_comando.h"
 #include "../include/colores.h"

 int todo_2plano=0;//bandera global

 static void limpiar(char* palabra) {//para quitar los caracteres no imprimibles que coloque para que strtock no se los comiera al picar y picara mal el argumento
    if (palabra == NULL){//si la palabra es nula, no hagas nada
        return;
    } 
    int l2=0;
    while(palabra[l2]!='\0'){//mientras no llegue al final de la cadena
        if(palabra[l2]=='\x04'){//revierto el cambio para que el caracter no imprimible vuelva a ser espacio
            palabra[l2]=' ';//pongo espacios en su lugar
        }
        l2++;//aumento
    }
}

 void error_o_liberar(Comando* comando, int numero) {//multiuso, o por error o liberacion, quiero liberar toda la memoria dinámica reservada
    if (comando == NULL) {//simplemente no me escribieron nada
        return;
    }
    
    for (int i = 0; i < numero; i++) {
        if (comando[i].argumentos != NULL) {
            for (int a = 0; comando[i].argumentos[a] != NULL; a++) {//este for libera el arreglo de argumentos
                free(comando[i].argumentos[a]);//libero cada uno individualmente
            }
            free(comando[i].argumentos); //libero el general
        }
        //libero todas las otras variblas que reservaron memoria dinámica
        if (comando[i].instruccion != NULL) {
            free(comando[i].instruccion);
        }
        if (comando[i].r_entrada != NULL) {
            free(comando[i].r_entrada);
        } 
        if (comando[i].r_salida != NULL) {
            free(comando[i].r_salida);
        }
    }
    free(comando);//finalmente libero la estructura completa, porque ya libere todo por separado
}

 Comando* procesar_c(char *linea_picada, int *numero){//ahora se va la aplicar las funciones de un parser, para cortar y guardar toda la informacion en el struct
    todo_2plano=0;//inicializo la bandera
    char *linea_copia = strdup(linea_picada);//hago una copia para poder trabajar adecuadamente el preprocesamiento de espacios y cantidad de comandos
    int cant_c = 0;//variable para la cantidad de comandos
    
    int k=0;//un iterador para el while
    char hay_comilla= 0;//nulo 

    while (linea_picada[k] != '\0') {//mientras no llegue al final de la cadena necesito bucar en este bloque todos los espacios dentro de comillas, puesto que sino 
        //strtok se los va a comer lamentablemente como una de sus limitaciones si uso espacios para cortar, lo cual paso haciendo pruebas, e investigando encontre la estrategia del caracter no imprimible y la use
        if (linea_picada[k] == '\'' || linea_picada[k] == '\"') {//si es comilla simple o doble debo quitarlas
            if (hay_comilla == 0) {
                hay_comilla = linea_picada[k]; // Es la primera comilla
            } else if (hay_comilla == linea_picada[k]) {
                hay_comilla = 0; // es la última comilla
            }
        }
        // Mientras permanezca dentro de mis comillas, debo sustituir los espacios por un caracter no imprimible para que no los quite o corte mal
        if (hay_comilla != 0 && (linea_picada[k] == ' ' || linea_picada[k] == '\t')) {//si encuentro espacio o tabulador
            linea_picada[k] = '\x04'; 
        }
        k++;
    }
    if (hay_comilla != 0) {//si al final no estaba la misma comilla y termino de picar sin devolver todo a la normalidad, entonces debo reportar error
        printf(ROJO "Error sintáctico: " LETRA_NEGRITA);
        printf("Comillas sin cerrar en la línea de comandos.\n"LETRA_NORMAL);
        *numero = 0;
        return NULL;
    }

    char* verificar = strtok(linea_copia, " \n\t");//pico la copia para obtener las secciones del comando
    while(verificar != NULL) {//si no es nulo es que hay algo
        
        if(strcmp(verificar, "|") == 0 || strcmp(verificar, "||") == 0 || strcmp(verificar, "&&") == 0 || strcmp(verificar, ";") == 0) {
            cant_c++;//hace un preprocesamiento donde ve cuantos caracteres especiales hay y en base a eso saca la cantidad de comandos individuales dentro de la línea general que va a procesar
        }
        verificar = strtok(NULL, " \n\t");//continuo el picado
    }

    *numero= (cant_c+1);//la cantidad es igual a los delimitadores mas 1 ya que son operaciones binarias

    Comando* comando= (Comando*)calloc((*numero),(sizeof(Comando))); //reservo la memoria dinámica que voy a después meterle al struct
    
    int i=0;//iterador
    int num_arg=0;//variable para la cantidad de argumentos

    char* palabra= strtok(linea_picada, " \n\t");//corta hasta que veas el primer espacio, salto de linea o tabulador
    
    if (palabra == NULL) {//si lo que picaste no tiene nada
        *numero = 0;
        free(comando);//libera la estructura
        return NULL;
    }
    if(strcmp(palabra,"|")== 0 || strcmp(palabra,"&&")== 0||strcmp(palabra,";")== 0 ||strcmp(palabra,"<")== 0 ||strcmp(palabra,">")== 0){
        //error porque no puede empezar con pipe, and, or o ; ni las redirecciones, asi que el if compra si el primer comando los posee
        printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
        printf("Comando no puede empezar sin instruccion\n"LETRA_NORMAL);
        *numero = 0;
        free(comando);//libero el comando
        return NULL;
    }
    //como ya me traje la primera palabra entonces la voy a guardar
    //inicializo de base mi primer elemento del struct, es decir, dejo valores base o iniciales para tener todo listo para guardar las cosas bien
    limpiar(palabra);
    comando[0].instruccion= strdup(palabra); //guardo mi instrucción
    comando[0].argumentos=(char**)calloc(256,(sizeof(char*)));//reservo la memoria dinámica e inicializo en 0 para no tener basura
    comando[0].hay_tuberia=0;
    comando[0].cant_argumentos=0;
    comando[0].modo=0;
    comando[0].r_entrada=NULL;
    comando[0].r_salida=NULL;
    comando[0].orden = 0;
    comando[0].bandera_2plano = 0;

    int cambio=0;//bandera para ver si hay operadores

    free(linea_copia);//libero la copia que ya use para lo de los primeros ya que no lo necesito

    while(palabra != NULL){//si no es vacío, entonces hay algo que procesar
        palabra= strtok(NULL, " \n\t");//corta con el delimitador

        if (palabra == NULL) {//si no se escribio nada no hay que procesar nada
            break;
        }

        if(cambio==1){//hay un operador identificado, así que el siguiente argumento debe ser algo adecuado y no otro operador
            
            if(strcmp(palabra,"|")== 0 || strcmp(palabra,"||")== 0 || strcmp(palabra,"&&")== 0 || strcmp(palabra,";")== 0 || strcmp(palabra,"<")== 0 || strcmp(palabra,">")== 0) {//si consigue otro operador es error
                printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                printf("No se pueden tener juntos esos dos operadores\n"LETRA_NORMAL);
                error_o_liberar(comando, i + 1); // libero todo por el error
                *numero = 0;
                return NULL;
            }
            cambio=0;//devuelvo la bandera a su sitio
            limpiar(palabra);//limpio el uso del caracter no imprimible
            comando[i].instruccion= strdup(palabra);//guardo la instruccion
            continue;//que vuelva a ejecutar el while porque no necesito que siga leyendo los ifs de abajo
        }

        if(strcmp(palabra,"|")== 0){//si es un pipe
            comando[i].hay_tuberia=1;//si hay tuberia
            comando[i].argumentos[num_arg]=NULL;//el comando no tiene más argumentos, llega hasta el y va a cambiar al otro argumento
            i++;//cambio de sub-comando dentro del comando completo
            //inicializo el resto de variables y banderas para el siguiente comando
            num_arg=0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));//reservo memoria
            comando[i].cant_argumentos=0;
            comando[i].modo=0;
            comando[i].orden = 0;
            comando[i].bandera_2plano = 0;
            cambio=1;//indico que se encontró un operador, para andar pendiente de que lo siguiente no sel un operador tambien

        }else if(strcmp(palabra,"||")== 0){
            comando[i].modo=1; //indica que el siguiente se hara solo si este falla
            comando[i].argumentos[num_arg]=NULL;
            i++;//me muevo al siguiente subcomando e inicializo todo en valores base
            num_arg = 0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));//reservo memoria
            comando[i].hay_tuberia=0;//valor por defecto
            comando[i].cant_argumentos=0;//cantidad por defecto
            comando[i].orden = 0;//aun no hay redireccion, asi que por defecto
            comando[i].bandera_2plano = 0;//aun no tengo el & en este comando, asi que por defecto
            cambio=1;
        }else if(strcmp(palabra,"&&")== 0){ //si este funciono hago el siguiente
            comando[i].modo=2;//indica que va a seguir las normas del and
            comando[i].argumentos[num_arg]=NULL;
            i++;//me muevo al siguiente subcomando e inicializo todo en valores base
            num_arg = 0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));//reservo memoria
            comando[i].hay_tuberia=0;//aun sin tuberías, asi que por defecto
            comando[i].cant_argumentos=0;//cantidad por defecto
            comando[i].orden = 0;//aun no hay redireccion, asi que por defecto
            comando[i].bandera_2plano = 0;//aun no tengo el & en este comando, asi que por defecto
            cambio=1;
        }else if(strcmp(palabra,";")== 0){//indica que ejecuta el de la izquierda y luego el de la derecha de forma incondicional
            comando[i].modo=3;
            comando[i].argumentos[num_arg]=NULL;//inicializo
            i++;//me muevo al siguiente subcomando e inicializo todo en valores base
            //de aqui a las siguientes 7 lineas, inicializa las variables por defecto
            num_arg = 0;
            comando[i].argumentos=(char**)calloc(256,(sizeof(char*)));
            comando[i].hay_tuberia=0;//sin tuberías, por defecto
            comando[i].cant_argumentos=0;//cantidad por defecto
            comando[i].orden = 0;//no hay redireccion, por defecto
            comando[i].bandera_2plano = 0;//no tengo el & en este comando, por defecto
            cambio=1;//hay un operador
        }else if(strcmp(palabra,"<")== 0){//redirección de entrada
            palabra= strtok(NULL, " \n\t");//me muevo a la palabra siguiente que debe ser el nombre de un archivo
            if(palabra == NULL || strcmp(palabra,"|")== 0 || strcmp(palabra,"||")== 0 || strcmp(palabra,"&&")== 0 || strcmp(palabra,";")== 0 || strcmp(palabra,"<")== 0 || strcmp(palabra,">")== 0){
                //verificacion de que lo que viene luego de la redirección es texto y no un operador lógico
                printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                printf("Esto no es una redirección valida'<', se espera un archivo \n"LETRA_NORMAL);
                error_o_liberar(comando, i + 1);
                *numero = 0;
                return NULL;
            }
            if(palabra != NULL){//si no es nulo lo guardo
                limpiar(palabra);//me aseguro de limpiar el argumento del caracter no imprimible
                comando[i].r_entrada=strdup(palabra);//guarda el argumento de redirección
            }
            if(comando[i].r_salida == NULL){
                comando[i].orden = 1;//si salida es nulo, primero viene el de entrada
            }
        }else if(strcmp(palabra,">")== 0){ //redirección de salida
            palabra= strtok(NULL, " \n\t");//me muevo a la palabra siguiente que debe ser el nombre de un archivo
            if(palabra == NULL || strcmp(palabra,"|")== 0 || strcmp(palabra,"||")== 0 || strcmp(palabra,"&&")== 0 || strcmp(palabra,";")== 0 || strcmp(palabra,"<")== 0 || strcmp(palabra,">")== 0){
                //comparo que lo que venga después de la redirección sea texto y no algún operador cualquiera
                printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                printf("Esto no es una redirección valida'>', se espera un archivo \n"LETRA_NORMAL);
                error_o_liberar(comando, i + 1);//libero todo lo que use
                *numero = 0;
                return NULL;
            }
            
            if(palabra != NULL){//si no es nulo lo guardo
                limpiar(palabra);//me aseguro de limpiar el argumento del caracter no imprimible
                comando[i].r_salida=strdup(palabra);//si el argumento no es vacío guardalo
            }

            if(comando[i].r_entrada == NULL){
                comando[i].orden = 2; //si la entrada no tiene nada, primero va la redirección de salida
            }
        } else { //si llego tan lejos es un argumento común y corriente, o no es un operador de los casos anteriores
            //medidas de tamaño que necesito para descartar
            size_t tam_linea = strlen(palabra);//guardo la longitud
            size_t anterior_final=((tam_linea) - 1);//veo cual es el que está entes del final

            // Primero, si llegó aquí puede ser que sea el & para que vaya a segundo plano y deba activar la bandera extra.
            if (strcmp(palabra, "&") == 0) {//significa que hay algo más y como ya probé los && entonces no es algo valido
                if (i == cant_c) { // si es el último comando de la línea entera
                    comando[i].bandera_2plano = 1; //bandera local activada
                    todo_2plano=1;//bandera global para mandar mas de un proceso a background
                    char* siguiente = strtok(NULL, " \n\t");//pico hasta espacio o tabulador
                    if (siguiente != NULL) {//si no tengo vacío luego de mi & entonces no es valido ya que el debe ir solito al final
                        printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                        printf("Solo se puede tener un solo & al final de toda la línea\n"LETRA_NORMAL);
                        error_o_liberar(comando, i + 1);//libero todo lo que use
                        *numero = 0;
                        return NULL;
                    }
                    break;
                } else { // si no es el último comando y está solo, no esta al final, así que no es válido
                    printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                    printf("El símbolo de background '&' solo puede ir al final de toda la línea\n"LETRA_NORMAL);
                    error_o_liberar(comando, i + 1);//libero todo lo que use debido al error
                    *numero = 0;
                    return NULL;
                }
            }
            
            if ((palabra[anterior_final] == '&') && (tam_linea >=2)) {//el último caracter de la línea antes del null final es un & y hay más de 2 caracteres en lo que pique
            //si el carácter antes del final nulo es &, verifico si el tamaño es mayor o igual a 2, porque ya verifique que && no está y si está y luego hay nulo que dicte error, Pero si hay un tercero, ya es error de sintaxis. O puede darse el caso que este pegado y debo ver. 
            //Problema, si el de antes es un & también, eso es sospechoso porque ya pasó el &&, así que debe ser algo como &&&&&&& o &&&|&& por ejemplo, y eso es error. Sino puede ser comando& Y que se olvidarán el espacio Pero sigue siendo válido.

                if (palabra[(anterior_final) - 1] != '&') { //si el anterior al & final que conseguimos, entonces si es igual a &, minimo hay &&& para que no lo agarre el && de antes u otra combinación rara
                    if (i == cant_c) {//si es el comando final
                    //si existe el segundo &, si es así muere como error de sintaxis, Pero como no es error de una prendo la bandera
                    comando[i].bandera_2plano = 1;//activo la bandera local
                    todo_2plano=1;//activo la bandera global
                    palabra[anterior_final] = '\0'; //como está pegado el argumento, para evitar problemas le quito el & y dejo el argumento tranquilito
                    limpiar(palabra);//limpio el argumento antes de guardarlo 
                    comando[i].argumentos[num_arg] = strdup(palabra);//guardo la cadena
                    num_arg++;//aumento cantidad de argumentos
                    comando[i].cant_argumentos = num_arg;//guardo el registro local de la cantidad real de argumentos
                    char* siguiente = strtok(NULL, " \n\t");//corto el siguiente y vuelvo a asegurarme de que el final sea nulo, porque después del & no puede haber nada, si piensa ir a segundo plano, así que hago una validación como la que tiene &&
                        if (siguiente != NULL) {//si después del & hay algo que no sea una cadena nula o vacía, entonces no esta al final o esta combinado con algo más y no es valido
                            printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                            printf("Únicamente se puede colocar el & al final para pasar a segundo plano, no se acepta combinado con otros parámetros\n" LETRA_NORMAL);
                            error_o_liberar(comando, i + 1);
                            *numero = 0;
                            return NULL;
                        }
                    break;
                    }else{//cae aqui si alguno de los comandos tiene el & al final antes de iniciar el siguiente, lo cual no es valido, debe estar al final
                        printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                        printf("Únicamente se puede colocar el símbolo de background '&' al final de todo el comando"LETRA_NORMAL);
                        error_o_liberar(comando, i + 1);//libero lo que use a causa del error
                        *numero = 0;
                        return NULL;  
                    }
                 }
            }
            //Si en caso remoto llega hasta aquí, es que por ejemplo tengo un caso como &&|& que no contemplan las validaciones anteriores y es error.
            if (strpbrk(palabra, "&|;><") != NULL) { //busca hasta que consigues alguno de esos en el argumento y devuelve su dirección 
                printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                printf("Ha escrito mal su comando, puede ser que uso muchos operadores que no deberían estar juntos o ha escrito mal, su problema exacto es: ");
                printf(AMARILLO LETRA_NEGRITA "'%s'\n" LETRA_NORMAL, palabra);
                error_o_liberar(comando, i + 1);//libero lo que use a causa del error
                *numero = 0;
                return NULL;
            }
            //Cualquier otra cosa Normal cae aquí y también las comillas, que las voy a quitar para facilidad del módulo de procesamiento 

            if(palabra[0]=='\'' && tam_linea>=2){//si consigo comillas y minimo hay 2 caracteres puede ser '' o un error o algo entre comillas, así que debo quitarlas para que todo se procese bien
                if(palabra[anterior_final]== '\''){//si consigue la comilla de cierra

                    for(int j=1;j<(anterior_final);j++){//ruedo todo una posicion
                    palabra[j-1]=palabra[j];
                    }
                palabra[(anterior_final)-1]='\0';//al final pongo el null para cerrar la cadena
                tam_linea=(anterior_final-1);//reduzco el tamaño de línea en 2 porque quite las comillas simples
                }else{//si no cerraron las comillas da error porque no estan encerrando bien el texto
                    printf(ROJO "Error sintáctico: "LETRA_NEGRITA);
                    printf("Uso incorrecto de comillas simples\n"LETRA_NORMAL);
                    error_o_liberar(comando, i + 1);//libera toda la memoria dinámica que use
                    *numero = 0;
                    return NULL;//una vez limpie todo, ya puedo retornar
                }
            }else if(palabra[0]=='\"'&& tam_linea>=2){//si consigo comillas y minimo hay 2 caracteres puede ser '' o un error o algo entre comillas, así que debo quitarlas para que todo se procese bien
                if(palabra[anterior_final]== '\"'){//si consigue la comilla de cierra
                    for(int j=1;j<(anterior_final);j++){
                        palabra[j-1]=palabra[j];//ruedo todo un espacio a la izquierda
                    }
                palabra[(anterior_final)-1]='\0';//cierro la cadena con el nulo al final del nuevo tamaño
                tam_linea=(anterior_final-1);//cambio el tamaño por el nuevo quitandole los espacios de las comillas
                }else{//si no cerraron las comillas da error porque no estan encerrando bien el texto
                    printf(ROJO LETRA_NEGRITA"Error sintáctico: ");
                    printf("Uso incorrecto de comillas dobles\n"LETRA_NORMAL);
                    error_o_liberar(comando, i + 1);//libero toda la memoria dinámica
                    *numero = 0;
                    return NULL;//si retorna nulo el sabe que fallo y no tiene que llegar al executer
                }
            }
            limpiar(palabra);//me aseguro de limpiar toda la palabra de caracteres no imprimibles

            comando[i].argumentos[num_arg] = strdup(palabra);//guardo en mi arreglo de argumentos
            num_arg++;//incremento el número
            comando[i].cant_argumentos = num_arg;//actualizo el indicador de número de argumentos actual total
        }  

    }

    
    if (cambio == 1) {//si llega aquí es que el ultimo era un operador lógico y no puede quedarse solo
        printf(ROJO LETRA_NEGRITA "Error sintáctico: "LETRA_NORMAL);
        printf("Luego de los operadores se debe incluir una instrucción no puede colocar un espacio en blanco\n");
        error_o_liberar(comando, i + 1);
        *numero = 0;
        return NULL;
    }

    
    comando[i].argumentos[num_arg]=NULL;//cierro delimitando el ultimo elemento como nulo
    *numero = i + 1;//sumo uno por lo del null para que opere hata antes del null en los for
    return comando;//retorno el inicializado para que se ingrese al del main y todas lo puedan usar
 }