/*SPECIFICATION TO BE IMPLEMENTED:
Implementare un programma che riceva in input tramite argv[2] un numero
intero N maggiore o uguale ad 1 (espresso come una stringa di cifre
decimali), e generi N nuovi processi. Ciascuno di questi leggera' in modo
continuativo un valore intero da standard input, e lo comunichera' al
processo padre tramite memoria condivisa. Il processo padre scrivera' ogni
nuovo valore intero ricevuto su di un file, come sequenza di cifre decimali.
I valori scritti su file devono essere separati dal carattere ' ' (blank).
Il pathname del file di output deve essere comunicato all'applicazione
tramite argv[1].
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che se il processo padre venga colpito il contenuto
del file di output venga interamente riversato su standard-output.
Nel caso in cui non vi sia immissione in input, l'applicazione non deve
consumare piu' del 5% della capacita' di lavoro della CPU.

*****************************************************************/
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define fflush(stdin) while(getchar() != '\n')
volatile sig_atomic_t sig = 0;

// Struttura per la memoria condivisa
typedef struct {
    int value;
    sem_t mutex; // Protegge l'accesso alla variabile value
    sem_t empty; // Semaforo per indicare che il buffer è vuoto
    sem_t full;  // Semaforo per indicare che il buffer contiene un dato
} shm_t;

void messaggio( char *messaggio) {
    printf("Errore: %s",messaggio);
    exit( EXIT_FAILURE);
}

void handler (int s) {
    sig = 1;

}
int main(int argc , char *argv[]) {

    if (argc < 3) messaggio("uso ./nomeProgramma numeroThread");

    char *fileName = argv[1];
    int N = atoi(argv[2]);

    shm_t *shm = mmap(NULL,sizeof(shm_t),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANON,-1,0);
    if (shm == NULL) messaggio("errore mmap");

    sem_init(&shm->mutex,1,1);
    sem_init(&shm->empty,1,1);
    sem_init(&shm->full,1,0);

    for (int i = 0; i <N; i++) {

        pid_t pid = fork();

        if (pid < 0) messaggio("Errore creazione figlio");
        else if (pid == 0) {
            signal(SIGINT,SIG_IGN);
            int val;

            while (1) {
                if (scanf("%d",&val)==1) {

                sem_wait(&shm->empty);
                sem_wait(&shm->mutex);
                shm->value = val;
                sem_post(&shm->mutex);
                sem_post(&shm->full);

            }else {
                fflush(stdin);
            }
            }
            exit(EXIT_SUCCESS);

        }
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("Errore nella ccreazione della sigaction");

    int  fd = open(fileName,O_CREAT|O_TRUNC|O_RDWR,0666);
    if (fd <0) messaggio("errore creazioen file ");

    while (1) {

        if (sig == 1) {

            printf("Catturato \n");
            lseek(fd,0,SEEK_SET);
            char buff[1024];
            ssize_t byteRead = 0 ;
            while ((byteRead = read(fd,buff,sizeof(buff)))> 0) {
                write(1,buff,byteRead);
            }

            lseek(fd,0,SEEK_END);

            sig = 0;
        }

        sem_wait(&shm->full);
        int val = shm->value ;
        sem_post(&shm-> mutex);
        sem_post(&shm->empty);
        char buff[512];
        snprintf(buff,sizeof(buff),"%d ",val);
        write(fd,buff,strlen(buff));
        fsync(fd);
    }



}