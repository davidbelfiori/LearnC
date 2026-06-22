#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_BUFFER_SIZE 4098
int main(int argc, char *argv[]) {

    int status;
    char buffer[MAX_BUFFER_SIZE];
    char *argExec[64];

    printf("Minishell, inserisci il tuo comando es ls -l:");
    if (fgets(buffer,sizeof(buffer),stdin) == NULL) return 0;
    buffer[strcspn(buffer,"\n")]='\0';

    char *token= strtok(buffer," ");

    int i= 0;
    while (token != NULL) {
        argExec[i]=token;
        i++;
        token = strtok(NULL," ");
    }
    argExec[i] = NULL;

    if (fork() == 0) {
        if (execvp(argExec[0],argExec)==-1) return 0;

    }else {
        wait(&status);

    }

    return 0;
}