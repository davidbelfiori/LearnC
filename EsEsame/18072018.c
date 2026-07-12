/*SPECIFICATION TO BE IMPLEMENTED:
Implementare un programma che riceva in input tramite argv[] i pathname
associati ad N file, con N maggiore o uguale ad 1. Per ognuno di questi
file generi un processo che legga tutte le stringhe contenute in quel file
e le scriva in un'area di memoria condivisa con il processo padre. Si
supponga per semplicita' che lo spazio necessario a memorizzare le stringhe
di ognuno di tali file non ecceda 4KB.
Il processo padre dovra' attendere che tutti i figli abbiano scritto in
memoria il file a loro associato, e successivamente dovra' entrare in pausa
indefinita.
D'altro canto, ogni figlio dopo aver scritto il contenuto del file nell'area
di memoria condivisa con il padre entrera' in pausa indefinita.
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo padre venga colpito da esso dovra'
stampare a terminale il contenuto corrente di tutte le aree di memoria
condivisa anche se queste non sono state completamente popolate dai processi
figli.

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
#include <sys/wait.h>


char **shdm;
#define SIZE 4096

char **file;

sem_t *aperto;
int numeroFile;

void messaggio(char *p) {

    perror(p);
    exit(EXIT_FAILURE);

}

void handler (int sig) {

    printf("Cattirato \n");
    for (int i = 0 ; i < numeroFile; i++) {
        write(1,shdm[i],SIZE);
    }

}


void work(int p) {
    signal(SIGINT,SIG_IGN);

    int id = p;

    int fd = open(file[id],O_RDONLY);
    if (fd < 0) messaggio("errore nell'apertura del file");

    char localBuff[SIZE];
    read(fd,localBuff,sizeof(localBuff));
    strncpy(shdm[id],localBuff,SIZE-1);
    sem_post(&aperto[id]);
    close(fd);
    exit(EXIT_SUCCESS);

}

int main (int argc, char *argv[]) {

    if (argc < 2) messaggio("uso ./nomeProgramma f1...fN ");
     numeroFile = argc -1;


    file = malloc(numeroFile*sizeof(char *));

    for (int i = 0 ; i < argc -1 ; i++) {

        file[i] = argv[i+1];

    }

    shdm = malloc(numeroFile*sizeof(char *));
    aperto = mmap(NULL,sizeof(sem_t)*numeroFile,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    for (int i = 0 ; i < argc-1; i++) {
        if (sem_init(&aperto[i],1,0)==-1) messaggio("errore init semaforo");
        shdm[i] = mmap(NULL,SIZE,PROT_READ|PROT_WRITE,MAP_ANON|MAP_SHARED,-1,0);
        if (shdm[i] == NULL) messaggio("errore mmap");
    }

    pid_t *pid = malloc(numeroFile * sizeof(pid_t));

    for (int i = 0; i < argc-1;i++) {

        pid[i]= fork();
        if ( pid[i] <0) messaggio("errore creazione processo");
        if (pid[i] == 0) work(i);
    }

    struct sigaction saFiglio;
    saFiglio.sa_handler = handler;
    sigemptyset(&saFiglio.sa_mask);
    saFiglio.sa_flags= SA_RESTART;
    if (sigaction(SIGINT,&saFiglio,NULL)==-1)messaggio("Errore sigaction");


    for (int i = 0; i < argc-1;i++) {
        sem_wait(&aperto[i]);
    }

    printf("Tutti i figli hanno popolato la memoria. Padre entra in pausa.\n");

    while (1) {
       pause();
   }


    for (int i = 0; i < argc-1;i++) {
        sem_destroy(&aperto[i]);
    }

    munmap(shdm,sizeof(SIZE));



}

