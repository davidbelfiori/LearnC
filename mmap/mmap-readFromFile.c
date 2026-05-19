/*? Brief : cosa fara questo programma, dato un un nome preso da cli , creo questo programma e chiedo all'utente di
 scriverci sopra, poi con mmap prendero quel file e lo mappero nel mio AS a partire da un certo offset e stamnpero il contenuto*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(int argc,char** argv) {
 if (argc != 2) {
  printf("usage: nomeProgramma <Nome File> \n");
  exit(0);
 }

}