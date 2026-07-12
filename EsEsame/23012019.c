/******************************************************************
SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che riceva in input, tramite argv[], il nome
di un file F ed N stringhe S_1 .. S_N (con N maggiore o uguale
ad 1.
Per ogni stringa S_i dovra' essere attivato un nuovo thread T_i, che fungera'
da gestore della stringa S_i.
Il main thread dovra' leggere indefinitamente stringhe dallo standard-input.
Ogni nuova stringa letta dovra' essere comunicata a tutti i thread T_1 .. T_N
tramite un buffer condiviso, e ciascun thread T_i dovra' verificare se tale
stringa sia uguale alla stringa S_i da lui gestita. In caso positivo, ogni
carattere della stringa immessa dovra' essere sostituito dal carattere '*'.
Dopo che i thread T_1 .. T_N hanno analizzato la stringa, ed eventualmente questa
sia stata modificata, il main thread dovra' scrivere tale stringa (modificata o non)
su una nuova linea del file F.
In altre parole, la sequenza di stringhe provenienti dallo standard-input dovra'
essere riportata su file F in una forma 'epurata'  delle stringhe S1 .. SN,
che verranno sostituite da strighe  della stessa lunghezza costituite esclusivamente
da sequenze del carattere '*'.
Inoltre, qualora gia' esistente, il file F dovra' essere troncato (o rigenerato)
all'atto del lancio dell'applicazione.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito esso dovra'
riversare su standard-output il contenuto corrente del file F.

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
volatile sig_atomic_t segnale = 0;

char *nomeFile;
char shdBuff[4096];
sem_t *datoPronto;
sem_t *datoLetto;

typedef struct {
    int id;
    char *str;
}ThreadInfo;

void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}

void handler(int i) {

    segnale = 1;

}

void *work (void *arg) {

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);

    ThreadInfo *info =(ThreadInfo *)arg;


    while (1) {

        sem_wait(&datoPronto[info->id]);
        if (strcmp(info->str,shdBuff)==0) {

            for (int i = 0; shdBuff[i] != '\0'; i++) {
                shdBuff[i]='*';
            }
        }
        sem_post(&datoLetto[info->id]);

    }


}


int main (int argc , char *argv[]) {

    if (argc < 3) messaggio("./nomeProgramma nomeFile s1 .... sN");

    nomeFile = argv[1];
    int numeroThread = argc -2;
    ThreadInfo *info = malloc(numeroThread*sizeof(ThreadInfo));

    datoLetto = malloc(numeroThread*sizeof(sem_t));
    datoPronto = malloc(numeroThread*sizeof(sem_t));
    pthread_t *tid = malloc(numeroThread*sizeof(pthread_t));

    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("Errore sigaction");



    for (int i = 0 ; i < numeroThread; i++) {
        info[i].id = i;
        info[i].str = argv[i+2];
        if (sem_init(&datoPronto[i],0,0)==-1) messaggio("errore sem_init pronto");
        if (sem_init(&datoLetto[i],0,0)==-1) messaggio("errore sem_init pronto");
    }
    for (int i = 0 ; i < numeroThread; i++) {
        if (pthread_create(&tid[i],NULL,work,&info[i])!=0) messaggio("Errore creazione thread");
    }

    int fd = open(nomeFile,O_CREAT|O_TRUNC|O_RDWR,0666);
    if (fd < 0)messaggio("Errore apertura");

    while (1) {

        if (segnale == 1) {
            printf("-------CATTURATO------");
            lseek(fd,0,SEEK_SET);
            ssize_t byteLetti =0;
            char  buff[4096];
            while ((byteLetti = read(fd,buff,sizeof(buff)))>0) {
                write(1,buff,byteLetti);
            }
            lseek(fd,0,SEEK_END);
            printf("---- FINITO --- \n");
            segnale = 0;
        }else {

            read(0,shdBuff,sizeof(shdBuff));
            shdBuff[strcspn(shdBuff,"\n")]='\0';
            for (int i = 0; i < numeroThread; i++) {
                sem_post(&datoPronto[i]);
            }

            for (int i = 0; i < numeroThread; i++) {
                sem_wait(&datoLetto[i]);
            }

            write(fd,shdBuff,strlen(shdBuff));
            write(fd,"\n",strlen("\n"));
        }

    }


}
