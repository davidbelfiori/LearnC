/*Scrivere un programma che riceva in input tramite argv[] i nomi di N file,
con N magiore o uguale a 1. L'applicazione dovrà controllare che i nomi dei
file siano diversi tra di loro.
I file dovranno essere creati oppure troncati se esistenti.
Per ogniuno dei file dovra' essere attivato un nuovo thread, che indicheremo
con Ti, che gestirà il contenuto del file.
I thread Ti leggeranno linee di caratteri da standard input a turno secondo
uno schema circolare, e scriveranno la linea letta all'interno del file
da loro gestito.

L'applicazione dovrà essere in grado di gestire il segnale SIGINT
(o CTRL_C_EVENT nel caso WinAPI) in modo tale che quando il processo
verrà colpito riporti su standard output il contenuto corrente (ovvero le linee
attualmente presenti) di tutti i file che erano stati specificati in argv[],
seguendo esattamente l'ordine tramite cui le linee sono state inserite al loro interno.
In ogni caso, nessuno dei thread dovrà terminare la sua esecuzione in caso di arrivo
della segnalazione.

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
#include <wait.h>

sem_t *sem;
int N;
void messaggio(char *p) {
    printf("Errore %s \n",p);
    exit(EXIT_FAILURE);
}

typedef struct {

    int id;
    int fd;

}threadInfo;

void * work(void * p) {
    threadInfo *info = (threadInfo *)p;
    int id = info->id;
    int fd = info -> fd;

    char buff[1024];
    while (1) {
        sem_wait(&sem[id]);
        ssize_t byteRead;
        printf("t%d \n",id);
        byteRead = read(0,buff,sizeof(buff));
        if (byteRead == 0) {
            sem_post(&sem[(id+1)%N]);
            return NULL;
        }
        write(fd,buff,byteRead);
        sem_post(&sem[(id+1)%N]);
    }

}

int main (int argc , char *argv[]) {

    if (argc < 2) messaggio("uso: ./nomeProgramma file1 file2 ... fileN");
     N = argc -1;

    for (int i = 1;i < argc; i++) {
        for (int j = i+1 ; j < argc ; j++ ) {
            if (strcmp(argv[i],argv[j])==0) messaggio("Errore i nomi dei file devono essere diversi");
        }
    }


    int *fd = malloc(N*sizeof(int));
    sem = malloc(N*sizeof(sem_t));
    printf("---Inizio la creazione dei semafori e dei file ----- \n");
    for (int i = 0 ; i < N; i++) {
        fd[i] = open(argv[i+1],O_CREAT|O_TRUNC|O_RDWR,0666);
        if (fd[i] <0) messaggio("errore apertura file");
        if ( i == 0) {
            if (sem_init(&sem[i],0,1) == -1) messaggio("Errore semaforo");
        }else{
            if (sem_init(&sem[i],0,0) == -1) messaggio("Errore semaforo");
        }
    }

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);

    threadInfo *info = malloc(N*sizeof(threadInfo));
    pthread_t *tid = malloc(N*sizeof(pthread_t));
    printf("---Inizio la creazione dei figli----- \n");
    for (int i = 0;i<N; i++) {
        info[i].id = i;
        info[i].fd = fd[i];
       if (pthread_create(&tid[i],NULL,work,&info[i]) != 0) messaggio("Errore nella creazione dei figli");
    }

    int signal;
    while (1) {
        sigwait(&sig,&signal);
        if (signal == SIGINT) {
            printf("---- Catturato ----\n");

            int turno = 0;
            char buff[1024];
            FILE *lettura[N];

            for (int i = 0; i <N ; i++) {
                lettura[i] = fopen(argv[i+1],"r");
            }

            while (1) {
                if (fgets(buff,sizeof(buff),lettura[turno])!= NULL) {

                    printf("%s",buff);

                }else {
                    break;
                }
                turno = (turno + 1)%N;
            }

            for (int i = 0; i <N ; i++) {
                fclose(lettura[i]);
            }
        }
    }


}
