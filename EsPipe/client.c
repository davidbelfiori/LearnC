#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define FIFO_A "/tmp/fifo_A1"
#define FIFO_B "/tmp/fifo_B1"

int main(int argc, char *argv[]) {

    char tx_buff[1024];
    char rx_buff[1024];


    int fd_leggi=open(FIFO_A,O_RDONLY);
    int fd_scrivi=open(FIFO_B,O_WRONLY);


    while (1) {

        int n = read(fd_leggi,rx_buff,sizeof(rx_buff));
        if (n > 0) {
            printf("Ricevuto da B: %s", rx_buff);
            if (strncmp(rx_buff, "exit", 4) == 0) break;
        }

        fgets(tx_buff,sizeof(tx_buff),stdin);

        write(fd_scrivi,tx_buff,strlen(tx_buff));
        if (strcmp(tx_buff, "exit") == 0) break;



    }

    close(fd_scrivi);
    close(fd_leggi);
    unlink(FIFO_A);
    unlink(FIFO_B);
    return 0;


}