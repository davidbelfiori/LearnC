#include <stdio.h>
#include <string.h>

#define MEM_SIZE 30

// Funzione per stampare l'intero array in esadecimale


int main() {
    char memoria[MEM_SIZE] = {0}; // Inizializza tutto l'array a 0
    int offset = 0;               // Tiene traccia dello spazio occupato
    char scelta;

    printf("--- GESTORE DI MEMORIA ---\n");
    printf("Dimensione totale: %d byte\n", MEM_SIZE);

    while (1) {

        //Scelta del dato
        printf("Spazio rimanente: %d byte \n", MEM_SIZE-offset);
        printf("Sezione di scelta del tipo di dato: \n");
        printf("1- Char Dimensione: %lu \n",sizeof(char));
        printf("2- Intero Dimensione: %lu \n",sizeof(int));
        printf("3- Long Dimensione: %lu \n",sizeof(long));
        printf("4- Double Dimensione: %lu \n",sizeof(double));
        printf("Inserisci la tua scelta:");
        if (scanf(" %c",&scelta)!=1) break;

        size_t dimensione_dato_richiesto = 0;
        switch (scelta) {
            case '1': dimensione_dato_richiesto = sizeof(char); break;
            case '2': dimensione_dato_richiesto = sizeof(int); break;
            case '3': dimensione_dato_richiesto = sizeof(long); break;
            case '4': dimensione_dato_richiesto = sizeof(double); break;
            default: printf("Scelta incorretta riprovare"); continue;
        }
        //verifica che la dimensione del dato richiesto sia possibile inserirla nelll'array

        if (offset + dimensione_dato_richiesto >= MEM_SIZE) {
            printf("stampa");
            //TODO
            break;

        }


        if (scelta=='1') {
            char val;
            printf("inserisci un carattere:");
            scanf(" %c",&val);
            mempcpy(memoria+offset,&val,dimensione_dato_richiesto);
        }else if(scelta == '2') {
            int val;
            printf("inserisci un intero:");
            scanf(" %d",&val);
            mempcpy(memoria+offset,&val,dimensione_dato_richiesto);
        }else if (scelta == '3') {
            long val;
            printf("inserisci un long:");
            scanf(" %ld",&val);
            mempcpy(memoria+offset,&val,dimensione_dato_richiesto);
        }else if (scelta == '4') {
            double val;
            printf("inserisci un double:");
            scanf(" %lf",&val);
            mempcpy(memoria+offset,&val,dimensione_dato_richiesto);
        }
        offset += dimensione_dato_richiesto;
        printf("dato salvato");
    }
}