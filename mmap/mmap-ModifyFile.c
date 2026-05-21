
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc , char** argv) {

    char *str = "ciao a tutti!";
    if (argc != 2) {
        printf("usage: nome programma <nome file> \n");
        exit(0);
    }

    int fd = open(argv[1],O_RDWR|O_CREAT|O_TRUNC,0664);

    if (fd == -1) {
        printf("errore nella creazione o apertura del file di nome %s",argv[1]);
        exit(0);
    }

    int ret = write(fd,str,strlen(str));

   if (ret ==-1 && ret <strlen(str)) {
       printf("errore nella print");
       exit(0);
   }

   char *ptr = mmap(NULL,strlen(str),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    if (ptr == MAP_FAILED) {
        printf("errore nella mmap");
        exit(0);
    }

   for (int i = 0; i<strlen(str);i++) {
       ptr[i] = (i==0)? 'C':'-';
   }

    printf("%s",ptr);

    close(fd);
    munmap(ptr,strlen(str));

}