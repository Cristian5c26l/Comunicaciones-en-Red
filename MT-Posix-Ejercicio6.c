// Este programa recibe un conjunto de palabras. Se crean n hilos dependiendo de las palabras que se especificaron
// Cada hilo recibe una palabra, busca el numero de veces que aparece en un archivo .txt especificado y retorna el
// numero al padre
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>// Biblioteca para convertir letras minusculas a mayusculas y viceversa

char const fileName[20] = "TestFile.txt";

typedef struct{// la estructura (que se envia al hilo) contiene
    char testFile[20];// nombre del archivo donde se va a buscar la palabra
    char testWord[20];// palabra a buscar
    int repetitionsNumber;// veces que fue encontrada la palabra
    long pThId;// id del hilo
}Struct;

char* getLowerString(char*);
void checkMallocError(void*);

void *wordSearch(void* word){//Flujo de instrucciones de un hilo. Funcion se convierte en un hilo
    char w[20];
    strcpy(w,(char*)word);//paso de lo que esta en argv[i + 1] (que es una palabra) a w
    //printf("WORD: %s\n",w);
    Struct *returnData = (Struct*) malloc(sizeof(Struct));
    checkMallocError((void*) returnData );
    // Inicializacion de informacion de estructura a enviar al padre
    strcpy(returnData->testFile,fileName);
    //printf("%s\n",argv[i + 1]);
    strcpy(returnData->testWord,getLowerString(w));
    returnData->repetitionsNumber= 0;
    FILE* fp = fopen(returnData->testFile,"r");// leer el nombre del archivo que se quiere analizar con este hilo
    if(fp != NULL){

        // Algoritmo para encontrar el numero de veces que aparece la palabra thStruct->strFind en el archivo .txt especificado
        char thWordF[100];
        char *tok; //token donde se guarda una palabra o cadena de caracteres con significado coheerente. En este caso, en tok se guardaran caracteres alfanumericos (para formar palabras o cantidades numericas) hasta que encuentre un espacio, coma, punto o algun signo de puntuacion extra
        char *thLowerWordF;// cadena thWordF convertida en minusculas
        while(!feof(fp)){ // Recorrer todo el archivo
            // Extraer palabra por palabra del archivo
            strcpy(thWordF,"");
            fscanf(fp,"%s",thWordF);// almacenar en thWordF la palabra del archivo
            thLowerWordF = getLowerString(thWordF);// convertirla a minusculas
            tok = NULL;
            tok = strtok(thLowerWordF," (),.:;-_!@?¡¿|#$\0\"\'\t\n\v");//strtok se encarga de extraer caracteres alfanumericos (que se pueda encontrar en thWordF) hasta que se encuentre un espacio o un ( o un ) o un . o un , ... y dichos caracteres se almacenan en tok. En teoria, tok tendria una palabra

            while(tok != NULL){//mientras tok (palabra de thWordF, que se extrae de el archivo) no sea nula 
                if ( strcmp(tok,returnData->testWord) == 0 ){
                    returnData->repetitionsNumber = returnData->repetitionsNumber + 1;
                }
                tok = strtok(NULL," (),.:;-_!@?¡¿|#$\0\"\'\t\n\v");//obten la siguiente palabra que pueda formarse con strtok utilizando la misma thWordF
            }
            
        }
        
        // Fin de algoritmo
        returnData->pThId = pthread_self();
        //printf("FIN DEL HILO %ld\n", returnData->pThId);
        return (void*) returnData;
    }else{
        printf("El archivo %s no se puede localizar en el directorio especificado!!\n",returnData->testFile);
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
    checkMallocError((void*) thArray );

    // Creación de hilos y ejecución
    for (int i = 0; i < nThreads; i++){
        
        if( pthread_create( &thArray[i], NULL, wordSearch, (void*) argv[i+1]) < 0 ){//paso de estructura al hilo
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }

    // Esperar la terminación de los hilos y recuperar lo que retornan a través de aux
    Struct *thDataRet;
    for (int i = 0; i < nThreads; i++){
        pthread_join( thArray[i], (void**) &thDataRet );
        // Resultados del numero de veces que aparece cada palabra
        printf("\nDatos de retorno de hilos\n\n");
        printf("Hilo %ld determinó que la palabra \"%s\" se repite %d veces en el archivo llamado %s\n",thDataRet->pThId,thDataRet->testWord,thDataRet->repetitionsNumber,thDataRet->testFile);
        printf("\n---------------------------------------------------------------\n");
        free(thDataRet);
    }
    
    //Liberamos espacio de memoria dinamica
    free(thArray);
    printf("\nFin del proceso padre!\n");
    
    return 0;
}

char* getLowerString(char* word){// Pasa todas las letras mayusculas que esten en una palabra a minusculas
    int size = strlen(word);
    char* lowerString = (char*)malloc(sizeof(char)*size);
    checkMallocError((void*) lowerString );
    int i = 0;
    while (word[i] != '\0'){
        lowerString[i] = tolower(word[i]);
        i++;
    }
    
    lowerString[i] = '\0';

    return lowerString;
    
}

void checkMallocError(void* pointer ){
    if( pointer == NULL ){
        printf("Error en Malloc. No se pudo generar memoria dinamica.\n");
        exit(1);
    }
}

//Compilacion -> gcc MT-Posix-Ejercicio6.c -lpthread
//Ejecucion -> ./a.out palabra1 palabra2...
