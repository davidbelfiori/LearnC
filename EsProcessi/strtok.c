#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){

    char config[] = "USER=antonio;HOME=/usr/antonio;SHELL=bash;PORT=8080";

    char *argomenti[64];
    char *argomenti2[64];

    char *token = strtok(config,";");

    int i= 0;
    while (token!=NULL)
    {
        argomenti[i]=token;
        i++;
        token=strtok(NULL,";");
    }

    for (int j = 0; j<i; j++)
    {
        char *chiave = strtok(argomenti[j],"=");
         char *valore = strtok(NULL,"=");
        printf("Chiave %s -> valore %s \n",chiave,valore);
    }
    
    return 0;

}