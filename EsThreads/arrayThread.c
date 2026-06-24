#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>

#define ARRAYSIZE 1000

 typedef struct 
 {  
    int *array;
    int startPosition;
    int endPosition;
    int total;
 }arrayThread;
 
 void *work(void *info){

    arrayThread *informazioni = (arrayThread *)info;

    for (int i = informazioni->startPosition; i < informazioni->endPosition+1; i++)
    {
       informazioni->total += informazioni->array[i] ;
    }
    pthread_exit(NULL);

 }

int main(int argc , char *argv[]){

    int *arr = malloc(sizeof(int)*ARRAYSIZE);
    int risultato = 0;

    for (int i = 0; i < ARRAYSIZE; i++)
    {
        arr[i]= rand() % 100;
        risultato += arr[i];
    }
    
    printf("Array completamente scritto risultato: %d",risultato);

    pthread_t a,b,c,d;


    arrayThread info[4];

    for (int i = 0; i < 4; i++)
    {
        info[i].array = arr;
        info[i].startPosition = i*(ARRAYSIZE/4);
        info[i].endPosition = info[i].startPosition + (ARRAYSIZE/4)-1;
        info[i].total = 0;
    }

    pthread_create(&a,NULL,work,&info[0]);
    pthread_create(&b,NULL,work,&info[1]);
    pthread_create(&c,NULL,work,&info[2]);
    pthread_create(&d,NULL,work,&info[3]);
    
    pthread_join(a,NULL);
    pthread_join(b,NULL);
    pthread_join(c,NULL);
    pthread_join(d,NULL);

    int totalethread = 0;
    for (int i = 0; i < 4; i++)
    {
        totalethread+=info[i].total;
    }
    
    printf("Totale thread %d \n",totalethread);

    

}