#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    char buffer[100];
    int status;
    bool cond = true;
    while (cond) {
        printf("inserisci una stringa:");
        scanf("%s",buffer);
        if (strcmp(buffer,"exit") == 0) {
            printf("chiesta una exit, il programma termina , addios \n");
            cond = false;
            break;
        }
        pid_t pid = fork();
        if (pid>0) {
            wait(&status);
            if (status>>8 == 10) {
                printf("errore nella exec, il progrmma termina");
                exit(0);
            }
            printf("Figlio ha completato \n");

        }else {
            if (execlp(buffer,buffer,NULL)==-1) exit(10);
        }
    }
}