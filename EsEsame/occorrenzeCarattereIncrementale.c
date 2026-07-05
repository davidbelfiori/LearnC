//
// Created by davidjulianbelfiori on 02/07/26.
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

volatile sig_atomic_t sigint_flag  = 0;
char *nomeFile;
char carattere ;
char *nomeShadow;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
off_t offsetLetto = 0;
int occorrenzeTotali = 0;



void messaggio (char *messaggio) {
    printf("errore:%s \n",messaggio);
    exit(EXIT_FAILURE);
}

void handle(int sig) {
    sigint_flag =1;
}

void *work( void *arg) {

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);

    //mi proteggo da sigint e dico di non bloccarmi e aspettare che ho fatto
    pthread_sigmask(SIG_BLOCK,&sig,NULL);
    //mi assicuro di essere il solo che lavora sul file
    pthread_mutex_lock(&mutex);


    int fdSorg= open(nomeFile,O_RDONLY);
    if (fdSorg<0) {
        pthread_sigmask(SIG_UNBLOCK,&sig,NULL);
        pthread_mutex_unlock(&mutex);
        messaggio("errore nell'apertura da parte del thread");
    }

    lseek(fdSorg,offsetLetto,SEEK_SET);

    char buffer[1024];
    ssize_t byteLetti;
    int occorrenze = 0;

    while ((byteLetti = read(fdSorg,buffer,sizeof(buffer)))>0) {

        for (int i = 0; i<byteLetti;i++) {
            if (buffer[i] == carattere) {
                occorrenze++;
                occorrenzeTotali++;
            }
        }
    }
    if (byteLetti <0) {
        printf("errore nella lettura");
    }

    offsetLetto = lseek(fdSorg,0,SEEK_CUR);
    // 7. Stampo i risultati
    printf("\n[THREAD] Analisi completata.\n");
    printf("Occorrenze di '%c' in quest'ultima porzione: %d\n", carattere, occorrenze);
    printf("Occorrenze di '%c' totali dall'inizio: %d\n", carattere, occorrenzeTotali);

    // 8. Pulizia
    close(fdSorg);
    pthread_sigmask(SIG_UNBLOCK,&sig,NULL);
    pthread_mutex_unlock(&mutex);
    return NULL;

}


int main(int argc , char *argv[]) {

    if (argc<3) messaggio("uso ./nomeProgramma nomeFile carattere");

    nomeFile = argv[1];
    carattere= argv[2][0];

    struct sigaction sa;
    sa.sa_handler = handle;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore sigaction");

    int fdSorgente = open(nomeFile,O_WRONLY|O_CREAT|O_TRUNC);
    if (fdSorgente <0 )messaggio("errore nell'apertura del file");

    printf("inizio lettura da stdin: \n");

    char buff[1024];
    while (1) {

        if (sigint_flag == 1) {
            sigint_flag = 0;
            printf("catturata faccio partire il thread \n");

            pthread_t tid;

            if (pthread_create(&tid,NULL,work,NULL) ==-1) {
                messaggio("errore nella creazione del thread");
            }
            else {
                pthread_detach(tid);
            }

        }else {

            ssize_t byteRead= read(0,buff,sizeof(buff));
            if (byteRead > 0) {
                write(fdSorgente,buff,byteRead);
            }else {
                if (errno== EINTR) {
                    continue;
                }else {
                    messaggio("errore nella write");
                }
            }

        }
    }

    close(fdSorgente);
    return 0;



}