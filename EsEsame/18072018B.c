/*Implementare un programma che riceva in input tramite argv[] i pathname
associati ad N file, con N maggiore o uguale ad 1. Per ognuno di questi
file generi un thread (quindi in totale saranno generati N nuovi thread
concorrenti).
Successivamente il main-thread acquisira' stringhe da standard input in
un ciclo indefinito, ed ognuno degli N thread figli dovra' scrivere ogni
stringa acquisita dal main-thread nel file ad esso associato.
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando uno qualsiasi dei thread dell'applicazione
venga colpito da esso dovra' stampare a terminale tutte le stringhe gia'
immesse da standard-input e memorizzate nei file destinazione.*/


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


typedef struct {
    pthread_t tid;
    int id;
    char *nomeFile;
}ThreadInfo;



char shdm[4096];
sem_t *scrittura;
sem_t *lettura;
int N;

char **nomeFile;
void messaggio(char *p) {

    perror(p);
    exit(EXIT_FAILURE);

}

void *work(void *p) {

    ThreadInfo *info = (ThreadInfo *)p;
    int fd = open(info->nomeFile,O_CREAT|O_WRONLY|O_TRUNC,0666);
    if (fd < 0) messaggio("errore");
    char  buff[4096];
    while (1) {
        sem_wait(&scrittura[info->id]);
        strncpy(buff,shdm,sizeof(buff)-1);
        write(fd,buff,strlen(buff));
        sem_post(&lettura[info->id]);
    }
    return NULL;

}

void handler(int sig) {

    printf("catturato \n");

    for (int i = 0 ; i<N; i++ ) {

        printf("STAMPO FILE %s \n",nomeFile[i]);
        int fd = open(nomeFile[i],O_RDONLY);
        if (fd< 0) messaggio("errore");
        ssize_t byteLetti =0;
        char buff[4096];
        while ((byteLetti = read(fd,buff,sizeof(buff)))>0) {
            write(1,buff,byteLetti);
        }
        printf("------------FINE----------- \n");
    }

}

int main (int argc, char *argv[]) {

    if (argc < 2) messaggio("Errore");

    struct sigaction saFiglio;
    saFiglio.sa_handler = handler;
    sigemptyset(&saFiglio.sa_mask);
    saFiglio.sa_flags= SA_RESTART;
    if (sigaction(SIGINT,&saFiglio,NULL)==-1)messaggio("Errore sigaction");

    N = argc-1;

    nomeFile = malloc(N *sizeof(char *));
    scrittura = malloc(N *sizeof(sem_t));
    lettura = malloc(N *sizeof(sem_t));

    for (int i = 0; i < N ; i++) {

        nomeFile[i] = argv[i+1];
        if (sem_init(&scrittura[i],0,0) == -1) messaggio("errore");
        if (sem_init(&lettura[i],0,1) == -1) messaggio("errore");
    }

    ThreadInfo *info  = malloc(N*sizeof(ThreadInfo));


    for (int i = 0; i < N ; i++) {
        info[i].id = i;
        info[i].nomeFile = argv[i+1];
        if (pthread_create(&info[i].tid,NULL,work,&info[i])!=0) messaggio("errore");
    }

    while (1) {

        for (int i = 0 ; i < N ; i++) {
            sem_wait(&lettura[i]);
        }

        if (fgets(shdm,sizeof(shdm),stdin)==NULL) {
            for (int i = 0 ; i < N ; i++) {
                sem_post(&scrittura[i]);
            }
            break;
        }
        for (int i = 0 ; i < N ; i++) {
            sem_post(&scrittura[i]);
        }
    }

    return 0;
}
