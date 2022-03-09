// ./a.out 3 2 50
//./a.out numProd numCons numProducionesPorProductor
// Que hayan mas productores que consumidores, pero que aun así, los consumidores consuman todo lo que producen
// Multiples productores intentan entrar a la seccion critica

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int seccionCritica; //recurso compartido
int numProd;// Numero de hilos productores
int numProdxProd;// Numero de producciones por hilo productor
int limit;// Ultimo Item, que es igual a (numProd*numProdxProd) - 1
int contadorAux = -1; // recurso compartido auxiliar para ir contando cada que el productor produce un nuevo item a la seccion critica
int flagAuxCons = 0;// La uso para imprimir solo una vez el ultimo item que un hilo va a consumir

sem_t semProd;
sem_t semCons;

typedef struct threadData{//Al hilo productor se le envia el primer Item que va a producir y el ultimo. Con un for, producimos los que esten dentro de ese rango [initItem,LastItem]
    int pseudoId;
    int initItem;//0
    int lastItem;//2 (entonces hay 3 producciones por hilo)
}ThreadDataProd;

void *consumerThread(void* thId){
    int *idCons= (int*) thId;
    //printf("Id conss: %d\n",*idCons);
    //int sConsVal;
    while(1){
        
        sem_wait(&semCons);//3. Aqui llega o C1 o C2 primero (despues, con sem_post(&semCons), dejo pasar a la seccion critica quien este, ya sea C1, C2)
        if( contadorAux ==  limit){// Si ya se llego al ultimo Item, 
            //sem_getvalue(&semCons,&sConsVal);
            //printf("Valor del semaforo consumidor: %d\n",sConsVal);
            //printf("Cont Aux: %d\n",contadorAux);
            //printf("Limit: %d\n",limit);
            if(flagAuxCons == 0){
                flagAuxCons = 1;
                printf("[Consumidor:id:%d][Item: %d]\n",*idCons,seccionCritica);
            }  
            sem_post(&semCons);//dejo pasar a otro hilo consumidor que este en sem_wait (para que vuelva entrar en este if) y termino al actual con break (RECORDEMOS QUE VARIOS HILOS CONSUMIDORES EJECUTAN ESTA FUNCION Y SOLO UNO TOMA EL SEM_WAIT, Y LOS DEMAS SE FORMAN PARA DESPUES TOMAR EL MISMO SEM_WAIT)
            break;
        }
        printf("[Consumidor:id:%d][Item: %d]\n",*idCons,seccionCritica);
        sem_post(&semProd);//4. Aqui dejo pasar al productor que esta en sem_wait(&semProd);
    }

    //printf("Fin del hilo C%d\n",*idCons);

    
}

void *producerThread(void* structData){//coloca items en la seccion critica de acuerdo a un rango en especifico. Por ejemplo, puede poner 3 items que pueden ser 0,1 y 2
    ThreadDataProd *prodData= (ThreadDataProd*) structData;
    //printf("Id prodd: %d\n",prodData->pseudoId);
    for (int i = prodData->initItem; i <= prodData->lastItem; i++){
        sem_wait(&semProd);//1. Un productor va a entrar primero a la seccion critica, los otros se ponen en espera. 
        contadorAux = contadorAux + 1;
        seccionCritica = i;
        //printf("[Productor:id:%d][Item: %d][limit: %d]\n",prodData->pseudoId,seccionCritica,prodData->lastItem);
        sem_post(&semCons);//2. Habilito al consumidor que este en sem_wait(&semCons)
    }

    //printf("Fin del hilo P%d\n",prodData->pseudoId);
}

int main(int argc, char const *argv[]){

    // Inicializar semaforos
    if(sem_init(&semCons,0,0) < 0){
        perror("Error en la inicialización del semaforo del consumidor\n");
        exit(1);
    }

    if(sem_init(&semProd,0,1) < 0){
        perror("Error en la inicialización del semaforo productor\n");
        exit(1);
    }

    if(argc != 4){
        printf("Faltan argumentos. Ingresa el numero de productores, hilos y producciones por productor\n");
        exit(1);
    }

    // Obtener argumentos
    numProd = atoi(argv[1]);
    int numCons = atoi(argv[2]);
    numProdxProd = atoi(argv[3]);
    limit = ( (numProdxProd*numProd) - 1 );

    if(numProd<=numCons){
        printf("Numero de productores debe ser mayor a numero de consumidores\n");
        exit(1);
    }

    int numThreads = numProd + numCons;

    //Creamos espacio para los hilos necesarios
    pthread_t *threadArray = (pthread_t *)malloc(sizeof(pthread_t )*numThreads);

    //thData contiene cada Id de cada hilo
    int *thData = (int*)malloc(sizeof(int )*numThreads);
    for (int i = 0; i < numThreads; i++){
        thData[i] = (i + 1);
    }

    //Datos para los productores
    ThreadDataProd* data = (ThreadDataProd*)malloc(sizeof(ThreadDataProd)*numProd);

    for (int i = 0; i < numProd; i++){
        data[i].pseudoId = thData[i];
        data[i].initItem = numProdxProd * i;
        data[i].lastItem = ((numProdxProd * (i+1)) - 1 );
    }
    
    // Ejecutamos los productores

    for (int i = 0; i < numProd; i++){
        if( pthread_create( &threadArray[i], NULL, producerThread, (void*) &data[i] ) < 0 ){
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }
    

    // Ejecutamos los consumidores 
    for (int i = 0; i < numCons; i++){
        if( pthread_create( &threadArray[i+numProd], NULL, consumerThread, (void*) &thData[i+numProd]) < 0 ){
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }

    // Espero hilos
    for (int i = 0; i < numThreads; i++){
        pthread_join(threadArray[i],NULL);
    }
    
    sem_destroy(&semProd);
    sem_destroy(&semCons);
    free(threadArray);
    free(thData);
    free(data);

    return 0;
}
