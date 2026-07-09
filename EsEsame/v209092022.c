/*SPECIFICATION TO BE IMPLEMENTED:
Implementare una programma che riceva in input, tramite argv[], i nomi
di N differenti file F1 ... FN, con N maggiore o uguale a 1, che dovranno essere creati
o troncati se gia' esistenti.
Per ognuno dei file dovra' essere attivato un nuovo processo che ne gestira' il contenuto
(indichiamo quindi con P1 ... PN i processi che dovranno essere attivati).
Ciascun processo Pi leggera' dallo standard input 5 caratteri per volta in modo atomico
rispetto alle attivita' degli altri processi, e dovra' scriverli sul file che sta gestendo.
Anche la scrittura dei 5 caratteri sul file destinazione deve risultare come un'azione atomica,
ovvero i caratteri non possono essere scritti sui file individualmente.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando il processo originale venga colpito esso dovra'
riportare su standard output i 5 ultimi caratteri correntemente presenti
su ciascuno degli N file gestiti. Tutti gli altri processi non dovranno
eseguire alcuna attivita' in caso di arrivo di segnalazione.

In caso non vi sia immissione di dati sullo standard input, e non vi siano segnalazioni,
l'applicazione dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.*/


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
# include <string.h>
#include <sys/wait.h>

#define fflush(stdin) while(getchar() != '\n');
volatile sig_atomic_t sigint = 0;

void messaggio(char *msg) {
    printf("Errore %s",msg);
    exit(EXIT_FAILURE);
}

void handler(int sig) {
    sigint = 1;
    printf("CATTURATA \n");

}


void child_task(int fd, sem_t * sem) {

    //se arriva una sigint la ignoro tanto la gestisce il padre;
    signal(SIGINT,SIG_IGN);
    char buff[5];

    while (1) {
        sem_wait(sem);
        int byteread = 0;
        while (byteread <5) {
            int r = read(0,buff + byteread,5-byteread);
            byteread+=r;}
        write(fd,buff,5);
        usleep(10000);
        sem_post(sem);
    }

}

void printLastFiveBites(int argc, char * argv[]) {

    for (int i =0;i<argc-1;i++) {
        int fd = open(argv[i+1],O_RDONLY,0666);
        ssize_t grandezza = lseek(fd,0,SEEK_END);
        char buff[6];
        lseek(fd,-5,SEEK_END);
        ssize_t byteLetti = read(fd,buff,5);
        lseek(fd,0,SEEK_END);
        if (byteLetti > 0) {
            printf("i 5 ultimi caratteri del file %s sono: %s  \n",argv[i+1],buff);
        }else {
            printf("non ci sono caratteri nel file %s" ,argv[i+1]);
        }

    }

    printf("FINE \n");

};

int main(int argc , char *argv[]) {


    if (argc <2) messaggio("Uso ./nomeFile file1 .... fileN");

    for (int i =1;i<argc; i++) {
        for (int j = i+1 ; j< argc; j++) {
           if (strcmp(argv[i],argv[j])==0) {
               messaggio("I nomi dei file devono essere diversi");
           }
        }
    }

    sem_t *sem = mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    if (sem == nullptr) messaggio("errore nella creazione del semaforo nella mmap");

    if (sem_init(sem,1,1) == -1) messaggio("errore nell'inizializzazizione del semaforo");


    for (int i = 0;i<argc-1;i++) {

        int fd = open(argv[i+1],O_CREAT|O_TRUNC|O_RDWR,0666);
        if (fd<0) messaggio("errore nell'apertura del file");

        pid_t pid = fork();
        if (pid <0) messaggio("errore nella creazione del figlio");
        if (pid == 0) {
            child_task(fd,sem);
        }

    }

    //essendo che la creo dopo aver creato i figli i figli non ereditano la sigaction;
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT,&sa,NULL);

    int numerofigli = argc-1;

    while (numerofigli > 0) {

        pause();
        if (sigint ==1) {
            printf("PADRE CATTURA SIGINT \n");
            sigint = 0;
            printLastFiveBites(argc,argv);
        }


        int p = 0;
        if ((p = waitpid(-1,NULL,WNOHANG))>0) {
            numerofigli--;
        }

    }




    return 0;
}
