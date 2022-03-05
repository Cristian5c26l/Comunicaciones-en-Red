#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int seccionCritica; //recurso compartido
int limit = 49;

sem_t semProd;
sem_t semCons;


void *consumerThread(void* par){
    int *consThId = (int*) par;
    for (int i = 0; i < (limit + 1); i++){
        sem_wait(&semCons);
        printf("[Consumidor:id:%d][Item: %d]\n",*consThId,seccionCritica);
        sem_post(&semProd);
    }
    
}

void *producerThread(void* par){
    int *prodThId = (int*) par;
    for (int i = 0; i < (limit + 1); i++){
        sem_wait(&semProd);
        seccionCritica = i;
        printf("[Productor:id:%d][Item: %d]\n",*prodThId,seccionCritica);
        sem_post(&semCons);
    }
}

int main(int argc, char const *argv[]){

    if(sem_init(&semCons,0,0) < 0){
        perror("Error en la inicialización del semaforo del consumidor\n");
        exit(1);
    }

    if(sem_init(&semProd,0,1) < 0){
        perror("Error en la inicialización del semaforo productor\n");
        exit(1);
    }

    pthread_t prodTh,consTh;
    int consThId, prodThId;
    consThId = 0;
    prodThId = 0;
    
    if( pthread_create( &prodTh, NULL, producerThread, (void*) &prodThId ) < 0 ){
        printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
        exit( 1 );
    }

    if( pthread_create( &consTh, NULL, consumerThread, (void*) &consThId) < 0 ){
        printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
        exit( 1 );
    }    

    pthread_join( prodTh, NULL);
    pthread_join( consTh, NULL);
    sem_destroy(&semProd);
    sem_destroy(&semCons);

    return 0;
}
