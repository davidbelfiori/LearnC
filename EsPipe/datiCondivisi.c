#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

#define PAGE_SIZE 4096
#define TOTAL_SIZE (PAGE_SIZE * 2)

void errore( char *errore) {

    printf("errore: %s",errore);
    exit(EXIT_FAILURE);

}

void produttore(char * addr, int i) {

    char buffer[128];
    printf("scrivi qualcosa:");
    fgets(buffer,100,stdin);
    memcpy(addr+PAGE_SIZE,buffer,strlen(buffer)+1);

    write(i,"Y",1);

    exit(0);

}

int main(int argc, char *argv[]) {
    char nomeFile[]="dati_condivisi.bin";
    int fdpipe[2];

    int fdFile = open(nomeFile,O_CREAT|O_RDWR|O_TRUNC,0666);

    if ( fdFile < 0) {
        errore("errore nella creazione del file");
    }

    if (pipe(fdpipe)== -1) {
        errore("errore nella creazione delle pipe");
    }

    if (ftruncate(fdFile,TOTAL_SIZE)==-1) {
        errore(("errore allocazione file"));
    }

    char *addr = mmap(NULL,TOTAL_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED, fdFile, 0);

    if (addr == MAP_FAILED) {
        errore("errore nella mmap");
    }

    pid_t figlio = fork();

    if (figlio == -1) errore("errore generazione figlio");
    if (figlio == 0) {
        close(fdpipe[0]);
        produttore(addr,fdpipe[1]);
    }

    else {
        close(fdpipe[1]);
        char messaggioFiglio;
        read(fdpipe[0],&messaggioFiglio,1);
        wait(NULL);

        printf("Stringa inserita dal filgio: %s",addr+PAGE_SIZE);
        munmap(addr,TOTAL_SIZE);
        close(fdFile);
        unlink(nomeFile);
    }

    return 0;
}
