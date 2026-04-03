#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*Scrivi un programma che prende N argomenti da riga di comando (es. ./catena cmd1 cmd2 cmd3). Il programma deve eseguire i
 *comandi in sequenza, ma ogni comando deve essere eseguito da un processo figlio diverso.
Il padre crea un figlio per eseguire cmd1.
Il padre aspetta che cmd1 finisca.
Una volta terminato, il padre crea un nuovo figlio per cmd2, lo aspetta, e così via fino all'ultimo comando.
Se un comando fallisce (exit status diverso da 0), la catena si deve interrompere e il padre deve stampare un messaggio di errore terminando a sua volta.
*/

int main(int argc, char  *argv[]) {
    if (argc < 2) {
        printf("inserisci almeno 2 comandi");
        return  1;
    }

    int status;

    for (int i = 1; i<argc; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (execlp(argv[i],argv[i],NULL)==-1) exit(3);
        }else {
            wait(&status);
            if (status >> 8) {
                printf("Errore nell'esecuzione di un processo figlio, l'esecuzione termina \n");
                exit(1);
            }
        }
    }
}