//
// Created by davidjulianbelfiori on 27/06/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

int main(int argc , char *argv[]) {

    struct  sigaction sa;
    sa.sa_handler = SIG_IGN; // Imposta l'azione su "Ignore"
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGPIPE,&sa,NULL)== -1) {
        printf("errore in sigaction");
        return 0;
    }

    int fd[2];
    if (pipe(fd)==-1) {
        printf("errore nella creazione delle pipe");
        return 0;
    }

    close(fd[0]);

    int byteScritti = write(fd[1],"hello",5);

    if (byteScritti == -1) {
        if (errno == EPIPE) {  /*  EPIPE  fd è connesso a una pipe o un socket la cui lettura è chiusa. Quando ciò ac‐
            cade  il processo di scrittura riceverà anche un segnale SIGPIPE. (Quindi il
            valore restituito in scrittura è visto  solo  se  il  programma  intercetta,
            blocca o ignora questo segnale).
*/

            printf("errore gestitio chiudo");

        }else {
            printf("errore generico");
        }

    }


    close(fd[1]);
    return  0;
}