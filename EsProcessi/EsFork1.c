/*Il processo figlio deve stampare il proprio PID (Process ID), il PID del padre e un messaggio (es. "Sono il figlio e sto terminando"),
 *per poi terminare con un codice di uscita a tua scelta (es. exit(42)).
Il processo padre deve stampare il proprio PID, attendere la terminazione del figlio usando wait() o waitpid(),
e infine stampare il codice di uscita restituito dal figlio.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

    pid_t figlio = fork();
    int status;
    if (figlio==0) {
        printf("Sono processo %d, il figlio del processo: %d \n",getpid(),getppid());
        printf("il filgio termina \n");
        exit(42);
    }
    printf("Ciao sono il padre con pid: %d \n",getpid());
    waitpid(figlio,&status,0);
    printf("Codice di ritorno del filgio: %d \n", status>>8);
}
