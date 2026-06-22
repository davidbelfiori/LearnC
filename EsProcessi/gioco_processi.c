//
// Created by davidjulianbelfiori on 22/06/26.
//

/*
*Padre (Generale): Genera Figlio A e Figlio B. Poi aspetta che entrambi finiscano. Riceve i loro "numeri segreti" e calcola la somma.

Figlio A (Il Matematico): Genera un numero casuale tra 1 e 10, lo stampa e termina restituendo quel numero come stato di uscita.

Figlio B (Il Geometra): Fa la stessa cosa, ma genera un numero tra 11 e 20. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[]) {


    pid_t pidPadre = getpid();
    pid_t figlioA ,figlioB;
    int numeroA, numeroB;
    printf("ciao sono il padre con pid %d",pidPadre);

    figlioA = fork();
    if (figlioA == 0) {
        int numero = rand()%10;
        printf("sono figlio A con pid %d e il mio numero è:%d \n",getpid(),numero);
        exit(numero);
    }else if(figlioA == -1) {
        printf("errore nella generazione del figlio 1 \n");
        exit(EXIT_FAILURE);
    }

    figlioB = fork();
    if (figlioB == 0) {
        int numero = 10+rand()%10;
        printf("sono figlio B con pid %d e il mio numero è:%d \n",getpid(),numero);
        exit(numero);
    }else if(figlioB == -1) {
        printf("errore nella generazione del figlio 2 \n");
        exit(EXIT_FAILURE);
    }

    waitpid(figlioA,&numeroA,0);
    waitpid(figlioB,&numeroB,0);

    printf("la somma dei due %d",(numeroA>>8)+(numeroB>>8));

}