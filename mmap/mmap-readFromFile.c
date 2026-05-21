/*? Brief : cosa fara questo programma, dato un un nome preso da cli , creo questo programma e chiedo all'utente di
 scriverci sopra, poi con mmap prendero quel file e lo mappero nel mio AS a partire da un certo offset e stamnpero il contenuto*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define BUFF_SIZE 1024

int main(int argc,char** argv) {
  char buff[BUFF_SIZE];
  int fd;
 if (argc != 2) {
  printf("usage: nomeProgramma <Nome File> \n");
  exit(0);
 }

 fd = open(argv[1],O_RDWR|O_TRUNC|O_CREAT,0664);

 if (fd == -1) {
  printf("erore nell'appertura del file con nome %s \n",argv[1]);
  exit(0);
 }

 printf("inserisci quello ceh vuoi scrivere nel file \n");

 fgets(buff,BUFF_SIZE,stdin);

 int res = write(fd,buff,strlen(buff));

 if (res==-1) {
   printf("eorre nella scrittura del file");
   exit(0);
 }

 //! se avessi chiesto MAP_SHARED Sarebbe stato inutile dargli il file descriptor perchè gli chiedo un pezzo di ram con tutti 0
 char *ptr = mmap(NULL,PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
 if (ptr == MAP_FAILED) {
  printf("errore in mmap");
  exit(0);
 }

 char *x1= ptr+900;
 printf("testo: %s",x1);

}