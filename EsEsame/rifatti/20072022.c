//
// Created by davidjulianbelfiori on 15/07/26.
//


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
#include <wait.h>


/*
* Scrivere un programma che riceva in input tramite argv[] i nomi di N file,
con N magiore o uguale a 1. L'applicazione dovrà controllare che i nomi dei
file siano diversi tra di loro.
I file dovranno essere creati oppure troncati se esistenti.
Per ogniuno dei file dovra' essere attivato un nuovo processo, che indicheremo
con Pi, che gestirà il contenuto del file.
Tutti i processi Pi leggeranno linee di caratteri da standard input, se la linea
letta ha una lunghezza (incluso il terminatore di linea) inferiore oppure
uguale a 10 caratteri essa dovra' essere scritta all'interno del file gestito
al processo Pi lettore. Altrimenti la linea dovrà essere comunicata al processo
parent il quale la riporterà sullo standard output.

L'applicazione dovrà essere in grado di gestire il segnale SIGINT
(o CTRL_C_EVENT nel caso WinAPI) in modo tale che ogni processo che
verrà colpito riporti su standard output il contenuto corrente di tutti
i file che erano stati specificati in argv[], seguendo l'ordine tramite cui
i nomi dei file erano presenti negli argomenti forniti all'applicazione.
In ogni caso, nessuno dei processi dovrà terminare la sua esecuzione in caso di arrivo
della segnalazione.

 */
char **nomeFile;
int N;
sem_t *turni ;
void messaggio (char *p) {
 printf("errore: %s \n",p);
 exit(EXIT_FAILURE);
}



void handler(int in,siginfo_t *info, void *uncontext) {

 printf("Catturato , chiamato dal pid %d \n",info->si_pid);
 for (int i = 0 ; i < N ; i++) {

  int fd = open(nomeFile[i+1],O_RDONLY);
  if (fd < 0) messaggio("errore apertura");
  char buff[1024];
  ssize_t byteRead=0;
  printf("--------- FILE : %s ----- \n",nomeFile[i+1]);
  while ((byteRead = read(fd,buff,sizeof(buff)))>0) {
   write(1,buff,byteRead);
  }
 printf("-----------FINE -------------- \n");
  fflush(stdout);
  close(fd);
 }

}

int main (int argc , char **argv) {

 if (argc < 2) messaggio("errore d'uso");
 N = argc -1;
 for (int i = 1 ;i < argc; i++) {
     for (int j = i+1 ; j < argc ; j++) {
      if (strcmp(argv[i],argv[j])==0) messaggio("i nomi devono essere diversi");
     }

 }

 nomeFile = argv;

//OK I NOMI SONO DIVERSI , CREIAMO I FILE

 int *fd = malloc((argc-1)*sizeof(int));
 if (fd == NULL) messaggio("errore malloc");
 int tubo[2]; //per la comunicazione padre figlio
 if (pipe(tubo)==-1)messaggio("errore pipe");

 for (int i = 0 ; i < N; i++) {
   fd[i]= open(argv[i+1], O_CREAT|O_TRUNC|O_RDWR,0666);
  printf("descrittore file %s: %d \n",argv[i+1],fd[i]);
   if (fd[i]< 0) messaggio("errore nell'apertura del file");
 }

 turni = mmap(NULL,N*sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
 if (turni == NULL) messaggio("errore mmap");

 for (int i = 0 ; i < N ; i++) {
  if ( i == 0) {
   if (sem_init(&turni[i],1,1)==-1)messaggio("errore sem init");
  }else {
   if (sem_init(&turni[i],1,0)==-1)messaggio("errore sem init");
  }
 }

 struct sigaction sa ;
 sigemptyset(&sa.sa_mask);
 sa.sa_sigaction= handler ;
 sa.sa_flags = SA_RESTART | SA_SIGINFO;
if (sigaction(SIGINT,&sa,NULL)==-1)messaggio("sigaction");

 //GENERAZIONE FIGLI
 for (int i = 0 ; i < N ; i++) {

  pid_t pid = fork ();
  if (pid < 0) messaggio("errore fork");
  if (pid == 0) {
   signal(SIGINT,SIG_IGN);
   //chiudo la lettura , scrivo e basta;
   close(tubo[0]);
  while (1) {
   sem_wait(&turni[i]);
   char buff[1024];
   printf("p%d > \n",i);
    ssize_t byteRead = read(0,buff,sizeof(buff)-1);
    buff[byteRead]='\0';
   if (byteRead<= 0) break;
    if (strlen(buff)<=10) {
     write(fd[i],buff,strlen(buff));
    }else {
     printf("tropo lungo comunuico al padre \n");
     write(tubo[1],buff,strlen(buff));
    }
   sem_post(&turni[(i+1)%N]);
  }
   close(fd[i]);
   close(tubo[1]);
   exit(EXIT_SUCCESS);
  }

 }

 for (int i = 0 ; i < N; i++) {
  close(fd[i]);
 }

 close(tubo[1]);
 char buff[1024];
 ssize_t byte;

 while ((byte=read(tubo[0],buff,sizeof(buff)-1))>0) {
    buff[byte]='\0';
    printf("%s \n",buff);
  fflush(stdout);
 }

 for (int i = 0 ; i < N; i++) {
  wait(NULL);
 }
 return 0;
}