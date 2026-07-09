//
// Created by davidjulianbelfiori on 01/07/26.
//

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

/*Implementare una programma che riceva in input, tramite argv[], il nome
di un file F. Il programa dovra' creare il file F e popolare il file
con lo stream priveniente da standard-input. Il programma dovra' generare
anche un ulteriore processo il quale dovra' riversare il contenuto  che
viene inserito in F su un altro file denominato shadow_F, tale inserimento
dovra' essere realizzato in modo concorrente rispetto all'inserimento dei dati su F.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando un qualsiasi processo (parent o child) venga colpito si
dovra' immediatamente emettere su standard-output il contenuto del file
che il processo child sta popolando.

Qualora non vi sia immissione di input, l'applicazione dovra' utilizzare
non piu' del 5% della capacita' di lavoro della CPU.*/

char nomeShadow[128];

void messaggio(char *msg) {
    printf("Errore: %s \n",msg);
    exit(EXIT_FAILURE);
}

void handler (int sig) {

    printf("-----------SIGINT ATTIVATA------------\n");
    int handlerFd = open(nomeShadow,O_RDONLY,0666);
    if (handlerFd < -1) messaggio("errore nell'apertura del file da parte della handler");

    char buff[1024];
    int byteRead = 0;
    while ((byteRead = read(handlerFd,buff,sizeof(buff)))>0) {
        printf("%s",buff);
    }
    close(handlerFd);
    exit(EXIT_FAILURE);
};



int main(int argc, char *argv[]) {

    if (argc<2) messaggio("uso ./nomeProgramma nomeFile");

    if (strlen(argv[1])>127) messaggio("il nome del file è troppo grande");

    char *nomeFile = argv[1];

    snprintf(nomeShadow,sizeof(nomeShadow),"shadow_%s",nomeFile);

    struct sigaction sa;
    sa.sa_handler= handler;
    sa.sa_flags= 0 ;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore");

    sem_t *syncSem = mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
    if (!syncSem) messaggio("errore nel semaforo");
    if (sem_init(syncSem,1,0)==-1) messaggio("errore nel sem init");

    int sourceFd=open(nomeFile,O_RDWR|O_CREAT|O_APPEND,0666);

    if (sourceFd <0) messaggio("errore nella open");

    pid_t figlio = fork();
    if (figlio <0) messaggio("errore creazione figlio");
    if (figlio>0) {
        //PADRE
        int padreFd= open(nomeFile, O_WRONLY|O_APPEND);
        if (padreFd <0) messaggio("errore apertura padre");
        char buff[1024];
        int byteRead = 0 ;
        while (( byteRead = read(0,buff,sizeof(buff)))>0) {
                write(padreFd,buff,byteRead);
            //forzo la scrittura;
            fsync(padreFd);
            sem_post(syncSem);
        } ;

        wait(NULL);
        close(padreFd);
        munmap(syncSem,sizeof(sem_t));
    }else {
        //FIGLIO

        int figlioFd = open(nomeShadow,O_CREAT|O_WRONLY|O_APPEND,0666);
        int byteLetti;
        char readBuffer[1024];
        while (1) {

            sem_wait(syncSem);
            while ((byteLetti= read(sourceFd,readBuffer,sizeof(readBuffer)))>0) {
                write(figlioFd,readBuffer,byteLetti);
            }
        }
    }
        return 0;
}