//
// Created by davidjulianbelfiori on 02/07/26.
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

volatile sig_atomic_t sigint_flag  = 0;

void messaggio(char *messaggio) {
    printf("Errore: %s \n",messaggio);
    exit(EXIT_FAILURE);
}

void handler(int sig) {
    sigint_flag = 1;
}

void strRev (char *arg,char *rev) {

    int len = strlen(arg);
    for (int i = 0; i< len; i++) {
        rev[i] = arg[len-1-i];
    }

    rev[len]='\0';
};

int main ( int argc , char *argv[]) {


    if (argc <3) messaggio("Uso ./nomeProgramma nomeFile stringa1 stringa2 ....");

    //Nome del file passato da cli
    char *nomeFile = argv[1];
    //Numero di processi da spownare
    int N = argc -2;

    struct sigaction sa;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;

    if (sigaction(SIGINT,&sa,NULL) == -1) messaggio("errore nell impostazione della sigaction");

    int fdSorgente = open(nomeFile,O_RDWR | O_CREAT | O_APPEND|O_TRUNC,0666);
    if (fdSorgente <0) messaggio("Errore nell'apertura del file");

    char bufferInput[1024];

    ssize_t byteRead = 0;

    while (1) {
        if (sigint_flag==1) {
            sigint_flag= 0;
            printf("-----------SIGINT CATTURATO----------- \n");
            printf("--------------------------------------- \n");
            //chiedo di sincornizzare il buffer cache e il file
            fsync(fdSorgente);

            for (int i= 0; i<N;i++) {

                pid_t pid = fork();
                if (pid <0) messaggio("errore nella creazione del i-esimo figlio");
                if (pid == 0) {

                    char rev[256];
                    strRev(argv[i+2],rev);

                    int len_rev = strlen(rev);
                    int fdFiglio = open(nomeFile,O_RDONLY);
                    if (fdFiglio <0) messaggio("errore nell'apertura del figlio");
                    int occorrenze = 0;
                    char line[1024];
                    while ((byteRead = read(fdFiglio,line,sizeof(line)))>0) {

                            char *temp=line;
                            while ((temp = strstr(temp,rev)) != NULL) {
                                occorrenze++;
                                temp+=len_rev;
                            }
                    }
                    close(fdFiglio);
                    printf("Occorrenze trovate da pid %d , figlio di %d , della stringa %s: %d \n",getpid(),getppid(),argv[i+2],occorrenze);
                    exit(EXIT_SUCCESS);
                }
            }

            //il padre aspetta che i figli abbiano terminato

            for (int i =0 ; i<N ;i++) {
                wait(nullptr);
            }

            int backupFd = open("backup",O_WRONLY|O_CREAT|O_APPEND,0666);
            if (backupFd <0) messaggio("errore nell'aperutra del file di backup");
            lseek(fdSorgente,0,SEEK_SET);
            ssize_t byteRead = 0;
            char backupBuff[1024];
            while ((byteRead = read(fdSorgente,backupBuff,sizeof(backupBuff)))>0) {
                write(backupFd,backupBuff,byteRead);
            }
            close(backupFd);
            ftruncate(fdSorgente,0);
            close(fdSorgente);
        }

            byteRead = read(0,bufferInput,sizeof(bufferInput));
            if (byteRead > 0) {
                write(fdSorgente,bufferInput,byteRead);
            }else if(byteRead <0) {
                if (errno == EINTR) {
                    continue;
                }else {
                    messaggio("errore nella write");
                }
            }else {
                break;
            }






    }




}