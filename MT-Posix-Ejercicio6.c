// Este programa recibe un conjunto de palabras. Se crean n hilos dependiendo de las palabras que se especificaron
// Cada hilo recibe una palabra, busca el numero de veces que aparece en un archivo .txt especificado y retorna el
// numero al padre
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

char const *fileName = "TestFile.txt";

typedef struct{// la estructura (que se envia al hilo) contiene
    char testFile[20];// nombre del archivo donde se va a buscar la palabra
    char testWord[100];// palabra a buscar
    int repetitionsNumber;// veces que fue encontrada la palabra
    long pThId;// id del hilo
}Struct;

void *routine(void* aStruct){//Flujo de instrucciones de un hilo. Funcion se convierte en un hilo
    Struct *thStruct = (Struct*) aStruct;
    FILE* fp = fopen(thStruct->testFile,"r");// leer el nombre del archivo que se quiere analizar con este hilo
    if(fp != NULL){

        // Algoritmo para encontrar el numero de veces que aparece la palabra thStruct->strFind en el archivo .txt especificado
        char thWordF[100];
        char *tok; //token donde se guarda una palabra o cadena de caracteres con significado coheerente. En este caso, en tok se guardaran caracteres alfanumericos (para formar palabras o cantidades numericas) hasta que encuentre un espacio, coma, punto o algun signo de puntuacion extra
        while(!feof(fp)){ // Recorrer todo el archivo
            // Extraer palabra por palabra del archivo
            strcpy(thWordF,"");
            fscanf(fp,"%s",thWordF);// almacenar en thWordF la palabra del archivo

            tok = NULL;
            tok = strtok(thWordF," (),.:;-_!@?|#$\"\'\t\n\v");//strtok se encarga de extraer caracteres alfanumericos (que se pueda encontrar en thWordF) hasta que se encuentre un espacio o un ( o un ) o un . o un , ... y dichos caracteres se almacenan en tok. En teoria, tok tendria una palabra

            while(tok != NULL){//mientras tok (palabra de thWordF, que se extrae de el archivo) no sea nula 
                if ( strcmp(tok,thStruct->testWord) == 0 ){
                    thStruct->repetitionsNumber = thStruct->repetitionsNumber + 1;
                }
                tok = strtok(NULL," (),.:;-_!@?|#$\"\'\t\n\v");//obten la siguiente palabra que pueda formarse con strtok utilizando la misma thWordF
            }
            
        }
        
        // Fin de algoritmo
        thStruct->pThId = pthread_self();
        printf("FIN DEL HILO %ld\n", thStruct->pThId);
        return (void*) thStruct;
    }else{
        printf("El archivo %s no se puede localizar en el directorio especificado!!\n",thStruct->testFile);
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

    // Inicializacion de informacion de estructuras a enviar a los hilos
    for (int i = 0; i < nThreads; i++){
        strcpy(structs[i].testFile,fileName);
        //printf("%s\n",argv[i + 1]);
        strcpy(structs[i].testWord,argv[i + 1]);//paso de lo que esta en argv[i + 1] (que es una palabra) a structs[i].testWord
        structs[i].repetitionsNumber= 0;
    }

    // Creación de hilos y ejecución
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
    printf("\nDatos de retorno de los hilos\n\n");
    for (int i = 0; i < nThreads; i++){
        printf("Hilo %ld determinó que la palabra \"%s\" se repite %d veces en el archivo llamado %s\n",thDataRet[i].pThId,thDataRet[i].testWord,thDataRet[i].repetitionsNumber,thDataRet[i].testFile);
    }
    
    //Liberamos espacio de memoria dinamica
    free(thArray);
    free(structs);
    free(thDataRet);
    printf("\nFin del proceso padre!\n");
    
    return 0;
}

//Compilacion -> gcc MT-Posix-Ejercicio6.c -lpthread
//Ejecucion -> ./a.out palabra1 palabra2...
