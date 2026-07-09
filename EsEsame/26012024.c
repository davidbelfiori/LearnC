/*svolgi questo esame : Scrivere un programma che riceva in input tramite argv[] un insime di N
stringhe, con N maggiore o uguale ad 1, che indicheremo con S1 ... SN.
Il main thread del programma dovra' leggere indefinitamente stringhe dallo standard input
e per ogni nuova stringa letta S dovra' attivare un nuovo thread, che riferiamo come T,
passando a questo nuovo thread la stringa S letta come parametro.
Questo thread T dovra' controllare se la stringa S sia uguale a ciascuna
delle N stringhe S1 ... SN originariamente ricevute in input dal programma. Per ogni
stringa trovata uguale all'atto del controllo, dovra' essere incrementato un contatore
apposito (si presuppne quindi che il programma gestisca N di questi contatori, uno per
ognuna delle stringhe S1 ... SN ricevute tramite argv[]).

L'applicazione dovra' gestire il segnale  SIGINT (o CTRL_C_EVENT nel caso WinAPI)
in modo tale che quando il processo venga colpito il suo main thread dovra'
riportare su standard output il valore degli N contatori, su linee diverse dello
stream di output, associando nel messaggio di output ciascuno dei valori alla relativa
stringa S1 ... SN. La stessa informazione dovra' essere scritta all'interno di un file
dal nome "output.txt" in modo che questo contenga sempre e solo i valori dei contatori
piu' aggionati all'atto del processamento del segnale ricevuto.*/

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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

volatile sig_atomic_t segnale = 0;

int N;
char **str;
//ad ogni entry associo il contatore della i-esima stringa;
int *contatore;
char *fileName = "output.txt";


void messaggio(char *p) {
    printf("Errore %s \n",p);
    exit(EXIT_FAILURE);
}

void handler (int sig) {

    printf("-----Sono stato attivato ----");
    for (int i=0 ;i<N;i++) {
        printf("%s -> %d \n",str[i],contatore[i]);
    }
    printf("---scrivo su file---- \n");
    int fd = open(fileName,O_CREAT|O_WRONLY|O_TRUNC,0666);
    if (fd <0) messaggio("errore creazione del file");
    char  linea[1024];
    for (int i=0 ;i<N;i++) {
        snprintf(linea,sizeof(linea),"%s -> %d \n",str[i],contatore[i]);
        write(fd,linea,strlen(linea));
    }
    printf("----Fine scrittura su file----- \n");
}


void * work(void * p) {

    char *s = (void *)p;
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,nullptr);

    for (int i = 0; i<N;i++) {
        if (strcmp(s,str[i])==0) {
            pthread_mutex_lock(&mutex);
            contatore[i]+=1;
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_sigmask(SIG_UNBLOCK,&sig,nullptr);
    return NULL;

}

int main(int argc, char *argv[]) {

    if (argc<2) messaggio("Uso ./nomeProgramma s1 s2 s3 ... sn");

     N = argc -1;
    str = malloc(N*sizeof(char *));
    contatore = malloc(N*sizeof( int));

    for (int i = 0; i < argc-1; i++) {
        str[i] = argv[i+1];
        contatore[i] = 0;
    }


    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT,&sa,NULL) == -1) messaggio("Errore nella sigaction");

    char  buff[1024];
    pthread_t tid;

    while (read(0,buff,sizeof(buff))> 0) {
            buff[strcspn(buff,"\n")]='\0';
            if (pthread_create(&tid,NULL,work,(void *)buff)!=0) messaggio("errore nella creazione del thread");
    }



}
