/*Implementare un'applicazione che riceva in input tramite argv[] il
nome di un file F ed una stringa indicante un valore numerico N maggiore
o uguale ad 1.
L'applicazione, una volta lanciata dovra' creare il file F ed attivare
N thread. Inoltre, l'applicazione dovra' anche attivare un processo
figlio, in cui vengano attivati altri N thread.
I due processi che risulteranno attivi verranno per comodita' identificati
come A (il padre) e B (il figlio) nella successiva descrizione.

Ciascun thread del processo A leggera' stringhe da standard input.
Ogni stringa letta dovra' essere comunicata al corrispettivo thread
del processo B tramite memoria condivisa, e questo la scrivera' su una
nuova linea del file F. Per semplicita' si assuma che ogni stringa non
ecceda la taglia di 4KB.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo A venga colpito esso dovra'
inviare la stessa segnalazione verso il processo B. Se invece ad essere
colpito e' il processo B, questo dovra' riversare su standard output il
contenuto corrente del file F.

Qalora non vi sia immissione di input, l'applicazione dovra' utilizzare
non piu' del 5% della capacita' di lavoro della CPU.*/

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
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

//this is the Posix version
//please compile with
//gcc prog-solution.c -lpthread -DPosix_compile

#define SIZE 4096

char *nomeFile ;
int pidB ;
int numeroThread;
typedef struct {
    sem_t empty;
    sem_t full;
    int stop;
    char buffer[SIZE];
}Canale;


typedef struct {
    sem_t mutex;
    Canale canali[];
}Shared;

Shared *shd;


void messaggio(char *p) {

    perror(p);
    exit(EXIT_FAILURE);

}


void figlioHandler(int i) {

    int fdOpen = open(nomeFile, O_RDONLY);
    if (fdOpen < 0 ) messaggio("Errore nell'apertura del file da parte dell handler");
    char buff[SIZE];
    ssize_t byteRead = 0;
    while ((byteRead = read(fdOpen,buff,sizeof(buff)))>0) {
        write(1,buff,byteRead);
    }
    close(fdOpen);


}

void handler(int i) {

    kill(pidB,SIGINT);

}


//FIGLIO B THREAD LEGGE DALLA MEMORIA E SCRIVE SU UN FILE
void * workB(void * p) {

    sigset_t sig ;
    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);

    int id = (int)p;
    char localBuff [SIZE];

    int fd = open(nomeFile , O_WRONLY);
    if (fd < 0 ) messaggio("errore nell'apertura del file");

    printf("%d",id);
    while (1) {

        sem_wait(&shd->canali[id].full);
        if (shd->canali[id].stop == 1) {

            sem_wait(&shd->canali[id].empty);
            break;
        }
        sem_wait(&shd->mutex);
        write(fd,shd->canali[id].buffer,strlen(shd->canali[id].buffer));
        sem_post(&shd->mutex);
        sem_post(&shd->canali[id].empty);
    }
    close(fd);
    return NULL;

}

//LEGGE DA STDINPUT E SCRIVE SULLA MEMORIA
void * workA(void * p) {

    sigset_t sig ;
    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);

    int id = (int)p;
    char localBuff [SIZE];
    while (1) {
        //printf("t%d > \n",id);
        if (fgets(localBuff,sizeof(localBuff),stdin) == NULL) {
            sem_wait(&shd->canali[id].empty);
            shd->canali[id].stop = 1;
            sem_post(&shd->canali[id].full);
            break;
        }

        sem_wait(&shd->canali[id].empty);
        strncpy(shd->canali[id].buffer,localBuff,SIZE-1);
        shd->canali[id].buffer[SIZE-1] = '\0';
        sem_post(&shd->canali[id].full);
    }

    return NULL;
}

int main(int argc , char *argv[]) {

    if (argc <2) messaggio("Uso ./nomeProgramma numero");


    numeroThread = atoi(argv[2]);

    nomeFile = argv[1];

    int fd = open(nomeFile,O_CREAT|O_TRUNC|O_RDWR,0666);
    if (fd < 0) messaggio("errore nell'apertura del file");


    //lo spazio che devo riservare sono n canali per n thread + lo spazio per la memoria condivida mutex
    ssize_t spazio = sizeof(Canale)*numeroThread+sizeof(Shared);

    shd = (Shared *)mmap(NULL,spazio,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);

    if (shd == NULL) messaggio("errore mmmap");

    //dico che il mutex è a uno quindi ho un token da distribuire
    if (sem_init(&shd->mutex,1,1)==-1) messaggio("errore semaforo mutex init");


    for (int i = 0 ; i < numeroThread ; i++) {
    //i spazii sono tutti liberi e nessuno è pieno
        if (sem_init(&shd->canali[i].empty,1,1)==-1)messaggio("errore semaforo mutex init");
        if (sem_init(&shd->canali[i].full,1,0)==-1)messaggio("errore semaforo mutex init");
        shd->canali[i].stop= 0;

    }

    pid_t figlio = fork();
    if (figlio < 0) messaggio("Errore creazione figlio");
    if (figlio == 0) {
        pidB = getpid();

        //costruisco la gestione dei segnali per sigint
        struct sigaction saFiglio;
        saFiglio.sa_handler = figlioHandler;
        sigemptyset(&saFiglio.sa_mask);
        saFiglio.sa_flags= SA_RESTART;
        if (sigaction(SIGINT,&saFiglio,NULL)==-1)messaggio("Errore sigaction");

        //alloco n thread
        pthread_t *figliB = malloc(numeroThread*sizeof(pthread_t));
        for (int i = 0; i < numeroThread ; i++) {

            if (pthread_create(&figliB[i],NULL,workB,(void*)i) != 0) messaggio("Errore generazione figli thread proc B");

        }

        for (int i = 0; i < numeroThread ; i++) {

          pthread_join(figliB[i],NULL);

        }

        free(figliB);
        exit(EXIT_SUCCESS);


    }else {

        struct sigaction sa;
        sa.sa_handler = handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags= SA_RESTART;
        if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("Errore sigaction");


        pthread_t *figliA = malloc(numeroThread*sizeof(pthread_t));
        for (int i = 0; i < numeroThread ; i++) {

            if (pthread_create(&figliA[i],NULL,workA,(void *)i) != 0) messaggio("Errore generazione figli thread proc B");

        }

        for (int i = 0; i < numeroThread ; i++) {

            pthread_join(figliA[i],NULL);

        }

        free(figliA);

        wait(NULL);


        sem_destroy(&shd->mutex);

        for (int i = 0; i < numeroThread ; i++) {
            sem_destroy(&shd->canali[i].empty);
            sem_destroy(&shd->canali[i].full);
        }

        munmap(shd,spazio);


    }


    return 0;

}
