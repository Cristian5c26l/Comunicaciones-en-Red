#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

typedef struct{
    int *fib;
    int totalElementos;
    long pthId;
}ThreadResponse;

void checkMallocError(void*);
void showFibonacci(int*,int);

void *fibonacciThread(void* limitFibo){//Flujo de instrucciones de un hilo. Funcion se convierte en un hilo
    char *limFibo = (char*) limitFibo;
    int limFib = atoi(limFibo);
    //printf("\nHilo produce fibonacci hasta %d :)\n",limFib);
    int *ptrFib;
    int nElementos;
    if(limFib == 0){
        nElementos = 1;
        ptrFib = (int*)malloc(sizeof(int)*nElementos);
        checkMallocError((void*)ptrFib);
        ptrFib[0] = 0;
    }else if(limFib == 1){
        nElementos = 3;
        ptrFib = (int*)malloc(sizeof(int)*nElementos);
        checkMallocError((void*)ptrFib);
        ptrFib[0] = 0;
        ptrFib[1] = 1;
        ptrFib[2] = 1;
    }else{
        int n_2 = 0;
        int n_1 = 1;
        nElementos = 2;
        ptrFib = (int*)malloc(sizeof(int)*nElementos);
        checkMallocError((void*)ptrFib);
        int p = 0;
        ptrFib[p] = n_2;
        p++;
        ptrFib[p] = n_1;
        int auxN1,n;
        while(1){
            n = n_1 + n_2;
            //printf(",%d",n);
            //printf("\n%ld",sizeof(ptrFib));
            nElementos++;
            ptrFib = realloc(ptrFib, sizeof(int)*nElementos);
            checkMallocError((void*)ptrFib);
            p++;
            //printf("\n%ld",sizeof(ptrFib));
            ptrFib[p] = n;
            if(limFib <= n){
                break;
            }
            auxN1 = n_1;
            n_1 = n;
            n_2 = auxN1;
        }
    }

    //printf("\nHilo produce fibonacci hasta %d :)\n",limFib);
    ThreadResponse* thR = (ThreadResponse*)malloc(sizeof(ThreadResponse));
    checkMallocError((void*)thR);
    thR->fib = ptrFib;
    thR->totalElementos = nElementos;
    thR->pthId = pthread_self();
    return (void*) thR;
}

int main(int argc, char const *argv[]){

    ThreadResponse* sFib;

    // Checar cuantos argumentos se pasaron en ./programa Args...
    if( argc == 1 ){
        printf("Falta especificar al menos una cantidad límite para que exista un hilo que haga la serie fibonacci!\n");
        exit( 1 );
    }

    // Checar si no hay cantidades negativas
    for (int i = 0; i < (argc - 1); i++){
        if(atoi(argv[i+1])<0){
            printf("No deben existir limites negativos de la serie fibonacci!\n");
            exit( 1 );
        }
    }
    

    // El numero de hilos es igual al numero de palabras que especificamos para buscar
    int nThreads = argc - 1;

    // Reservación de memoria dinámica (en el heap)
    pthread_t* thArray = ( pthread_t* )malloc( nThreads*sizeof(pthread_t) );
    checkMallocError((void*) thArray );

    // Creación de hilos y ejecución
    for (int i = 0; i < nThreads; i++){
        
        if( pthread_create( &thArray[i], NULL, fibonacciThread, (void*) argv[i+1]) < 0 ){//paso de estructura al hilo
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }

    // Esperar la terminación de los hilos y recuperar lo que retornan a través de aux
    for (int i = 0; i < nThreads; i++){
        pthread_join( thArray[i], (void**) &sFib);//(void**) &thDataRet
        printf("[Hilo %ld:]\n\n",sFib->pthId);
        showFibonacci(sFib->fib,sFib->totalElementos);
        free(sFib);
    }
    
    //Liberamos espacio de memoria dinamica
    free(thArray);
    printf("\nFin del proceso padre!\n");
    
    return 0;
}

void checkMallocError(void* pointer ){
    if( pointer == NULL ){
        printf("Error en Malloc. No se pudo generar memoria dinamica.\n");
        exit(1);
    }
    return;
}

void showFibonacci(int* fib, int nElementos){
    int i = 0;
    while(i<nElementos){
        printf("%d,",fib[i]);
        i++;
    }
    printf("\n\n");
}

//Compilacion -> gcc fibo.c -lpthread
//Ejecucion -> ./a.out limite1 limite2
