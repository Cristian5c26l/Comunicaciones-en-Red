// Este programa crea n hilos usando memoria dinamica y retornan un número random del 900 al 1000 al proceso padre
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>/*Provee acceso a la API de POSIX en SO's UNIX/LINUX*/
#include <pthread.h>
#include <string.h>

typedef struct{
    int integer;
    float flotant;
    char *str;
}Struct;

void *routine(void* aStruct){//FLujo de instrucciones de un hilo. FUncion se convierte en un hilo
    srand((int)time(NULL));
    Struct *thStruct = (Struct*) aStruct;
    printf("\nHola mundo! Soy el hilo %ld en el índice %d. Recibí una estructura con los siguientes datos\n",pthread_self(), thStruct->integer);
    printf("\nEntero: %d\n",thStruct->integer);
    printf("Flotante: %.3f\n",thStruct->flotant);
    printf("Cadena: %s\n",thStruct->str);
    int *nr = (int*)malloc(sizeof(int));
    int rv = rand () % (1000-900+1) + 900;
    *nr = rv;
    return (void*) nr;//hilo retorna un numero entre 900 y 1000 
}

int main(int argc, char const *argv[]){
    
    if( argc != 2 ){
        printf("Falta especificar el numero de hilos en la ejecución de %s!\n",argv[0]);
        exit( 1 );
    }

    int nThreads = atoi( argv[1] );
    
    //Reservación de memoria dinámica (en el heap)
    pthread_t* thArray = ( pthread_t* )malloc( nThreads*sizeof(pthread_t) );//void* malloc(size_t size): malloc devuelve un apuntador de cualquier tipo de dato
    Struct *structs = (Struct*)malloc(sizeof(Struct)*nThreads);
    int *thDataRet= (int*)malloc(sizeof(int)*nThreads);

    for (int i = 0; i < nThreads; i++){
        structs[i].integer = i;
        structs[i].flotant = (float) i * (float) 2;
        structs[i].str = "Cadena que pertenece al hilo";
        
        if( pthread_create( &thArray[i], NULL, routine, (void*) &structs[i]) < 0 ){//el cuarto parametro puede ser NULL (no le paso parametros a la funcion routine)
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }


    // Esperar la terminación de los hilos así como la recuperación de datos que retornan
    int *aux;
    for (int i = 0; i < nThreads; i++){
        pthread_join( thArray[i], (void**) &aux );
        thDataRet[i] = *aux;
    }

    printf("\nDatos de retorno de los hilos\n");
    for (int i = 0; i < nThreads; i++){
        printf("%d\n",thDataRet[i]);
    }
    

    //Liberamos espacio de memoria dinamica
    free(thArray);//free(void* ptr);//Libera el espacio de memoria ocupado en heap (donde se encuentra un tipo de dato) apuntado por un apuntador de tipo cualquiera (void puede ser int, char, float, pthread_t, etc)
    free(structs);
    free(thDataRet);
    printf("Fin del proceso padre!\n");
    //pthread_exit(NULL);
    return 0;
}
