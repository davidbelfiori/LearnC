#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 40 // Numero di thread da creare

// Funzione eseguita da ogni thread
void* child_thread(void *arg){
    // Cast e dereferenziamento dell'argomento per ottenere l'ID del thread
    int idt = *(int *)arg;
    printf("Codice thread %d e tid %lu \n", idt, pthread_self());
    // Restituisce NULL (non utilizziamo il valore di ritorno)
    return NULL;
}


int main(){
    // Array per memorizzare gli ID dei thread
    pthread_t tid[NUM_THREADS];
    // Array per memorizzare gli indici dei thread
    int idt[NUM_THREADS];

    printf("Numero dei thread che verranno lanciati %d \n", NUM_THREADS);

    // Creazione dei thread
    for (int i = 0; i < NUM_THREADS; i++)
    {
        idt[i] = i;
        // Crea un nuovo thread che esegue child_thread
        if(pthread_create(&tid[i], NULL, child_thread, &idt[i]) != 0){
            printf("errore nella creazione di un thread, addio! \n");
            exit(EXIT_FAILURE);
        }
    }

    // Attesa della terminazione di tutti i thread
    for (int i = 0; i < NUM_THREADS; i++)
    {
        // Attende che il thread termini (non ci interessa il valore di ritorno)
        pthread_join(tid[i], NULL);
        printf("il thread %lu è stato sincronizzato \n", (unsigned long)tid[i]);
    }

    return 0;
}
