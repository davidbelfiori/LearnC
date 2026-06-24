#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>


void creaAlberoProcessi(int altezzaCorrente, int altezzaAlbero){
    if(altezzaCorrente>altezzaAlbero){
        printf("terminato");
         return;
    }

    pid_t A,B;

    A = fork();
    if (A < 0)
    {
       exit(EXIT_FAILURE);
    }
    if (A== 0)
    {
        printf("Ciao sono il processo %d generato da %d , della %d iterazione \n",getpid(),getppid(),altezzaCorrente);
        creaAlberoProcessi(altezzaCorrente+1,altezzaAlbero);
        exit(EXIT_SUCCESS);
    }
     B = fork();
    if (B < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (B== 0)
    {
        printf("Ciao sono il processo %d generato da %d , della %d iterazione \n",getpid(),getppid(),altezzaCorrente);
        creaAlberoProcessi(altezzaCorrente+1,altezzaAlbero);
        exit(EXIT_SUCCESS);
    }
    int status;
    waitpid(A,&status,0);
    waitpid(B,&status,0);
    

}

int main(int argc, char *argv[]){

    if (argc <2 ){
        printf("Erorre , uso nome programma <Numero> ");
        return 0;
    }

    int altezzaALbero = atoi(argv[1]);

    printf("Numero di livelli di albero scelto %d \n",altezzaALbero);
    printf("Sono la base e sono il padre di tutti il mio pid è %d \n",getpid());
    creaAlberoProcessi(0,altezzaALbero);

    return 0;

}