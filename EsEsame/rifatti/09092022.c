    //
// Created by davidjulianbelfiori on 15/07/26.
//
/*
SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che riceva in input, tramite argv[], i nomi
di N differenti file F1 ... FN, con N maggiore o uguale a 1, che dovranno essere creati
o troncati se gia' esistenti.
Per ognuno dei file dovra' essere attivato un nuovo processo che ne gestira' il contenuto
(indichiamo quindi con P1 ... PN i processi che dovranno essere attivati).
Ciascun processo Pi leggera' dallo standard input 5 caratteri per volta in modo atomico
rispetto alle attivita' degli altri processi, e dovra' scriverli sul file che sta gestendo.
Anche la scrittura dei 5 caratteri sul file destinazione deve risultare come un'azione atomica,
ovvero i caratteri non possono essere scritti sui file individualmente.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo originale venga colpito esso dovra'
riportare su standard output i 5 ultimi caratteri correntemente presenti
su ciascuno degli N file gestiti. Tutti gli altri processi non dovranno
eseguire alcuna attivita' in caso di arrivo di segnalazione.

In caso non vi sia immissione di dati sullo standard input, e non vi siano segnalazioni,
l'applicazione dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.

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
#include <sys/wait.h>
int N;
char **nomiFile;
sem_t *turno;

void messaggio(char *p) {
    printf("Errore: %s \n",p);
    exit(EXIT_FAILURE);
}

void handler (int sig) {
    printf("CATTURATO \n");

    for (int i = 0 ; i < N ; i++) {
        int fd = open(nomiFile[i+1],O_RDONLY);
        if (fd < 0) messaggio("errore apertura file");
        printf("ULTIMI 5 BYTE DEL FILE %s : \n",nomiFile[i+1]);
        lseek(fd,-5,SEEK_END);
        char buff[5];
        read(fd,buff,5);
        printf("%s \n",buff);

    }


}

int main (int argc , char **argv) {

    if (argc < 2) messaggio("errore d'uso");

    for (int i = 1 ; i < argc; i++) {
        for (int j =i+1;j < argc; j++ ) {
            if (strcmp(argv[i],argv[j])==0) messaggio("devono avere nomi diversi");
        }
    }
    N = argc -1;
    nomiFile = argv;
    int *fd = malloc(N*sizeof(int));
    if (fd == NULL) messaggio("allocazione malloc fd");

    turno = mmap(NULL,N*sizeof(sem_t),PROT_WRITE|PROT_READ,MAP_ANON|MAP_SHARED,-1,0);
    if (turno == NULL) messaggio("errore mmap");

    for (int i = 0  ;i < N ; i++) {
        if (i ==0) {
            sem_init (&turno[i],1,1);
        }else {
            sem_init(&turno[i],1,0);
        }
    }

    for (int i = 0; i < N ; i++) {
        fd[i] = open(argv[i+1],O_CREAT|O_TRUNC|O_WRONLY,0666);
        if (fd[i]< 0) messaggio("apertura fd");
    }

    for (int i = 0 ; i < N ; i++) {

        pid_t pid = fork();
        if (pid < 0) messaggio("errore fork");
        if (pid ==0) {
            //FIGLIO
            signal(SIGINT,SIG_IGN);
            char buff[6]; //uno in più per il terminatore di stringa

            while (1) {
                sem_wait(&turno[i]);
                ssize_t byteLetti = 0;
                printf("P%d > \n",i);
                fflush(stdout);
                while (byteLetti <5) {
                    char c;
                    ssize_t r = read (0,&c,1);
                    if (r <= 0) {
                        sem_post(&turno[(i+1)%N]);
                        close(fd[i]);
                        exit(0);
                    }
                    if (c == '\n') continue;
                    buff[byteLetti]=c;
                    byteLetti++;
                }
                write(fd[i],buff,5);
                sem_post(&turno[(i+1)%N]);
            }


        }
    }


    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler ;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore sigaction");

    for (int i = 0 ;i <N ; i++) {
        wait(NULL);

    }
}