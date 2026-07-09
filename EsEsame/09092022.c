/*
 *
Implementare una programma che riceva in input, tramite argv[], i nomi
di N differenti file F1 ed FN, con N maggiore o uguale a 1, che dovranno essere creati.
Per ognuno dei file dovra' essere attivato un nuovo thread che ne gestira' il contenuto
(indichiamo quindi con T1 ... TN i thread che dovranno essere attivati).
A turno secondo una regola circolare ogni thread Ti dovra' leggere 5 caratteri dallo stream
di standard input e dovra' scriverli sul file che sta gestendo.
La scrittura dei 5 caratteri su ciascuno dei file deve risultare come una azione atomica,
ovvero i caratteri non possono essere scritti sui file individualmente.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito esso dovra'
riportare su standard output i 5 ultimi caratteri correntemente presenti
su ciascuno degli N file gestiti.

In caso non vi sia immissione di dati sullo standard input, e non vi siano segnalazioni,
l'applicazione dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.
 *
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

typedef struct {

    int id;
    int fd;
    char *nomeFile;

}ThreadInfo;

#define fflush(stdin) while(getchar() != '\n')

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *semafori;
int numThreads = 0;

int turno = 0;

void messaggio(char *msg) {

 printf("Messaggio: %s",msg);
 exit(EXIT_FAILURE);

}

void *work(void *arg) {
    ThreadInfo *info = (ThreadInfo *)arg;
    char buff[5];
    int prossimoturno = ((info->id) + 1) % numThreads;

    while (1) {
        // 1. Aspetto il mio turno
        sem_wait(&semafori[info->id]);

        int letti = 0;
        int interrotto = 0;

        // 2. Accumulo i caratteri fino a 5
        while (letti < 5) {
            ssize_t r = read(0, buff + letti, 5 - letti);

            if (r < 0) {
                // Gestione errore di lettura
                interrotto = 1;
                break;
            }
            if (r == 0) {
                // Gestione EOF (L'utente ha chiuso l'input o premuto CTRL+D)
                interrotto = 1;
                break;
            }

            letti += r;
        }

        fflush(stdin);

        // 3. Scrivo SOLO se ho tutti e 5 i caratteri
        if (!interrotto && letti == 5) {
            pthread_mutex_lock(&mutex);
            write(info->fd, buff, 5);
            pthread_mutex_unlock(&mutex);
        } else {
            // Se l'input è finito o c'è un errore, passo il turno un'ultima volta
            // per cortesia istituzionale verso gli altri thread e poi chiudo questo thread.
            sem_post(&semafori[prossimoturno]);
            break;
        }

        // 4. Passa il turno in modo pulito al thread successivo per il prossimo giro
        sem_post(&semafori[prossimoturno]);
    }

    return NULL;
}


int main (int argc ,char *argv[]) {

  if (argc < 2) {
    messaggio("uso ./nomeProgramma file1 file2 ... fileN");
  }
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGINT);

    pthread_sigmask(SIG_BLOCK,&set,NULL);

    pthread_t *tid= malloc((argc-1)*sizeof(pthread_t));
    ThreadInfo *info = malloc((argc-1)*sizeof(ThreadInfo));
    semafori = malloc((argc-1)*sizeof(sem_t));

    numThreads= (argc-1);

    for (int i=0; i<(argc-1);i++) {

        if (i==0) {
            sem_init(&semafori[i],0,1);
        }else {
            sem_init(&semafori[i],0,0);
        }

        int fdFile = open(argv[i+1],O_RDWR|O_APPEND|O_CREAT,0666);
        if (fdFile < 0) messaggio("errore nell'apertura del file \n");
        info[i].fd=fdFile;
        info[i].nomeFile = argv[i+1];
        info[i].id = i;

        pthread_create(&tid[i],NULL,work,&info[i]);
    }

    int sig;
    while (1) {
        //RIMANGO IN ATTESA DI UN EVENTO
        sigwait(&set,&sig);

        if (sig == SIGINT) {

            printf("SIGINT CATTURATA");
            pthread_mutex_lock(&mutex);
            for (int i = 0; i<numThreads;i++) {
                char buff[5];
                int fd = info[i].fd;

                off_t grandezza = lseek(fd,0,SEEK_END);
                lseek(fd,-5,SEEK_END);
                read(fd,buff,5);

                lseek(fd,0,SEEK_END);

                printf("Ultimi 5 byte del file %s : %s \n",info[i].nomeFile,buff);

            }
            printf("Fine \n");
            pthread_mutex_unlock(&mutex);
        }
    }

    for (int i = 0; i<numThreads; i++) {
        pthread_join(tid[i],NULL);
        sem_destroy(&semafori[i]);
        close(info[i].fd);
    }

    free(tid);
    free(info);
    free(semafori);
    return 0;

}