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

pthread_mutex_t mutex;
int saldo = 1000;
int operazioni;

void handler(int sig) {
    printf("------------ SIGINT ATTIVATA ---------\n");
    printf("Saldo %d Numero di operazioni effettuate %d \n",saldo,operazioni);
    printf("---------------------------------------\n");
}

void *produttore(void *arg) {

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGINT);

    while (1) {
        pthread_sigmask(SIG_BLOCK,&sigset,NULL);
        pthread_mutex_lock(&mutex);

        if (saldo >= 20) {
            saldo-= 20;
            operazioni++;
        }
        pthread_mutex_unlock(&mutex);
        pthread_sigmask(SIG_UNBLOCK,&sigset,NULL);
        sleep(1);
    }
}

void *consumatore( void *arg) {

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGINT);

    while (1) {
        pthread_sigmask(SIG_BLOCK,&sigset,NULL);
        pthread_mutex_lock(&mutex);

            saldo+= 20;
            operazioni++;
        printf("Saldo %d \n",saldo);
        pthread_mutex_unlock(&mutex);
        pthread_sigmask(SIG_UNBLOCK,&sigset,NULL);
        sleep(1);
    }

}

int main() {

    struct sigaction sa;
    sa.sa_handler= handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);



    pthread_mutex_init(&mutex,NULL);
    pthread_t marito, moglie;

    pthread_create(&marito,NULL,consumatore,NULL);
    pthread_create(&moglie,NULL,produttore,NULL);

    pthread_join(marito,NULL);
    pthread_join(moglie,NULL);


}
