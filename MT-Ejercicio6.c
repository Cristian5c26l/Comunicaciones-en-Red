// Este programa recibe un conjunto de palabras. Se crean n hilos dependiendo de las palabras que se especificaron
// Cada hilo recibe una palabra, busca el numero de veces que aparece en un archivo .txt especificado y retorna el
// numero al padre
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

char const *fileName = "TestFile.txt";

typedef struct{// la estructura contiene
    char fn[20];// nombre del archivo donde se va a buscar la palabra
    char strFind[100];// palabra a buscar
    int nTimesFound;// veces que fue encontrada la palabra
}Struct;

int isSignoDeTerminacion(char);

void *routine(void* aStruct){//Flujo de instrucciones de un hilo. Funcion se convierte en un hilo
    Struct *thStruct = (Struct*) aStruct;
    FILE* fp = fopen(thStruct->fn,"r");
    if(fp != NULL){

        // Algoritmo para encontrar el numero de veces que aparece la palabra thStruct->strFind = todos en el archivo .txt especificado
        char thWordF[100];
        while(!feof(fp)){
            strcpy(thWordF,"");
            fscanf(fp,"%s",thWordF);

            if(strlen(thWordF) == strlen(thStruct->strFind)){// Caso base: Comparar cadenas con el mismo tamaño
                if( strcmp(thWordF,thStruct->strFind) == 0 ){// Si tienen el mismo contenido
                    thStruct->nTimesFound = thStruct->nTimesFound + 1;//Registrar que se ha encontrado una vez
                }
            }else{
                if( strncmp(thWordF, thStruct->strFind, strlen(thStruct->strFind)) == 0 ){// si coinciden los primeros caracteres (n primeros caracteres de la palabra estandar a buscar), por ejemplo todos y todos!
                    if(isSignoDeTerminacion(thWordF[strlen(thStruct->strFind)])){//si esta un signo de terminacion en la palabra del archivo todos! y aparte ya comprobado que los primeros caracteres son iguales
                        thStruct->nTimesFound = thStruct->nTimesFound + 1;
                    }
                }else if(thWordF[0] == '('){//para el caso de (todos y todos
                    char *p1 = thWordF;
                    p1++;
                    char *p2 = thStruct->strFind;
                    int coincidences = 0;
                    while(*p1 != '\0'){
                        if(*p1 == *p2){
                            coincidences++;
                            p1++;
                            p2++;
                        }else{
                            break;
                        }
                    }

                    if(coincidences == strlen(thStruct->strFind)){
                        thStruct->nTimesFound = thStruct->nTimesFound + 1;
                    }
                }else if(thWordF[0] == '(' && thWordF[strlen(thWordF) - 1 ] == ')'){//para el caso de (todos) y todos
                    char *p1 = thWordF;
                    p1++;
                    char *p2 = thStruct->strFind;
                    int coincidences = 0;
                    while(*p1 != '\0'){
                        if(*p1 == *p2){
                            coincidences++;
                            p1++;
                            p2++;
                            if(coincidences == strlen(thStruct->strFind)){
                                thStruct->nTimesFound = thStruct->nTimesFound + 1;
                                break;
                            }
                        }else{
                            break;
                        }
                    }
                }
                
            }
        }
        
        // Fin de algoritmo
        //printf("FIN HILO\n");
        return (void*) thStruct;
    }else{
        printf("El archivo %s no se puede localizar en el directorio especificado!!\n",thStruct->fn);
        exit(1);
    }
}

int main(int argc, char const *argv[]){

    // Checar cuantos argumentos se pasaron en ./programa Args...
    if( argc < 2 ){
        printf("Falta especificar al menos una palabra para buscar en el archivo!\n");
        exit( 1 );
    }

    // El numero de hilos es igual al numero de palabras que especificamos para buscar
    int nThreads = argc - 1;

    // Reservación de memoria dinámica (en el heap)
    pthread_t* thArray = ( pthread_t* )malloc( nThreads*sizeof(pthread_t) );
    Struct *structs = (Struct*)malloc(sizeof(Struct)*nThreads);
    Struct *thDataRet= (Struct*)malloc(sizeof(Struct)*nThreads);

    // Asignacion de datos a estructuras
    for (int i = 0; i < nThreads; i++){
        strcpy(structs[i].fn,fileName);
        //printf("%s\n",argv[i + 1]);
        strcpy(structs[i].strFind,argv[i + 1]);
        structs[i].nTimesFound = 0;
    }

    //Creación de hilos y ejecución

    for (int i = 0; i < nThreads; i++){
        
        if( pthread_create( &thArray[i], NULL, routine, (void*) &structs[i]) < 0 ){//paso de estructura al hilo
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }


    // Esperar la terminación de los hilos y recuperar lo que retornan a través de aux
    Struct *aux;
    for (int i = 0; i < nThreads; i++){
        pthread_join( thArray[i], (void**) &aux );
        thDataRet[i] = *aux;
    }

    // Resultados del numero de veces que aparece cada palabra
    printf("\nDatos de retorno de los hilos dado el archivo de prueba %s\n\n", fileName);
    for (int i = 0; i < nThreads; i++){
        printf("%d veces aparece la palabra %s\n",thDataRet[i].nTimesFound,thDataRet[i].strFind);
    }
    

    //Liberamos espacio de memoria dinamica
    free(thArray);
    free(structs);
    free(thDataRet);
    printf("\nFin del proceso padre!\n");
    return 0;
}

int isSignoDeTerminacion(char car){
    return car == ',' || car == ';' || car == '!' || car == '.' || car == '?' || car == ':' || car == ')' ||  car == '\n';
}

//Compilacion -> gcc MT-Ejercicio6.c -lpthread
//Ejecucion -> ./a.out palabra1 palabra2...
