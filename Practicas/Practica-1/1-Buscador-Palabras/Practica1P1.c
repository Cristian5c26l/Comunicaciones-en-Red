#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>// Biblioteca para convertir letras minusculas a mayusculas y viceversa

char *words[] = {"Héroe","Princesa","Bosque","Roca","Castillo","Aves","Había","Reino","Animales","Aventura"};

typedef struct wordData{// Bloque usado para almacenar cuantas veces apareció una palabra en particular (Usado en el padre, para estadistica general. En el hilo, para un archivo en particular)
    int repetitionsNumber;
    char *testWord;
}WordData;

typedef struct responseData{// Bloque de información de retorno al padre
    long pThId;// id del hilo
    int pseudoId;
    char testFile[20];// archivo que el hilo analizo
    WordData *wordReport;
}ResponseData;

typedef struct ThreadArgumentStruct{// Estructura que se manda como parametro al hilo
    int pseudoId;
    char fileName[20];
}StructArgument;

char* getLowerString(char*);// Tiene como parametro una cadena
int sizeWordArray(void);
long getWordsNumberTotal(char const *argv[],int);

void *searchWord(void* structPar){//Flujo de instrucciones de un hilo. Funcion se convierte en un hilo
    StructArgument* sA = (StructArgument*) structPar;
    //Concatenar nombre del archivo recibido con la extension .txt
    char tf[20];
    strcpy(tf, sA->fileName);
    char ext[] = ".txt";
    strcat(tf,ext);
    FILE* fp = fopen(tf,"r");// leer el nombre del archivo que se quiere analizar con este hilo
    if(fp != NULL){
        //Crear bloque de información de retorno al padre
        ResponseData* respData = (ResponseData*)malloc(sizeof(ResponseData));
        respData->pThId = pthread_self();
        respData->pseudoId = sA->pseudoId;
        strcpy(respData->testFile,tf);
        
        int sArrWord = sizeWordArray();
        respData->wordReport = (WordData *)malloc(sizeof(WordData)*sArrWord);
        // Inicializar estructuras que contienen una palabra en particular (que este en minusculas) asi como cuantas veces aparece en el archivo ft analizado
        int i = 0;
        while (i<sArrWord){
            respData->wordReport[i].testWord = getLowerString(words[i]);
            respData->wordReport[i].repetitionsNumber = 0;
            i++;
        }
        char thWordF[100];// cadena extraida del archivo
        char *tok;// token
        char *thLowerWordF;// cadena thWordF convertida en minusculas

        while(!feof(fp)){// lectura total del archivo
            thLowerWordF = NULL;
            strcpy(thWordF,"");// inicializar cadena donde guardo palabra del archivo
            fscanf(fp,"%s",thWordF);// almacenar en thWordF la palabra del archivo
            thLowerWordF = getLowerString(thWordF);// convertirla a minusculas
            tok = NULL;
            tok = strtok(thLowerWordF," ()—,.:;-_!@?¿¡|#$\0\"\'\t\n\v");//strtok se encarga de extraer caracteres alfanumericos (que se pueda encontrar en la cadena convertida en minusculas) hasta que se encuentre un espacio o un ( o un ) o un . o un , ... y dichos caracteres se concatenan y se almacenan en tok. En teoria, tok tendria una palabra

            while(tok != NULL){//mientras tok (cadena extraida de la palabra en minusculas) no sea nula 
                i = 0;
                while(i<sArrWord){
                    if ( strcmp(tok,respData->wordReport[i].testWord) == 0 ){
                        respData->wordReport[i].repetitionsNumber = respData->wordReport[i].repetitionsNumber + 1;
                        break;
                    }
                    i++;
                }
                tok = strtok(NULL," ()—,.:;-_!@?¿¡|#$\0\"\'\t\n\v");//obten la siguiente palabra que pueda formarse con strtok utilizando la misma thWordF
            }
        }
        //printf("FIN DEL HILO %ld\n", pthread_self());
        return (void*) respData;
    }else{
        printf("El archivo %s no se puede localizar en el directorio actual!!\n",tf);
        exit(1);
    }
}

int main(int argc, char const *argv[]){
    
    printf("\n");
    ResponseData* newResponseData;//apuntador a la respuesta del hilo
    // Comprobar si hay estrictamente 11 argumentos (Los 10 nombres de los archivos y el nombre del ejecutable)
    if(argc != 11){
        printf("Debes especificar exactamente 10 archivos de cuentos para buscar palabras en ellos!\n");
        exit( 1 );
    }
    int nThreads = (argc - 1);// Numero de hilos es el total de argumentos menos el del nombre del ejecutable
    int sArrWord = sizeWordArray();// Numero de palabras contenidas en words 
    
    WordData *wDataReport = (WordData*)malloc(sizeof(WordData)*(sArrWord));//Arreglo para almacenar el total de apariciones general (en los 10 archivos) que hubo de cada palabra

    // Inicializar total de veces que aparece cada palabra para la estadistica final
    for (int i = 0; i < (sArrWord); i++){
        wDataReport[i].repetitionsNumber = 0;
        wDataReport[i].testWord = words[i];
    }
    
    // Conteo del total de palabras que se analizan
    long nWords = getWordsNumberTotal(argv,argc);

    // Preparar argumentos a enviar a cada hilo
    StructArgument *argumentArray = (StructArgument*)malloc(sizeof(ResponseData)*nThreads);
    for (int i = 0; i < (nThreads); i++){
        argumentArray[i].pseudoId = i;
        strcpy(argumentArray[i].fileName,argv[i+1]);
    }

    pthread_t* thArray = ( pthread_t* )malloc( (nThreads)*sizeof(pthread_t) );// tam = 10, 10 archivos a analizar
    ResponseData *responseArray = (ResponseData *)malloc(sizeof(ResponseData)*nThreads);// Arreglo para almacenar temporalmente las estructuras tipo ResponseData que retornan los hilos

    // Creación y ejecución de los hilos
    for (int i = 0; i < (nThreads); i++){
        if( pthread_create( &thArray[i], NULL, searchWord, (void*) &argumentArray[i]) < 0 ){//paso del nombre del archivo a analizar al hilo
            printf("El sistema operativo no pudo crear un hilo POSIX de usuario\n");
            exit( 1 );
        }
    }
    
    // Esperar terminacion de los hilos
    for (int i = 0; i < (nThreads); i++){
        pthread_join( thArray[i], (void**) &newResponseData);
        responseArray[i] = *newResponseData;
        free(newResponseData);
    }

    //Imprimir resultados de respuesta de todos los hilos
    for (int i = 0; i < nThreads; i++){
        printf("El hilo %d devolvió la siguiente información tras analizar el archivo \"%s\":\n\n",responseArray[i].pseudoId,responseArray[i].testFile);
        //Informacion de las palabras (numero de veces que apareció dicha en un archivo en particular)
        for (int j = 0; j < (sArrWord); j++){
            printf("[HILO %d] La palabra \"%s\" se encontró %d veces\n",responseArray[i].pseudoId,responseArray[i].wordReport[j].testWord,responseArray[i].wordReport[j].repetitionsNumber);
            //wDataReport[j].testWord = responseArray[i]->wordReport[j].testWord;
            // Actualización de suma del numero de veces que apareció la palabra en el archivo particular
            wDataReport[j].repetitionsNumber = wDataReport[j].repetitionsNumber + responseArray[i].wordReport[j].repetitionsNumber;
        }
        printf("----------------------------------------------------------------\n\n");
    }
    
    printf("\nNumero de palabras en los 10 archivos: %ld\n\nEstadística general\n\n",nWords);
    
    // Mostrar la estadistica general 
    char* w;
    int nrw;
    float percent;
    for (int i = 0; i < sArrWord; i++){
        w = wDataReport[i].testWord;
        nrw = wDataReport[i].repetitionsNumber;
        percent = ((float) nrw * (float) 100) / ( (float) nWords );// Regla de 3
        printf("Palabra: %s. Porcentaje de aparición: %f%%\n",w,percent);
    }
    
    // Liberar espacio en memoria dinamica
    free(thArray);
    free(wDataReport);
    free(responseArray);// Liberar respuesta generada en memoria dinamica desde la funcion del hilo

    return 0;
}

char* getLowerString(char* word){// Pasa todas las letras mayusculas que esten en una palabra a minusculas
    int size = strlen(word);
    char* lowerString = (char*)malloc(sizeof(char)*size);
    int i = 0;
    while (word[i] != '\0'){
        lowerString[i] = tolower(word[i]);
        i++;
    }
    
    lowerString[i] = '\0';

    return lowerString;
    
}

int sizeWordArray(){// Determinar tamaño que contiene el array words
    int i = 0;
    while(words[i] != NULL){
        i++;
    }

    return i;
}

long getWordsNumberTotal(char const *argv[],int argc){
    char fileName[100];
    char empty[100] = "";
    char ext[5] = ".txt";
    long nWords = 0;
    char anyWord[20];
    for (int i = 1; i < argc; i++){// Lectura de los 10 archivos
        strcpy(fileName,empty);
        //char* literalFileName = argv[i];
        strcpy(fileName,argv[i]);
        strcat(fileName,ext);
        char* tok;
        FILE* fp = fopen(fileName,"r");
        if(fp!=NULL){
            while (!feof(fp)){//lectura de archivo actual (mientras no sea el final)
                fscanf(fp,"%s",anyWord);//extraccion de palabra (recorro archivo)
                //nWords++;//dado que se extrae una palabra, hay una palabra mas
                tok = strtok(anyWord," (),.:;-_!@?¿¡|#$\0\"\'\t\n\v");
                while(tok != NULL){//mientras tok (cadena extraida de la palabra en minusculas) no sea nula 

                    if((tok[0]>= 'A' && tok[0]<= 'Z') || (tok[0]>= 'a' && tok[0]<= 'z')){
                        //printf("Word: %s\n",tok);
                        nWords++;
                    }
                    tok = strtok(NULL," (),.:;-_!@?¿¡|#$\0\"\'\t\n\v");//obten la siguiente palabra que pueda formarse con strtok utilizando la misma thWordF
                }
            }
            fclose(fp);
        }else{
            printf("No se pudo leer el archivo %s\n",fileName);
            exit(1);
        }
    }

    return nWords;
}