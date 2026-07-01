//
// Created by davidjulianbelfiori on 01/07/26.
//

/*
* Implementare un programma in C che crei due thread: un Produttore e un Consumatore.
I due thread condividono una singola variabile globale int buffer; e un contatore globale int elaborati = 0;.

Il Produttore: In un ciclo infinito, genera un numero (può essere semplicemente un contatore che cresce, 1, 2, 3...), lo
inserisce nel buffer e poi aspetta che il Consumatore lo abbia letto prima di poterne inserire un altro.

Il Consumatore: In un ciclo infinito, aspetta che ci sia un nuovo numero nel buffer, lo legge, lo stampa a video,
incrementa la variabile elaborati e dà il via libera al Produttore per il prossimo numero. Tra una lettura e l'altra, simula un po' di lavoro con una sleep(1).

Il Main (Gestione Eventi): Il thread principale, dopo aver inizializzato semafori e thread, non fa nulla di attivo
(può dormire in un ciclo o usare pause()). Deve però gestire il segnale SIGINT (CTRL+C). Quando l'utente preme CTRL+C, il programma deve intercettare
il segnale, stampare a schermo "Programma terminato. Totale numeri elaborati: X" (dove X è il valore della variabile elaborati) e chiudersi con exit(0).
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

sem_t letto;
sem_t scritto;
int buffer=0;
int contatore = 0;


void handler (int sig) {
 printf("SIGINT intercettato, il valore del buffer è %d il valore del contatore %d",buffer,contatore);
 exit(0);
};


void *produttore (void *arg) {

 sigset_t sigset;
 sigemptyset(&sigset);
 sigaddset(&sigset,SIGINT);

 while (1) {

  sem_wait(&letto);
  pthread_sigmask(SIG_BLOCK,&sigset,NULL);

  buffer ++;

  pthread_sigmask(SIG_UNBLOCK,&sigset,NULL);
  sem_post(&scritto);

  //sleep(1) //do tempo di consumare
 }

}
void *consumatore (void *arg) {
 sigset_t sigset;
 sigemptyset(&sigset);
 sigaddset(&sigset,SIGINT);

 while (1) {

  sem_wait(&scritto);
  pthread_sigmask(SIG_BLOCK,&sigset,NULL);
  printf("valore %d \n",buffer);
  sleep(1);
  contatore ++;
  pthread_sigmask(SIG_UNBLOCK,&sigset,NULL);
  sem_post(&letto);
 }

}

void messaggio ( char *messaggio) {
 printf("errore %s \n",messaggio);
 exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

 struct sigaction sa;
 sa.sa_handler = handler;
 sigemptyset(&sa.sa_mask);
 sa.sa_flags = 0;
 if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("sigaction");

 sem_init(&letto,0,1);
 sem_init(&scritto,0,0);

 pthread_t figli[2];

 pthread_create(&figli[0],NULL,produttore,NULL);
 pthread_create(&figli[1],NULL,consumatore,NULL);

 pthread_join(figli[0],NULL);
 pthread_join(figli[1],NULL);



}