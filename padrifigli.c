#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define FIGLI 10;

int main() {

    for (int i = 0; i<FIGLI) {
        pid_t figlio = fork();
        int Status;
        if (figlio >0) {
            printf("Creazione figlio %d-esimo con pid: %d \n",i,figlio);
            wait(&Status);
        }
        else {
            printf("ciao sono figlio con pid %d \n", getpid());
            exit(1);
        }
        // if (figlio == 0) {
        //     printf("ciao sono figlio con pid %d \n", getpid());
        //     break;
        // }
        i++;
    }

}