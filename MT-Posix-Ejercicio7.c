//Uso de mutex para que un conjunto de n hilos especificados por el usuario incrementen una variable por 10 miles

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

long x = 0; //recurso compartido
pthread_mutex_t mutex;

void *increment(void* par){
    for (long i = 0; i < 10000; i++){
        pthread_mutex_lock(&mutex);// hilo usa un mutex (o bloqueo) a la sección critica (recurso compartido) para poner en espera a otros hilos que quieran acceder el recurso compartido
        x++;
        pthread_mutex_unlock(&mutex);// hilo libera el mutex para el siguiente hilo que va primero en la cola de hilos de espera para entrar a la sección critica (recurso compartido)
    }
    
}

int main(int argc, char const *argv[]){
    if( argc < 2 ){
        printf("Falta especificar el numero de hilos en la ejecución de %s!\n",argv[0]);
        exit( 1 );
    }

    int nThreads = atoi( argv[1] );
    
    //Reservación de memoria dinámica (en el heap)
    pthread_t* thArray = ( pthread_t* )malloc( nThreads*sizeof(pthread_t) );
    pthread_mutex_init(&mutex,NULL);
    for (int i = 0; i < nThreads; i++){
        if( pthread_create( &thArray[i], NULL, increment, NULL) < 0 ){
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }

    // Esperar la terminación de los hilos
    for (int i = 0; i < nThreads; i++){
        pthread_join( thArray[i], NULL);
    }

    printf("\nx = %ld\n",x);
    pthread_mutex_destroy(&mutex);
    //Liberamos espacio de memoria dinamica
    free(thArray);
    printf("Fin del proceso padre!\n");
    return 0;
}