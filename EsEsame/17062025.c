/*SPECIFICATION TO BE IMPLEMENTED:

Scrivere un programma che riceva in input tramite argv[] le seguenti due stringhe:
1) numthreads in argv[1]
2) filename in argv[2]

Il programma deve creare (o troncare se gia' esistente) il file filename.
Il programma deve poi attivare numthreads thread, ciascuno dei quali legge
indefinitamente linee provenienti dallo standard input.

Quando numthreads linee sono state lette, una per ciascun thread,
esse devono essere scritte in ordine inverso rispetto alla loro immissione
da standard input all'interno del file dal nome filename. Ogni thread deve
eseguire la scrittura della sola linea da lui acquisita da standard input.

Quindi supponendo di avere numthreads indicante il valore 2, e immettendo
in input le 4 linee L1, L2, L3 ed L4, queste dovranno comparire all'interno
del file filename nel seguente ordine L2, L1, L4, L3.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI)
in modo tale che quando il processo venga colpito esso dovra' ripresentare su
standard output le linee originariamente lette da standard input recuperandole
dal file filename, ricostruendo l'ordine di arrivo di tali linee
dallo standard input.

Quindi considerando il precedente esempio, l'arrivo
della segnalazione deve dar luogo a una attivita' di accesso al file
filename in modo da ripresentare le linee L2, L1, L4, L3 registrate su tale
file nell'ordine originario L1, L2, L3, L4.

In caso non vi sia immissione di dati sullo standard input e non vi siano
segnalazioni, l'applicazione dovra utilizzare non piu' del 10% della capacita' 62 di lavoro della CPU.

*/

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
#define MAX_LINE 1024

sem_t *turno;
char *nomeFile ;
int N;


void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}



void * work(void * p) {

    int id = (int)p;
    char linea[MAX_LINE];

    while (1) {

        sem_wait(&turno[id]);
        if (fgets(linea,MAX_LINE,stdin)== NULL) {

            //nel caso fallisce la fgets , se non sono l'ultimo passo il testimone a quello dopo
            if (id < N-1) {
                sem_post(&turno[id+1]);
            }else {

                sem_post(&turno[0]);

            }
            break;
        }
        // SE E ANDATO TUTTO BENE E HO LA LINEA VERIFICO SE SONO L'ULTIMO E PASSO IL TURNO T0-> T1 ->T2 LEGGONO T2->T1->T0

        if (id < N-1) {
            sem_post(&turno[id+1]);
        }else {

            sem_post(&turno[N-1]);

        }

        //aspetto il mio turno per scrivere;
        sem_wait(&turno[id]);

       int fd = open(nomeFile,O_CREAT|O_WRONLY|O_APPEND,0666);
        if (fd < 0) messaggio("Errore file");

        write(fd,linea,strlen(linea));

        if (id > 0) {
            sem_post(&turno[id-1]);
        }else {
            sem_post(&turno[0]);
        }
    }

    return NULL;
}

int main(int argc , char *argv[]) {

    if (argc < 3) messaggio("errore");

    N = atoi(argv[1]);
    nomeFile = argv[2];

    turno = malloc(N*sizeof(sem_t));

    for (int i = 0 ; i < N ; i++) {
        if (i == 0) {
            sem_init(&turno[i],0,1);
        }else {
            sem_init(&turno[i],0,0);
        }
    }

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig,SIGINT);
    pthread_sigmask(SIG_BLOCK,&sig,NULL);

    pthread_t *tid = malloc(N*sizeof(pthread_t));

    for (int i = 0; i < N ; i++) {
        if (pthread_create(&tid[i],NULL,work,(void *)i)!=0) messaggio("errore creazione thread");
    }


    while (1) {
        int segnale ;
        sigwait(&sig,&segnale);

        if (segnale == SIGINT) {

            printf("------CATTURATO ---- \n");

            char buff[N][MAX_LINE];
            FILE *f = fopen(nomeFile,"r");
            if (f == NULL) messaggio("Errore apertura");



            while (1) {

                int lineRead = 0;
                for (int i = 0 ; i < N ; i++) {
                   if (fgets(buff[i],MAX_LINE,f)!= NULL) {
                       lineRead++;
                   }else {
                       break;
                   }
                }
                printf("lineRead %d \n",lineRead);

                if (lineRead == 0) break;
                for (int i = lineRead -1 ; i >=0 ; i--) {
                    printf("%s \n",buff[i]);
                }

            }
            fclose(f);
        }

    }

    for (int i = 0; i < N ; i++) {
        pthread_join(tid[i],NULL);
    }

    free(tid);
    for (int i = 0; i < N ; i++) {
        sem_destroy(&turno[i]);
    }



}
