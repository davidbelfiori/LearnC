/*
22 settembre 2023.

Scrivere un programma che riceva in input tramite arvg[1] il nome di un file F.
Il programma, attivo in un processo P, dovra' attivare un nuovo processo P' che
dovra' creare il file F (o se esistente dovra' troncarlo).
Il processo P dovra' poi attivare un thread T che legga linee dallo standard input
e le trasferisca al processo P', il quale dovra' scriverle sul file F.
Il trasferimento delle linee verso il processo P' dovra' avvenire seguendo l'ordine di
lettura delle linee dallo standard input. Il canale di comunicazione per il
trasferimento di dati e' a scelta dello studente.

L'applicazione dovra' gestire il segnale  SIGINT (o CTRL_C_EVENT nel caso WinAPI)
in modo tale che quando il processo P venga colpito dovra' essere attivato
un nuovo thread equivalente a T, che dovra' anche lui leggere linee dallo
standard input e trasferirle al processo P', sempre rispettando che l'ordine
di lettura delle linee dallo standard input corrisponda all'ordine con cui i
dati sono traferiti al processo P'.  Il numero massimo di thread equivalenti a T
che potranno essere attivati e' 4, quindi la gestione della segnalazione dovra'
tenere in conto di questo limite. Se invece la segnalazione colpisse il processo P',
non dovra' avere alcun effetto sulla sua esecuzione.
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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int sigRicevute;
int threadCreati;
char *nomeFile;
int connettori[2];

typedef struct {
    int id;
}threadInfo;

void messaggio(char *p) {
    printf("Errore %s \n",p);
    exit(EXIT_FAILURE);
}

void lavoroFiglio(int fdPipe) {
    signal(SIGINT,SIG_IGN);
    printf("Ciao sono figlio \n");
    int fdOpen = open(nomeFile,O_WRONLY|O_CREAT|O_TRUNC,0666);
    if (fdOpen <0) messaggio("errore creazione");

    char buff[1024];
    ssize_t byteRead;

    while((byteRead = read(fdPipe,buff,sizeof(buff)))>0) {

        write(fdOpen,buff,byteRead);
    }
    close(fdPipe);
    close(fdOpen);
    exit(EXIT_SUCCESS);
}


void * work(void * p) {

    threadInfo *info = (threadInfo *)p;

    char buff[1024];
    while (1) {

        pthread_mutex_lock(&mutex);

        if (fgets(buff,sizeof(buff)-4,stdin)==NULL) {
            pthread_mutex_unlock(&mutex);
            exit(EXIT_SUCCESS);
        }

        char nBuff[1024];
        snprintf(nBuff,sizeof(buff),"t%d: %s",info->id,buff);
        write(connettori[1],nBuff,strlen(nBuff));
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    return NULL;

}

int main(int argc,char *argv[]) {

    if (argc<2) messaggio("uso ./nomeProgramma nomeFile");

   nomeFile = argv[1];
threadInfo *info = malloc(4*sizeof(threadInfo));


    if (pipe(connettori) == -1) messaggio("errore nella creazione della pipe");


    pid_t pid = fork();
    if (pid < 0) messaggio("errore nella creazione del figlio");
    if (pid == 0) {
        close(connettori[1]);
        lavoroFiglio(connettori[0]);
    }

    close(connettori[0]);

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    if (pthread_sigmask(SIG_BLOCK,&sig,NULL)==-1) messaggio("Errore pthread sigmask");

     threadCreati = 1;
    pthread_t tid ;
    info[threadCreati-1].id=threadCreati;

    if (pthread_create(&tid,NULL,work,&info[threadCreati-1])!=0) messaggio("Errore creazione thread");


    int signal;
    while (1) {
        sigwait(&sig,&signal);
        if (signal == SIGINT) {
            if (threadCreati <4) {
                threadCreati++;
                info[threadCreati-1].id = threadCreati;
                printf("Creazione nuovo thread ,numero thread %d \n",threadCreati);
                pthread_t newtid ;
                if (pthread_create(&newtid,NULL,work,&info[threadCreati-1])!=0) messaggio("Errore creazione thread");
            }else {
                printf("non posso creare nuovo thread, numero thread %d \n",threadCreati);
            }

        }



    }


}

