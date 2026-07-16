/*Implementare una programma che riceva in input, tramite argv[],
N differenti stringhe S1 ... SN, con N maggiore o uguale a 1.
Per ognuna delle stringhe dovra' essere attivato un nuovo processo
che gestira' tale stringa  (indichiamo quindi con P1 ... PN i
processi che dovranno essere attivati).
Il processo originale dovra' leggere stringhe dallo standard input, e dovra'
comunicare ogni stringa letta a P1. P1 dovra' verificare se la stringa ricevuta
e' uguale alla stringa S1 da lui gestita, e dovra' incrementare un contatore
in caso positivo. Altrimenti, in caso negativo, dovra' comunicare la stringa
ricevuta al processo P2 che fara' lo stesso controllo, e cosi' via fino a PN.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando uno qualsiasi dei processi Pi venga colpito
esso dovra' riportare su standard output il valore del contatore che indica
quante volte la stringa Si e' stata trovata uguale alla stringa che
il processo originale aveva letto da standard input. Il processo originale
non dovra' invece eseguire alcuna attivita' all'arrivo della segnalazione.

In caso non vi sia immissione di dati sullo standard input, e non vi siano
segnalazioni, l'applicazione dovra' utilizzare non piu' del 5% della capacita'
di lavoro della CPU*/

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define MAXLINE 1024


int N;
char *str;
int count ;


void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}

void handler(int i) {

    printf("stringa %s , occorrenze %d \n",str,count);

}

/* viene richiesto che per ogni stringa si venga generato un processo , ogni processo figlio dovra avere
    * due descrittori uno per la laettura dal padre o dal fratello precedente e uno scritto verso il fratello o -1 se l'ultimo
    * se i == 0 ovvero la prima pipe condivisa main figlio , il figlio chiude la sua scrittua e chiude tutte le altre tranne i+1 ,
    * il figlio chiude le letture tranne se i==0
    * se il figlio e N-1 , chiude tutte le scritture e tiene aperta in lettura solo la pipe N 0
    *
    * */

int main(int argc, char **argv) {
    if (argc < 2) messaggio("errore uso");
    N = argc -1;
    int tubi[N][2];

    for (int i = 0; i < N ; i++) {
        if (pipe(tubi[i])==-1)messaggio("errore creazione pipe");
    }


    for (int i = 0 ; i < N ; i++) {

        pid_t pid= fork();
        if (pid < 0)messaggio("errore generazioen figlio");
        if (pid == 0) {

            str = argv[i+1];
            count = 0;
            int fdLettura = tubi[i][0];
            int fdScrittura;
            if (i < N -1){
                fdScrittura = tubi[i+1][1];

            }else {
                fdScrittura = -1;
            }
            for (int j = 0 ; j < N ; j++) {
                if (tubi[j][0]!=fdLettura) {
                    close(tubi[j][0]);
                }
                if (tubi[j][1]!=fdScrittura) {
                    close(tubi[j][1]);
                }
            }

            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags=SA_RESTART;
            sa.sa_handler = handler;
            if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("Errore sigaction");


            char buffer[MAXLINE];

            while (1) {

               ssize_t byteRead = read(fdLettura,buffer,MAXLINE);
                if (byteRead <= 0) break;

                if (strcmp(buffer,str)==0) {count++;}
                else{ write(fdScrittura,buffer,MAXLINE);}
            }

            close(fdLettura);
            if (fdScrittura !=-1) close(fdScrittura);
            exit(EXIT_SUCCESS);

        }

    }

    for (int i = 0 ; i < N ; i++) {
        if (i == 0) {
            close(tubi[i][0]);
        }else {
            close(tubi[i][0]);
            close(tubi[i][1]);
        }
    }

    int fdScrittura = tubi[0][1];
    signal(SIGINT,SIG_IGN);
    char buff[MAXLINE];
    while (fgets(buff,MAXLINE,stdin)!=NULL){
        buff[strcspn(buff,"\n")]='\0';
        write(fdScrittura,buff,strlen(buff)+1);
        printf("ok \n");
    }

    close(fdScrittura);
    for (int i = 0 ; i < N ; i++) {
        wait(NULL);
    }

    return 0;
}