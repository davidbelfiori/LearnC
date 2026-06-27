#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MESSAGEDATA 30

int main(int argc, char *argv[]) {

    char messaggio[MESSAGEDATA];

    int fd[2];

    if (pipe(fd) == -1) {
        printf("errore nella creazione della pipe");
        return  0;
    }

    pid_t figlio = fork();
    if (figlio < 0) {
        printf("errore nella cereazione del figlio");
        return  0;
    }else if (figlio == 0) {
        //FIGLIO
        close(fd[1]); //chiudo la scrittura
        while (read(fd[0],messaggio,sizeof(messaggio))>0) {
            printf("Dati dal padre: %s \n",messaggio);
        }

    }else {
        //Padre
        close(fd[0]); //chiudo la lettura
        do {
            fgets(messaggio,MESSAGEDATA,stdin);
            write(fd[1],messaggio,sizeof(messaggio));
        } while (strcmp(messaggio,"exit")!=0);

    }

    close(fd[0]);
    close(fd[1]);
    return  0;

}