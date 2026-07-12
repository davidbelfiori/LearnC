/*SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che riceva in input, tramite argv[], il nome
di un file F. Il programa dovra' creare il file F e popolare il file
con lo stream proveniente da standard-input. Il programma dovra' generare
anche un ulteriore processo il quale dovra' riversare il contenuto del file F
su un altro file denominato shadow_F, inserendo mano a mano in shadow_F soltanto
i byte che non erano ancora stati prelevati in precedenza da F. L'operazione
di riversare i byte di F su shadow_F dovra' avvenire con una periodicita' di 10
secondi.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando un qualsiasi processo (parent o child) venga colpito si
dovra' immediatamente riallineare il contenuto del file shadow_F al contenuto
del file F sempre tramite operazioni che dovra' eseguire il processo child.

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
#include <sys/wait.h>

ssize_t byteReadFiglio ;
char *nomeSorgente;
char nomeShadow[512];
volatile sig_atomic_t segnale;

void messaggio(char *p) {
    perror(p);
    exit(EXIT_FAILURE);
}
void handler ( int i) {
    printf("---- Catturato --- \n");
    segnale = 1;
}
int main(int argc, char *argv[]) {

    if (argc <2) messaggio("uso ./nomeProgramma nomeFile");

    nomeSorgente = argv[1];

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore sigaction");

    int fdSorg = open(argv[1],O_CREAT|O_TRUNC|O_WRONLY,0666);

    if (fdSorg < 0 ) messaggio("errore nella aperutra del file sorgente");

    pid_t figlio = fork();

    if (figlio < 0) messaggio("errore nella fork");
    if (figlio == 0) {

        //! figlio
        snprintf(nomeShadow,sizeof(nomeShadow),"shadow_%s",argv[1]);

        int fdShadow = open(nomeShadow,O_CREAT|O_TRUNC|O_WRONLY,0666);
        if (fdShadow <0) messaggio("Errore creazione file shadow");
        int fdSorgente = open(argv[1],O_RDONLY);
        if (fdSorgente < 0) messaggio("Errore apertura file sorgente del figlio");
        byteReadFiglio = 0;
        char buff[1024];
        while (1) {
            if (segnale == 1) {
                while ((byteReadFiglio = read(fdSorgente,buff,sizeof(buff)))>0) {
                    write(fdShadow,buff,byteReadFiglio);
                }
                segnale = 0;
            }else {
                sleep(10);
                while ((byteReadFiglio = read(fdSorgente,buff,sizeof(buff)))>0) {
                    write(fdShadow,buff,byteReadFiglio);
                }
            }
        }
        close(fdShadow);
        close(fdSorgente);


    }else if (figlio > 0) {

        //! padre
        ssize_t byteRead = 0;
        char buff[1024];
        while ((byteRead = read(0,buff,sizeof(buff)))> 0) {
                write(fdSorg,buff,byteRead);
                fsync (fdSorg);
        }

        wait(NULL);
        close(fdSorg);

    }



}