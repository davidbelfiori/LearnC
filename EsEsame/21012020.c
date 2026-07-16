//
// Created by davidjulianbelfiori on 13/07/26.
//


/*SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che ricevento in input tramite argv[] una stringa S
esegua le seguenti attivita'.
Il main thread dovra' attivare due nuovi thread, che indichiamo con T1 e T2.
Successivamente il main thread dovra' leggere indefinitamente caratteri dallo
standard input, a blocchi di 5 per volta, e dovra' rendere disponibili i byte
letti a T1 e T2.
Il thread T1 dovra' inserire di volta in volta i byte ricevuti dal main thread
in coda ad un file di nome S_diretto, che dovra' essere creato.
Il thread T2 dovra' inserirli invece nel file S_inverso, che dovra' anche esso
essere creato, scrivendoli ogni volta come byte iniziali del file (ovvero in testa al
file secondo uno schema a pila).

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito esso dovra'
calcolare il numero dei byte che nei due file hanno la stessa posizione ma sono
tra loro diversi in termini di valore. Questa attivita' dovra' essere svolta attivando
per ogni ricezione di segnale un apposito thread.

In caso non vi sia immissione di dati sullo standard input, l'applicazione dovra'
utilizzare non piu' del 5% della capacita' di lavoro della CPU.

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

char *S;
char buff[6];
sem_t *datiLetti;
sem_t *datiPronti;


void messaggio(char *p) {

    perror(p);
    exit(EXIT_FAILURE);

}

void *trova(void *p) {

    char nomeDir[512],nomeInv[512];
    snprintf(nomeDir,sizeof(nomeDir),"%s_diretto.txt",S);
    snprintf(nomeInv,sizeof(nomeInv),"%s_inverso.txt",S);

    int fdDir = open(nomeDir,O_RDONLY);
    int fdInv = open(nomeInv,O_RDONLY);

    if (fdDir <0 || fdInv <0) messaggio("errore apertura trova");

    unsigned long diversi = 0;
    char a,b;
    while (read(fdDir,&a,1) >0 && read(fdInv,&b,1)>0) {
        if (a != b) diversi++;
    }

    printf("FINITO DIVERSI = %lu \n",diversi);
    close(fdDir);
    close(fdInv);
    return NULL;
}

void handler(int i) {

    printf("catturato \n");
    pthread_t searcher;
    if (pthread_create(&searcher,NULL,trova,NULL)!=0) messaggio("errore spawn lavoratore");

}

// T1
void * work1(void * p) {

    int id = (int)p;
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);

    char buffer[512];
    snprintf(buffer,sizeof(buffer),"%s_diretto.txt",S);
    int fd = open(buffer,O_CREAT|O_WRONLY|O_TRUNC,0666);
    if (fd < 0) messaggio("errore apertura file");


    while (1) {
        sem_wait(&datiPronti[id]);
        write(fd,buff,5);
        sem_post(&datiLetti[id]);
    }


}

//T2
void * work2(void * p) {
    int id = (int)p;
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);
    char buffer[512];
    snprintf(buffer,sizeof(buffer),"%s_inverso.txt",S);

    int fd = open(buffer,O_CREAT|O_RDWR|O_TRUNC,0666);
    if (fd < 0) messaggio("errore apertura file");


    while (1) {
        sem_wait(&datiPronti[id]);
        off_t size = lseek(fd,0,SEEK_END);
        char *temp_buffer = NULL;
        if (size > 0) {
            temp_buffer = malloc(size);
            lseek(fd,0,SEEK_SET);
            read(fd,temp_buffer,size);
        }
        ftruncate(fd,0);
        lseek(fd,0,SEEK_SET);
        write(fd,buff,5);
        write(fd,temp_buffer,size);
        free(temp_buffer);
        sem_post(&datiLetti[id]);
    }
}

int main (int argc , char **argv) {

    if (argc < 2) messaggio("errore d'uso");
    S = argv[1];

    datiLetti = malloc(2*sizeof(sem_t));
    datiPronti = malloc(2*sizeof(sem_t));

    for (int i = 0 ; i < 2; i++) {
        sem_init(&datiLetti[i],0,1);
        sem_init(&datiPronti[i],0,0);
    }


    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore sigaction");


    pthread_t *tid = malloc(2*sizeof(pthread_t));
    if (pthread_create(&tid[0],NULL,work1,(void *)0)!=0) messaggio("errore thread");
    if (pthread_create(&tid[0],NULL,work2,(void *)1)!=0) messaggio("errore thread");

    while (1) {
        sem_wait(&datiLetti[0]);
        sem_wait(&datiLetti[1]);
        int eof = 0;
        ssize_t byteLetti = 0;
        while (byteLetti < 5) {
            char  c;
            ssize_t r = read(0,&c,1);


            if (r < 0) {
                if (errno == EINTR) continue;
                messaggio("errore lettura");
            }
            if (r == 0) {
                eof =1;
                break;
            }

            if (c == '\n') continue;

            buff[byteLetti]=c;
            byteLetti += r;

        }
        if (byteLetti == 5) {
            byteLetti = 0;
            sem_post(&datiPronti[0]);
            sem_post(&datiPronti[1]);

        }else if (eof == 1) {
            sem_post(&datiPronti[0]);
            sem_post(&datiPronti[1]);
            break;
        }
    }



}
