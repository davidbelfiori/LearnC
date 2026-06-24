/*Scrivere un programma C che crei 5 “Thread
” a cui vengono
passati tramite una struttura due parametri: il numero del
thread ed un messa*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#define NUMTHREADS 5

typedef struct
{
    pthread_t tid;
    char *messaggio;
}InfoThread;


void *work (void *arg){
    InfoThread *thread = (InfoThread *) arg;
    printf("Ciao sono il thread con tid %lu e questo è quello che ti devo dire %s \n",thread->tid,thread->messaggio);
    pthread_exit(NULL);
}

int main(int argc ,char *argv[]){

    InfoThread thread[NUMTHREADS];

    for (int i = 0; i < NUMTHREADS; i++)
    {
        thread[i].messaggio="OMO";
        pthread_create(&thread[i].tid,NULL,work,&thread);
    }

       for (int i = 0; i < NUMTHREADS; i++)
    {
    
        pthread_join(thread[i].tid,NULL);
    }

    
    

}