#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_ARRAYSIZE 10

void* worker_thread(void *arg){

    int *arr = malloc(sizeof(int)*MAX_ARRAYSIZE);
    if (arr == NULL)
    {
        printf("malloc errorr");
        pthread_exit(NULL);
    }
    for (int i = 0; i < MAX_ARRAYSIZE; i++)
    {
        arr[i] = i+1 ;
    }
    pthread_exit(arr);

}

int main(){
    pthread_t tid;
    void *vret;

    if(pthread_create(&tid,NULL,worker_thread,NULL)!=0){
        printf("errore");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(tid,&vret)!=0)
    {
        perror("error join");
        exit(EXIT_FAILURE);
    }

    int *arr = (int *)vret;

    for (int i = 0; i < MAX_ARRAYSIZE; i++)
    {
        printf("%d \n",arr[i]);
    }
    free(arr);
}