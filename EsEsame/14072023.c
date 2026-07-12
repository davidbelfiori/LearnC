 /*
SPECIFICATION TO BE IMPLEMENTED:
argomenti: filename, N thread, B byte
ogni thread legge B byte da stdin, quando tutti i thread hanno letto il
main process scrive i byte letti partendo da quelli letti dal Tn fino al T1
sul file (filename).
gestione sigint: contatore di sigint che va a stampare su stdout i byte
letti dal thread i-esimo (es: prima volta che ricevo sigint -> stampo
i byte letti dal primo thread)

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

#define fflsuh(stdin) while(getchar()!= '\n')


int numThreads,numByte;
sem_t *sem;
char **buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


volatile sig_atomic_t *datiProdotti;
volatile sig_atomic_t counter= 0;

typedef struct {
    int id;
}threadInfo;

void messaggio(char *msg) {
    printf("Errore %s \n",msg);
    exit(EXIT_FAILURE);
}

 void handler(int i) {
    //se il numero di quelli che hanno scritto è minore del numeo di thread

    if (counter <numThreads) {
        int id = counter;
        //se nella posizione iesima ci sono dei dati
        if (datiProdotti[id]==1) {
            //mostro i dati
            write(1,buffer[id],numByte);
        }else {
            char *msg = "no dati \n";
            write(1,msg,strlen(msg));
        }

        counter ++;
    }

}

 void * work(void * p) {

    threadInfo *info= (threadInfo *)p;

    ssize_t byteRead = 0;
    //faccio un lock perchè leggo solo io da stdin
    pthread_mutex_lock(&mutex);
    //se il numero di byte letti e minore del numero di byte richiesti
    while (byteRead < numByte) {
        //leggo da stdinput e lo inserisco nel buffer del mio id spiazzandomi di
        //quando byte ho letto e leggendo il numero che devo leggerne e quanti ne ho letti
        ssize_t r = read(STDIN_FILENO, buffer[info->id] + byteRead, numByte - byteRead);
        if (r == 0 || r == -1) break;
        byteRead += r;
    }

    //dico che ho scritto qualcosa
    datiProdotti[info->id]=1;
    //sblocco il mutex
    pthread_mutex_unlock(&mutex);
    //dico che ho scritto al main
    sem_post(&sem[info->id]);

}

int main(int argc, char* argv[]){


    if (argc < 4) messaggio("Uso ./nomeProgrmma fileName NumThread Byte");

    //prendo i nome del file il numeo dei thread e il numero dei byte
    char *nomeFile = argv[1];
    numThreads = atoi(argv[2]);
    numByte = atoi(argv[3]);


    //costruisco il gestore dei segnali
    struct sigaction sa;
    //la funziona che gestira i segnali
    sa.sa_handler = handler;
    //se ho interrotto una read , falla riparite
    sa.sa_flags=SA_RESTART;
    //dico che volgio ignorare tutti
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("Errore sigaction");

    //alloco un array du tid
    pthread_t *tid = malloc(numThreads*sizeof(pthread_t));
    //allocco un array di info per tenermi il loro id
    threadInfo *info= malloc(numThreads*sizeof(threadInfo));

    //essendo un vettotre di sig_t volatile devo aggiungerci dentro un numero
    datiProdotti = calloc(numThreads,sizeof(int));
    //un buffer globale dove metteo i byte letti
    buffer= malloc(numThreads*sizeof(char *));

    //semaforo per gestire il fatto che ho letto
    sem = malloc(numThreads*sizeof(sem_t));


    for (int i = 0; i < numThreads; i++) {
        //buffer era numero thread * 1 es 4 , solo che non sono 4 caratteri ma 4 spot per i caratteri
        //con calloc aumento lo spazio
        buffer[i]=calloc(numByte,sizeof(char));
        if (sem_init(&sem[i],0,0)==-1) messaggio("Errore nella creazione del semaforo");
    }


    for (int i = 0; i< numThreads; i++) {
        //passo l'id
        info[i].id = i;
        //creo n thread
        if (pthread_create(&tid[i],NULL,work,&info[i])==-1) messaggio("errore nella creazione del thread");
    }

    //aspetto che tutti abbiano letto;
    for (int i = 0; i <numThreads; i++) {
        sem_wait(&sem[i]);
    }
    //quando tutti hanno letto apro il file
    int fd = open(nomeFile,O_CREAT|O_TRUNC|O_RDWR,0666);

    if (fd <0) messaggio("Errore in apertura");
    //scrivo dall'ultimo al primo
    for (int i = numThreads-1 ; i>= 0;i-- ) {
        if (write(fd,buffer[i],numByte)!=numByte) messaggio("Errore");
        write(fd,"\n",strlen("\n"));
    }
    close(fd);



    return 0;
}
