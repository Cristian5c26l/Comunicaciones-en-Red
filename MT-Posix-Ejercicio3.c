// Este programa crea n hilos usando memoria dinamica. Todos estos hilos se ejecutan al mismo tiempo. Cada uno recibe un parametro

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>/*Provee acceso a la API de POSIX en SO's UNIX/LINUX*/
#include <pthread.h>


void *routine(void* thId){//FLujo de instrucciones de un hilo. FUncion se convierte en un hilo
    int *thIdPx = (int*) thId;
    printf("Hola mundo! Soy el hilo %ld en el indice %d\n",pthread_self(),*thIdPx);
}

int main(int argc, char const *argv[]){
    
    if( argc != 2 ){
        printf("Falta especificar el numero de hilos en la ejecución de %s!\n",argv[0]);
        exit( 1 );
    }

    int nThreads = atoi( argv[1] );

    //Reservación de memoria dinámica (en el heap)
    pthread_t* thArray = ( pthread_t* )malloc( nThreads*sizeof(pthread_t) );//void* malloc(size_t size): malloc devuelve un apuntador de cualquier tipo de dato
    int *idTh = (int*)malloc(sizeof(int)*nThreads);

    for (int i = 0; i < nThreads; i++){
        idTh[i] = i;
        if( pthread_create( &thArray[i], NULL, routine, (void*) &idTh[i]) < 0 ){//el cuarto parametro puede ser NULL (no le paso parametros a la funcion routine)
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }


    // Esperar la terminación de los hilos
    for (int i = 0; i < nThreads; i++){
        pthread_join( thArray[i], NULL );
    }


    //Liberamos espacio de memoria dinamica
    free(thArray);//free(void* ptr);//Libera el espacio de memoria ocupado en heap (donde se encuentra un tipo de dato) apuntado por un apuntador de tipo cualquiera (void puede ser int, char, float, pthread_t, etc)
    free(idTh);
    printf("Fin del proceso padre!\n");
    //pthread_exit(NULL);
    return 0;
}
