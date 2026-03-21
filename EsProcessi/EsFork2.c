/*
*Scrivi un programma che crea un processo figlio.
Il figlio deve sostituire la propria immagine in memoria
con il comando di sistema ls -l utilizzando una funzione della famiglia exec (ad esempio execlp o execvp).
Il padre deve attendere che il figlio termini e poi stampare "Comando completato".
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
 pid_t pid = fork();
 int status;
 if (pid == -1) return 0;

 if (pid > 0) {
  wait(&status);
  if (status>>8 == 3) printf("errore nella chiamata");
  printf("Comando completato");
 }else {
   // execlp("ls","ls","-l",NULL);
  if (execlp("./EsFork1",NULL) == -1) exit( 3);
  exit(2);
 }

}