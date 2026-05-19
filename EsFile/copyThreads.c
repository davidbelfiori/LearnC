/* scrivere un programma c , dove dato un file in F in input , si scriva il contenuto in un file F' , la copia deve essere fatta da N thread che si dividono il
 il file in chunk e lo copiano sul file F'
 */


#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define buffersize 1024

typedef struct
{
    int fd_src; //descrittore file sorgente
    int fd_dst; //descrittore file destinazione
    off_t offset;//byte inizio el chunk
    size_t size;
    int thread_id;
} thread_args;


int main(int argc, char *argv[]){
    if (argc !=4) {
        printf("expeted format: sorgetnte destinazione num_thread \n");
        exit(0);
    }
    const char *fileSorgente = argv[1];
    const char *fileDestinazione = argv[2];
    int numThreads = atoi(argv[3]);

   if (numThreads <=0) {
       printf("Il numero dei thread deve essere maggiore di 0\n addios\n");
       exit(0);
   }

    int fdSorgente = open(fileSorgente,O_RDONLY);
    if (fdSorgente == -1){printf("Errore apertura del file\n"); exit(0);}

    struct stat st;
    if (fstat(fdSorgente,&st)==-1) exit(0);
    off_t sizeSorgente = st.st_size;

    //se la grandezza del file è inferiore al numero dei thread impongo che il numero dei threads è uguale numero dei byte , per non avere thread che lavorano a vuoto
    if ((off_t)numThreads > sizeSorgente) numThreads= (int)sizeSorgente;

    int fdDestinazione = open(fileDestinazione,O_CREAT|O_TRUNC|O_WRONLY,0644);

    if (fdDestinazione == -1){printf("Errore apertura del file di destinazione\n");exit(0);}

    if (posix_fallocate(fdSorgente,0,sizeSorgente)==-1) {
        printf("errore nella allocazione dei blocchi del file destinazione \n");
        exit(0);
    }

    off_t chunckBase = sizeSorgente / numThreads;
    off_t resto= sizeSorgente-(chunckBase*numThreads);

    pthread_t *vThreads= malloc((size_t)numThreads*sizeof(pthread_t));
    thread_args *args= malloc((size_t)numThreads*sizeof(thread_args));

    off_t currOffster = 0;
    for (int i = 0; i < numThreads; ++i) {
        args[i].fd_dst=fdDestinazione;
        args[i].fd_src=fdSorgente;
        args[i].offset= currOffster;
        args[i].size = (size_t)(chunckBase +(i==0?resto:0));
        args[i].thread_id = i;
        currOffster += (off_t)args[i].size;
    }


}