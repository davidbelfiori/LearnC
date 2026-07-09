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

int N;
int contatore;
char *str;

void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}


void handler(int i) {

    printf("Sig chiamata da processo %d , str %s , contatore %d \n",getpid(),str,contatore);

}

int main(int argc , char *argv[]) {

    if (argc < 2) messaggio("Errore : uso ./nomeProgramma s1 s2 ... sN");

    N = argc -1;
    // mi creo una zona di memoria condivisa ai processi dove incrementeranno i loro contatori;

   int  connettori[N][2];

    //Creo le pipe
    for (int i= 0 ; i <N ; i++) {
       if (pipe(connettori[i])==-1) messaggio("Errore nella creazione delle pipe");
    }

    for (int i =0; i < N; i++) {
        //creo il procesos iesimo
        pid_t pid = fork();
        if (pid <0)messaggio("errore nella creazione del figlio");
        if (pid == 0) {
            str = argv[i+1];
            contatore = 0;
            for (int j = 0; j <N;j++) {
                //se sto sulla mia pipe chiudo la scrittura il padre o un altro processo chiudera la scrittura invece
                if (i == j) {
                    close(connettori[j][1]);
                }else if(j == i+1 && i < N-1) {
                    //se sono la pipe dopo e non sono l'ultima chiudo la lettura
                    close(connettori[j+1][0]);
                }else {
                    close(connettori[j][1]);
                    close(connettori[j][0]);
                }
            }
            //gestione dei segnali
            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags=SA_RESTART;
            sa.sa_handler = handler;
            if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("Errore sigaction");

            int fdLettura = connettori[i][0];
            int fdScrittura;
            if (i < N-1) {
                 fdScrittura = connettori[i+1][1];
            }else {
                 fdScrittura = -1;
            }

            char buff[1024];

            while (1) {
                ssize_t byteRead = read(fdLettura,buff,sizeof(buff));
                //se leggo 0 è terminato ed esco
                if (byteRead<=0) break;
                //se la stringa che ho letto è uguale a quella che ho in carico aumento il contatore
                if (strcmp(str,buff)==0) {
                    contatore++;
                }else {
                    //ammeno che non sono l'ultimo la passo al prossimo
                    if (fdScrittura != -1) {
                        write(fdScrittura,buff,sizeof(buff));
                    }
                }

            }

            close(fdLettura);
            if (fdScrittura != -1) close(fdScrittura);
            exit(EXIT_SUCCESS);
        }
    }
    signal(SIGINT,SIG_IGN);
        //PADRE
        for (int j = 0; j <N ; j++) {
            //chiudo tutte le letture
            close(connettori[j][0]);
            if (j!=0) {
                close(connettori[j][1]);
            }
        }
    int fdPipe = connettori[0][1];
        char buff[1024];
    //prendo un blocco e quando incontro \n ci metto \0  e la scrivo sul p1
        while (fgets(buff,sizeof(buff),stdin)!=NULL) {
            buff[strcspn(buff,"\n")]='\0';
            write(fdPipe,buff,sizeof(buff));
        }


    //PADRE
    for (int j = 0; j <N ; j++) {
        //chiudo tutte le letture
        wait(NULL);
    }

    return 0;

}
