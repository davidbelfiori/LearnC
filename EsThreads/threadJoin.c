#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>

#define CICLI 10000000

int counter = 0;

/*void *work(){

    int *res;
    for (int i = 0; i < CICLI; i++)
    {
        counter ++;
    }
    res = &counter;
    pthread_exit(NULL);
}*/


void *work(void *mutex){
    for(int i = 0; i<CICLI;i++){
        pthread_mutex_lock(mutex);
        counter++;
        pthread_mutex_unlock(mutex);
    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[]){

    //CREO IL MUTEX
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);

    //creo thread1
    pthread_t t1 ,t2;
    int res1,res2;


    pthread_create(&t1,NULL, work,&mutex);

    pthread_create(&t2,NULL, work,&mutex);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    printf("counter: %d",counter);

}