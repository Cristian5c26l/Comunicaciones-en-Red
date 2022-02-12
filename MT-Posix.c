#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Flujo de instrucciones asociado al hilo creado (hilo en cuestión)
void *routine( void* anything ){
    printf("Hola mundo! Soy el hilo %ld\n",pthread_self());
}

int main(int argc, char const *argv[]){
    
    // Comprobar el número de argumentos especificados por el usuario 
    if( argc != 2 ){
        printf("Falta especificar el numero de hilos en la ejecución de '\'%s!\n",argv[0]);
        exit( 1 );
    }

    // Recuperación de los nHilos del usuario
    int nThreads = atoi( argv[1] );
    
    // Reservación de memoria dinámica (en el heap)
    pthread_t* thArray = ( pthread_t* )malloc( nThreads*sizeof( pthread_t ) );

    /* Inicio de la región de concurrencia/paralelismo */
    // Creación y ejecución de los hilos
    for (int i = 0; i < nThreads; i++){
        if( pthread_create( &thArray[i], NULL, routine, NULL ) < 0 ){
            printf( "El sistema operativo no pudo crear un hilo POSIX de usuario\n" );
            exit( 1 );
        }
    }
    /* Fin de la región de concurrencia/paralelismo */

    // Esperar la terminación de los hilos
    for (int i = 0; i < nThreads; i++){
        pthread_join( thArray[i], NULL );
    }

    // Liberamos espacio de memoria dinamica
    free(thArray);
    printf("Fin del proceso padre!\n");

    return 0;
}

