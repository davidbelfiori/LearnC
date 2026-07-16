//
// Created by davidjulianbelfiori on 14/07/26.
//
/*
SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che riceva in input, tramite argv[], il nome
di un file F e un insieme di N stringhe (con N almeno pari ad 1). Il programa dovra' creare
il file F e popolare il file con le stringhe provenienti da standard-input.
Ogni stringa dovra' essere inserita su una differente linea del file.
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito si
dovranno  generare N processi concorrenti ciascuno dei quali dovra' analizzare il contenuto
del file F e verificare, per una delle N stringhe di input, quante volte la inversa di tale stringa
sia presente nel file. Il risultato del controllo dovra' essere comunicato su standard
output tramite un messaggio. Quando tutti i processi avranno completato questo controllo,
il contenuto del file F dovra' essere inserito in "append" in un file denominato "backup"
e poi il file F dova' essere troncato.

Qualora non vi sia immissione di input o di segnali, l'applicazione dovra' utilizzare
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
#include<wait.h>
#include <string.h>
volatile sig_atomic_t segnale = 0 ;
char *nomeFile;

void handler (int i ) {
    segnale = 1;
}

void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}

void revStr(char * str, char * rev) {

    int len = strlen (str);
    for (int i = 0 ; i < len ; i++) {
        rev[i]=str[len-1-i];
    }

    rev[len] = '\0';
    printf("rev: %s \n",rev);
}

int main (int argc , char **argv) {

    if (argc < 3) messaggio("errore d'uso");
    nomeFile = argv[1];

    int N = argc -2;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("errore sigaction");

    int fd = open(nomeFile , O_CREAT|O_TRUNC|O_RDWR,0666);
    if (fd < 0) messaggio("errore apertura file");
    char buff[1024];
    while (1) {
        if (segnale == 1) {
            fsync(fd);
            printf("catturato \n");
            for (int i = 0 ; i < N ; i++) {
                pid_t pid = fork();
                if (pid < 0) messaggio("errore fork ");
                if (pid == 0) {
                    char rev[1024];
                    revStr(argv[i+2],rev);
                    int fdSorg = open(nomeFile , O_RDONLY);
                    char localBuff[1024];
                    char c;
                    int idx = 0;
                    int contatore = 0;
                    while (read(fdSorg,&c,1)>0) {
                        if (c==' '|| c=='\n'||c=='\t') {
                            if (idx > 0) {
                                localBuff[idx]='\0';
                                if (strcmp(rev,localBuff)==0) {
                                    contatore++;
                                }

                            }
                            idx = 0;
                        }else {
                            localBuff[idx++]=c;
                        }
                    }
                    close(fdSorg);
                    printf("parola da trovare %s occorrenze %d \n",rev,contatore);
                    fflush(stdout);
                    exit(EXIT_SUCCESS);
                }
            }

            printf("-------INIZIO BACKUP----- \n");
            char backup[512];
            snprintf(backup,sizeof(backup),"backup_%s.txt",nomeFile);
            int fdB = open(backup,O_CREAT|O_WRONLY,0666);
            char localBuff[1024];
            ssize_t byteRead = 0 ;
            lseek(fd,0,SEEK_SET);
            while ((byteRead = read(fd,localBuff,sizeof(localBuff)))>0) {
                write(fdB,localBuff,byteRead);
            }
            printf("-------FINE BACKUP----- \n");


            segnale = 0;
        }else{
            ssize_t byteRead = read(0,buff,sizeof(buff));
            if (byteRead <= 0) {
                if (errno == EINTR) {
                    continue;
                }else {
                    close(fd);
                    messaggio("errore lettura");
                }
            }
            write(fd,buff,byteRead);
        }

    }
}