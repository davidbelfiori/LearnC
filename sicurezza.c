#include  <stdio.h>

int V[10];

int main () {
    int *x =  V;

    *(x-10)= 2;
    printf("%d\n", *(x-10));
}