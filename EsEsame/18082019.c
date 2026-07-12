/*SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che riceva in input, tramite argv[], un insieme di
stringhe S_1 ..... S_n con n maggiore o uguale ad 1.
Per ogni stringa S_i dovra' essere attivato un thread T_i.
Il main thread dovra' leggere indefinitamente stringhe dallo standard-input.
Ogni stringa letta dovra' essere resa disponibile al thread T_1 che dovra'
eliminare dalla stringa ogni carattere presente in S_1, sostituendolo con il
carattere 'spazio'.
Successivamente T_1 rendera' la stringa modificata disponibile a T_2 che dovra'
eseguire la stessa operazione considerando i caratteri in S_2, e poi la passera'
a T_3 (che fara' la stessa operazione considerando i caratteri in S_3) e cosi'
via fino a T_n.
T_n, una volta completata la sua operazione sulla stringa ricevuta da T_n-1, dovra'
passare la stringa ad un ulteriore thread che chiameremo OUTPUT il quale dovra'
stampare la stringa ricevuta su un file di output dal nome output.txt.
Si noti che i thread lavorano secondo uno schema pipeline, sono ammesse quindi
operazioni concorrenti su differenti stringhe lette dal main thread dallo
standard-input.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito esso dovra'
stampare il contenuto corrente del file output.txt su standard-output.

In caso non vi sia immissione di dati sullo standard-input, l'applicazione
dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.

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

#define MAXSIZE 4096

typedef struct {
    int id;
    int fdSorg;
    int fdDest;
    char *str;
}ThreadInfo;

void messaggio(char *p) {

    perror(p);
    exit(EXIT_FAILURE);

}


void *work(void *p) {
    ThreadInfo *info = (ThreadInfo *)p;
    char buff[MAXSIZE];

    while (read(info->fdSorg, buff, MAXSIZE) > 0) {
        // Scorriamo fino a MAXSIZE dato che la pipe trasferisce blocchi fissi
        for (int i = 0; i < MAXSIZE; i++) {
            // Interrompiamo se incontriamo la fine della stringa reale per evitare cicli inutili
            if (buff[i] == '\0') break;

            if (strchr(info->str, buff[i]) != NULL && buff[i] != '\n') {
                buff[i] = ' ';
            }
        }
        write(info->fdDest, buff, MAXSIZE);
    }
    return NULL;
}

void *out(void *p) {

    int fdPipe = *(int *)p;

    int fd = open("output.txt",O_CREAT|O_APPEND|O_WRONLY,0666);

    if (fd < 0) messaggio("Errore creazione output");
    char buff[MAXSIZE];
    ssize_t byteRead = 0 ;
    while ((byteRead = read(fdPipe, buff,MAXSIZE))>0) {
        write(fd, buff, byteRead);
    }
    return NULL;

}

void handler (int s) {

    int fd = open("output.txt",O_RDONLY);
    if (fd < 0 ) messaggio("Errore ");
    ssize_t byteRead;
    char buff[MAXSIZE];
    while ((byteRead = read(fd, buff,MAXSIZE))>0) {
        write(1, buff, byteRead);
    }
}


int main(int argc , char **argv) {

    if (argc < 2)messaggio("Errore ./nomeprog  s1 ... sN");

    int N = argc - 1;
    int (* conn)[2]  = malloc((N + 1) * sizeof(int[2]));

    for (int i = 0 ; i < N+1 ; i++) {
        if (pipe(conn[i])==-1)messaggio("Errore pipe");
    }

    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("errore sigint");

    pthread_t *tid = malloc(N*sizeof(pthread_t));
    ThreadInfo *info = malloc(N*sizeof(ThreadInfo));

    for (int i = 0 ; i < N ; i++) {
        info[i].id = i;
        info[i].fdSorg = conn[i][0];
        info[i].fdDest = conn[i+1][1];
        info[i].str = argv[i+1];
        if (pthread_create(&tid[i],NULL,work,&info[i])!=0) messaggio("errore creazione thread");

    }

    pthread_t tidOut ;
    int fdFinale = conn[N][0];
    if (pthread_create(&tidOut,NULL,out,&fdFinale)!= 0) messaggio("errore creazione thread output");

    printf("Inserisci una stringa: ");

    char buffer[MAXSIZE];
    while (fgets(buffer,sizeof(buffer),stdin)!=NULL) {
        write(conn[0][1],buffer,MAXSIZE);
    }
    return 0 ;

}
