#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char const *argv[]){
    FILE* fp = fopen("TestFile.txt","r");

    // Contar cuantas cadenas hay en un archivo
    if(fp !=NULL){
        char s[100];
        long n = 0;
        char *tok;
        while(!feof(fp)){
            fscanf(fp,"%s",s);
            //printf("Word: %s\n",s);
            tok = strtok(s," ()—,.:;-_!@?¿¡|#$\0\"\'\t\n\v");
            while(tok != NULL){//mientras tok (cadena extraida de la palabra en minusculas) no sea nula 

                if((tok[0]>= 'A' && tok[0]<= 'Z') || (tok[0]>= 'a' && tok[0]<= 'z')){
                    printf("Word: %s\n",tok);
                    n++;
                }
                tok = strtok(NULL," ()—,.:;-_!@?¿¡|#$\0\"\'\t\n\v");//obten la siguiente palabra que pueda formarse con strtok utilizando la misma thWordF
            }
        }

        fclose(fp);

        printf("n = %ld\n",n);
    }

    /*char *cad ="HoLa,";
    printf("Cadena de entrada: %s\n",cad);
    int i=0;
    char* cadLower = (char*)malloc(sizeof(char)*(strlen(cad)));
    while (cad[i]!='\0'){
        cadLower[i] = tolower(cad[i]);
        i++;
    }

    cadLower[i] = '\0';

    printf("Cadena final: %s\n",cadLower);*/
    
    return 0;
}
