//
// Created by davidjulianbelfiori on 22/06/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define fflush(stdin) while(getchar()!='\n');

int main(int argc, char *argv[]) {

    int a;



    while (1) {
        printf("ciao sono il padre con pid %d , inserisci un valore intero:",getpid());
        if (scanf("%d",&a) != 1) {
            fflush(stdin);
            printf("si prega di riprovare \n");
        }else {

            break;
        }
    }


    pid_t figlio = fork();

    if (figlio == -1) {
        printf("errore nella generazione del figlio \n");
        exit(0);
    }

    if (figlio>0) {
        printf("il valore di a del padre: %d \n",a);
    }else {
        a = 4;
        printf("il valore di a del figlio: %d \n",a);
        exit(0);
    }

}