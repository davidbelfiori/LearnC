/*SPECIFICATION TO BE IMPLEMENTED:
Scrivere un programma che riceva in input tramite argv[] il nome di un file F
e un numero N.
L'applicazione dovra' creare il file F, e dovra' generare N thread, che indicheremo con
T1, ..., TN.
A turno a partire da T1, e seguendo uno schema basato su circolarita', ogni thread dovra'
leggere una stringa dallo standard-input. Quando tutti gli N thread avranno letto una stringa
ciascuno, tali sringhe dovranno essere scritte ciascuna su una linea del file F,
partendo da quella letta da TN, poi da TN-1 e cosi' via fino a T1.
Tale attivita' di lettura di stringhe dallo standard input e scrittura sul file F andra'
avanti in nodo indefinito.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI)
in modo tale che quando il processo venga colpito si riportino le stringhe presenti
nel file F su standard-output, una per linea, ripresentando esattamente la sequenza
di stringhe originariamente lette da standard-input.

In caso non vi sia immissione di dati sullo standard input e non vi siano segnalazioni,
l'applicazione dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.

*****************************************************************/
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


int N ;
char *nomeFile;
int fdSorg;
sem_t *turnoLettura;
sem_t *turnoScrittura;

#define MAXLINE 4096

typedef struct {
    int id;
    char buffer[MAXLINE];
}ThreadInfo;

void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}

void * work(void * p) {

    ThreadInfo *info = (ThreadInfo *)p;
    int id = info->id;

    while (1) {

        sem_wait(&turnoLettura[id]);
        printf("t%d legge > \n",id);
        if (fgets(info->buffer,MAXLINE,stdin)==NULL) {
            sem_post(&turnoLettura[(id+1)%N]);
            messaggio("Errore scrittura");
            break;
        }

        if (id < N-1) {
            sem_post(&turnoLettura[(id+1)%N]);
        }else {
            sem_post(&turnoScrittura[N-1]);
        }


        sem_wait(&turnoScrittura[id]);
        printf("T%d scrive \n",id);
        write(fdSorg,info->buffer,strlen(info->buffer));
        if (id > 0) {
            sem_post(&turnoScrittura[id-1]);
        }else {
            sem_post(&turnoLettura[0]);
        }

    }
    printf("an errror was occurred bye \n");
    return NULL;

}

int main(int argc , char *argv[]) {

    if (argc < 3) messaggio("./nomeProg nomeFile N \n");

    nomeFile = argv[1];
    N = atoi(argv[2]);

    turnoLettura = malloc(N*sizeof(sem_t));
    turnoScrittura = malloc(N*sizeof(sem_t));

    for (int i = 0;i < N ; i++) {

        if (i == 0) {
            sem_init(&turnoLettura[i],0,1);
        }else {
            sem_init(&turnoLettura[i],0,0);
        }

    }

    for (int i = 0;i < N ; i++) {
        if (i == N-1) {
            sem_init(&turnoScrittura[i],0,1);
        }else {
            sem_init(&turnoScrittura[i],0,0);
        }

    }

    fdSorg = open(nomeFile, O_WRONLY|O_CREAT|O_TRUNC,0666);
    if (fdSorg < 0) messaggio("errore apertura file");

    ThreadInfo *info = malloc(N *sizeof(ThreadInfo));
    pthread_t *tid = malloc(N*sizeof(pthread_t));
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);


    //CREAZIONE THREAD;
    for (int i = 0 ; i < N ; i++) {
        info[i].id = i;
        if (pthread_create(&tid[i],NULL,work,&info[i])!=0) messaggio("errore creazione thread \n");
    }

    //CREAZIONE THREAD;
    for (int i = 0 ; i < N ; i++) {
        pthread_detach(tid[i]);

    }


    char localBuff[N][MAXLINE];
    while (1) {

        int segnale ;
        sigwait(&sig,&segnale);
        if (segnale == SIGINT) {
            printf("CATTURATO \n ");
            FILE *f = fopen(nomeFile,"r");
            while (1) {
                int linee = 0;
                for (int i = 0; i < N; i++) {
                    if (fgets(localBuff[i],MAXLINE,f)!=NULL) {
                        linee++;
                    }else {
                        break;
                    }
                }

                if (linee == 0) break;

                for (int i = linee-1; i >=0; i--) {
                    printf("%s \n",localBuff[i]);
                }

            }
            fclose(f);
        }

    }


}