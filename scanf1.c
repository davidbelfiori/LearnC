
#include <stdio.h>

int x;

int  main(int argc, char *argv[]) {
    int returnvalue = scanf("%d", &x);

    //scanf mi ritorna il numero di paramentri accettati, se io gli passo qualcosa
    //che non è un intero mi ritornerà il numero degli elementi che sono accettati
    //se ho n elementi e il primo è errato , mi ritornerà 0
    //se ho n elementi e l'elemento n-m è errato mi ritornera n-m-1 come numero

    printf("%d", returnvalue);
}

