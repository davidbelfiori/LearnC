/*Scrivere un programma che riceva in input tramite argv[] un insieme di n nomi
di file, con n maggiore o uguale ad 1. I nomi dei file per essere correttamente
gestiti dovranno essere tutti diversi tra di loro. Indicheremo tali nomi come
F1, ..., Fn.
Per ogni nome di file Fi dovra' essere attivato un thread, che indicheremo con Ti.
I thread attivati dovranno a turno leggere una stringa dallo standard input e ciascuno
di essi dovra' scriverla su una nuova linea in tutti i file F1, ..., Fn. La scrittura
dovra' avvenire n modo che le stringhe lette in input compaiano in tutti i file esattamente
nello stesso ordine di lettura. Tale attivita' di lettura di stringhe e scrittura sui
file dovra' andare avanti in modo indefinito.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI)
in modo tale che quando il processo venga colpito il suo main thread dovra'
verificare quante linee siano presenti in ciascun file e se tale numero sia pari o dispari.
Esso dovra' anche riportare tale informazione tramite un messaggio su standard ouput.

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


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *sem;
int turno = 0;
int numFiles ;


typedef struct {
    int id;
    int *fd;
}threadInfo;

int lineetotali = 0;
void messaggio(char *msg) {
    printf("Errore %s \n",msg);
    exit(EXIT_FAILURE);
}

void * work(void * arg) {

    threadInfo *info = (threadInfo *)arg;
    int id = info->id;
    char buffer[1024];
    int *fdTotali  = info->fd;

    while (1) {

        sem_wait(&sem[id]);
        pthread_mutex_lock(&mutex);

       if (fgets(buffer,sizeof(buffer),stdin) != NULL) {

           for (int i = 0;i<numFiles; i++) {
               write(fdTotali[i],buffer,strlen(buffer));
               //forzo l salvataggio
               fsync(fdTotali[i]);
               lineetotali ++;
           }

       } else {
           clearerr(stdin);
           sleep(1);
       }
        turno = (turno+1)%numFiles;

        pthread_mutex_unlock(&mutex);
        sem_post(&sem[turno]);

    }

    return NULL;

}

int main(int argc , char *argv[]) {

    if (argc <2)messaggio("Uso ./nome programma nomefile1 ... nomefileN");

    numFiles = argc -1;

    for (int i = 0;i<argc-1;i++) {

        for (int j=i+1;j<argc;j++) {
            if (strcmp(argv[i],argv[j])==0) {
                messaggio("I nomi dei file devono essere diversi");
            }
        }

    }

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);

    pthread_sigmask(SIG_BLOCK,&sig,NULL);

    sem = malloc((argc-1)*sizeof(sem_t));
    pthread_t *tid = malloc((argc-1)*sizeof(pthread_t));
    int *fd= malloc((argc-1)*sizeof(int));
    threadInfo *info = malloc((argc-1)*sizeof(threadInfo));
    if (tid==nullptr || sem == nullptr || fd ==nullptr) messaggio("errore nella malloc");



    for (int i = 0 ;i<argc-1;i++) {

        if (i==0) {
            sem_init(&sem[i],0,1);
        }else {
            sem_init(&sem[i],0,0);
        }

        fd[i] = open(argv[i+1],O_CREAT|O_RDWR|O_TRUNC,0666);
        if (fd[i] <0) messaggio("errore nell'apertra del file");
        info[i].id = i;
        info[i].fd = fd;
    }



    for (int i = 0; i<argc-1;i++) {

        if (pthread_create(&tid[i],NULL,work,&info[i])!=0) messaggio("errore nella crwazione del thread");

    }
    int sigricevuto;

    while (1) {

        sigwait(&sig,&sigricevuto);
        if (sigricevuto == SIGINT) {
            printf("SIGINT CATTURATO");
            for (int i =0 ; i<argc-1; i++) {
                printf("Il file %s ha %d linee  , numero %s \n",argv[i+1],lineetotali,(lineetotali % 2 == 0) ? "pari": "dispari");
            }
        }
    }

}
