/******************************************************************
Welcome to the Operating System examination

You are editing the '/home/esame/prog.c' file. You cannot remove 
this file, just edit it so as to produce your own program according to
the specification listed below.

In the '/home/esame/'directory you can find a Makefile that you can 
use to compile this program to generate an executable named 'prog' 
in the same directory. Typing 'make posix' you will compile for 
Posix, while typing 'make winapi' you will compile for WinAPI just 
depending on the specific technology you selected to implement the
given specification. Most of the required header files (for either 
Posix or WinAPI compilation) are already included in the head of the
prog.c file you are editing. 

At the end of the examination, the last saved snapshot of this file
will be automatically stored by the system and will be then considered
for the evaluation of your exam. Modifications made to prog.c which are
not saved by you via the editor will not appear in the stored version
of the prog.c file. 
In other words, unsaved changes will not be tracked, so please save 
this file when you think you have finished software development.
You can also modify the Makefile if requesed, since this file will also
be automatically stored together with your program and will be part
of the final data to be evaluated for your exam.

PLEASE BE CAREFUL THAT THE LAST SAVED VERSION OF THE prog.c FILE (and of
the Makfile) WILL BE AUTOMATICALLY STORED WHEN YOU CLOSE YOUR EXAMINATION 
VIA THE CLOSURE CODE YOU RECEIVED, OR WHEN THE TIME YOU HAVE BEEN GRANTED
TO DEVELOP YOUR PROGRAM EXPIRES. 


SPECIFICATION TO BE IMPLEMENTED:
Scrivere un programma che riceva in input tramite argv[] un insieme di n nomi
di file, con n maggiore o uguale ad 1. I nomi dei file per essere correttamente
gestiti dovranno essere tutti diversi tra di loro. Indicheremo tali nomi come 
F1, ..., Fn.
Per ogni nome di file Fi dovra' essere attivato un thread, che indicheremo con Ti.
I thread attivati dovranno a turno leggere una stringa dallo standard input e ciascuno
di essi dovra' scriverla su una nuova linea in tutti i file F1, ..., Fn. La scrittura
dovra' avvenire n modo che le stringhe lette in input compaiano in tutti i file esattamente
nello stesso ordine di lettura. Tale attivita' di lettura di stringhe e scrittura sui 
file dovra' andare avanti in modo indefinito.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI) 
in modo tale che quando il processo venga colpito il suo main thread dovra' 
verificare quante linee siano presenti in ciascun file e se tale numero sia pari o dispari.
Esso dovra' anche riportare tale informazione tramite un messaggio su standard ouput.

In caso non vi sia immissione di dati sullo standard input e non vi siano segnalazioni, 
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
#define MAX_SIZE 1024

sem_t *turno;
int *fd;
int N;
int linee;
int *lineeThread;

void messaggio(char *p) {

	perror(p);
	exit(EXIT_FAILURE);

}

void *work(void *p) {

	int id = (int)p;
	char buff[MAX_SIZE];
	while (1) {
		sem_wait(&turno[id]);
		printf("t%d > \n",id);
		if (fgets(buff,MAX_SIZE,stdin)==NULL) {
			messaggio("errore fgest");
		}
		for (int i = 0 ; i < N ; i++) {
			write(fd[i],buff,strlen(buff));
			fsync(fd[i]);
			lineeThread[i]++;
		}
		sem_post(&turno[(id+1)%N]);
	}


}


int main(int argc, char** argv){

	if (argc< 2) messaggio("errore uso");
	 N = argc -1;

	for (int i = 1 ; i < argc ; i++) {

		for (int j = i+1; j < argc ; j++) {
			if (strcmp(argv[i],argv[j])==0) messaggio("i nomi dei file devono essere diversi");
		}

	}

	turno = malloc(N*sizeof(sem_t));
	fd = malloc(N*sizeof(int));
	lineeThread = malloc(N*sizeof(int));

	for (int i = 0; i< N; i++) {
		lineeThread[i] = 0;
		fd[i] = open(argv[i+1],O_CREAT|O_RDWR|O_TRUNC,0666);
		if (fd[i] < 0) messaggio("errore apertura file");

	}

	for (int i = 0; i< N; i++) {
		if (i == 0) {
			if (sem_init(&turno[i],0,1)==-1)messaggio("errore sem_init");
		}else {
			if (sem_init(&turno[i],0,0)==-1)messaggio("errore sem_init");
		}
	}

	sigset_t sig;
	sigemptyset(&sig);
	sigaddset(&sig,SIGINT);
	pthread_sigmask(SIG_BLOCK,&sig,NULL);

	pthread_t *tid = malloc(N*sizeof(pthread_t));

	for (int i = 0 ; i <N ; i++) {
		if (pthread_create(&tid[i],NULL,work,(void *)i)!=0)messaggio("errorre spown thread");
	}

	while (1) {

		int segnale;
		sigwait(&sig,&segnale);

		if (segnale ==SIGINT){
			printf("cattirato il numero di lineeScritte : %d %s \n",lineeThread[0],(lineeThread[0] % 2 == 0)?"pari": "Dispari");
		}
	}


	return 0;
}
