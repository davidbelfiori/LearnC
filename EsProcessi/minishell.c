#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main(int argc, char *argv[]){

    char buffer[MAX_LINE];
    char *arg[MAX_ARGS];

    while (1)
    {
        printf("Mini shell (scrivi exit per terminare)>");
        if(fgets(buffer,sizeof(buffer),stdin) == NULL){
            printf("errore nella fgets");
            exit(EXIT_FAILURE);
        }
        buffer[strcspn(buffer,"\n")]='\0'; // Perchè perchè fgets alla fine si porta \n es tu scrivi ln -l, fget es prende ls -l\n\0 , 
                                            //facendo strtok prende {ln,-l\n} infatti fallisce
        if(strcmp(buffer,"exit")==0){
            break;
        }
        

        char *token = strtok(buffer," ");
        int i = 0;
        while (token!=NULL)
        {
             arg[i]= token;
            i++;
            token = strtok(NULL," ");
        }
        arg[i]=NULL;

    


        pid_t figlio = fork();

        if (figlio == -1)
        {
            printf("errore nella fork");
            exit(EXIT_FAILURE);
        }
        if(figlio == 0){
           if(execvp(arg[0],arg) == -1){
            printf("Errore nella exec");
            exit(EXIT_FAILURE);
           }

        }else{
            wait(NULL);
        }
        
        

    
    }
    return 0;

}