#include <stdio.h>


    int main () {

        int x = 10; // variabile intera

        int *p = &x;
        // p contiene l'indirizzo di memoria di x

        int **q = &p;
        // q contiene l'indirizzo di memoria di p, che a sua volta contiene l'indirizzo di x

        int ***h = &q;
        // h contiene l'indirizzo di memoria di q, che punta a p, che punta a x

        printf("%d", ***h); // stampa il valore di x (10)
    }