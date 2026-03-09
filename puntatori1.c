#include  <stdio.h>

int main () {
    int *a ;
    int b;
    b = 10;
    // a = b; errore non posso assegnare un puntatore ad un numero
    printf("%p\n", &b);

    a=&b;
    printf("%d",*a);
}