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
#define fflush(stdin) while(getchar() != '\n')


int N_global;
sem_t *sem_paccoPronto;
sem_t *sem_nastroPronto;

typedef struct {
    int peso;
} pacco_t;


pacco_t **nastri;
int *pacchiLavorati; //numero di pacchi gestiti da ogni thread;

void handler(int sig) {
    printf("-----SIGINT ATTIVATO-------");
    for (int i = 0;i<N_global;i++) {
        printf("pacchi lavorati da id:%d %d \n",i,pacchiLavorati[i]);
    }
    printf("----------------------------");
};

void messaggio(char *messaggio) {

    printf("errore %s",messaggio);
    exit(EXIT_FAILURE);

}

void *nastro(void *arg) {

    int id = *(int *)arg;

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);

    while (1) {

        sem_wait(&sem_paccoPronto[id]);
        pthread_sigmask(SIG_BLOCK,&sig,NULL);

        pacco_t *miopacco = nastri[id];
        int pesoletto = miopacco->peso;

        free(miopacco);
        pacchiLavorati[id]++;

        pthread_sigmask(SIG_UNBLOCK,&sig,NULL);

        sleep(1);

        sem_post(&sem_nastroPronto[id]);
    }
    return NULL;

}

int main(int argc, char *argv[]) {

    if (argc<2) messaggio("uso nomeprogramma numero ");
    int N = atoi(argv[1]);

    if (N<2){
        messaggio("il numero deve essere maggiore di 1");
    }

    N_global = N;

    struct sigaction sa;
    sa.sa_handler= handler;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore sigaction");

    sem_paccoPronto = malloc(N_global*sizeof(sem_t));
    sem_nastroPronto = malloc(N_global*sizeof(sem_t));

    pthread_t *tidLavoratori = malloc(N_global*sizeof(pthread_t));
    nastri = malloc(N_global * sizeof(pacco_t));
    pacchiLavorati = malloc(N_global * sizeof(int));

    // Array per passare l'ID ai thread in modo sicuro senza race conditions
    int *thread_ids = malloc(N_global * sizeof(int));

    if (!tidLavoratori || !nastri || !pacchiLavorati || !sem_paccoPronto || !sem_nastroPronto || !thread_ids) {
        messaggio("Errore di allocazione memoria globale");
    }

    for (int i = 0 ; i<N_global;i++) {

        pacchiLavorati[i] = 0;
        thread_ids[i]=i;
        nastri[i]= NULL;

        sem_init(&sem_nastroPronto[i],0,1);
        sem_init(&sem_paccoPronto[i],0,0);

        if (pthread_create(&tidLavoratori[i],NULL,nastro,&thread_ids[i])==-1) messaggio("errore nalla creazione dei thread");

    }

    int letto;
    int turno = 0 ;//per sapere chi è il prossimo


    while (1) {
        printf("inserisci un numero:");
        if (scanf("%d",&letto)==1) {
            pacco_t *nuovo_pacco = malloc(sizeof(pacco_t));
            if (!nuovo_pacco) messaggio("errore nuovo pacco");
            nuovo_pacco->peso=letto;
            sem_wait(&sem_nastroPronto[turno]);
            nastri[turno] = nuovo_pacco;
            sem_post(&sem_paccoPronto[turno]);
            turno = (turno +1)%N_global;
        }else {
            fflush(stdin);
        }

    }
    return 0;

}