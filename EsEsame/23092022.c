/*Implementare una programma che riceva in input, tramite argv[],
N differenti stringhe S1 ... SN, con N maggiore o uguale a 1.
Per ognuna delle stringhe dovra' essere attivato un nuovo thread per gestirla
(indichiamo quindi con T1 ... TN i thread che dovranno essere attivati).
Il main thread  dovra' leggere stringhe dallo standard input, e dovra'
rendere disponibile ogni stringa letta a T1. T1 dovra' verificare se la stringa ricevuta
e' uguale alla stringa S1 da lui gestita, e dovra' incrementare un contatore
in caso positivo. Altrimenti, in caso negativo, dovra' rendere la stringa ricevuta dal
main disponibile al thread T2 che fara' lo stesso controllo, e cosi' via fino a TN.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando sia colpita
essa dovra' riportare su standard output il valore dei contatori che indicano
quante volte le stringhe S1 ... SN sono state trovate uguali alla stringhe che
il main thread aveva letto da standard input.

In caso non vi sia immissione di dati sullo standard input, e non vi siano segnalazioni,
l'applicazione dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.*/

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

#define MAX_LEN 1024

void messaggio(char *m) {
    printf("Errore: %s \n",m);
    exit(EXIT_FAILURE);
}

typedef struct {
    int id;
    char *stringa;
    int fdIngresso;
    int fdUscita;
    int contatore;
}threadInfo;

// Struttura fissa per comunicare in modo sicuro tramite pipe senza frammentazione
typedef struct {
    char testo[MAX_LEN];
} MessaggioPipe;

threadInfo *info;
int numThreadGlobal;


void * work(void * p) {

    threadInfo *infoMie = (threadInfo *)p;
    MessaggioPipe msg ;

    while (read(infoMie->fdIngresso,&msg,sizeof(MessaggioPipe))== sizeof(MessaggioPipe)) {

        if (strcmp(infoMie->stringa,msg.testo)==0) {

            infoMie->contatore++;
        }else {
            if (infoMie->fdUscita != -1) {
                write(infoMie->fdUscita,&msg,sizeof(MessaggioPipe));
            }

        }

    }

    return NULL;

}

void * gestoreSegnali(void * p) {
    sigset_t *sig = (sigset_t *)p;
    int segnale ;

    while (1) {
        sigwait(sig,&segnale);

        if (segnale == SIGINT) {

            printf("catturata \n");

            for (int i= 0; i<numThreadGlobal; i++) {
                printf("Stringa bersaglio %s , occorrenze %d \n",info[i].stringa,info[i].contatore);
            }

            printf("____FINE____ \n");
        }
    }
}

    int main(int argc, char *argv[]) {

        if (argc <2)messaggio("Uso ./nomeProgramma stringa1 ... stringaN-1");

        int numThread = argc-1;
        numThreadGlobal = numThread;

        int connettori[numThread][2];

        info = malloc(numThread*sizeof(threadInfo));
        pthread_t *tid = malloc(numThread*sizeof(pthread_t));
        for (int i = 0 ; i < numThread;i++) {
            if (pipe(connettori[i])==-1)messaggio("errore nella creazione della pipe");
        }

        sigset_t sig;
        sigemptyset(&sig);
        sigaddset(&sig,SIGINT);

        pthread_sigmask(SIG_BLOCK,&sig,NULL);

        pthread_t gestore;
        pthread_create(&gestore,NULL,gestoreSegnali,&sig);

        for (int i = 0; i<numThread;i++) {

            info[i].id = i+1;
            info[i].stringa = argv[i+1];
            info[i].fdIngresso = connettori[i][0];
            // Ogni thread scrive nella pipe del thread successivo,
            // l'ultimo thread imposta fdUscita a -1 per indicare la fine.
            info[i].fdUscita = (i < numThread - 1) ? connettori[i + 1][1] : -1;
            info[i].contatore = 0;

            pthread_create(&tid[i], NULL, work, &info[i]);
        }

        MessaggioPipe msg;

        while (fgets(msg.testo,MAX_LEN,stdin)!=NULL) {

            msg.testo[strcspn(msg.testo,"\n")]='\0';

            write(connettori[0][1],&msg,sizeof(MessaggioPipe));

        }


        return 0;
    }

