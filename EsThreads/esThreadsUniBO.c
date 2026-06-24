/*Un main lancia in esecuzione 4 thread passando a ciascuno di essi una struttura dati
chiamata Struttura che è formata da 3 campi:
un intero N che dice al pthread quanti altri thread deve creare; una stringa Str di 100
caratteri; un intero Indice con un valore compreso tra 0 ed N-1, diverso per ciascun
thread creato da uno stesso thread.
Ciascun thread, iniziando la sua esecuzione, aspetta 1 secondo, poi legge il contenuto
della struttura dati ricevuta, in particolare il valore di N. Sia M il valore contenuto in N.
Se M>1 allora il thread lancia in esecuzione M-1 thread passando a ciascuno una copia
della struttura dati che nel campo N contiene il valore M-1, e nel campo Indice contiene
un valore compreso tra 0 ed (M-1)-1, diverso per ciascun thread creato dal nostro
thread.
Ciascun thread deve restituire una struttura dati simile a quella che è stata avuta come
argomento. Nel campo stringa Str della struttura deve essere collocato in formato
testuale il valore di N ricevuto poi uno spazio e poi il valore di Indice ricevuto.
Prima di terminare, ciascun thread deve aspettare la terminazione di ciascun thread che
lui ha creato e stampare a video la stringa Str ricevuta da ciascuno di quei thread.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct 
{
    int N;
    int indice;
}Struttura;

void *work(void *arg){
     Struttura *strutturaRicevuta = (Struttura *)arg;
    int M = strutturaRicevuta->N;
    int figliDaLanciare = M-1;
    sleep(1);
   

    if(M>1){
        Struttura *argFigli = malloc(figliDaLanciare*sizeof(Struttura));
        pthread_t *tidFigli = malloc(figliDaLanciare*sizeof(pthread_t));

        for (int i = 0; i < figliDaLanciare; i++)
        {
            argFigli[i].indice = i;
            argFigli[i].N =figliDaLanciare;
           if(pthread_create(&tidFigli[i],0,work,&argFigli[i])!=0){ //tidFigli[i] non è un puntatore ma l'elemento specifico, tidFigli si
                printf("errore nella creazione dei thread");
                exit(EXIT_FAILURE);
           } 
        }

        for (int i = 0; i < figliDaLanciare; i++)
        {
            Struttura *retValFiglio;
            if(pthread_join(tidFigli[i],(void **)&retValFiglio)!=0){
                 printf("errore nella creazione dei thread");
                exit(EXIT_FAILURE);
            }

            printf("[Thread M=%d, Indice=%d]\n", M, strutturaRicevuta->indice);

            free(retValFiglio);
        }
        free(tidFigli);
        free(argFigli);
        
    }
        Struttura *ritorno = malloc(sizeof(Struttura));
            if (ritorno == NULL) {
                perror("Errore malloc");
                exit(EXIT_FAILURE);
            }
            ritorno->N = M;
            ritorno->indice = strutturaRicevuta->indice;

            pthread_exit((void *)ritorno);

}


int main(int argc,char *argv[]){
    int numero_threadiniziale = 4;
    int N_iniziale = 4;


    Struttura arg[numero_threadiniziale];
    pthread_t thread[numero_threadiniziale];


    for(int i = 0; i<numero_threadiniziale; i++){
        arg[i].indice=i;
        arg[i].N = N_iniziale;

        if(pthread_create(&thread[i],0,work,&arg[i])!=0){
            perror("errore nella genrazione dei thread.");
            exit(EXIT_FAILURE);
        }


    }

    for (int i = 0; i < numero_threadiniziale; i++)
    {
        Struttura *retVal;
        if(pthread_join(thread[i],(void **)&retVal)!=0){
            perror("errore nella join");
            exit(EXIT_FAILURE);
        }
        free(retVal);
    }
    
    return 0;

}
