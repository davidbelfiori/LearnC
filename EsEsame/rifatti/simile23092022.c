/*
 * -----------------------------------------------------------------------------
 * PROVA D'ESAME DI SISTEMI OPERATIVI
 * -----------------------------------------------------------------------------
 *
 * Scrivere un programma C in ambiente POSIX che riceva in input, tramite argv[],
 * una sequenza di N valori interi positivi K1, K2, ..., KN (con N maggiore o
 * uguale a 1).
 *
 * Per ognuno dei valori forniti in input dovrà essere attivato un nuovo processo
 * figlio (indichiamo con P1, P2, ..., PN i processi che dovranno essere attivati).
 * Ciascun processo figlio Pi sarà associato al rispettivo coefficiente Ki.
 *
 * Il processo originale (padre) dovrà leggere continuamente numeri interi (uno
 * per riga) dallo standard input. All'atto della lettura di un intero X, il padre
 * dovrà inoltrare tale valore al primo processo della catena, P1.
 *
 * Ciascun processo Pi (con i da 1 a N-1), una volta ricevuto un intero dal
 * canale di comunicazione con il processo precedente, dovrà:
 * 1. Moltiplicare il valore ricevuto per il proprio coefficiente Ki
 * (calcolando quindi Y = X * Ki).
 * 2. Inviare il risultato Y al processo successivo Pi+1.
 *
 * L'ultimo processo della catena, PN, dopo aver applicato la propria moltiplicazione
 * (calcolando Y = X * KN), dovrà visualizzare il valore finale risultante sullo
 * standard output, formattato su una singola riga.
 *
 * L'applicazione dovrà gestire il segnale SIGINT (Ctrl+C da terminale) in modo
 * tale che, quando un qualsiasi processo figlio Pi venga colpito dal segnale,
 * esso riporti sullo standard output un messaggio contenente il proprio PID e
 * il proprio coefficiente di moltiplicazione Ki. Il processo originale (padre)
 * non dovrà invece eseguire alcuna attività all'arrivo di tale segnalazione.
 *
 * All'atto della chiusura dello standard input (EOF), l'intera catena di processi
 * dovrà terminare in modo pulito: ciascun processo dovrà chiudere i propri canali
 * di comunicazione e terminare volontariamente. Il processo padre dovrà attendere
 * la corretta terminazione di tutti i figli (wait) prima di uscire.
 *
 * In caso non vi sia immissione di dati sullo standard input e non vi siano
 * segnalazioni, l'applicazione dovrà utilizzare non più del 5% della capacità
 * di lavoro della CPU (garantendo l'assenza di busy waiting).
 *
 * -----------------------------------------------------------------------------
 */

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

int K;

void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}

void handler(int i) {
    printf("pid:%d , coefficente:%d \n",getpid(),K);
}

int main (int argc , char **argv) {

    if (argc < 2) messaggio("errore d'uso");

    int N = argc -1;
    int tubi[N][2];

    for (int i = 0 ; i < N ; i++) {
        if (pipe(tubi[i])==-1)messaggio("errore pipe");
    }

    //creazione figli
    for (int i = 0; i < N ; i++) {

        pid_t pid = fork();
        if (pid < 0) messaggio("errore fork");
        if (pid ==0) {
          K = atoi(argv[i+1]);

          int  fdLettura= tubi[i][0];
          int fdScrittura ;

            if (i < N-1) {
                fdScrittura= tubi[i+1][1];
            }else {
                fdScrittura = 1;
            }

            for (int j = 0; j<N; j++) {
                if (tubi[j][0] != fdLettura) {
                    close(tubi[j][0]);
                }
                if (tubi[j][1] != fdScrittura) {
                    close(tubi[j][1]);
                }
            }

            struct sigaction sa;
            sa.sa_handler = handler;
            sa.sa_flags=SA_RESTART;
            sigemptyset(&sa.sa_mask);
            if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore sig action ");
            int val =0;
            while (1) {
                ssize_t  byteRead = read(fdLettura,&val,sizeof(int));
                if (byteRead <= 0)break;
                val*=K;
                if (i < N-1) {
                    write(fdScrittura,&val,sizeof(int));
                }else {
                    printf("%d \n",val);
                }
            }

            close(fdScrittura);
            close(fdLettura);
            exit(EXIT_SUCCESS);
        }
    }
    signal(SIGINT,SIG_IGN);
    int fdScrittura = tubi[0][1];
    for (int i = 0 ; i < N; i++) {
        if (tubi[i][1]!=fdScrittura) {
            close(tubi[i][1]);
            close(tubi[i][0]);
        }
        close(tubi[i][0]);
    }

    char buff[1024];
    while (fgets(buff,1024,stdin)!=NULL) {
        int val= atoi(buff);
        write(fdScrittura,&val,sizeof(int));
    }

    for (int i = 0; i < N ; i++) {
        wait(NULL);
    }
    close(fdScrittura);
    return 0;
}