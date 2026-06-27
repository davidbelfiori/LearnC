#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define TAGLIA 128


 typedef struct {
     int msgType;
     char msg[TAGLIA];
 }Messaggio;


void consumatore (int descrittore) {

    Messaggio rx;
    int ris = msgrcv(descrittore,&rx,sizeof(Messaggio),0,0);
    if (ris == -1) {
        printf("errore nella recive");
        exit(EXIT_FAILURE);
    }

    do {

        int ris = msgrcv(descrittore,&rx,sizeof(Messaggio),0,0);
        if (ris == -1) {
            printf("errore nella recive");
            exit(EXIT_FAILURE);
        }
        printf("Messaggio: %s",rx.msg);
        fflush(stdout);
    } while (strcmp(rx.msg,"exit")!=0);

}

void produttore(int descrittore) {
    Messaggio messaggio;
   printf("Inserire il messaggio da inviare");

    do {

        fgets(messaggio.msg,sizeof(messaggio.msg),stdin);
        messaggio.msgType = 1;
        int ris = msgsnd(descrittore,&messaggio,sizeof(messaggio),0);
        if (ris == -1) {
            printf("errore nella send");
            exit(EXIT_FAILURE);

        }
    } while (strcmp(messaggio.msg,"exit") != 0);
   exit(0);
}

int main(int argc, char *argv[]) {
    int fdCoda, status;
    long key = 40; //chiave da dare alla msgget

    fdCoda = msgget(key,IPC_CREAT | 0666);
    if (fdCoda < 0) {
        printf("errore nella creazione della message queque");
        return 0;
    }
    if (fork()!= 0) {

        if (fork()!=0) {
            wait(&status);
            wait(&status);
        }else {
            produttore(fdCoda);
        }
    }else {
        consumatore(fdCoda);
    }

}