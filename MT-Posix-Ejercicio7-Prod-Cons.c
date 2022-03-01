#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

long *s = NULL; //recurso compartido
long limit = 49;

sem_t semProd;
sem_t semCons;

void putItemIntoBuffer(long*);
long removeItemFromBuffer(void);
long *produceNewItem(long*);
void consumeItem(long);

void *consumerThread(void* par){
    long valueS = -1;
    while (valueS < limit){
        sem_wait(&semCons);
        valueS = removeItemFromBuffer();
        sem_post(&semProd);
        consumeItem(valueS);
    }
    
}

void *producerThread(void* par){
    long valueS = -1;
    long *item;
    while (valueS < limit){
        item = produceNewItem(&valueS);
        sem_wait(&semProd);
        putItemIntoBuffer(item);
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

    pthread_t th1,th2;
    
    if( pthread_create( &th1, NULL, producerThread, NULL) < 0 ){
        printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
        exit( 1 );
    }

    if( pthread_create( &th2, NULL, consumerThread, NULL) < 0 ){
        printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
        exit( 1 );
    }    

    pthread_join( th1, NULL);
    pthread_join( th2, NULL);
    sem_destroy(&semProd);
    sem_destroy(&semCons);

    return 0;
}

long* produceNewItem(long *value){
    *value = *value + 1 ;
    long *it= malloc(sizeof(long));
    *it = *value;
    return it;
}

void putItemIntoBuffer(long *it){
    s = it;
    return;
}

long removeItemFromBuffer(){
    long auxS = *s;
    free(s);
    return auxS;
}

void consumeItem(long s){
    printf("Item: %ld\n",s);
}