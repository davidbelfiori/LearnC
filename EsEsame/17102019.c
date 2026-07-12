/*SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che riceva in input, tramite argv[], il nomi
di N file (con N maggiore o uguale a 1).
Per ogni nome di file F_i ricevuto input dovra' essere attivato un nuovo thread T_i.
Il main thread dovra' leggere indefinitamente stringhe dallo standard-input
e dovra' rendere ogni stringa letta disponibile ad uno solo degli altri N thread
secondo uno schema circolare.
Ciascun thread T_i a sua volta, per ogni stringa letta dal main thread e resa a lui disponibile,
dovra' scriverla su una nuova linea del file F_i.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito esso dovra'
riversare su standard-output e su un apposito file chiamato "output-file" il
contenuto di tutti i file F_i gestiti dall'applicazione
ricostruendo esattamente la stessa sequenza di stringhe (ciascuna riportata su
una linea diversa) che era stata immessa tramite lo standard-input.

In caso non vi sia immissione di dati sullo standard-input, l'applicazione dovra' utilizzare
non piu' del 5% della capacita' di lavoro della CPU.

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

sem_t *turno;
sem_t *datoPronto;
sem_t *datoLetto;
char **nomeFile;
char **buffer;
int N;

void messaggio(char *p) {

    perror(p);
    exit(EXIT_FAILURE);

}

void * work(void * p) {

    int id = (int)p;

    printf("Mio id %d \n",id);
    int fd = open(nomeFile[id],O_CREAT|O_TRUNC|O_WRONLY,0666);
    if (fd < 0)messaggio("errore apertura file");

    while (1) {
        sem_wait(&turno[id]);
        sem_wait(&datoPronto[id]);
        write(fd,buffer[id],strlen(buffer[id]));
        sem_post(&datoLetto[id]);
    }


}

void handler(int sig) {

    char *message = "---CATTURATO---- \n";
    write(1,message,strlen(message));
   int *fd = malloc(N*sizeof(int));

    for (int i = 0; i < N ; i++) {
        fd[i]=open(nomeFile[i],O_RDONLY);
    }

    int fdSorg= open("output-file.txt",O_CREAT|O_APPEND|O_WRONLY,0666);
    if (fdSorg < 0) messaggio("Errore apertura out");
    char c;
    int i = 0;
    int continua = 1;
    while (continua==1) {
        continua = 0;
        if (fd[i] > 0) {
            ssize_t b;
            while ((b = read(fd[i],&c,1))>0) {
                continua= 1;
                write(1,&c,1);
                write(fdSorg,&c,1);
                if (c == '\n') break;
            }
        }
        i=(i+1)%N;

    }

    char *message1 = "---FINE---- \n";
    write(1,message1,strlen(message1));

}

int main (int argc, char *argv[]) {

    if (argc < 2) messaggio("errore d'uso");

    N = argc -1;
    nomeFile = &argv[1];


    turno = malloc(N*sizeof(sem_t));
    datoPronto = malloc(N*sizeof(sem_t));
    datoLetto = malloc(N*sizeof(sem_t));
    buffer = malloc(N* sizeof(char *));

    for (int i = 0; i < N; i++) {
        buffer[i] = malloc(1024*sizeof(char));
        if (i ==0) {
            sem_init(&turno[i],0,1);
        }else {
            sem_init(&turno[i],0,0);

        }
        sem_init(&datoPronto[i],0,0);
        sem_init(&datoLetto[i],0,1);
    }

    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("errore sigint");

    pthread_t *tid = malloc(N*sizeof(pthread_t));
    for (int i = 0; i < N; i++) {
       pthread_create(&tid[i],NULL,work,(void *)i);
    }


    int i = 0;

    while (1) {

        sem_wait(&datoLetto[i]);
        if (fgets(buffer[i],1024,stdin)==NULL){
            sem_post(&datoPronto[i]);
            break;
        }
        sem_post(&datoPronto[i]);
        i = (i +1)%N;
        sem_post(&turno[i]);
    }


}
