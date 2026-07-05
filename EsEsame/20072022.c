//
// Created by davidjulianbelfiori on 04/07/26.
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

char **globalArgv;
int globalArgc;

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


void messaggio(char *msg) {
 printf("errore: %s \n",msg);
 exit(EXIT_FAILURE);
}


void handler(int sig) {

 printf("----SIGINT RECUPERATO---");

 for (int i=1;i<globalArgc;i++) {
   int fd= open(globalArgv[i],O_RDONLY);
   if (fd <0) messaggio("errore di apertura dentro al handler");
   ssize_t byteLetti = 0;
  char *msg= "-------Il contenuto del file ";
  write(0,msg,strlen(msg));
  write(0,globalArgv[i],strlen(globalArgv[i]));
   char buff[1024];
   while ((byteLetti = read(fd,buff,sizeof(buff)))>0) {
       write(0,buff,strlen(buff));
   }
  close(fd);

 }
 printf("---FINE DEI FILE---");

}

int main(int argc, char *argv[]) {

 // --- CONTROLLO DEI NOMI DEI FILE ----
 if (argc<2) messaggio("Uso ./nomeProgramma file1 file2 ... fileN");

 for (int i = 1 ; i< argc; i++) {
    for (int j = i+1 ; j<argc ; j++) {
     if (strcmp(argv[i],argv[j])==0) {
      messaggio("i nomi dei file devono essere diversi");
     }
    }
 }

 //per il sigint
  globalArgv = argv;
  globalArgc = argc;

 //apertura dei file e creazione
 for (int i= 1; i<argc; i++) {
   int fd = open(argv[i],O_CREAT|O_TRUNC|O_WRONLY,0644);
   if (fd <0) messaggio("Errore nell'apertura del file");
 }


 //creazione della sigaction
 struct sigaction sa;
 sa.sa_handler=handler;
 sigemptyset(&sa.sa_mask);
 sa.sa_flags = SA_RESTART;

 sigaction(SIGINT,&sa,0);



 //pipe per la comunicazione tra il padre e i figli
 int pipe_fd[2];

 if (pipe(pipe_fd) == -1) messaggio("Errore nell'apertura della pipe");

 int N = argc-1;
 for (int i=1; i<argc; i++) {
  pid_t pid = fork();

  if (pid <0) messaggio("errore nella creazione del figlio");
  if (pid ==0) {
   //-------------Figlio----------
   close(pipe_fd[0]); // chiudo la scrittura da parte del figlio

   int fdDest = open(argv[i],O_WRONLY|O_APPEND,0644);
   if (fdDest < 0) messaggio("Errore nell'apertura del file del figlio");

   char buff[1024];

   while (fgets(buff,sizeof(buff),stdin)!=NULL) {

    if (strlen(buff)<=10) {

     write(fdDest,buff,strlen(buff));
    }else {
     write(pipe_fd[1],buff,strlen(buff));
    }


   }

   close(fdDest);
   close(pipe_fd[1]);
   exit(EXIT_SUCCESS);
  }
  //-----------PADRE-------------
 }
  close(pipe_fd[1]);

  char buff[1024];
  ssize_t byteRead;

  while ((byteRead = read(pipe_fd[0],buff,sizeof(buff)))>0) {
   char *pre = "il padre dice:";
   write(0,pre,strlen(pre));
   write(0,buff,byteRead);
  }


  for (int i=0; i<N;i++) {
   wait(NULL);
  }

 close(pipe_fd[0]);
 exit(EXIT_SUCCESS);
 return 0;
 }


