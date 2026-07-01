//
// Created by davidjulianbelfiori on 01/07/26.
//
/*Implementare un programma che riceva in input tramite argv[1] un numero
intero N maggiore o uguale ad 1 (espresso come una stringa di cifre
decimali), e generi N nuovi thread. Ciascuno di questi, a turno, dovra'
inserire in una propria lista basata su memoria dinamica un record
strutturato come segue:

typedef struct _data{
int val;
struct _data* next;
} data;

I record vengono generati e popolati dal main thread, il quale rimane
in attesa indefinita di valori interi da standard input. Ad ogni nuovo
valore letto avverra' la generazione di un nuovo record, che verra'
inserito da uno degli N thread nella sua lista.
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito esso dovra'
stampare a terminale il contenuto corrente di tutte le liste (ovvero
i valori interi presenti nei record correntemente registrati nelle liste
di tutti gli N thread). */

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

#define  fflush(stdin) while (getchar()!= '\n')

typedef struct _data{
    int val;
    struct _data* next;
} data;

typedef struct {
    int id;          // ID del thread: 0, 1, ..., N-1
    data* head;      // La testa della lista dinamica di QUESTO thread
} thread_info;

int N_global ;
thread_info *infoThread;

// Strutture per la sincronizzazione e scambio dati
sem_t *sem_data_ready; // Segnala al thread che c'è un nodo pronto
sem_t *sem_main_ready; // Segnala al main che il thread è pronto a ricevere
data **shared_nodes;

void MessaggiErrore( char *messaggio) {
    printf("Errore: %s",messaggio);
    exit( EXIT_FAILURE);
}

void *lavoro ( void *att) {
        thread_info *mieInfo = (thread_info *)att;
        int id = mieInfo->id;

        sigset_t sigset;
        sigemptyset(&sigset);
    sigaddset(&sigset,SIGINT);
    while (1) {

        //aspetto che i dati sono pronti
        sem_wait(&sem_data_ready[id]);

        data *nuovo = shared_nodes[id];

        //non volgio essere interrotto
        pthread_sigmask(SIG_BLOCK,&sigset,NULL);

        nuovo->next = mieInfo->head;
        mieInfo->head = nuovo;

        pthread_sigmask(SIG_UNBLOCK,&sigset,NULL);

        sem_post(&sem_main_ready[id]);
    }
    return  NULL;

}

void sig_int_handler(int sig) {

    for (int i = 0; i <N_global ; i++) {

        printf("\n thread %d -> \n",i);
        data *curr = infoThread[i].head;

        while (curr != NULL) {
            printf("%d -> ",curr->val);
            curr = curr->next;
        }

    }

}

int main(int argc, char *argv[]) {

    int N = atoi(argv[1]) ;
    if (argc<2) {
        MessaggiErrore("Usage nomeProgramma");
    }
    N_global = N;
    if (N < 1) MessaggiErrore("il numero inserito deve essere maggiore di 1");
     struct  sigaction sa;
    sa.sa_handler = sig_int_handler; //scrivo chi la prende in carico
    sigemptyset(&sa.sa_mask);
    sa.sa_flags= 0;
    if (sigaction(SIGINT,&sa,NULL)==-1) MessaggiErrore("errore nella sigaction");


    pthread_t tidFiglio[N];
    infoThread = malloc(N*sizeof(thread_info));
    sem_data_ready = malloc((N*sizeof(sem_t)));
    sem_main_ready= malloc(N*sizeof(sem_t));
    shared_nodes = malloc(N*sizeof(data));

    if (infoThread == NULL) MessaggiErrore("errore ");

    for (int i = 0 ; i <N; i++) {

        sem_init(&sem_data_ready[i],0,0);
        sem_init(&sem_main_ready[i],0,1);
        infoThread[i].id = i;
        infoThread[i].head= NULL;
        if (pthread_create(&tidFiglio[i],NULL,lavoro,&infoThread[i]) == -1) MessaggiErrore("errore nella creazione del thread");
    }

    int letto;
    int turno= 0;
    while (1) {
        printf("inserisci un valore intero \n");
        if (scanf("%d", &letto) == 1) {
                data *nuovoRecord = malloc(sizeof(data));
            if (!nuovoRecord) MessaggiErrore("errore nella malloc");

            printf("ok");
            nuovoRecord->val = letto;
            nuovoRecord->next = NULL;

            //aspetto che il thread che ha il turno sia pronto a ricevere;
            sem_wait(&sem_main_ready[turno]);

            shared_nodes[turno] = nuovoRecord;

            //sveeglio il thread che aspetta;
            sem_post(&sem_data_ready[turno]);

            turno = (turno + 1)%N_global;

        }else {
            printf("riprovare \n");
            fflush(stdin);
        }
    }
}

