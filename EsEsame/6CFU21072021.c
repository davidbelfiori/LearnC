/*
* Implementare una programma che riceva in input, tramite argv[], il nome
di un file F e un insieme di N stringhe (con N almeno pari ad 1). Il programa dovra' creare
il file F e popolare il file con le stringhe provenienti da standard-input.
Ogni stringa dovra' essere inserita su una differente linea del file.
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo venga colpito si
dovranno  generare N thread concorrenti ciascuno dei quali dovra' analizzare il contenuto
del file F e verificare, per una delle N stringhe di input, quante volte tale stringa
sia presente nel file. Il risultato del controllo dovra' essere comunicato su standard
output tramite un messaggio. Quando tutti i thread avranno completato questo controllo,
il contenuto del file F dovra' essere inserito in "append" in un file denominato "backup"
e poi il file F dova' essere troncato.
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
#include<wait.h>
#include <string.h>

volatile sig_atomic_t segnale = 0;
char *nome;


void handler (int sig) {
    segnale = 1;

}



void messaggio(char *messaggio) {
    printf("Errore: %s \n",messaggio);
    exit(EXIT_FAILURE);
}

void * work(void * p) {
    char *str = (char *)p;

    FILE *f = fopen(nome,"r");
    if (f == NULL)messaggio("Errore nell'apertura del file");

    char buff[1024];
    int contatore = 0;
    while (fgets(buff,sizeof(buff),f) != NULL) {
        buff[strcspn(buff,"\n")]='\0';
        if (strcmp(str,buff)==0) {
            contatore ++;
        }

    }
    fclose(f);

    printf("Numero di occorrenze trovate della parola %s : %d \n",str,contatore);
    return NULL;

}

void backup(void) {
    char *nomeB = "backup.txt";
    int fdSorg = open(nome,O_RDONLY);
    if (fdSorg <0) messaggio("errore apertura sorgente");
    int fdDest = open(nomeB,O_CREAT|O_WRONLY|O_TRUNC,0666);
    if (fdDest < 0) messaggio("errore apertura destinazione");

    ssize_t byteRead = 0;
    char  buff[1024];
    while ((byteRead = read(fdSorg,buff,sizeof(buff)))> 0) {
        write(fdDest,buff,byteRead);
    }

    if (truncate(nome,0)==-1) messaggio("errore nel troncamento");

    close(fdSorg);
    close(fdDest);

    printf("Backup terminato \n");
    return;


}

int main (int argc , char *argv[]) {

    if (argc < 3) messaggio("Uso ./nomeProgramma nomeFile s1 ... sN");

    nome = argv[1];
    int N = argc -2;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("Errore sigasction");

    int fd = open(nome,O_CREAT|O_WRONLY|O_TRUNC,0666);
    if (fd <0) messaggio("errore nell'apertura del file");
    char buffer[1024];
    while (1) {
        if (segnale == 1) {
            printf("catturato \n");

            pthread_t *tid = malloc(N*sizeof(pthread_t));

            for (int i = 0; i < N ; i++) {
                if (pthread_create(&tid[i],NULL,work,argv[i+2])!=0) messaggio("errore generazione thread");
            }

            for (int i = 0; i < N ; i++) {
                pthread_join(tid[i],NULL);
            }
            segnale = 0;
            printf("i thread hanno finito eseguo back up \n");
            backup();


        }else {
            ssize_t byteRead = read(0,buffer,sizeof(buffer));
            if (byteRead > 0) {

                buffer[byteRead]= '\0';
                char *delimitatori = " \t\n";
                char *token = strtok(buffer,delimitatori);
                while (token != NULL) {
                    write(fd,token,strlen(token));
                    write(fd,"\n",strlen("\n"));
                    token = strtok(NULL,delimitatori);
                }
            }else if (byteRead < 0) {
                if (errno == EINTR) {
                    continue;
                }

            }else {
                break;
            }

        }


    }



}
