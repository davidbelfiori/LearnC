#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

    if (argc <2) {
        printf("usage nome programma nome fifo");
        return  0;
    }

    int ret = mkfifo(argv[1],0666);
    if (ret==-1) {
        printf("Errore nella creazione della named pipe");
        return  0;
    }

    int fd =open(argv[1],O_WRONLY);
    if (fd<0) {
        printf("Errore nell'apertura della named pipe");
        return  0;
    }

    close(1);
    dup(fd);
    close(fd);
    execlp("./fifoLettore","./fifoLettore",NULL);
    perror("errore nella exec");
    return 0;
}