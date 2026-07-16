/*
SPECIFICATION TO BE IMPLEMENTED:

Scrivere un programma che riceva in input tramite argv[] i seguenti tre parametri:
1) numthreads in argv[1] (valore intero N > 0)
2) chunksize in argv[2] (valore intero K > 0)
3) filename in argv[3]

Il programma deve creare (o troncare se gia' esistente) il file filename.
Il programma deve poi attivare numthreads thread.

Il thread principale (main) deve leggere indefinitamente linee provenienti dallo
standard input e distribuirle ai thread in modo Round-Robin (la prima linea al
Thread 0, la seconda al Thread 1, la N-esima al Thread N-1, la (N+1)-esima di
nuovo al Thread 0, e cosi' via). Ciascun thread deve memorizzare le linee a lui
assegnate all'interno di un proprio buffer locale.

Quando un thread ha accumulato esattamente chunksize (K) linee nel proprio buffer
locale, esso deve interrompere temporaneamente la ricezione di nuove linee e
procedere alla scrittura di tali K linee all'interno del file filename.

I thread devono eseguire la scrittura sul file rispettando rigidamente l'ordine
inverso dei loro ID. Il blocco di K linee nel file deve quindi essere scritto dal
Thread N-1, seguito dal Thread N-2, fino al Thread 0. Nessun thread puo' scrivere
il proprio blocco se il thread con ID superiore non ha completato la propria
scrittura per quel ciclo.

L'applicazione dovra' gestire il segnale SIGINT (Ctrl+C) in modo tale che quando
il processo venga colpito esso dovra' interrompere l'attivita' di ricezione e
scrittura, accedere al file filename, e ripresentare su standard output i blocchi
di linee registrati escludendo tutte le linee che contengono il carattere di
spazio (' '), mantenendo l'ordine di scrittura originario del file. Al termine
di tale attivita' il programma dovra' terminare in modo pulito.

In caso non vi sia immissione di dati sullo standard input e non vi siano
segnalazioni, l'applicazione dovra' utilizzare l'attesa passiva tramite primitive
di sincronizzazione, garantendo un utilizzo non superiore al 10% della capacita'
di lavoro della CPU.
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
#define MAX_LINE 1024

volatile sig_atomic_t sig = 0;
int numThreads;
int K;
char *nomeFile;
sem_t *datoPronto;
sem_t *datoLetto;
sem_t *turnoFile;
char buffer[MAX_LINE];
int fd;


void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}

void handler(int s) {
    sig = 1;
}

void * work(void * p) {
    int id = (int)p;
    int occupate = 0 ;
    char    localBuffer[K][MAX_LINE];

    while (1) {
        sem_wait(&datoPronto[id]);
        strcpy(localBuffer[occupate],buffer);
        occupate++;
        sem_post(&datoLetto[id]);
        if (occupate == K) {
            sem_wait(&turnoFile[id]);
            printf("---SCRIVO SUL FILE---- \n");
            for (int i = 0 ; i < K ; i++) {
                write(fd,localBuffer[i],strlen(localBuffer[i]));
            }

            if (id> 0) {
                sem_post(&turnoFile[id-1]);
            }else {
                sem_post(&turnoFile[numThreads -1]);
            }
            occupate = 0;
        }

    }


}

int main(int argc, char *argv[]) {
    if (argc < 4) messaggio("Errore d'uso");
    numThreads = atoi(argv[1]);
    K = atoi(argv[2]);
    nomeFile = argv[3];

    datoPronto = malloc(numThreads * sizeof(sem_t));
    datoLetto = malloc(numThreads * sizeof(sem_t));
    turnoFile = malloc(numThreads * sizeof(sem_t));


    for (int i = 0; i < numThreads; i++) {
        sem_init(&datoPronto[i],0,0);
        sem_init(&datoLetto[i],0,1);
        if (i==numThreads-1) {
            sem_init(&turnoFile[i],0,1);
        }else {
            sem_init(&turnoFile[i],0,0);
        }
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("Errore sigaction");

    pthread_t *tid = malloc(numThreads*sizeof(pthread_t));

    for (int i = 0; i < numThreads; i++) {
        if (pthread_create(&tid[i],NULL,work,(void *)i)!=0)messaggio("errore");
    }

    int turno = 0;
   fd = open(nomeFile,O_CREAT|O_APPEND|O_WRONLY,0666);

    while (1) {

        if (sig == 1) {
            printf("CATTURATO ---- \n");
            FILE *f = fopen(nomeFile,"r+");
            char buff[MAX_LINE];
            while (fgets(buff,MAX_LINE,f) != NULL) {
                if (strchr(buff,' ')==NULL) {
                    printf("%s \n",buff);
                }
            }
            fclose(f);

            sig = 0;
        }else {
            if (fgets(buffer,MAX_LINE,stdin)==NULL) {
                if (errno == EINTR) {
                    sem_post(&datoPronto[turno]);
                    continue;
                }else {
                    break;
                }
            }
            sem_post(&datoPronto[turno]);
            sem_wait(&datoLetto[turno]);

            turno =( turno +1)%numThreads;
        }

    }


}
