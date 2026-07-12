/*Implementare un programma che riceva in input tramite argv[] i pathname
associati ad N file (F1 ... FN), con N maggiore o uguale ad 1.
Per ognuno di questi file generi un thread che gestira' il contenuto del file.
Dopo aver creato gli N file ed i rispettivi N thread, il main thread dovra'
leggere indefinitamente la sequenza di byte provenienti dallo standard-input.
Ogni 5 nuovi byte letti, questi dovranno essere scritti da uno degli N thread
nel rispettivo file. La consegna dei 5 byte da parte del main thread
dovra' avvenire secondo uno schema round-robin, per cui i primi 5 byte
dovranno essere consegnati al thread in carico di gestire F1, i secondi 5
byte al thread in carico di gestire il F2 e cosi' via secondo uno schema
circolare.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito esso dovra',
a partire dai dati correntemente memorizzati nei file F1 ... FN, ripresentare
sullo standard-output la medesima sequenza di byte di input originariamente
letta dal main thread dallo standard-input.

Qualora non vi sia immissione di input, l'applicazione dovra' utilizzare
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
volatile sig_atomic_t sig = 0;
char **nomeFile;
char (*buffer)[5];
sem_t *turno;
sem_t *datoPronto;
sem_t *datoLetto;
int N;


void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}

void handler(int i) {
    sig = 1;
}

void print(int id) {

   fflush(stdout);
    int *fd = malloc(N*sizeof(int));
    for (int i = 0 ; i < N ; i++) {

        fd[i] = open (nomeFile[i],O_RDONLY);
        if (fd[i]< 0) messaggio("errore apertura file handle");
    }
    int turnoRead = 0;
    char buff[5];
    int fileAttivi = N;
    int *fileFiniti = calloc(N,sizeof(int));

    while (fileAttivi > 0) {
        if (fileFiniti[turnoRead]!=1) {
            ssize_t tr = 0;
            while (tr < 5) {
                ssize_t r = read(fd[turnoRead],buff+tr,5-tr);
                if (r == 0) break;
                tr += r;
            }
            if (tr > 0) write(1,buff,tr);
            if(tr == 0) {
                fileFiniti[turnoRead]= 1;
                fileAttivi --;
            }
        }
        turnoRead = (turnoRead+1)%N;
    }

    printf("------FINITO-----");
}

void * work(void * p) {

    int id = (int)p ;

    sigset_t si;
    sigemptyset(&si);
    sigaddset(&si,SIGINT);
    pthread_sigmask(SIG_BLOCK,&si,NULL);
    int fd = open(nomeFile[id],O_WRONLY);
    if (fd < 0 ) messaggio("Errore apertura file thread figlio");

    while (1) {
        sem_wait(&datoPronto[id]);
        write(fd,buffer[id],5);
        sem_post(&datoLetto[id]);
    }
}

int main (int argc , char *argv[]) {

    if (argc < 2) messaggio("errore ./nomeProgramma file1 ... fileN");

    N = argc-1;
    turno = malloc(N*sizeof(sem_t));
    datoPronto = malloc(N*sizeof(sem_t));
    datoLetto = malloc(N*sizeof(sem_t));
    nomeFile = malloc(N* sizeof(char *));

    for (int i = 0; i < N ; i++) {

        if ( sem_init(&datoPronto[i],0,0)==-1) messaggio("Errore sem_init");
        if ( sem_init(&datoLetto[i],0,1)==-1) messaggio("Errore sem_init");
        nomeFile[i] = argv[i+1];
    }

    for (int i = 0; i < N ; i++) {
        int fd = open(nomeFile[i],O_CREAT|O_TRUNC|O_RDWR,0666);
        if (fd < 0) messaggio("errore nell'apertura del file");
    }


    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = print;
    if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("Errore sigaction");

    pthread_t *tid = malloc(N*sizeof(pthread_t));

    for (int i = 0; i < N; i++) {
        if (pthread_create(&tid[i],NULL,work,(void*)i)!=0) messaggio("errore creazioen figlio");
    }

    int t = 0;
    ssize_t bR = 0;
    while (1) {
            sem_wait(&datoLetto[t]);
            while (bR < 5) {
                ssize_t r = read(0,buffer[t]+bR,5-bR);
                bR+= r;
            }

        // Se abbiamo letto con successo 5 byte, passiamo la palla al thread
        if (bR == 5) {
            bR= 0; // Resetta per il prossimo turno
            sem_post(&datoPronto[t]);
            t = (t + 1) % N; // Passa al prossimo thread in Round-Robin
        } else {
            // Se siamo qui (e non è SIGINT), l'input è finito prematuramente (EOF)
            sem_post(&datoLetto[t]);
            break;
        }

    }


    }


