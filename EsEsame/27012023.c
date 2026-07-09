/*SPECIFICATION TO BE IMPLEMENTED:
Scrivere un programma che riceva in input tramite argv[1] e argv[2] seguenti
due parametri:
- in argv[1] la taglia del blocco B
- in argv[2] il numero di thread N.
L'applicazione dovra' quindi generare N tread, che indicheremo con T1 ... TN.
Ciascuno di questi thread dovra', secondo una turnazione circolare, acquisire
un blocco di B bytes dallo standard input e dovra' scriverli in un file il cui
nome dovra' essere "output_<THREAD_ID>".
Questo file dovra' essere creato dallo stesso thread in carico di gestirlo, in
particolare al suo stesso startup.
Il main thread, dopo aver creato gli N thread che effettueranno le operazioni sopra
indicate, rimarra' in pausa.

L'applicazione dovra' gestire il segnale  SIGINT (o CTRL_C_EVENT nel caso WinAPI)
in modo tale che quando il processo venga colpito il suo main thread dovra'
riportare su standard output il contenuto dei file aggiornati dagli N thread
in modo che sia ricostruita esattamente la stessa sequenza di bytes originariamente
acquisita tramite standard input.

In caso non vi sia immissione di dati sullo standard input e non vi siano segnalazioni,
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


int N,B;
sem_t *turno;

typedef struct {
    pthread_t tid;
    int id;
}tInfo;

void messaggio(char *p) {
    printf("Errore %s \n",p);
    exit(EXIT_FAILURE);
}

void * work(void * p) {
    tInfo *info = (tInfo *)p;
    pthread_t tid = info -> tid;
    int id = info->id;

    char nomeFile[512];
    snprintf(nomeFile,sizeof(nomeFile),"output_%lu.txt",tid);
    int fd = open(nomeFile,O_CREAT|O_TRUNC|O_WRONLY,0666);
    if (fd <0 ) messaggio("errore nell'apertura del file del figlio");

    char buff[B];
    while (1) {
        //aspetto che sia il mio turno
        sem_wait(&turno[id]);
        printf("sono il thread %d: \n",id);
        ssize_t byteRead;
        if ((byteRead = read(0,buff,sizeof(buff)))<=0) {
            close(fd);
            sem_post(&turno[(id+1)%N]);
            return NULL;
        }
        write(fd,buff, byteRead);
        sem_post(&turno[(id+1)%N]);
    }

}

int main (int argc, char *argv[]) {

    if (argc <3) messaggio("Uso ./nomeProgramma numeroThread numeroByte");

    N = atoi(argv[2]);
    B = atoi(argv[1]);


    tInfo *info= malloc(N*sizeof(tInfo));
    turno = malloc(N*sizeof(sem_t));

    if (info == NULL || turno == NULL) messaggio("Errore malloc");

    for (int i = 0 ; i<N ; i++) {
        info[i].id = i;
        if (i == 0) {
            if (sem_init(&turno[i],0,1) == -1) messaggio("Errore creazione semaforo");
        }else {
            if (sem_init(&turno[i],0,0) == -1) messaggio("Errore creazione semaforo");
        }
    }

    sigset_t sig ;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);


    for (int i = 0; i <N ; i++) {

        if (pthread_create(&info[i].tid,NULL,work,&info[i])!= 0) messaggio("Errore creazione thread ");

    }

    int signal;
    while (1) {
        sigwait(&sig,&signal);
        if (signal == SIGINT) {
            printf("----Catturato----\n");
            int fd[N];
            for (int i= 0; i < N; i++) {
                char nomeFile [512];
                snprintf(nomeFile,sizeof(nomeFile),"output_%lu.txt",info[i].tid);
                 fd[i] = open(nomeFile,O_RDONLY);
                if (fd[i] <0) messaggio("errore nell'aperura del file");
            }
            int turno =0;
            char buff[B];
            while (1) {

                ssize_t byteRead = read(fd[turno],buff,B);
                if (byteRead < 0) {
                    messaggio("errore in lettura");
                }
                if (byteRead == 0 ) {
                    break;
                }
                write(1,buff,strlen(buff));

                if (byteRead < B) {
                    break;
                }

                turno =( turno + 1)%N;


            }

            for (int i = 0 ; i<N ; i++) {
                close(fd[i]);
            }
            printf("FINE \n");

        }else {
            return 0;
        }

    }
}
