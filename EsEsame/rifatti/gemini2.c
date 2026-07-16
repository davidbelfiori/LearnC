/*
* Implementare un programma in C che riceva in input, tramite argv[], i nomi
 * di N differenti file di testo F1 ... FN, con N maggiore o uguale a 2.
 * All'avvio, i file dovranno essere creati oppure, se gia' esistenti, troncati
 * a dimensione zero.
 * * Il processo originale (padre) dovra' generare N processi figli (P1 ... PN).
 * Ciascun processo Pi sara' associato all'i-esimo file passato come argomento.
 * I processi dovranno competere per acquisire righe di testo provenienti
 * dallo standard input (sequenze di caratteri di lunghezza arbitraria terminate
 * dal carattere '\n'), operando in un turno rigorosamente circolare
 * (P1 -> P2 -> ... -> PN -> P1).
 * * Quando arriva il suo turno, il processo Pi dovra' leggere un'intera riga
 * in modo atomico dallo standard input e scriverla all'interno del proprio file
 * di competenza.
 * * L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT) in modo tale
 * che, alla ricezione del segnale, il processo originale stampi sullo standard
 * output il numero totale di righe lette e scritte complessivamente da tutti
 * i processi fino a quel momento. Tutti gli altri processi (i figli) non dovranno
 * eseguire alcuna attivita' particolare e dovranno ignorare la segnalazione,
 * continuando la loro esecuzione senza interruzioni.
 * * In caso non vi sia immissione di dati sullo standard input, e non vi siano
 * segnalazioni, l'applicazione dovra' utilizzare non piu' del 5% della capacita'
 * di lavoro della CPU.
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
#include <sys/wait.h>

sem_t *turno;
int *lineeProcesso;
int N;

void messaggio(char *p) {

 printf("errore: %s \n",p);
 exit(EXIT_FAILURE);

}

void handler(int sig) {
 printf("CATTURATO \n");
 int totale = 0;
 for (int i = 0; i < N ; i++) {
    totale += lineeProcesso[i];
 }
 printf("Il numero di linee lette fino ad ora: %d \n",totale);
}

int main (int argc , char **argv) {

 if (argc < 2) messaggio("errore d'uso");

 for (int i = 1 ; i < argc; i++) {
  for (int j = i+1 ;j < argc; j++) {
   if (strcmp(argv[i],argv[j])==0) messaggio("i nomi dei file devono essere diversi");
  }
 }

  N = argc -1;
 printf("numero file gestiti: %d \n",N);
int *fd = malloc(N *sizeof(int));
 for (int i = 0 ; i < N ; i++) {
  fd[i] = open(argv[i+1],O_CREAT|O_TRUNC|O_WRONLY,0666);
  if (fd[i] < 0 ) messaggio("errore apertura file");
 }

 turno = mmap(NULL,N*sizeof(sem_t),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANON,-1,0);
 if (turno == NULL) messaggio("errore mmap turno");

 lineeProcesso = mmap(NULL,N*sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANON,-1,0);
 if (lineeProcesso==NULL) messaggio("errore mmap linee");

 for (int i = 0 ; i < N; i++) {
  if (i == 0) {
   sem_init(&turno[i],1,1);
  }else {
   sem_init(&turno[i],1,0);
  }
 }

 for (int i = 0 ; i < N ; i++) {

  pid_t pid = fork();
  if (pid < 0) messaggio("errore fork");
  if (pid ==0) {
   //FIGLIO
   signal(SIGINT, SIG_IGN);
   char buffer[1024];

   while (1) {
    sem_wait(&turno[i]);
    printf("P%d > \n",i);
    ssize_t byteRead = read(0,buffer,sizeof(buffer)-1);
    if (byteRead <= 0 ) {
     sem_post(&turno[(i+1)%N]);
     exit(0);
    }
    buffer[byteRead]='\0';
    write(fd[i],buffer,byteRead);
    lineeProcesso[i]++;
    printf("lineeScirtte dal processo %d %d \n",i,lineeProcesso[i]);
    sem_post(&turno[(i+1)%N]);
   }
  }

 }

 struct sigaction sa;
 sigemptyset(&sa.sa_mask);
 sa.sa_handler = handler;
 sa.sa_flags = SA_RESTART;

 if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("sigacion");

 for (int i = 0 ;i <N ; i++) {
  wait(NULL);
 }

 for (int i = 0 ;i <N ; i++) {
  sem_destroy(&turno[i]);
 }

 munmap(turno, N * sizeof(sem_t));
 munmap(lineeProcesso, N * sizeof(int));

 return 0;

}